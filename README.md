# nfx-datetime

<!-- Project Info -->

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/blob/main/LICENSE.txt) [![GitHub release (latest by date)](https://img.shields.io/github/v/release/ronan-fdev/nfx-datetime?style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/releases) [![GitHub tag (latest by date)](https://img.shields.io/github/tag/ronan-fdev/nfx-datetime?style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/tags)<br/>

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square) ![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg?style=flat-square) ![Cross Platform](https://img.shields.io/badge/Platform-Linux_Windows-lightgrey?style=flat-square)

<!-- CI/CD Status -->

[![Linux GCC](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-datetime/build-linux-gcc.yml?branch=main&label=Linux%20GCC&style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/actions/workflows/build-linux-gcc.yml) [![Linux Clang](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-datetime/build-linux-clang.yml?branch=main&label=Linux%20Clang&style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/actions/workflows/build-linux-clang.yml) [![Windows MinGW](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-datetime/build-windows-mingw.yml?branch=main&label=Windows%20MinGW&style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/actions/workflows/build-windows-mingw.yml) [![Windows MSVC](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-datetime/build-windows-msvc.yml?branch=main&label=Windows%20MSVC&style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/actions/workflows/build-windows-msvc.yml) [![CodeQL](https://img.shields.io/github/actions/workflow/status/ronan-fdev/nfx-datetime/codeql.yml?branch=main&label=CodeQL&style=flat-square)](https://github.com/ronan-fdev/nfx-datetime/actions/workflows/codeql.yml)

> A high-performance C++20 datetime library with 100-nanosecond precision, full ISO 8601 support, and cross-platform timezone handling

## Overview

nfx-datetime is a modern C++20 library providing high-precision temporal operations with 100-nanosecond resolution. It offers comprehensive ISO 8601 parsing and formatting, timezone-aware datetime handling, and arithmetic operations optimized for performance across multiple platforms and compilers.

## Features

### 🕒 High-Precision Temporal Types

- **DateTime**: UTC-only datetime operations with 100-nanosecond precision (ticks)
- **DateTimeOffset**: Timezone-aware datetime with UTC offset handling
- **TimeSpan**: Duration and interval arithmetic with high precision

### 📅 ISO 8601 Compliance

- Full ISO 8601 parsing and formatting support
- Basic format: `20250124T054200Z`
- Extended format: `2025-01-24T05:42:00Z`
- Timezone offsets: `2025-01-24T05:42:00+02:00`
- Duration format: `P1DT2H30M` (1 day, 2 hours, 30 minutes)

### ⚡ Performance Optimized

- Sub-nanosecond arithmetic operations
- Highly optimized parsing
- Efficient string formatting
- Zero-cost abstractions with constexpr support
- Compiler-optimized inline implementations

### 🌍 Cross-Platform Support

- Linux, Windows
- GCC 14+, Clang 19+, MSVC 2022+
- Thread-safe operations
- Consistent behavior across platforms

## Quick Start

### Requirements

- C++20 compatible compiler:
  - **GCC 14+** (14.2.0 tested)
  - **Clang 18+** (19.1.7 tested)
  - **MSVC 2022+** (19.44+ tested)
- CMake 3.20 or higher

### CMake Integration

```cmake
# Build options
option(NFX_DATETIME_BUILD_STATIC         "Build static library"               ON  )
option(NFX_DATETIME_BUILD_SHARED         "Build shared library"               ON  )
option(NFX_DATETIME_USE_CACHE            "Enable compiler cache"              ON  )

# Development options
option(NFX_DATETIME_BUILD_TESTS          "Build tests"                        ON  )
option(NFX_DATETIME_BUILD_SAMPLES        "Build samples"                      ON  )
option(NFX_DATETIME_BUILD_BENCHMARKS     "Build benchmarks"                   ON  )
option(NFX_DATETIME_BUILD_DOCUMENTATION  "Build Doxygen documentation"        ON  )

# Installation and packaging
option(NFX_DATETIME_INSTALL_PROJECT      "Install project"                    ON  )
option(NFX_DATETIME_PACKAGE_SOURCE       "Enable source package generation"   ON  )
option(NFX_DATETIME_PACKAGE_ARCHIVE      "Enable TGZ/ZIP package generation"  ON  )
option(NFX_DATETIME_PACKAGE_DEB          "Enable DEB package generation"      ON  )
option(NFX_DATETIME_PACKAGE_RPM          "Enable RPM package generation"      ON  )
option(NFX_DATETIME_PACKAGE_WIX          "Enable WiX MSI installer"           ON  )
```

### Using in Your Project

#### Option 1: Using FetchContent (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
  nfx-datetime
  GIT_REPOSITORY https://github.com/ronan-fdev/nfx-datetime.git
  GIT_TAG        main  # or use specific version tag like "1.0.0"
)
FetchContent_MakeAvailable(nfx-datetime)

# Link with static library (recommended)
target_link_libraries(your_target PRIVATE nfx-datetime::static)

# Or link with shared library
# target_link_libraries(your_target PRIVATE nfx-datetime::nfx-datetime)
```

#### Option 2: As a Git Submodule

```bash
# Add as submodule
git submodule add https://github.com/ronan-fdev/nfx-datetime.git third-party/nfx-datetime
```

```cmake
# In your CMakeLists.txt
add_subdirectory(third-party/nfx-datetime)
target_link_libraries(your_target PRIVATE nfx-datetime::static)
```

#### Option 3: Using find_package (After Installation)

```cmake
find_package(nfx-datetime REQUIRED)
target_link_libraries(your_target PRIVATE nfx-datetime::static)
```

### Building

```bash
# Clone the repository
git clone https://github.com/ronan-fdev/nfx-datetime.git
cd nfx-datetime

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release --parallel

# Run tests (optional)
ctest -C Release --output-on-failure

# Run benchmarks (optional)
./build/bin/benchmarks/BM_DateTime
./build/bin/benchmarks/BM_DateTimeOffset
./build/bin/benchmarks/BM_TimeSpan
```

### Documentation

nfx-datetime includes comprehensive API documentation generated with Doxygen.

#### 📚 Online Documentation

The complete API documentation is available online at:
**https://ronan-fdev.github.io/nfx-datetime**

#### Building Documentation Locally

```bash
# Configure with documentation enabled
cmake .. -DCMAKE_BUILD_TYPE=Release -DNFX_DATETIME_BUILD_DOCUMENTATION=ON

# Build the documentation
cmake --build . --target documentation
```

#### Requirements

- **Doxygen** - Documentation generation tool
- **Graphviz Dot** (optional) - For generating class diagrams

#### Accessing Local Documentation

After building, open `./build/doc/html/index.html` in your web browser.

## Usage Examples

### DateTime - UTC Operations

```cpp
#include <nfx/datetime/DateTime.h>

using namespace nfx::datetime;

// Current time
DateTime now = DateTime::now();

// Construction from components
DateTime dt1(2025, 1, 24, 5, 42, 0);  // 2025-01-24 05:42:00 UTC

// ISO 8601 parsing
DateTime dt2 = DateTime::parse("2025-01-24T05:42:00Z");

// Arithmetic operations
TimeSpan oneHour = TimeSpan::fromHours(1);
DateTime later = dt1 + oneHour;

// Formatting
std::string iso = dt1.toIso8601Extended();  // "2025-01-24T05:42:00Z"

// Unix timestamp conversions
std::int64_t unixTime = dt1.toUnixSeconds();
DateTime fromUnix = DateTime::sinceEpochSeconds(unixTime);

// Component access
int year = dt1.year();
int month = dt1.month();
int day = dt1.day();
```

### DateTimeOffset - Timezone-Aware Operations

```cpp
#include <nfx/datetime/DateTimeOffset.h>

using namespace nfx::datetime;

// Current time with local offset
DateTimeOffset now = DateTimeOffset::now();

// Construction with offset
TimeSpan offset = TimeSpan::fromHours(2);  // +02:00
DateTimeOffset dto1(2025, 1, 24, 5, 42, 0, offset);

// ISO 8601 parsing with offset
DateTimeOffset dto2 = DateTimeOffset::parse("2025-01-24T05:42:00+02:00");

// Convert to different timezone
DateTimeOffset utc = dto1.toUniversalTime();
DateTimeOffset newYork = dto1.toOffset(TimeSpan::fromHours(-5));

// Get underlying UTC datetime
DateTime utcDateTime = dto1.utcDateTime();

// Formatting
std::string iso = dto1.toString();  // "2025-01-24T05:42:00+02:00"
```

### TimeSpan - Duration Calculations

```cpp
#include <nfx/datetime/TimeSpan.h>

using namespace nfx::datetime;

// Construction from various units
TimeSpan ts1 = TimeSpan::fromHours(2);
TimeSpan ts2 = TimeSpan::fromMinutes(30);
TimeSpan ts3 = TimeSpan::fromSeconds(45);

// ISO 8601 duration parsing
TimeSpan ts4 = TimeSpan::parse("PT2H30M");  // 2 hours 30 minutes

// Arithmetic
TimeSpan total = ts1 + ts2;  // 2.5 hours
TimeSpan diff = ts1 - ts2;   // 1.5 hours

// Conversions
double hours = total.totalHours();          // 2.5
double minutes = total.totalMinutes();      // 150
std::int64_t seconds = total.totalSeconds(); // 9000

// Formatting
std::string iso = total.toString();  // "PT2H30M"
```

### Complete Example

```cpp
#include <iostream>
#include <nfx/datetime/DateTime.h>
#include <nfx/datetime/DateTimeOffset.h>
#include <nfx/datetime/TimeSpan.h>

int main()
{
    using namespace nfx::datetime;

    // Get current time
    DateTime now = DateTime::now();
    std::cout << "Current UTC time: " << now.toIso8601Extended() << std::endl;

    // Calculate future date
    TimeSpan oneWeek = TimeSpan::fromDays(7);
    DateTime nextWeek = now + oneWeek;
    std::cout << "One week from now: " << nextWeek.toIso8601Extended() << std::endl;

    // Work with timezones
    DateTimeOffset localNow = DateTimeOffset::now();
    std::cout << "Local time: " << localNow.toString() << std::endl;

    // Calculate duration
    TimeSpan duration = nextWeek - now;
    std::cout << "Duration: " << duration.toString() << std::endl;
    std::cout << "In hours: " << duration.totalHours() << std::endl;

    return 0;
}
```

**Sample Output:**

```
Current UTC time: 2025-10-25T12:42:36.8324975Z
One week from now: 2025-11-01T12:42:36.8324975Z
Local time: 2025-10-25T13:42:36+01:00
Duration: P7DT0S
In hours: 168
```

## Installation & Packaging

nfx-datetime provides comprehensive packaging options for distribution.

### Package Generation

```bash
# Configure with packaging options
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNFX_DATETIME_BUILD_STATIC=ON \
         -DNFX_DATETIME_BUILD_SHARED=ON \
         -DNFX_DATETIME_PACKAGE_ARCHIVE=ON \
         -DNFX_DATETIME_PACKAGE_DEB=ON \
         -DNFX_DATETIME_PACKAGE_RPM=ON

# Generate binary packages
cmake --build . --target package
# or
cd build && cpack

# Generate source packages
cd build && cpack --config CPackSourceConfig.cmake
```

### Supported Package Formats

| Format      | Platform       | Description                        | Requirements |
| ----------- | -------------- | ---------------------------------- | ------------ |
| **TGZ/ZIP** | Cross-platform | Compressed archive packages        | None         |
| **DEB**     | Debian/Ubuntu  | Native Debian packages             | `dpkg-dev`   |
| **RPM**     | RedHat/SUSE    | Native RPM packages                | `rpm-build`  |
| **WiX**     | Windows        | Professional MSI installer         | `WiX 3.11+`  |
| **Source**  | Cross-platform | Source code distribution (TGZ+ZIP) | None         |

### Installation

```bash
# Linux (DEB-based systems)
sudo dpkg -i nfx-datetime_*_amd64.deb

# Linux (RPM-based systems)
sudo rpm -ivh nfx-datetime-*-Linux.rpm

# Windows
# Run the .exe installer with administrator privileges
nfx-datetime-*-win64.exe

# Manual installation (extract archive)
tar -xzf nfx-datetime-*-Linux.tar.gz -C /usr/local/
```

## Project Structure

```
nfx-datetime/
├── benchmark/             # Performance benchmarks with Google Benchmark
├── cmake/                 # CMake modules and configuration
├── include/nfx/           # Public headers: DateTime, DateTimeOffset, TimeSpan
├── samples/               # Example usage and demonstrations
├── src/                   # Implementation files
└── test/                  # Comprehensive unit tests with GoogleTest
```

## Performance

For detailed performance metrics across all compilers (GCC, Clang, MSVC, MinGW) and platforms, see the [benchmark documentation](benchmark/README.md).

## Changelog

See the [changelog](CHANGELOG.md) for a detailed history of changes, new features, and bug fixes.

## License

This project is licensed under the MIT License.

## Dependencies

- **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
- **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building tests or benchmarks.

---

_Updated on October 26, 2025_
