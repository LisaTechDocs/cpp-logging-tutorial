
#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using json = nlohmann::json;

// Create a JSON log entry with consistent metadata.
void log_event(
    const std::shared_ptr<spdlog::logger>& logger,
    spdlog::level::level_enum level,
    const std::string& event,
    const std::string& message,
    json fields = json::object())
{
    if (!logger->should_log(level)) {
        return;
    }

    const auto now = std::chrono::system_clock::now();
    const auto seconds = std::chrono::system_clock::to_time_t(now);

    std::tm utc_time{};
    gmtime_r(&seconds, &utc_time);

    char timestamp[32];
    std::strftime(
        timestamp,
        sizeof(timestamp),
        "%Y-%m-%dT%H:%M:%SZ",
        &utc_time);

    json entry = {
        {"timestamp", timestamp},
        {"level", spdlog::level::to_string_view(level).data()},
        {"component", logger->name()},
        {"event", event},
        {"message", message}
    };

    entry.update(fields);

    logger->log(level, "{}", entry.dump());
}

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

    // Output only the JSON payload, without spdlog's default prefix.
    for (const auto& sink : sinks) {
        sink->set_pattern("%v");
    }

    // Create named loggers using the selected sinks.
    auto sensor = std::make_shared<spdlog::logger>(
        "sensor", sinks.begin(), sinks.end());

    auto control = std::make_shared<spdlog::logger>(
        "control", sinks.begin(), sinks.end());

    auto output = std::make_shared<spdlog::logger>(
        "output", sinks.begin(), sinks.end());

    // Severity thresholds.
    sensor->set_level(spdlog::level::trace);
    control->set_level(spdlog::level::warn);
    output->set_level(spdlog::level::err);

    // Simulated sensor events.
    log_event(sensor, spdlog::level::info,
        "temperature_reading",
        "Temperature reading received",
        {{"sensor_id", "temp_01"}, {"temperature_c", 24.5}});

    log_event(sensor, spdlog::level::warn,
        "temperature_threshold_exceeded",
        "Temperature exceeds the configured threshold",
        {{"sensor_id", "temp_01"},
         {"temperature_c", 85.2},
         {"threshold_c", 80.0}});

    log_event(sensor, spdlog::level::err,
        "sensor_read_timeout",
        "Sensor read failed",
        {{"sensor_id", "temp_01"}, {"reason", "timeout"}});

    // Simulated control events.
    log_event(control, spdlog::level::info,
        "control_cycle_started",
        "Cooling control cycle started",
        {{"cycle_id", 42}});

    log_event(control, spdlog::level::warn,
        "cooling_response_delayed",
        "Cooling response exceeded the configured limit",
        {{"cycle_id", 42},
         {"elapsed_ms", 250},
         {"limit_ms", 200}});

    log_event(control, spdlog::level::err,
        "cooling_control_failed",
        "Cooling control failed",
        {{"cycle_id", 42}, {"reason", "actuator_timeout"}});

    // Simulated actuator events.
    log_event(output, spdlog::level::info,
        "actuator_activated",
        "Cooling actuator activated",
        {{"actuator_id", "fan_01"}});

    log_event(output, spdlog::level::warn,
        "actuator_response_delayed",
        "Cooling actuator response delayed",
        {{"actuator_id", "fan_01"}, {"elapsed_ms", 250}});

    log_event(output, spdlog::level::err,
        "actuator_command_failed",
        "Cooling actuator did not acknowledge the command",
        {{"actuator_id", "fan_01"},
         {"reason", "no_acknowledgement"}});

    return 0;
}
