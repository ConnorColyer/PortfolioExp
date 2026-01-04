#include "core/log.hpp"

int main() {
  engine::core::init_logging("engine", engine::core::LogLevel::Info, true);
  engine::core::log_info("Engine booted.");
  return 0;
}
