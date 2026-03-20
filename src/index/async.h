#ifndef SOURCEMETA_ONE_INDEX_ASYNC_H_
#define SOURCEMETA_ONE_INDEX_ASYNC_H_

#include <cassert>    // assert
#include <cerrno>     // errno, EINTR
#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint8_t
#include <filesystem> // std::filesystem::path
#include <functional> // std::function
#include <span>       // std::span
#include <vector>     // std::vector

#include <fcntl.h>  // open, O_RDONLY, O_NOATIME
#include <unistd.h> // pread, close

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#endif

#ifdef SOURCEMETA_ONE_HAS_URING
#include <liburing.h>
#endif

static auto parallel_read_single(
    const std::filesystem::path &path, const std::size_t file_index,
    const std::size_t initial_size,
    const std::function<std::size_t(std::size_t index, const std::uint8_t *data,
                                    std::size_t length)> &callback) -> void {
#ifdef __linux__
  const int file_descriptor{open(path.c_str(), O_RDONLY | O_NOATIME)};
#else
  const int file_descriptor{open(path.c_str(), O_RDONLY)};
#endif
  if (file_descriptor < 0) {
    callback(file_index, nullptr, 0);
    return;
  }

  std::vector<std::uint8_t> buffer(initial_size);
  std::size_t total_bytes_read{0};

  ssize_t result;
  do {
    result = pread(file_descriptor, buffer.data(), initial_size, 0);
  } while (result == -1 && errno == EINTR);

  if (result <= 0) {
    close(file_descriptor);
    callback(file_index, nullptr, 0);
    return;
  }

  total_bytes_read = static_cast<std::size_t>(result);

  for (;;) {
    const auto requested{callback(file_index, buffer.data(), total_bytes_read)};
    if (requested == 0) {
      break;
    }

    buffer.resize(total_bytes_read + requested);
    do {
      result = pread(file_descriptor, buffer.data() + total_bytes_read,
                     requested, static_cast<off_t>(total_bytes_read));
    } while (result == -1 && errno == EINTR);

    if (result <= 0) {
      break;
    }

    total_bytes_read += static_cast<std::size_t>(result);
  }

  close(file_descriptor);
}

#ifdef SOURCEMETA_ONE_HAS_URING

static auto parallel_read_uring(
    std::span<const std::filesystem::path> paths,
    const std::size_t initial_size,
    const std::function<std::size_t(std::size_t index, const std::uint8_t *data,
                                    std::size_t length)> &callback) -> void {
  const auto file_count{paths.size()};
  constexpr unsigned RING_SIZE{4096};

  struct io_uring ring{};
  if (io_uring_queue_init(RING_SIZE, &ring, 0) < 0) {
    // Fallback to sequential if io_uring init fails
    for (std::size_t file_index = 0; file_index < file_count; ++file_index) {
      parallel_read_single(paths[file_index], file_index, initial_size,
                           callback);
    }
    return;
  }

  // Per-file state
  struct FileState {
    int file_descriptor{-1};
    std::vector<std::uint8_t> buffer;
    std::size_t total_bytes_read{0};
  };

  std::vector<FileState> states(file_count);

  // Round 1: Open all files
  for (std::size_t file_index = 0; file_index < file_count; ++file_index) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    while (sqe == nullptr) {
      io_uring_submit(&ring);
      sqe = io_uring_get_sqe(&ring);
    }

    io_uring_prep_openat(sqe, AT_FDCWD, paths[file_index].c_str(),
                         O_RDONLY | O_NOATIME, 0);
    io_uring_sqe_set_data64(sqe, file_index);
  }

  io_uring_submit(&ring);
  for (std::size_t completed = 0; completed < file_count; ++completed) {
    struct io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);
    const auto index{io_uring_cqe_get_data64(cqe)};
    states[index].file_descriptor = cqe->res;
    io_uring_cqe_seen(&ring, cqe);
  }

  // Round 2: Initial read from all files
  std::size_t read_count{0};
  for (std::size_t file_index = 0; file_index < file_count; ++file_index) {
    if (states[file_index].file_descriptor < 0) {
      callback(file_index, nullptr, 0);
      continue;
    }

    states[file_index].buffer.resize(initial_size);

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    while (sqe == nullptr) {
      io_uring_submit(&ring);
      sqe = io_uring_get_sqe(&ring);
    }

    io_uring_prep_read(sqe, states[file_index].file_descriptor,
                       states[file_index].buffer.data(),
                       static_cast<unsigned>(initial_size), 0);
    io_uring_sqe_set_data64(sqe, file_index);
    ++read_count;
  }

  io_uring_submit(&ring);
  for (std::size_t completed = 0; completed < read_count; ++completed) {
    struct io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);
    const auto index{io_uring_cqe_get_data64(cqe)};
    if (cqe->res > 0) {
      states[index].total_bytes_read = static_cast<std::size_t>(cqe->res);
    }
    io_uring_cqe_seen(&ring, cqe);
  }

  // Call callbacks for initial reads, collect files needing more data
  std::vector<std::size_t> pending_indices;
  std::vector<std::size_t> pending_sizes;
  for (std::size_t file_index = 0; file_index < file_count; ++file_index) {
    if (states[file_index].file_descriptor < 0 ||
        states[file_index].total_bytes_read == 0) {
      if (states[file_index].file_descriptor >= 0) {
        callback(file_index, nullptr, 0);
      }
      continue;
    }

    const auto requested{callback(file_index, states[file_index].buffer.data(),
                                  states[file_index].total_bytes_read)};
    if (requested > 0) {
      pending_indices.push_back(file_index);
      pending_sizes.push_back(requested);
    }
  }

  // Round 3: Read extension data for files that need more
  if (!pending_indices.empty()) {
    for (std::size_t pending = 0; pending < pending_indices.size(); ++pending) {
      const auto file_index{pending_indices[pending]};
      const auto requested{pending_sizes[pending]};
      auto &state{states[file_index]};

      state.buffer.resize(state.total_bytes_read + requested);

      struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
      while (sqe == nullptr) {
        io_uring_submit(&ring);
        sqe = io_uring_get_sqe(&ring);
      }

      io_uring_prep_read(sqe, state.file_descriptor,
                         state.buffer.data() + state.total_bytes_read,
                         static_cast<unsigned>(requested),
                         static_cast<__u64>(state.total_bytes_read));
      io_uring_sqe_set_data64(sqe, file_index);
    }

    io_uring_submit(&ring);
    for (std::size_t completed = 0; completed < pending_indices.size();
         ++completed) {
      struct io_uring_cqe *cqe;
      io_uring_wait_cqe(&ring, &cqe);
      const auto index{io_uring_cqe_get_data64(cqe)};
      if (cqe->res > 0) {
        states[index].total_bytes_read += static_cast<std::size_t>(cqe->res);
      }
      io_uring_cqe_seen(&ring, cqe);
    }

    // Call callbacks with the full data
    for (const auto file_index : pending_indices) {
      callback(file_index, states[file_index].buffer.data(),
               states[file_index].total_bytes_read);
    }
  }

  // Round 4: Close all files
  std::size_t close_count{0};
  for (std::size_t file_index = 0; file_index < file_count; ++file_index) {
    if (states[file_index].file_descriptor < 0) {
      continue;
    }

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    while (sqe == nullptr) {
      io_uring_submit(&ring);
      sqe = io_uring_get_sqe(&ring);
    }

    io_uring_prep_close(sqe, states[file_index].file_descriptor);
    io_uring_sqe_set_data64(sqe, file_index);
    ++close_count;
  }

  io_uring_submit(&ring);
  for (std::size_t completed = 0; completed < close_count; ++completed) {
    struct io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);
    io_uring_cqe_seen(&ring, cqe);
  }

  io_uring_queue_exit(&ring);
}

#endif // SOURCEMETA_ONE_HAS_URING

/// Read from N files. For each file, reads `initial_size` bytes from offset 0
/// and calls `callback(index, data, length)`. If the callback returns > 0,
/// reads that many more bytes continuing from the current offset and calls the
/// callback again with the cumulative buffer. Repeats until the callback
/// returns 0. If a file cannot be opened or read, the callback receives
/// (index, nullptr, 0).
static auto parallel_read(
    std::span<const std::filesystem::path> paths,
    const std::size_t initial_size,
    const std::function<std::size_t(std::size_t index, const std::uint8_t *data,
                                    std::size_t length)> &callback) -> void {
  assert(initial_size > 0);
  if (paths.empty()) {
    return;
  }

#ifdef SOURCEMETA_ONE_HAS_URING
  parallel_read_uring(paths, initial_size, callback);
#elif defined(__APPLE__)
  const auto file_count{paths.size()};
  const auto thread_count{
      static_cast<std::size_t>(sysconf(_SC_NPROCESSORS_ONLN))};
  const auto chunk_count{std::min(thread_count, file_count)};
  dispatch_queue_t queue =
      dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
  const auto &paths_ref = paths;
  const auto &callback_ref = callback;
  dispatch_apply(chunk_count, queue, ^(std::size_t chunk_index) {
    const auto start{chunk_index * file_count / chunk_count};
    const auto end{(chunk_index + 1) * file_count / chunk_count};
    for (std::size_t file_index = start; file_index < end; ++file_index) {
      parallel_read_single(paths_ref[file_index], file_index, initial_size,
                           callback_ref);
    }
  });
#else
  for (std::size_t file_index = 0; file_index < paths.size(); ++file_index) {
    parallel_read_single(paths[file_index], file_index, initial_size, callback);
  }
#endif
}

#endif
