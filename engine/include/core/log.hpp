#pragma once

#include <string_view>

namespace engine::core {

enum class LogLevel {
  Trace,
  Debug,
  Info,
  Warn,
  Error,
  Critical
};

// Call once at startup.
void init_logging(std::string_view app_name = "engine",
                  LogLevel level = LogLevel::Info,
                  bool to_file = true);

// Convenience helpers (keep hot code clean).
void log_info(std::string_view msg);
void log_warn(std::string_view msg);
void log_error(std::string_view msg);

} // namespace engine::core
