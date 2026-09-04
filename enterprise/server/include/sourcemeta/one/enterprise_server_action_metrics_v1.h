#ifndef SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_METRICS_V1_H
#define SOURCEMETA_ONE_ENTERPRISE_SERVER_ACTION_METRICS_V1_H

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonrpc.h>
#include <sourcemeta/core/mcp.h>
#include <sourcemeta/core/process.h>
#include <sourcemeta/core/text.h>
#include <sourcemeta/core/uritemplate.h>

#include <sourcemeta/one/actions.h>
#include <sourcemeta/one/http.h>
#include <sourcemeta/one/router.h>
#include <sourcemeta/one/shared.h>

#include <algorithm>   // std::ranges::transform
#include <array>       // std::array
#include <chrono>      // std::chrono::duration
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <filesystem>  // std::filesystem::path
#include <format>      // std::format
#include <span>        // std::span
#include <string>      // std::string
#include <string_view> // std::string_view

class ActionMetricsV1 : public sourcemeta::one::RouterAction {
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

  ActionMetricsV1(
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
           const sourcemeta::core::JSON &request_id,
           const sourcemeta::core::JSON &,
           const sourcemeta::one::Authentication::Caller &)
      -> sourcemeta::core::JSON override {
    return sourcemeta::core::jsonrpc_make_error_method_not_found(request_id);
  }

private:
  static auto family(std::string &output, const std::string_view name,
                     const std::string_view help, const std::string_view type)
      -> void {
    output +=
        std::format("# HELP {} {}\n# TYPE {} {}\n", name, help, name, type);
  }

  [[nodiscard]] auto serialize() const -> std::string {
    const auto &metrics{sourcemeta::one::http_metrics()};
    const auto state{metrics.snapshot()};
    const auto usage{sourcemeta::core::process_usage()};
    const auto descriptors{sourcemeta::core::process_descriptors()};
    const auto started{sourcemeta::core::process_start_time()};

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

    // A platform that cannot answer is not made to. A series nobody receives
    // is one nobody plots a flat zero for
    if (started.has_value()) {
      family(output, "process_start_time_seconds",
             "Start time of the process since the Unix epoch", "gauge");
      output += std::format(
          "process_start_time_seconds {}\n\n",
          std::chrono::duration<double>{started.value().time_since_epoch()}
              .count());
    }

    if (usage.cpu_time.has_value()) {
      family(output, "process_cpu_seconds_total",
             "Total user and system CPU time spent", "counter");
      output += std::format(
          "process_cpu_seconds_total {}\n\n",
          std::chrono::duration<double>{usage.cpu_time.value()}.count());
    }

    if (usage.resident_bytes.has_value()) {
      family(output, "process_resident_memory_bytes", "Resident memory size",
             "gauge");
      output += std::format("process_resident_memory_bytes {}\n\n",
                            usage.resident_bytes.value());
    }

    if (usage.virtual_bytes.has_value()) {
      family(output, "process_virtual_memory_bytes", "Virtual memory size",
             "gauge");
      output += std::format("process_virtual_memory_bytes {}\n\n",
                            usage.virtual_bytes.value());
    }

    if (descriptors.open.has_value()) {
      family(output, "process_open_fds", "Number of open file descriptors",
             "gauge");
      output +=
          std::format("process_open_fds {}\n\n", descriptors.open.value());
    }

    if (descriptors.maximum.has_value()) {
      family(output, "process_max_fds",
             "Maximum number of open file descriptors", "gauge");
      output +=
          std::format("process_max_fds {}\n\n", descriptors.maximum.value());
    }

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
