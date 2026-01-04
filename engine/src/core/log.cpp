#include "core/log.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <memory>
#include <vector>
#include <string>

namespace engine::core {

static spdlog::level::level_enum to_spd(LogLevel lvl) {
  switch (lvl) {
    case LogLevel::Trace:    return spdlog::level::trace;
    case LogLevel::Debug:    return spdlog::level::debug;
    case LogLevel::Info:     return spdlog::level::info;
    case LogLevel::Warn:     return spdlog::level::warn;
    case LogLevel::Error:    return spdlog::level::err;
    case LogLevel::Critical: return spdlog::level::critical;
  }
  return spdlog::level::info;
}

void init_logging(std::string_view app_name, LogLevel level, bool to_file) {
  std::vector<spdlog::sink_ptr> sinks;
  sinks.reserve(2);

  auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  sinks.push_back(console);

  if (to_file) {
    // Write logs to ./logs/<app>.log (relative to working dir)
    // Keep it simple; we can evolve to daily rotation later.
    const std::string path = std::string("logs/") + std::string(app_name) + ".log";
    auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true);
    sinks.push_back(file);
  }

  auto logger = std::make_shared<spdlog::logger>(std::string(app_name), sinks.begin(), sinks.end());
  spdlog::set_default_logger(std::move(logger));
  spdlog::set_level(to_spd(level));
  spdlog::flush_on(spdlog::level::info);

  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
}

void log_info(std::string_view msg)  { spdlog::info("{}", msg); }
void log_warn(std::string_view msg)  { spdlog::warn("{}", msg); }
void log_error(std::string_view msg) { spdlog::error("{}", msg); }

} // namespace engine::core
