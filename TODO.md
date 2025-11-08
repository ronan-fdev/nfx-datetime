# nfx-datetime TODO

Project roadmap and task tracking for the nfx-datetime library.

### Todo

- [ ] Add C++20 std::format support for DateTime, DateTimeOffset, and TimeSpan
  - [ ] Add custom format string support
- [ ] Enhance parse/tryParse methods
  - [ ] Add locale support for date/time parsing
  - [ ] Support additional date/time format patterns
  - [ ] Flexible whitespace handling in ISO 8601 parsing
- [ ] Add timezone database support
  - [ ] IANA timezone database integration (e.g., "America/New_York" → offset with DST rules)
- [ ] Performance optimizations
  - [ ] Optimize string parsing and formatting
  - [ ] Cache frequently used calculations (e.g., day-of-week lookup tables)

### v2.0.0 (Breaking changes)

- [ ] Consider API improvements based on user feedback
  - [ ] Evaluate constructor/factory method consistency (review task)
  - [ ] Review method naming conventions (review task)
  - [ ] Consider adding constexpr support where applicable
- [ ] Refactor parsing API for DateTime, DateTimeOffset and TimeSpan
  - [ ] Remove DateTime::parse() method (redundant with throwing constructor)
  - [ ] Remove DateTimeOffset::parse() method (redundant with throwing constructor)
  - [ ] Remove TimeSpan::parse() method (redundant with throwing constructor)
  - [ ] Rename DateTime::tryParse() → DateTime::fromString()
  - [ ] Rename DateTimeOffset::tryParse() → DateTimeOffset::fromString()
  - [ ] Rename TimeSpan::tryParse() → TimeSpan::fromString()
  - [ ] Add std::optional<T> fromString(string_view) overload (in addition to out-parameter)
  - [ ] Update all tests, samples, and documentation

### In Progress

- NIL

### Done ✓

#### v1.0.1 (2025-10-30)

- [x] Fixed CMake configuration bug (duplicate `NFX_DATETIME_INCLUDE_DIR` → `NFX_DATETIME_SOURCE_DIR`)
- [x] Refactored CMake directory variables (`NFX_DATETIME_ROOT_DIR` → `NFX_DATETIME_DIR`)
- [x] Enhanced GitHub release workflow with draft releases and checksums
