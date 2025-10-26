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
 * @file DateTimeOffset.h
 * @brief Timezone-aware DateTime class with UTC offset support
 * @details Provides timezone-aware datetime operations with 100-nanosecond precision,
 *          ISO 8601 parsing/formatting with timezone offsets, and cross-platform compatibility.
 *          Supports date range from January 1, 0001 to December 31, 9999 with offset ±14:00:00.
 *
 * @note This implementation includes timezone offset support. For UTC-only operations,
 *       use the DateTime class instead.
 *
 * @section iso8601_offset_format ISO 8601 DateTime with Timezone Offset
 *
 * @par DateTimeOffset uses ISO 8601 format with timezone offset for string representation:
 * @code
 * ┌───────────────────────────────────────────────────────────────────┐
 * │           ISO 8601 DateTime Format with Timezone Offset           │
 * ├───────────────────────────────────────────────────────────────────┤
 * │  YYYY-MM-DD  T  HH:MM:SS  [.fffffff]  ±HH:MM  or  Z               │
 * │   │   │  │   │   │  │  │       │        │  │      │               │
 * │   │   │  │   │   │  │  │       │        │  │      └── Zulu (UTC)  │
 * │   │   │  │   │   │  │  │       │        │  └───────── Minutes     │
 * │   │   │  │   │   │  │  │       │        └──────────── Hours       │
 * │   │   │  │   │   │  │  │       └───────────────────── Fraction    │
 * │   │   │  │   │   │  │  └───────────────────────────── Seconds     │
 * │   │   │  │   │   │  └──────────────────────────────── Minutes     │
 * │   │   │  │   │   └─────────────────────────────────── Hours       │
 * │   │   │  │   └─────────────────────────────────────── Separator   │
 * │   │   │  └─────────────────────────────────────────── Day         │
 * │   │   └────────────────────────────────────────────── Month       │
 * │   └────────────────────────────────────────────────── Year        │
 * └───────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Format Examples:
 * @code
 * ┌────────────────────────────────┬─────────────────────────────────┐
 * │          Input String          │             Meaning             │
 * ├────────────────────────────────┼─────────────────────────────────┤
 * │  2024-01-15T12:30:45Z          │  UTC time (Z = zero offset)     │
 * │  2024-01-15T12:30:45+02:00     │  Local: 12:30, UTC+2 (East)     │
 * │  2024-01-15T12:30:45-05:00     │  Local: 12:30, UTC-5 (West)     │
 * │  2024-01-15T12:30:45.123+01:00 │  With milliseconds, UTC+1       │
 * │  2024-06-20T08:15:00+05:30     │  India Standard Time (UTC+5:30) │
 * │  2024-12-25T00:00:00-08:00     │  Pacific Standard Time (UTC-8)  │
 * └────────────────────────────────┴─────────────────────────────────┘
 * @endcode
 *
 * @par Timezone Offset Rules:
 * - Valid range: **±14:00:00** (±840 minutes, ±50,400 seconds)
 * - Positive offset: East of UTC (e.g., +09:00 for Japan)
 * - Negative offset: West of UTC (e.g., -05:00 for US Eastern)
 * - **Z** notation: Represents UTC (zero offset), equivalent to +00:00
 * - Offset format: Always ±HH:MM in string representation
 *
 * @par Internal Composition:
 * @code
 * ┌───────────────────────────────────────────────────────────────────────┐
 * │              DateTimeOffset = DateTime + TimeSpan offset              │
 * ├───────────────────────────────────────────────────────────────────────┤
 * │  ┌─────────────────────────┐         ┌───────────────────────┐        │
 * │  │  DateTime (m_dateTime)  │         │  TimeSpan (m_offset)  │        │
 * │  │                         │         │                       │        │
 * │  │     Local date/time     │    +    │    Offset from UTC    │        │
 * │  │      (100-ns ticks)     │         │     (100-ns ticks)    │        │
 * │  └─────────────────────────┘         └───────────────────────┘        │
 * │                                                                       │
 * │  Example: 2024-01-15T14:30:00+02:00                                   │
 * │            ↓                    ↓                                     │
 * │     DateTime: 14:30 local   Offset: +2 hours                          │
 * │     UTC equivalent: 14:30 - 2:00 = 12:30 UTC                          │
 * │                                                                       │
 * │  Comparisons: Always performed on UTC values (m_dateTime - m_offset)  │
 * │  Arithmetic: Modifies DateTime, offset remains unchanged              │
 * └───────────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Key Differences from DateTime:
 * @code
 * ┌─────────────────┬──────────────────────┬────────────────────────────┐
 * │      Aspect     │       DateTime       │       DateTimeOffset       │
 * ├─────────────────┼──────────────────────┼────────────────────────────┤
 * │  Timezone       │  UTC only            │  Offset-aware (±14:00:00)  │
 * │  Storage        │  Single tick value   │  DateTime + offset         │
 * │  String format  │  ...Z (UTC only)     │  ...±HH:MM or Z            │
 * │  Comparisons    │  Direct tick compare │  Convert to UTC first      │
 * │  Use case       │  System timestamps   │  User-facing times         │
 * │  Size           │  8 bytes             │  16 bytes                  │
 * └─────────────────┴──────────────────────┴────────────────────────────┘
 * @endcode
 *
 * @par Common Timezone Offsets:
 * @code
 * ┌──────────────────────────┬──────────┬──────────────────────────┐
 * │         Timezone         │  Offset  │         Examples         │
 * ├──────────────────────────┼──────────┼──────────────────────────┤
 * │  UTC (GMT)               │  +00:00  │  London (winter)         │
 * │  Central European Time   │  +01:00  │  Paris, Berlin           │
 * │  Eastern European Time   │  +02:00  │  Athens, Helsinki        │
 * │  India Standard Time     │  +05:30  │  Mumbai, Delhi           │
 * │  China Standard Time     │  +08:00  │  Beijing, Shanghai       │
 * │  Japan Standard Time     │  +09:00  │  Tokyo, Osaka            │
 * │  Australian Eastern Time │  +10:00  │  Sydney, Melbourne       │
 * │  Eastern Standard Time   │  -05:00  │  New York, Toronto       │
 * │  Pacific Standard Time   │  -08:00  │  Los Angeles, Vancouver  │
 * │  Hawaii-Aleutian Time    │  -10:00  │  Honolulu                │
 * └──────────────────────────┴──────────┴──────────────────────────┘
 * @endcode
 */

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "DateTime.h"
#include "TimeSpan.h"

namespace nfx::datetime
{
	//=====================================================================
	// DateTimeOffset class
	//=====================================================================

	/**
	 * @brief Timezone-aware DateTime with UTC offset
	 * @details Represents a point in time, typically expressed as a date and time of day,
	 *          relative to Coordinated Universal Time (UTC). Composed of a DateTime value
	 *          and a TimeSpan offset that defines the difference from UTC.
	 *
	 *          Key features:
	 *          - 100-nanosecond tick precision for maximum accuracy
	 *          - Range: January 1, 0001 to December 31, 9999 with offset ±14:00:00
	 *          - ISO 8601 string parsing and formatting with timezone offset
	 *          - Cross-platform timezone-aware operations
	 *          - Arithmetic operations that account for timezone offsets
	 *          - Unix timestamp support with timezone awareness
	 */
	class DateTimeOffset final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/** @brief Default constructor (minimum DateTimeOffset value with zero offset) */
		DateTimeOffset() noexcept;

		/**
		 * @brief Construct from DateTime and offset
		 * @param dateTime The DateTime component (local time)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( const DateTime& dateTime, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from DateTime (assumes local timezone offset)
		 * @param dateTime The DateTime to use (local timezone offset will be determined automatically)
		 */
		explicit DateTimeOffset( const DateTime& dateTime ) noexcept;

		/**
		 * @brief Construct from tick count and offset
		 * @param ticks Number of 100-nanosecond intervals since January 1, 0001 UTC (local time)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int64_t ticks, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date components and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date and time components and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date and time components with milliseconds and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param millisecond Millisecond component (0-999)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Construct from date and time components with microseconds and offset (UTC)
		 * @param year Year component (1-9999)
		 * @param month Month component (1-12)
		 * @param day Day component (1-31)
		 * @param hour Hour component (0-23)
		 * @param minute Minute component (0-59)
		 * @param second Second component (0-59)
		 * @param millisecond Millisecond component (0-999)
		 * @param microsecond Microsecond component (0-999)
		 * @param offset The timezone offset from UTC
		 */
		DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
			std::int32_t millisecond, std::int32_t microsecond, const TimeSpan& offset ) noexcept;

		/**
		 * @brief Parse from ISO 8601 string with timezone offset
		 * @param iso8601String ISO 8601 formatted string with timezone offset to parse
		 */
		explicit DateTimeOffset( std::string_view iso8601String );

		/** @brief Copy constructor */
		DateTimeOffset( const DateTimeOffset& ) = default;

		/** @brief Move constructor */
		DateTimeOffset( DateTimeOffset&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DateTimeOffset() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this DateTimeOffset after assignment
		 */
		DateTimeOffset& operator=( const DateTimeOffset& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this DateTimeOffset after assignment
		 */
		DateTimeOffset& operator=( DateTimeOffset&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if both DateTimeOffsets represent the same UTC instant, false otherwise
		 */
		inline bool operator==( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Inequality comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if DateTimeOffsets represent different UTC instants, false otherwise
		 */
		inline bool operator!=( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Less than comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is earlier in UTC than other, false otherwise
		 */
		inline bool operator<( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is earlier than or equal to other in UTC, false otherwise
		 */
		inline bool operator<=( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Greater than comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is later in UTC than other, false otherwise
		 */
		inline bool operator>( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Greater than or equal comparison (compares UTC values)
		 * @param other The DateTimeOffset to compare with
		 * @return true if this DateTimeOffset is later than or equal to other in UTC, false otherwise
		 */
		inline bool operator>=( const DateTimeOffset& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Add time duration
		 * @param duration The TimeSpan to add to this DateTimeOffset
		 * @return New DateTimeOffset representing this DateTimeOffset plus the duration
		 */
		inline DateTimeOffset operator+( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Subtract time duration
		 * @param duration The TimeSpan to subtract from this DateTimeOffset
		 * @return New DateTimeOffset representing this DateTimeOffset minus the duration
		 */
		inline DateTimeOffset operator-( const TimeSpan& duration ) const noexcept;

		/**
		 * @brief Get time difference between DateTimeOffsets (in UTC)
		 * @param other The DateTimeOffset to subtract from this DateTimeOffset
		 * @return TimeSpan representing the difference (this - other) in UTC
		 */
		inline TimeSpan operator-( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Add time duration (in-place)
		 * @param duration The TimeSpan to add to this DateTimeOffset
		 * @return Reference to this DateTimeOffset after adding the duration
		 */
		inline DateTimeOffset& operator+=( const TimeSpan& duration ) noexcept;

		/**
		 * @brief Subtract time duration (in-place)
		 * @param duration The TimeSpan to subtract from this DateTimeOffset
		 * @return Reference to this DateTimeOffset after subtracting the duration
		 */
		inline DateTimeOffset& operator-=( const TimeSpan& duration ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get the DateTime component (local time)
		 * @return The DateTime component representing the local time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr const DateTime& dateTime() const noexcept;

		/**
		 * @brief Get the offset from UTC
		 * @return The TimeSpan representing the offset from UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr const TimeSpan& offset() const noexcept;

		/**
		 * @brief Get UTC DateTime equivalent
		 * @return The DateTime representing this time in UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTime utcDateTime() const noexcept;

		/**
		 * @brief Get local DateTime equivalent
		 * @return The DateTime representing this time in local timezone
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTime localDateTime() const noexcept;

		/**
		 * @brief Get tick count (100-nanosecond units of local time)
		 * @return The number of 100-nanosecond intervals representing the local time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr std::int64_t ticks() const noexcept;

		/**
		 * @brief Get UTC tick count
		 * @return The number of 100-nanosecond intervals representing the UTC time
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int64_t utcTicks() const noexcept;

		/**
		 * @brief Get year component (1-9999)
		 * @return The year component of this DateTimeOffset (1-9999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t year() const noexcept;

		/**
		 * @brief Get month component (1-12)
		 * @return The month component of this DateTimeOffset (1-12)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t month() const noexcept;

		/**
		 * @brief Get day component (1-31)
		 * @return The day component of this DateTimeOffset (1-31)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t day() const noexcept;

		/**
		 * @brief Get hour component (0-23)
		 * @return The hour component of this DateTimeOffset (0-23)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t hour() const noexcept;

		/**
		 * @brief Get minute component (0-59)
		 * @return The minute component of this DateTimeOffset (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t minute() const noexcept;

		/**
		 * @brief Get second component (0-59)
		 * @return The second component of this DateTimeOffset (0-59)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t second() const noexcept;

		/**
		 * @brief Get millisecond component (0-999)
		 * @return The millisecond component of this DateTimeOffset (0-999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t millisecond() const noexcept;

		/**
		 * @brief Get microsecond component (0-999)
		 * @return The microsecond component of this DateTimeOffset (0-999)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t microsecond() const noexcept;

		/**
		 * @brief Get nanosecond component (0-900, in hundreds)
		 * @return The nanosecond component of this DateTimeOffset in hundreds (0-900)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t nanosecond() const noexcept;

		/**
		 * @brief Get day of week (0=Sunday, 6=Saturday)
		 * @return The day of week as an integer (0=Sunday, 1=Monday, ..., 6=Saturday)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t dayOfWeek() const noexcept;

		/**
		 * @brief Get day of year (1-366)
		 * @return The day of year as an integer (1-366, where 366 occurs in leap years)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t dayOfYear() const noexcept;

		/**
		 * @brief Get offset in total minutes
		 * @return The total minutes offset from UTC (positive for East, negative for West)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t totalOffsetMinutes() const noexcept;

		//----------------------------------------------
		// Conversion methods
		//----------------------------------------------

		/**
		 * @brief Convert to Unix timestamp (seconds since epoch)
		 * @return Number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int64_t toUnixSeconds() const noexcept;

		/**
		 * @brief Convert to Unix timestamp (milliseconds since epoch)
		 * @return Number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int64_t toUnixMilliseconds() const noexcept;

		/**
		 * @brief Get date component (time set to 00:00:00)
		 * @return DateTimeOffset with the same date but time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset date() const noexcept;

		/**
		 * @brief Get time of day as duration since midnight
		 * @return TimeSpan representing the elapsed time since midnight (00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline TimeSpan timeOfDay() const noexcept;

		/**
		 * @brief Convert to specified offset
		 * @param newOffset The new timezone offset to convert to
		 * @return DateTimeOffset representing the same instant in time with the specified offset
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset toOffset( const TimeSpan& newOffset ) const noexcept;

		/**
		 * @brief Convert to UTC (offset = 00:00:00)
		 * @return DateTimeOffset representing the same instant in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset toUniversalTime() const noexcept;

		/**
		 * @brief Convert to local time (system timezone)
		 * @return DateTimeOffset representing the same instant in local timezone
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset toLocalTime() const noexcept;

		/**
		 * @brief Convert to Windows FILETIME format
		 * @details Works on any platform - performs mathematical epoch conversion only.
		 *          Useful for interoperability with Windows-originated data on any system.
		 * @return 100-nanosecond intervals since January 1, 1601 UTC
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::int64_t toFILETIME() const noexcept;

		//----------------------------------------------
		// Arithmetic methods
		//----------------------------------------------

		/**
		 * @brief Add time span
		 * @param value The TimeSpan to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the time span
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline DateTimeOffset add( const TimeSpan& value ) const noexcept;

		/**
		 * @brief Add days
		 * @param days The number of days to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified days
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addDays( double days ) const noexcept;

		/**
		 * @brief Add hours
		 * @param hours The number of hours to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified hours
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addHours( double hours ) const noexcept;

		/**
		 * @brief Add milliseconds
		 * @param milliseconds The number of milliseconds to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified milliseconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addMilliseconds( double milliseconds ) const noexcept;

		/**
		 * @brief Add minutes
		 * @param minutes The number of minutes to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified minutes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addMinutes( double minutes ) const noexcept;

		/**
		 * @brief Add months
		 * @param months The number of months to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified months
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addMonths( std::int32_t months ) const noexcept;

		/**
		 * @brief Add seconds
		 * @param seconds The number of seconds to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified seconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addSeconds( double seconds ) const noexcept;

		/**
		 * @brief Add ticks
		 * @param ticks The number of 100-nanosecond ticks to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified ticks
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline DateTimeOffset addTicks( std::int64_t ticks ) const noexcept;

		/**
		 * @brief Add years
		 * @param years The number of years to add to this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset plus the specified years
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DateTimeOffset addYears( std::int32_t years ) const noexcept;

		/**
		 * @brief Subtract DateTimeOffset and return TimeSpan
		 * @param value The DateTimeOffset to subtract from this DateTimeOffset
		 * @return TimeSpan representing the difference between the two DateTimeOffsets
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline TimeSpan subtract( const DateTimeOffset& value ) const noexcept;

		/**
		 * @brief Subtract TimeSpan and return DateTimeOffset
		 * @param value The TimeSpan to subtract from this DateTimeOffset
		 * @return DateTimeOffset representing this DateTimeOffset minus the time span
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline DateTimeOffset subtract( const TimeSpan& value ) const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/**
		 * @brief Convert to ISO 8601 string with offset
		 * @return String representation in ISO 8601 format with timezone offset (e.g., "2024-01-01T12:00:00+02:00")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Convert to string using specified format
		 * @param format The format to use for string conversion
		 * @return String representation using the specified format
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString( DateTime::Format format ) const;

		/**
		 * @brief Convert to ISO 8601 extended format with full precision and offset
		 * @return String representation in ISO 8601 extended format with fractional seconds and offset (e.g., "2024-01-01T12:00:00.1234567+02:00")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toIso8601Extended() const;

		//----------------------------------------------
		// Comparison methods
		//----------------------------------------------

		/**
		 * @brief Compare to another DateTimeOffset
		 * @param other The DateTimeOffset to compare with
		 * @return Negative value if this is earlier, zero if equal, positive if this is later
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::int32_t compareTo( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Check if this DateTimeOffset equals another (same UTC time)
		 * @param other The DateTimeOffset to compare with
		 * @return true if both DateTimeOffsets represent the same UTC time, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equals( const DateTimeOffset& other ) const noexcept;

		/**
		 * @brief Check if this DateTimeOffset equals another exactly (same local time and offset)
		 * @param other The DateTimeOffset to compare with
		 * @return true if both DateTimeOffsets have the same local time and offset, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool equalsExact( const DateTimeOffset& other ) const noexcept;

		//----------------------------------------------
		// Validation methods
		//----------------------------------------------

		/**
		 * @brief Check if this DateTimeOffset is valid
		 * @return true if this DateTimeOffset represents a valid date and time with valid offset, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] bool isValid() const noexcept;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Get current local time with system timezone offset
		 * @return DateTimeOffset representing the current local date and time with system timezone offset
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset now() noexcept;

		/**
		 * @brief Get current UTC time (offset = 00:00:00)
		 * @return DateTimeOffset representing the current UTC date and time (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset utcNow() noexcept;

		/**
		 * @brief Get current local date (time set to 00:00:00)
		 * @return DateTimeOffset representing the current local date with time set to 00:00:00
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset today() noexcept;

		/**
		 * @brief Get minimum DateTimeOffset value
		 * @return DateTimeOffset representing the minimum representable date (January 1, 0001 00:00:00.0000000 with zero offset)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset minValue() noexcept;

		/**
		 * @brief Get maximum DateTimeOffset value
		 * @return DateTimeOffset representing the maximum representable date (December 31, 9999 23:59:59.9999999 with zero offset)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset maxValue() noexcept;

		/**
		 * @brief Get Unix epoch DateTimeOffset (January 1, 1970 00:00:00 UTC)
		 * @return DateTimeOffset representing the Unix epoch (January 1, 1970 00:00:00 UTC with zero offset)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset unixEpoch() noexcept;

		/**
		 * @brief Compare two DateTimeOffset values
		 * @param left The first DateTimeOffset to compare
		 * @param right The second DateTimeOffset to compare
		 * @return Negative value if left is earlier, zero if equal, positive if left is later
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline static std::int32_t compare( const DateTimeOffset& left, const DateTimeOffset& right ) noexcept;

		/**
		 * @brief Parse ISO 8601 string with timezone offset
		 * @param iso8601String The ISO 8601 formatted string with timezone offset to parse
		 * @return DateTimeOffset parsed from the ISO 8601 string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset parse( std::string_view iso8601String );

		/**
		 * @brief Try parse ISO 8601 string without throwing
		 * @param iso8601String The ISO 8601 formatted string with timezone offset to parse
		 * @param result Reference to store the parsed DateTimeOffset if successful
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static bool tryParse( std::string_view iso8601String, DateTimeOffset& result ) noexcept;

		/**
		 * @brief Create from Unix timestamp seconds with UTC offset
		 * @param seconds The number of seconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTimeOffset representing the specified Unix timestamp in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset fromUnixTimeSeconds( std::int64_t seconds ) noexcept;

		/**
		 * @brief Create from Unix timestamp milliseconds with UTC offset
		 * @param milliseconds The number of milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
		 * @return DateTimeOffset representing the specified Unix timestamp in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset fromUnixTimeMilliseconds( std::int64_t milliseconds ) noexcept;

		/**
		 * @brief Create DateTimeOffset from Windows FILETIME format
		 * @details Works on any platform - performs mathematical epoch conversion only.
		 *          Useful for processing Windows-originated data on any system.
		 * @param FILETIME 100-nanosecond intervals since January 1, 1601 UTC
		 * @return DateTimeOffset representing the same instant in UTC (offset = 00:00:00)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DateTimeOffset fromFILETIME( std::int64_t FILETIME ) noexcept;

	private:
		/** @brief Local date and time */
		DateTime m_dateTime;

		/** @brief Offset from UTC */
		TimeSpan m_offset;
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator (ISO 8601 format with offset)
	 * @param os Output stream to write to
	 * @param dateTimeOffset DateTimeOffset value to format and output
	 * @return Reference to the output stream for chaining
	 */
	std::ostream& operator<<( std::ostream& os, const DateTimeOffset& dateTimeOffset );

	/**
	 * @brief Input stream operator (ISO 8601 format with offset)
	 * @param is Input stream to read from
	 * @param dateTimeOffset DateTimeOffset reference to store parsed value
	 * @return Reference to the input stream for chaining
	 */
	std::istream& operator>>( std::istream& is, DateTimeOffset& dateTimeOffset );
} // namespace nfx::datetime

#include "nfx/detail/datetime/DateTimeOffset.inl"
