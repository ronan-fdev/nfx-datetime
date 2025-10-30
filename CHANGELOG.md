# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- NIL

### Changed

- NIL

### Deprecated

- NIL

### Removed

- NIL

### Fixed

- NIL

### Security

- NIL

## [1.0.1] - 2025-10-30

### Changed

- Refactored directory variable naming from `NFX_DATETIME_ROOT_DIR` to `NFX_DATETIME_DIR` for consistency

- **GitHub Release Workflow**
  - Refactored release workflow to create GitHub release before building packages
  - All build jobs now depend on release creation job
  - Added tag validation and semantic versioning verification
  - Releases are now created as drafts for review before publishing
  - Added package verification steps to ensure artifacts are created
  - Added SHA256 checksum generation for all packages
  - Added build artifact uploads for debugging
  - Installed GCC-14 for Linux builds

## [1.0.0] - 2025-10-26 - Initial Release

### Added

- **DateTime**: UTC-based date and time representation

  - 100-nanosecond precision (10 million ticks per second)
  - Range: January 1, 0001 to December 31, 9999
  - Complete arithmetic operators (+, -, comparisons)
  - Factory methods (`now()`, `today()`, `minValue()`, `maxValue()`, `epoch()`, `sinceEpochSeconds()`, `sinceEpochMilliseconds()`)
  - Component accessors (`year()`, `month()`, `day()`, `hour()`, `minute()`, `second()`, `millisecond()`, `dayOfWeek()`, `dayOfYear()`)
  - ISO 8601 parsing and formatting (`parse()`, `tryParse()`, `toString()`, `toIso8601Extended()`)
  - Unix timestamp conversion (`toUnixSeconds()`, `toUnixMilliseconds()`)
  - Date/time manipulation (`date()`, `timeOfDay()`)
  - std::chrono interoperability (`toChrono()`, `fromChrono()`)

- **DateTimeOffset**: Timezone-aware date and time representation

  - All DateTime features plus timezone offset support
  - Offset range: -14:00 to +14:00
  - Component accessors (`year()`, `month()`, `day()`, `hour()`, `minute()`, `second()`, `millisecond()`, `microsecond()`, `nanosecond()`, `dayOfWeek()`, `dayOfYear()`)
  - Timezone conversion (`toOffset()`, `toUniversalTime()`, `toLocalTime()`)
  - ISO 8601 parsing and formatting with timezone (`parse()`, `tryParse()`, `toString()`, `toIso8601Extended()`)
  - UTC and local time accessors (`utcDateTime()`, `localDateTime()`, `utcTicks()`)
  - Factory methods (`now()`, `utcNow()`, `today()`, `fromUnixTimeSeconds()`, `fromUnixTimeMilliseconds()`, `fromFILETIME()`)
  - Date/time arithmetic (`add()`, `addDays()`, `addHours()`, `addMinutes()`, `addSeconds()`, `addMonths()`, `addYears()`)
  - Windows FILETIME conversion support

- **TimeSpan**: Duration and time interval representation

  - 100-nanosecond precision matching DateTime
  - Range: -10,675,199 days to +10,675,199 days
  - Complete arithmetic operators (+, -, +=, -=, unary -, comparisons)
  - Factory methods (`fromDays()`, `fromHours()`, `fromMinutes()`, `fromSeconds()`, `fromMilliseconds()`)
  - Total value accessors (`totalDays()`, `totalHours()`, `totalMinutes()`, `totalSeconds()`, `totalMilliseconds()`)
  - Tick count accessor (`ticks()`)
  - ISO 8601 duration parsing and formatting (`parse()`, `tryParse()`, `toString()`)
  - Duration calculations and comparisons
  - std::chrono::duration interoperability (`toChrono()`, `fromChrono()`)

- **Build System**

  - CMake 3.20+ configuration with modular structure
  - Static and shared library targets
  - Cross-platform support (Linux GCC/Clang, Windows MinGW/Clang/MSVC)
  - Google Test integration for comprehensive unit testing
  - Google Benchmark integration for performance benchmarking
  - Doxygen documentation generation with GitHub Pages deployment
  - CPack packaging for distribution (DEB, RPM, ZIP, MSI)
  - Installation support with CMake package config

- **Documentation**

  - README with feature overview
  - Detailed API documentation with Doxygen comments
  - Sample application demonstrating library usage
  - Build and installation instructions

- **Testing & Benchmarking**
  - Unit test suite
  - Performance benchmarks for all operations
  - Cross-compiler performance validation
