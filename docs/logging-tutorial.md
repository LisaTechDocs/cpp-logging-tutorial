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