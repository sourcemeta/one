#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_METRICS_V1_H
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_METRICS_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <algorithm>   // std::ranges::transform
#include <array>       // std::array
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem::path
#include <format>      // std::format
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

#if defined(__APPLE__)
#include <libproc.h>      // proc_pidinfo, PROC_PIDLISTFDS, PROC_PIDLISTFD_SIZE
#include <mach/mach.h>    // mach_task_self, task_info, MACH_TASK_BASIC_INFO
#include <sys/resource.h> // getrusage, getrlimit, RUSAGE_SELF, RLIMIT_NOFILE
#include <unistd.h>       // getpid
#elif defined(__linux__)
#include <fstream>        // std::ifstream
#include <sstream>        // std::istringstream
#include <sys/resource.h> // getrlimit, RLIMIT_NOFILE
#include <system_error>   // std::error_code
#include <unistd.h>       // sysconf, _SC_CLK_TCK, _SC_PAGESIZE
#endif

class ActionMetrics_v1 : public sourcemeta::one::RouterAction {
public:
  static constexpr std::string_view DESCRIPTION{
      "Report instance telemetry in the Prometheus exposition format"};
  static constexpr bool READ_ONLY{true};
  static constexpr bool DESTRUCTIVE{false};
  static constexpr bool IDEMPOTENT{true};
  static constexpr bool OPEN_WORLD{false};

  // How a boundary is spelled is part of the name of the series it bounds, so
  // it is decided here rather than by whatever a formatter does with the same
  // number on a given platform
  static constexpr std::array<std::string_view,
                              sourcemeta::one::HTTPMetrics::BUCKET_COUNT>
      BOUNDARIES{{"0.0001", "0.00025", "0.0005", "0.001", "0.0025", "0.005",
                  "0.01", "0.05", "0.25", "1.0"}};

  ActionMetrics_v1(
      const std::filesystem::path &base,
      const sourcemeta::core::URITemplateRouterView &router,
      const sourcemeta::core::URITemplateRouter::Identifier identifier,
      sourcemeta::one::Router &dispatcher)
      : sourcemeta::one::RouterAction{base, router.base_url(), dispatcher} {
    router.arguments(
        identifier, [this](const auto &key, const auto &value) -> void {
          if (key == "errorSchema") {
            this->error_schema_ = std::get<std::string_view>(value);
          }
        });
  }

  auto rest(const std::span<std::string_view>,
            const sourcemeta::one::Authentication::Caller &,
            sourcemeta::one::HTTPRequest &request,
            sourcemeta::one::HTTPResponse &response) -> void override {
    if (request.method() == "options") {
      sourcemeta::one::cors_preflight(request, response, "GET, HEAD, OPTIONS",
                                      "Accept, Accept-Encoding");
      return;
    }

    if (request.method() != "get" && request.method() != "head") {
      sourcemeta::one::json_error(
          request, response, sourcemeta::core::HTTP_STATUS_METHOD_NOT_ALLOWED,
          "urn:sourcemeta:one:method-not-allowed",
          "This HTTP method is invalid for this URL", this->error_schema_, "*",
          "GET, HEAD, OPTIONS");
      return;
    }

    const auto payload{this->serialize()};
    response.write_status(sourcemeta::core::HTTP_STATUS_OK);
    // The exposition format names its own revision in the media type, which is
    // how a scraper knows what it is reading without asking
    response.write_header("Content-Type",
                          "text/plain; version=0.0.4; charset=utf-8");
    // A scrape stands for the moment it was taken, so an answer kept and handed
    // to the next one would report a past that never comes back
    response.write_header("Cache-Control",
                          sourcemeta::one::cache_control_no_store());
    sourcemeta::one::send_response(sourcemeta::core::HTTP_STATUS_OK, request,
                                   response, payload,
                                   sourcemeta::one::Encoding::Identity);
  }

  auto mcp(const sourcemeta::core::MCPProtocolVersion,
           const sourcemeta::core::JSON &id, const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(id);
  }

private:
  // What a process says about itself, which the platform answers rather than
  // this program keeping count of. Anything a platform cannot cheaply say is
  // left out rather than guessed at
  struct Process {
    double cpu_seconds{0};
    std::uint64_t resident_bytes{0};
    std::uint64_t virtual_bytes{0};
    std::uint64_t open_descriptors{0};
    std::uint64_t maximum_descriptors{0};
  };

  [[nodiscard]] static auto descriptor_limit() -> std::uint64_t {
    rlimit limit{};
    if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
      return 0;
    }

    return static_cast<std::uint64_t>(limit.rlim_cur);
  }

#if defined(__APPLE__)

  [[nodiscard]] static auto read_process() -> Process {
    Process sample;

    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
      sample.cpu_seconds =
          static_cast<double>(usage.ru_utime.tv_sec) +
          static_cast<double>(usage.ru_utime.tv_usec) / 1000000.0 +
          static_cast<double>(usage.ru_stime.tv_sec) +
          static_cast<double>(usage.ru_stime.tv_usec) / 1000000.0;
    }

    mach_task_basic_info info{};
    mach_msg_type_number_t count{MACH_TASK_BASIC_INFO_COUNT};
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                  reinterpret_cast<task_info_t>(&info),
                  &count) == KERN_SUCCESS) {
      sample.resident_bytes = info.resident_size;
      sample.virtual_bytes = info.virtual_size;
    }

    const auto descriptors{
        proc_pidinfo(getpid(), PROC_PIDLISTFDS, 0, nullptr, 0)};
    if (descriptors > 0) {
      sample.open_descriptors =
          static_cast<std::uint64_t>(descriptors) / PROC_PIDLISTFD_SIZE;
    }

    sample.maximum_descriptors = descriptor_limit();
    return sample;
  }

#elif defined(__linux__)

  // The second field is a command name in parentheses that may itself contain
  // spaces, so what follows it is found from the last parenthesis rather than
  // by counting separators from the beginning
  [[nodiscard]] static auto read_process() -> Process {
    Process sample;

    std::ifstream stream{"/proc/self/stat"};
    if (stream.is_open()) {
      std::string line;
      std::getline(stream, line);
      const auto comm{line.rfind(')')};
      if (comm != std::string::npos) {
        std::istringstream fields{line.substr(comm + 1)};
        std::vector<std::string> tokens;
        std::string token;
        while (fields >> token) {
          tokens.push_back(token);
        }

        const auto ticks{static_cast<double>(sysconf(_SC_CLK_TCK))};
        if (tokens.size() > 21 && ticks > 0) {
          sample.cpu_seconds =
              (std::stod(tokens.at(11)) + std::stod(tokens.at(12))) / ticks;
          sample.virtual_bytes = std::stoull(tokens.at(20));
          sample.resident_bytes =
              std::stoull(tokens.at(21)) *
              static_cast<std::uint64_t>(sysconf(_SC_PAGESIZE));
        }
      }
    }

    std::error_code error;
    const std::filesystem::directory_iterator descriptors{"/proc/self/fd",
                                                          error};
    if (!error) {
      for (const auto &entry : descriptors) {
        static_cast<void>(entry);
        sample.open_descriptors += 1;
      }
    }

    sample.maximum_descriptors = descriptor_limit();
    return sample;
  }

#else

  [[nodiscard]] static auto read_process() -> Process { return {}; }

#endif

  static auto family(std::string &output, const std::string_view name,
                     const std::string_view help, const std::string_view type)
      -> void {
    output +=
        std::format("# HELP {} {}\n# TYPE {} {}\n", name, help, name, type);
  }

  [[nodiscard]] auto serialize() const -> std::string {
    const auto &metrics{sourcemeta::one::http_metrics()};
    const auto state{metrics.snapshot()};
    const auto process{read_process()};

    std::string edition{sourcemeta::one::edition()};
    std::ranges::transform(edition, edition.begin(),
                           [](const char character) -> char {
                             return sourcemeta::core::to_lowercase(character);
                           });

    std::string output;
    output.reserve(8192);

    family(output, "sourcemeta_one_build_info", "Build information", "gauge");
    output += std::format(
        "sourcemeta_one_build_info{{version=\"{}\",edition=\"{}\"}} 1\n\n",
        sourcemeta::one::version(), edition);

    family(output, "process_start_time_seconds",
           "Start time of the process since the Unix epoch", "gauge");
    output +=
        std::format("process_start_time_seconds {}\n\n", metrics.started());

    family(output, "process_cpu_seconds_total",
           "Total user and system CPU time spent", "counter");
    output +=
        std::format("process_cpu_seconds_total {}\n\n", process.cpu_seconds);

    family(output, "process_resident_memory_bytes", "Resident memory size",
           "gauge");
    output += std::format("process_resident_memory_bytes {}\n\n",
                          process.resident_bytes);

    family(output, "process_virtual_memory_bytes", "Virtual memory size",
           "gauge");
    output += std::format("process_virtual_memory_bytes {}\n\n",
                          process.virtual_bytes);

    family(output, "process_open_fds", "Number of open file descriptors",
           "gauge");
    output += std::format("process_open_fds {}\n\n", process.open_descriptors);

    family(output, "process_max_fds", "Maximum number of open file descriptors",
           "gauge");
    output +=
        std::format("process_max_fds {}\n\n", process.maximum_descriptors);

    family(output, "sourcemeta_one_http_requests_in_flight",
           "Requests currently being served", "gauge");
    output += std::format("sourcemeta_one_http_requests_in_flight {}\n\n",
                          state.in_flight);

    family(output, "sourcemeta_one_metrics_dropped_total",
           "Observations that could not be recorded", "counter");
    output += std::format("sourcemeta_one_metrics_dropped_total {}\n\n",
                          state.dropped);

    family(output, "sourcemeta_one_http_requests_total",
           "Total HTTP requests handled", "counter");
    for (const auto &entry : state.requests) {
      output += std::format("sourcemeta_one_http_requests_total{{action=\"{}\","
                            "code=\"{}\"}} {}\n",
                            sourcemeta::one::ACTION_NAMES.at(entry.handler),
                            entry.status, entry.count);
    }

    output += "\n";
    family(output, "sourcemeta_one_http_request_duration_seconds",
           "Request duration", "histogram");
    for (std::size_t action = 0; action < state.buckets.size(); action++) {
      std::uint64_t cumulative{0};
      for (const auto count : state.buckets[action]) {
        cumulative += count;
      }

      if (cumulative == 0) {
        continue;
      }

      cumulative = 0;
      const auto &name{sourcemeta::one::ACTION_NAMES.at(action)};
      for (std::size_t bucket = 0;
           bucket < sourcemeta::one::HTTPMetrics::BUCKET_COUNT; bucket++) {
        cumulative += state.buckets[action][bucket];
        output += std::format("sourcemeta_one_http_request_duration_seconds_"
                              "bucket{{action=\"{}\",le=\"{}\"}} {}\n",
                              name, BOUNDARIES.at(bucket), cumulative);
      }

      cumulative +=
          state.buckets[action][sourcemeta::one::HTTPMetrics::BUCKET_COUNT];
      output += std::format("sourcemeta_one_http_request_duration_seconds_"
                            "bucket{{action=\"{}\",le=\"+Inf\"}} {}\n",
                            name, cumulative);
      output += std::format("sourcemeta_one_http_request_duration_seconds_sum{{"
                            "action=\"{}\"}} {}\n",
                            name, state.sums[action]);
      output +=
          std::format("sourcemeta_one_http_request_duration_seconds_count{{"
                      "action=\"{}\"}} {}\n",
                      name, cumulative);
    }

    return output;
  }

  std::string_view error_schema_;
};

#endif
