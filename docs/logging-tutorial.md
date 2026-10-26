# Configure and verify logging in a C++ application

## Overview 

This tutorial demonstrates how to configure application logging using `spdlog`, an open-source C++ logging library.

The example application uses three named loggers with independent severity thresholds. It supports logging to the terminal, a file or both destinations.

This tutorial demonstrates how to configure named loggers with independent severity thresholds.

In this tutorial:

- Build a C++ application with CMake and `spdlog`.
- Configure named loggers with different minimum severity levels.
- Select logging destinations using command-line arguments.
- Verify that messages are filtered and written to the expected destinations.

## Prerequisites

The following tools and dependencies are required:

- A Linux development environment.
- A C++17-compatible compiler, such as GCC.
- CMake 3.16 or later.
- The spdlog development library.

For instructions on preparing a Chromebook development environment, see [Set up a C++ development environment on a Chromebook](chromebook-setup.md).

## Build the application

The project uses CMake to configure and build the C++ application. CMake locates the installed `spdlog` library and links it to the executable.

1. Clone the repository:

   ```bash
   git clone https://github.com/LisaTechDocs/cpp-logging-tutorial.git

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

| Logger | Minimum severity | Messages emitted |
|---|---|---|
| `sensor` | `trace` | Info, warning and error |
| `control` | `warn` | Warning and error |
| `output` | `error` | Error only |

Messages below the configured minimum severity are suppressed.

For example, the `control` logger suppresses info messages but emits warning and error messages.

The application generates three messages for each logger, resulting in nine logging calls. The configured thresholds allow six messages to be emitted.


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
