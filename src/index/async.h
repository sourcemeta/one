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
#include <dispatch/dispatch.h> // GCD
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

#ifdef __APPLE__
  dispatch_group_t group = dispatch_group_create();
  dispatch_queue_t queue =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

  for (std::size_t file_index = 0; file_index < paths.size(); ++file_index) {
    dispatch_group_async(group, queue, ^{
      parallel_read_single(paths[file_index], file_index, initial_size,
                           callback);
    });
  }

  dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
#else
  // Sequential pread fallback
  // TODO(perf): Add io_uring backend for Linux
  for (std::size_t file_index = 0; file_index < paths.size(); ++file_index) {
    parallel_read_single(paths[file_index], file_index, initial_size, callback);
  }
#endif
}

#endif
