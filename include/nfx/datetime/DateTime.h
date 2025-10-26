/*
 * MIT License
 *
 * Copyright (c) 2025 nfx
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file DateTime.h
 * @brief Cross-platform UTC DateTime class with ISO 8601 support
 * @details Provides UTC-only datetime operations with 100-nanosecond precision,
 *          ISO 8601 parsing and formatting, and cross-platform compatibility.
 *          Supports date range from January 1, 0001 to December 31, 9999.
 *
 * @note This implementation is UTC-only. For timezone-aware operations,
 *       use the DateTimeOffset class instead.
 *
 * @section iso8601_datetime_format ISO 8601 DateTime Format (UTC)
 *
 * @par DateTime uses ISO 8601 format with UTC timezone for string representation:
 * @code
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │                 ISO 8601 DateTime Format (UTC Only)                 │
 * ├─────────────────────────────────────────────────────────────────────┤
 * │  YYYY-MM-DD  T  HH:MM:SS  [.fffffff]  Z                             │
 * │   │  │  │   │   │  │  │      │        │                             │
 * │   │  │  │   │   │  │  │      │        └─ Zulu (UTC indicator)       │
 * │   │  │  │   │   │  │  │      └────────── Fraction (optional)        │
 * │   │  │  │   │   │  │  └───────────────── Seconds (00-59)            │
 * │   │  │  │   │   │  └──────────────────── Minutes (00-59)            │
 * │   │  │  │   │   └─────────────────────── Hours (00-23)              │
 * │   │  │  │   └─────────────────────────── Time separator             │
 * │   │  │  └─────────────────────────────── Day (01-31)                │
 * │   │  └────────────────────────────────── Month (01-12)              │
 * │   └───────────────────────────────────── Year (0001-9999)           │
 * │                                                                     │
 * │  Note: All DateTime values represent UTC time. The 'Z' suffix       │
 * │        (Zulu time) explicitly indicates this is UTC with no offset. │
 * └─────────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Format Examples:
 * @code
 * ┌────────────────────────────────┬───────────────────────────────────┐
 * │          Input String          │              Meaning              │
 * ├────────────────────────────────┼───────────────────────────────────┤
 * │  2024-01-15T12:30:45Z          │  Jan 15, 2024, 12:30:45 UTC       │
 * │  2024-06-20T00:00:00Z          │  Jun 20, 2024, midnight UTC       │
 * │  2024-12-31T23:59:59Z          │  Dec 31, 2024, 23:59:59 UTC       │
 * │  2024-01-15T12:30:45.123Z      │  With milliseconds (123 ms)       │
 * │  2024-01-15T12:30:45.123456Z   │  With microseconds (123.456 ms)   │
 * │  2024-01-15T12:30:45.1234567Z  │  Full precision (100 ns ticks)    │
 * │  1970-01-01T00:00:00Z          │  Unix epoch (start of Unix time)  │
 * │  0001-01-01T00:00:00Z          │  Minimum DateTime value           │
 * │  9999-12-31T23:59:59.9999999Z  │  Maximum DateTime value           │
 * └────────────────────────────────┴───────────────────────────────────┘
 * @endcode
 *
 * @par Date-Only Format:
 * @code
 * ┌──────────────┬────────────────────────────────────────────────┐
 * │  YYYY-MM-DD  │  Date without time (time implicitly 00:00:00)  │
 * ├──────────────┼────────────────────────────────────────────────┤
 * │  2024-01-15  │  January 15, 2024 at midnight UTC              │
 * │  2024-06-20  │  June 20, 2024 at midnight UTC                 │
 * └──────────────┴────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Internal Representation:
 * @code
 * ┌──────────────────────────────────────────────────────────────────────┐
 * │        DateTime stores absolute time as 100-nanosecond ticks         │
 * ├──────────────────────────────────────────────────────────────────────┤
 * │                                                                      │
 * │  Tick = 100 nanoseconds (0.0000001 seconds)                          │
 * │                                                                      │
 * │   ┌─────────────────────────────────────────────────────────────┐    │
 * │   │                  Tick Conversion Reference                  │    │
 * │   ├─────────────────────────────────────────────────────────────┤    │
 * │   │  1 tick                = 100 nanoseconds                    │    │
 * │   │  10 ticks              = 1 microsecond                      │    │
 * │   │  10,000 ticks          = 1 millisecond                      │    │
 * │   │  10,000,000 ticks      = 1 second                           │    │
 * │   │  600,000,000 ticks     = 1 minute                           │    │
 * │   │  36,000,000,000 ticks  = 1 hour                             │    │
 * │   │  864,000,000,000 ticks = 1 day                              │    │
 * │   └─────────────────────────────────────────────────────────────┘    │
 * │                                                                      │
 * │  Epoch: Ticks are counted from January 1, 0001 00:00:00.0000000 UTC  │
 * │                                                                      │
 * │  Range: 0001-01-01T00:00:00.0000000Z to 9999-12-31T23:59:59.9999999Z │
 * │         (0 ticks to 3,155,378,975,999,999,999 ticks)                 │
 * │                                                                      │
 * │  Storage: Single int64_t (8 bytes) - compact and efficient           │
 * └──────────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Key Characteristics:
 * @code
 * ┌──────────────────┬───────────────────────────────────────────────┐
 * │     Property     │                  Description                  │
 * ├──────────────────┼───────────────────────────────────────────────┤
 * │  Timezone        │  UTC only (no offset information)             │
 * │  Precision       │  100 nanoseconds (10 million ticks/second)    │
 * │  Range           │  Year 1 to Year 9999 (Gregorian calendar)     │
 * │  Storage size    │  8 bytes (single int64_t)                     │
 * │  String format   │  ISO 8601 with 'Z' suffix (UTC)               │
 * │  Leap years      │  Fully supported (Feb 29 in leap years)       │
 * │  Leap seconds    │  Not supported (follows standard time)        │
 * │  Calendar        │  Proleptic Gregorian (extended before 1582)   │
 * │  Use case        │  System logs, timestamps, UTC-only operations │
 * └──────────────────┴───────────────────────────────────────────────┘
 * @endcode
 *
 * @par DateTime vs DateTimeOffset:
 * @code
 * ┌───────────────────┬─────────────────────┬─────────────────────────────┐
 * │      Feature      │      DateTime       │       DateTimeOffset        │
 * ├───────────────────┼─────────────────────┼─────────────────────────────┤
 * │  Timezone info    │  None (UTC assumed) │  Offset from UTC            │
 * │  Storage          │  8 bytes            │  16 bytes (DateTime+offset) │
 * │  String format    │  ...Z               │  ...±HH:MM or Z             │
 * │  Local time       │  Not supported      │  Supported                  │
 * │  Comparisons      │  Direct             │  Via UTC conversion         │
 * │  Use for          │  Server/DB times    │  User-facing times          │
 * │  Example          │  Log timestamps     │  Appointment scheduling     │
 * └───────────────────┴─────────────────────┴─────────────────────────────┘
 * @endcode
 *
 * @par Common DateTime Operations:
 * @code
 * ┌──────────────────────────────────────────────────────────────────────┐
 * │                            Operation Examples                        │
 * ├──────────────────────────────────────────────────────────────────────┤
 * │  // Current UTC time                                                 │
 * │  DateTime now{ DateTime::now() };                                    │
 * │                                                                      │
 * │  // Create specific date/time                                        │
 * │  DateTime dt{ 2024, 6, 15, 14, 30, 0 };  // Jun 15, 2024 14:30 UTC   │
 * │                                                                      │
 * │  // Parse from string                                                │
 * │  DateTime parsed{ DateTime::parse("2024-06-15T14:30:00Z") };         │
 * │                                                                      │
 * │  // Arithmetic                                                       │
 * │  DateTime later{ dt + TimeSpan::fromHours(2.5) };                    │
 * │  TimeSpan duration = later - dt;                                     │
 * │                                                                      │
 * │  // Unix timestamp conversion                                        │
 * │  int64_t unixTime = dt.toUnixSeconds();                              │
 * │  DateTime fromUnix{ DateTime::sinceEpochSeconds(unixTime) };         │
 * │                                                                      │
 * │  // Format output                                                    │
 * │  std::string basic{ dt.toString() };             // Basic ISO 8601   │
 * │  std::string precise{ dt.toIso8601Extended() };  // With fractions   │
 * └──────────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Important Notes:
 * - **Always UTC**: DateTime has no timezone offset; all values are UTC
 * - **No Daylight Saving**: UTC does not observe DST (constant offset from solar time)
 * - **Leap Years**: Automatically handled (Feb 29 exists in 2024, 2028, etc.)
 * - **No Leap Seconds**: Follows standard civil time (ignores leap seconds)
 * - **std::chrono Integration**: Convertible to/from std::chrono::system_clock::time_point
 * - **Thread-Safe**: All operations are thread-safe (immutable value type)
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

#include "TimeSpan.h"

namespace nfx::datetime
{
	/**
	 * @brief Cross-platform UTC DateTime type with 100-nanosecond precision
	 * @details Implements UTC-only datetime operations with:
	 *          - 100-nanosecond tick precision (10 million ticks per second)
	 *          - Range: January 1, 0001 to December 31, 9999 UTC
	 *          - ISO 8601 string parsing and formatting
	 *          - System clock interoperability
	 *          - Arithmetic operations with time intervals
	 *
	 *          Note: This is UTC-only. For timezone-aware operations, use DateTimeOffset.
	 */
	class DateTime final
	{
	public:
		//----------------------------------------------
		// Enumerations
		//----------------------------------------------

		/**
		 * @brief DateTime string format options
		 * @details Provides type-safe format selection with self-documenting format names
		 */
		enum class Format : std::uint8_t
		{
			/** @brief ISO 8601 basic format: "2024-01-01T12:00:00Z" */
			Iso8601Basic,

			/** @brief ISO 8601 extended format with fractional seconds: "2024-01-01T12:00:00.1234567Z" */
			Iso8601Extended,

			/** @brief Date and time with timezone: "2024-01-01T12:00:00+02:00" */
			Iso8601WithOffset,

			/** @brief Date only format: "2024-01-01" */
			DateOnly,

			/** @brief Time only: "12:00:00" */
			TimeOnly,

			/** @brief Unix timestamp format: "1704110400" (seconds since epoch) */
			UnixSeconds,

			/** @brief Unix timestamp with milliseconds: "1704110400123" */
			UnixMilliseconds,
		};

		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor (minimum DateTime value) */
		DateTime() noexcept;

		/**
		 * @brief Construct from tick count (100-nanosecond units since year 1)
		 * @param ticks Number of 100-nanosecond intervals since January 1, 0001 UTC
		 */
		explicit DateTime( std::int64_t ticks ) noexcept;

		/**
		 * @brief Construct from system clock time point
		 * @param timePoint System clock time point to convert from
		 */
		explicit DateTime( std::chrono::system_clock::time_point timePoint ) noexcept;

		/**
		 * @brief Construct from date components (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept;

		/**
		 * @brief Construct from date and time components (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
			std::int32_t hour, std::int32_t minute, std::int32_t second ) noexcept;

		/**
		 * @brief Construct from date and time components with milliseconds (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param millisecond Millisecond component (0-999)
		 */
		DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
			std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond ) noexcept;

		/**
		 * @brief Parse from ISO 8601 string
		 * @param iso8601String ISO 8601 formatted string to parse
		 */
		explicit DateTime( std::string_view iso8601String );

		/** @brief Copy constructor */
		DateTime( const DateTime& ) = default;

		/** @brief Move constructor */
		DateTime( DateTime&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DateTime() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this DateTime after assignment
		 */
		DateTime& operator=( const DateTime& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this DateTime after assignment
		 */
		DateTime& operator=( DateTime&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison
		 * @param other The DateTime to compare with
		 * @return true if both DateTimes represent the same instant, false otherwise
		 */
		bool operator==( const DateTime& other ) const noexcept;

		/**
		 * @brief Inequality comparison
		 * @param other The DateTime to compare with
		 * @return true if DateTimes represent different instants, false otherwise
		 */
		bool operator!=( const DateTime& other ) const noexcept;

		/**
		 * @brief Less than comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is earlier than other, false otherwise
		 */
		bool operator<( const DateTime& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is earlier than or equal to other, false otherwise
		 */
		bool operator<=( const DateTime& other ) const noexcept;

		/**
		 * @brief Greater than comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is later than other, false otherwise
		 */
		bool operator>( const DateTime& other ) const noexcept;

		/**
		 * @brief Greater than or equal comparison
		 * @param other The DateTime to compare with
		 * @return true if this DateTime is later than or equal to other, false otherwise
		 */
		bool operator>=( const DateTime& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Add time duration
		 * @param duration The TimeSpan to add to this DateTime
		 * @return New DateTime representing this DateTime plus the duration
		 */
		DateTime operator+( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Subtract time duration
		 * @param duration The TimeSpan to subtract from this DateTime
		 * @return New DateTime representing this DateTime minus the duration
		 */
		DateTime operator-( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Get time difference between DateTimes
		 * @param other The DateTime to subtract from this DateTime
		 * @return TimeSpan representing the difference (this - other)
		 */
		TimeSpan operator-( const DateTime& other ) const noexcept;

		/**
		 * @brief Add time duration (in-place)
		 * @param duration The TimeSpan to add to this DateTime
		 * @return Reference to this DateTime after adding the duration
		 */
		DateTime& operator+=( const TimeSpan& duration ) noexcept;

		/**
		 * @brief Subtract time duration (in-place)
		 * @param duration The TimeSpan to subtract from this DateTime
		 * @return Reference to this DateTime after subtracting the duration
		 */
		DateTime& operator-=( const TimeSpan& duration ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get tick count (100-nanosecond units since year 1)
		 * @return The number of 100-nanosecond intervals since January 1, 0001 UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] constexpr std::int64_t ticks() const noexcept;

		/**
		 * @brief Get year component (1-9999)
		 * @return The year component of this DateTime (1-9999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t year() const noexcept;

		/**
		 * @brief Get month component (1-12)
		 * @return The month component of this DateTime (1-12)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t month() const noexcept;

		/**
		 * @brief Get day component (1-31)
		 * @return The day component of this DateTime (1-31)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t day() const noexcept;

		/**
		 * @brief Get hour component (0-23)
		 * @return The hour component of this DateTime (0-23)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t hour() const noexcept;

		/**
		 * @brief Get minute component (0-59)
		 * @return The minute component of this DateTime (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t minute() const noexcept;

		/**
		 * @brief Get second component (0-59)
		 * @return The second component of this DateTime (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t second() const noexcept;

		/**
		 * @brief Get millisecond component (0-999)
		 * @return The millisecond component of this DateTime (0-999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t millisecond() const noexcept;

		/**
		 * @brief Get day of week (0=Sunday, 6=Saturday)
		 * @return The day of week as an integer (0=Sunday, 1=Monday, ..., 6=Saturday)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t dayOfWeek() const noexcept;

		/**
		 * @brief Get day of year (1-366)
		 * @return The day of year as an integer (1-366, where 366 occurs in leap years)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int32_t dayOfYear() const noexcept;

		//----------------------------------------------
		// Conversion methods
		//----------------------------------------------

		/**
		 * @brief Convert to Unix timestamp (seconds since epoch)
		 * @return Number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int64_t toUnixSeconds() const noexcept;

		/**
		 * @brief Convert to Unix timestamp (milliseconds since epoch)
		 * @return Number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int64_t toUnixMilliseconds() const noexcept;

		/**
		 * @brief Get date component (time set to 00:00:00)
		 * @return New DateTime with the same date but time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTime date() const noexcept;

		/**
		 * @brief Get time of day as duration since midnight
		 * @return TimeSpan representing the elapsed time since midnight (00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] TimeSpan timeOfDay() const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/**
		 * @brief Convert to ISO 8601 string (basic format)
		 * @return String representation in ISO 8601 basic format (e.g., "2024-01-01T12:00:00Z")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Convert to string using specified format
		 * @param format The format to use for string conversion
		 * @return String representation using the specified format
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString( Format format ) const;

		/**
		 * @brief Convert to ISO 8601 extended format with full precision
		 * @return String representation in ISO 8601 extended format with fractional seconds (e.g., "2024-01-01T12:00:00.1234567Z")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toIso8601Extended() const;

		//----------------------------------------------
		// Validation methods
		//----------------------------------------------

		/**
		 * @brief Check if this DateTime is valid
		 * @return true if this DateTime represents a valid date and time, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isValid() const noexcept;

		/**
		 * @brief Check if given year is a leap year
		 * @param year The year to check for leap year status
		 * @return true if the year is a leap year, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr bool isLeapYear( std::int32_t year ) noexcept;

		/**
		 * @brief Get days in month for given year and month
		 * @param year The year to check (1-9999)
		 * @param month The month to check (1-12)
		 * @return Number of days in the specified month (28-31)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static constexpr std::int32_t daysInMonth( std::int32_t year, std::int32_t month ) noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Get current UTC time
		 * @return DateTime representing the current UTC date and time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime now() noexcept;

		/**
		 * @brief Get current UTC date (time set to 00:00:00)
		 * @return DateTime representing the current UTC date with time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime today() noexcept;

		/**
		 * @brief Get minimum DateTime value
		 * @return DateTime representing the minimum representable date (January 1, 0001 00:00:00.0000000 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime minValue() noexcept;

		/**
		 * @brief Get maximum DateTime value
		 * @return DateTime representing the maximum representable date (December 31, 9999 23:59:59.9999999 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime maxValue() noexcept;

		/**
		 * @brief Get Unix epoch DateTime (January 1, 1970 00:00:00 UTC)
		 * @return DateTime representing the Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime epoch() noexcept;

		/**
		 * @brief Parse ISO 8601 string
		 * @param iso8601String The ISO 8601 formatted string to parse
		 * @return DateTime parsed from the ISO 8601 string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime parse( std::string_view iso8601String );

		/**
		 * @brief Try parse ISO 8601 string without throwing
		 * @param iso8601String The ISO 8601 formatted string to parse
		 * @param result Reference to store the parsed DateTime if successful
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view iso8601String, DateTime& result ) noexcept;

		/**
		 * @brief Create from Unix timestamp (seconds since epoch)
		 * @param seconds The number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTime representing the specified Unix timestamp
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime sinceEpochSeconds( std::int64_t seconds ) noexcept;

		/**
		 * @brief Create from Unix timestamp (milliseconds since epoch)
		 * @param milliseconds The number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTime representing the specified Unix timestamp in milliseconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime sinceEpochMilliseconds( std::int64_t milliseconds ) noexcept;

		//----------------------------------------------
		// std::chrono interoperability
		//----------------------------------------------

		/**
		 * @brief Convert to std::chrono::system_clock::time_point
		 * @details Values outside the representable range of std::chrono::system_clock
		 *          (approximately years 1677-2262 on 64-bit Linux systems) will be
		 *          clamped to the nearest representable value. This means extreme dates
		 *          (near year 1 or year 9999) cannot round-trip through chrono.
		 * @return A system_clock::time_point representing this DateTime
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::chrono::system_clock::time_point toChrono() const noexcept;

		/**
		 * @brief Create DateTime from std::chrono::system_clock::time_point
		 * @details Values outside the representable range of DateTime
		 *          will be clamped to the nearest valid DateTime value.
		 * @param timePoint The system_clock time point to convert
		 * @return A DateTime representing the same instant in time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTime fromChrono( const std::chrono::system_clock::time_point& timePoint ) noexcept;

	private:
		/** @brief 100-nanosecond ticks since January 1, 0001 UTC */
		std::int64_t m_ticks;
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator (ISO 8601 format)
	 * @param os Output stream to write to
	 * @param dateTime DateTime value to format and output
	 * @return Reference to the output stream for chaining
	 */
	std::ostream& operator<<( std::ostream& os, const DateTime& dateTime );

	/**
	 * @brief Input stream operator (ISO 8601 format)
	 * @param is Input stream to read from
	 * @param dateTime DateTime reference to store parsed value
	 * @return Reference to the input stream for chaining
	 */
	std::istream& operator>>( std::istream& is, DateTime& dateTime );
} // namespace nfx::datetime

#include "nfx/detail/datetime/DateTime.inl"
