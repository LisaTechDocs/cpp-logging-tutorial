# Configure and verify structured logging in a C++ application

## Overview 

This tutorial demonstrates how to configure structured logging in a C++ application using spdlog, an open-source C++ logging library.

The example application simulates a temperature-monitoring and cooling-control system. Three named loggers (sensor, control and output) generate events representing temperature readings, control responses and actuator failures. Each logger has an independent severity threshold, and log messages are formatted as JSON. Logs can be writen to the terminal, a file or both destinations.

In this tutorial:

- Build a C++ application with CMake and spdlog.
- Configure named loggers with different minimum severity levels.
- Select logging destinations using command-line arguments.
- Verify that messages are filtered and written to the expected destinations.

## Prerequisites

The following tools and dependencies are required:

- A Linux development environment.
- A C++17-compatible compiler, such as GCC.
- CMake 3.16 or later.
- The spdlog development library.
- The nlohmann/json development library, version 3.2.0 or later.
- The `jq` command-line tool for parsing and verifying JSON log output.

On Ubuntu, install the required libraries and tools:

```bash
sudo apt update
sudo apt install libspdlog-dev nlohmann-json3-dev jq
```

## Build the application

The project uses CMake to configure and build the application. CMake locates the installed spdlog and nlohmann/json libraries and makes them available to the executable.

1. Clone the repository and navigate to the project directory:

   ```bash
   git clone https://github.com/LisaTechDocs/cpp-logging-tutorial.git
   cd cpp-logging-tutorial
   ```

2. From the repository root configure the build:

    ```bash
    cmake -S . -B build
    ```
3. Build the application:

    ```bash
    cmake --build build
    ```

### Verify the build

The build completes without errors and creates the `logging_example` executable in the `build` directory.

**Expected output:**

```text
[100%] Built target logging_example
```

## Configure logging

The application uses the `spdlog` library to configure three named loggers: `sensor`, `control` and `output`.

Each logger represents a separate application component and has an independent minimum severity threshold.

### Configure severity thresholds

The application sets the minimum severity level for each logger using the `set_level()` method in `src/main.cpp`:

```cpp
sensor->set_level(spdlog::level::trace);
control->set_level(spdlog::level::warn);
output->set_level(spdlog::level::err);
```

The configured thresholds determine which messages are emitted.

| Logger | Minimum severity | Permitted severity levels |
|---|---|---|
| `sensor` | `trace` | Trace, debug, info, warning, error, critical |
| `control` | `warn` | Warning, error, critical |
| `output` | `error` | Error, critial |

Messages below the configured minimum severity are suppressed.

For example, the `control` logger suppresses info messages but emits warning, error and critical messages.

The application makes nine logging calls across the three loggers. Six messages meet the configured severity thresholds and are emitted; the remaining three are suppressed.

### Configure logging destinations

The application uses `spdlog` sinks to determine where log messages are written. Each sink represents an output destination.

The following sinks are configured in `src/main.cpp`:

- `stdout_color_sink_mt`: Writes coloured log messages to the terminal.
- `basic_file_sink_mt`: Writes log messages to `logging.log`.

The application selects the sinks using a command-line argument.

```cpp
if (mode == "console" || mode == "both") {
    sinks.push_back(
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
}

if (mode == "file" || mode == "both") {
    sinks.push_back(
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "logging.log", true));
}
```

Three logging modes are available:

| Mode | Destination | Argument |
|---|---|---|
| Console | Terminal | `console` |
| File | `logging.log` | `file` |
| Combined | Terminal and `logging.log` | `both` |

The selected sinks are shared by all three named loggers. Each logger retains its independently configured minimum severity threshold.

The file sink uses append mode, so subsequent executions add messages to the existing log file.


## Run the application

1. **Console logging:** Run the application with the `console` argument to display log messages in the terminal.

   ```bash
   ./build/logging_example console
   ```

2. **File logging:** Run the application with the `file` argument to write log messages to `logging.log` in the current working directory.

   ```bash
   ./build/logging_example file
   cat logging.log
   ```

3. **Combined logging:** Run the application with the `both` argument to write log messages to the terminal and `logging.log`.

   ```bash
   ./build/logging_example both
   cat logging.log
   ```

The terminal and log file contain the same messages.

**Example output (timestamps omitted)**

Each mode produces six JSON log entries. The following example shows two entries:

```json
{"component":"sensor","event":"temperature_reading","level":"info","message":"Temperature reading received","sensor_id":"temp_01","temperature_c":24.5}
{"component":"control","cycle_id":42,"elapsed_ms":250,"event":"cooling_response_delayed","level":"warning","limit_ms":200,"message":"Cooling response exceeded the configured limit"}
```

The remaining entries report a temperature threshold warning, a sensor timeout, a cooling-control failure and an actuator-command failure.

The `rm -f logging.log` command removes any existing log file so that each test starts with an empty file.


## Verify logging behaviour

Remove any existing log file and run the application to generate a new set of log entries:

```bash
rm -f logging.log
./build/logging_example both
```

Use `logging.log` to verify severity filtering and JSON formatting.

### Verify severity filtering

Count the messages written to the log file:

```bash
wc -l logging.log
```

**Expected result:** The file contains six log entries.

Display the component, severity and event identifier for each entry:

```bash
jq -r '[.component, .level, .event] | @tsv' logging.log
```

**Expected output:**

```text
sensor   info     temperature_reading
sensor   warning  temperature_threshold_exceeded
sensor   error    sensor_read_timeout
control  warning  cooling_response_delayed
control  error    cooling_control_failed
output   error    actuator_command_failed
```

The output confirms that the `control` logger suppresses info messages and the `output` logger emits only error messages.

### Verify JSON formatting

Parse the log file using `jq`:

```bash
jq -c . logging.log
```

**Expected result:** The command successfully parses and displays six JSON objects, one per line. If the file contains invalid JSON, `jq` reports a parsing error.

## Troubleshooting

### CMake cannot find spdlog

**Problem:** CMake fails to configure the project and reports that it cannot find `spdlog`.

**Solution:** Install the `spdlog` development package:

```bash
sudo apt update
sudo apt install libspdlog-dev
```

Run the CMake configuration command again:

```bash
cmake -S . -B build
```

### The executable cannot be found

**Problem:** Running `./build/logging_example` returns `No such file or directory`.

**Solution:** Confirm that the application has been built:

```bash
cmake --build build
```

Run the executable from the repository's root directory.

### No messages appear in the terminal

**Problem:** The application runs without displaying log messages.

**Solution:** Check the selected logging mode. The `file` mode writes messages to `logging.log` rather than the terminal.

To display messages in the terminal, run:

```bash
./build/logging_example console
```

### The log file contains unexpected messages

**Problem:** `logging.log` contains messages from previous executions.

**Solution:** The file sink uses append mode, which preserves existing log messages.

Remove the existing file before running the application:

```bash
rm -f logging.log
./build/logging_example file
```

The new log file contains only messages from the current execution.


## Next steps

The application demonstrates severity filtering and multiple logging destinations using `spdlog`.

To explore additional logging functionality:

- Modify the severity thresholds in `src/main.cpp` and compare the output.
- Experiment with different log message formats using `set_pattern()`.
- Explore additional sinks, such as rotating file sinks, to manage log file size.

For more information, see the [spdlog documentation](https://github.com/gabime/spdlog).




