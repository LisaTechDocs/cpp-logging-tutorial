#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main(int argc, char* argv[])
{
    // Select the logging destination.
    std::string mode = argc > 1 ? argv[1] : "console";

    std::vector<spdlog::sink_ptr> sinks;

    if (mode == "console" || mode == "both") {
        sinks.push_back(
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    if (mode == "file" || mode == "both") {
        sinks.push_back(
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                "logging.log", true));
    }

    if (sinks.empty()) {
        std::cerr << "Invalid mode. Use console, file, or both.\n";
        return 1;
    }

    // Create named loggers using the selected sinks.
    auto sensor = std::make_shared<spdlog::logger>(
        "sensor", sinks.begin(), sinks.end());

    auto control = std::make_shared<spdlog::logger>(
        "control", sinks.begin(), sinks.end());

    auto output = std::make_shared<spdlog::logger>(
        "output", sinks.begin(), sinks.end());

    // Set independent severity thresholds.
    sensor->set_level(spdlog::level::trace);
    control->set_level(spdlog::level::warn);
    output->set_level(spdlog::level::err);

    // Generate messages for each logger.
    sensor->info("Sensor reading received");
    sensor->warn("Sensor reading outside expected range");
    sensor->error("Sensor connection lost");

    control->info("Control cycle started");
    control->warn("Control response delayed");
    control->error("Control operation failed");

    output->info("Output operation started");
    output->warn("Output response delayed");
    output->error("Output operation failed");

    return 0;
}
