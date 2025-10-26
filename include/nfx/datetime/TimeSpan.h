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
 * @file TimeSpan.h
 * @brief High-precision time interval representation with 100-nanosecond tick precision
 * @details Provides time duration operations with ISO 8601 duration format support,
 *          arithmetic operations, and conversions between various time units.
 *          Supports durations ranging from ±10,675,199 days (approximately ±29,247 years).
 *
 * @note This class represents time intervals/durations, not absolute points in time.
 *       For absolute time values, use DateTime or DateTimeOffset classes.
 *
 * @section iso8601_format ISO 8601 Duration Format
 *
 * @par TimeSpan uses a subset of ISO 8601 duration format for string representation:
 * @code
 * ┌──────────────────────────────────────────────────────────────────────┐
 * │              ISO 8601 Duration Format (TimeSpan Subset)              │
 * ├──────────────────────────────────────────────────────────────────────┤
 * │  [−]  P  [n]D  T  [n]H  [n]M  [n]S                                   │
 * │   │   │   │    │   │     │     │                                     │
 * │   │   │   │    │   │     │     └─ Seconds (with optional decimal)    │
 * │   │   │   │    │   │     └─────── Minutes                            │
 * │   │   │   │    │   └───────────── Hours                              │
 * │   │   │   │    └───────────────── Time designator (REQUIRED)         │
 * │   │   │   └────────────────────── Days                               │
 * │   │   └────────────────────────── Period designator (REQUIRED)       │
 * │   └────────────────────────────── Optional negative sign             │
 * │                                                                      │
 * │                                                                      │
 * │  Note: Years and months have variable durations (leap years,         │
 * │          different month lengths) and cannot be converted to fixed   │
 * │          tick values. Only fixed-duration components are supported.  │
 * └──────────────────────────────────────────────────────────────────────┘
 * @endcode
 *
 * @par Format Examples:
 * @code
 * ┌──────────────────┬───────────────────────────────────────────────┐
 * │   Input String   │                    Meaning                    │
 * ├──────────────────┼───────────────────────────────────────────────┤
 * │  PT1H            │  1 hour (compact format)                      │
 * │  PT1H0M0S        │  1 hour (verbose format - equivalent to PT1H) │
 * │  PT1H30M45S      │  1 hour, 30 minutes, 45 seconds               │
 * │  PT1.5S          │  1.5 seconds (fractional seconds allowed)     │
 * │  -PT2H           │  Negative 2 hours (duration in reverse)       │
 * │  P1D             │  1 day (date component only)                  │
 * │  P1DT12H30M      │  1 day, 12 hours, 30 minutes                  │
 * │  PT0S            │  Zero duration (minimum valid format)         │
 * └──────────────────┴───────────────────────────────────────────────┘
 * @endcode
 *
 * @par Key Rules:
 * - **P** (Period) is REQUIRED as the first character (after optional '-')
 * - **T** (Time) is REQUIRED if any time components (H/M/S) are present
 * - Components are optional but at least ONE must be present (D, H, M, or S)
 * - Compact format omits zero components (PT1H instead of PT1H0M0S)
 * - Fractional values allowed for seconds (PT1.5S = 1.5 seconds)
 * - Negative sign goes BEFORE the P (e.g., -PT1H for negative durations)
 *
 * @par Internal Representation:
 * @code
 * ┌───────────────────────────────────────────────────────────┐
 * │     TimeSpan stores durations as 100-nanosecond ticks     │
 * ├───────────────────────────────────────────────────────────┤
 * │  1 tick                = 100 nanoseconds                  │
 * │  10,000 ticks          = 1 millisecond                    │
 * │  10,000,000 ticks      = 1 second                         │
 * │  600,000,000 ticks     = 1 minute                         │
 * │  36,000,000,000 ticks  = 1 hour                           │
 * │  864,000,000,000 ticks = 1 day                            │
 * │                                                           │
 * │  Range: ±10,675,199 days (≈ ±29,247 years)                │
 * └───────────────────────────────────────────────────────────┘
 * @endcode
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace nfx::datetime
{
	//=====================================================================
	// TimeSpan class
	//=====================================================================

	/**
	 * @brief Represents a time interval in 100-nanosecond ticks
	 * @details Provides high-precision time duration representation and arithmetic operations.
	 *          Compatible with ISO 8601 duration format for parsing and formatting operations.
	 *
	 *          Key features:
	 *          - 100-nanosecond tick precision for maximum accuracy
	 *          - Range: ±10,675,199 days (approximately ±29,247 years)
	 *          - Arithmetic operations (addition, subtraction, comparison)
	 *          - ISO 8601 duration parsing and formatting (P[n]Y[n]M[n]DT[n]H[n]M[n]S)
	 *          - Factory methods for common time units (days, hours, minutes, seconds)
	 *          - Conversion methods to various time units with fractional support
	 *
	 *          This class represents a duration or elapsed time interval, not a specific
	 *          point in time. For absolute time values, use DateTime or DateTimeOffset.
	 *
	 * @note TimeSpan is designed for duration calculations and should not be used to
	 *       represent calendar dates or clock times. Use DateTime or DateTimeOffset instead.
	 */
	class TimeSpan final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Construct from ticks (100-nanosecond units)
		 * @param ticks Number of 100-nanosecond intervals for this TimeSpan (defaults to 0)
		 */
		explicit inline constexpr TimeSpan( std::int64_t ticks = 0 ) noexcept;

		/**
		 * @brief Construct from std::chrono duration
		 * @param duration System clock duration to convert from
		 */
		explicit inline TimeSpan( std::chrono::system_clock::duration duration ) noexcept;

		/** @brief Copy constructor */
		TimeSpan( const TimeSpan& ) = default;

		/** @brief Move constructor */
		TimeSpan( TimeSpan&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~TimeSpan() = default;

		//----------------------------------------------
		// Assignment
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 * @return Reference to this TimeSpan after assignment
		 */
		TimeSpan& operator=( const TimeSpan& ) = default;

		/**
		 * @brief Move assignment operator
		 * @return Reference to this TimeSpan after assignment
		 */
		TimeSpan& operator=( TimeSpan&& ) noexcept = default;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison
		 * @param other The TimeSpan to compare with
		 * @return true if both TimeSpans represent the same duration, false otherwise
		 */
		inline constexpr bool operator==( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Inequality comparison
		 * @param other The TimeSpan to compare with
		 * @return true if TimeSpans represent different durations, false otherwise
		 */
		inline constexpr bool operator!=( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Less than comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is shorter than other, false otherwise
		 */
		inline constexpr bool operator<( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Less than or equal comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is shorter than or equal to other, false otherwise
		 */
		inline constexpr bool operator<=( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Greater than comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is longer than other, false otherwise
		 */
		inline constexpr bool operator>( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Greater than or equal comparison
		 * @param other The TimeSpan to compare with
		 * @return true if this TimeSpan is longer than or equal to other, false otherwise
		 */
		inline constexpr bool operator>=( const TimeSpan& other ) const noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Add time durations
		 * @param other The TimeSpan to add to this TimeSpan
		 * @return New TimeSpan representing the sum of both durations
		 */
		inline constexpr TimeSpan operator+( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Subtract time durations
		 * @param other The TimeSpan to subtract from this TimeSpan
		 * @return New TimeSpan representing the difference between durations
		 */
		inline constexpr TimeSpan operator-( const TimeSpan& other ) const noexcept;

		/**
		 * @brief Negate time duration
		 * @return New TimeSpan representing the negated duration
		 */
		inline constexpr TimeSpan operator-() const noexcept;

		/**
		 * @brief Add time duration (in-place)
		 * @param other The TimeSpan to add to this TimeSpan
		 * @return Reference to this TimeSpan after adding the duration
		 */
		inline constexpr TimeSpan& operator+=( const TimeSpan& other ) noexcept;

		/**
		 * @brief Subtract time duration (in-place)
		 * @param other The TimeSpan to subtract from this TimeSpan
		 * @return Reference to this TimeSpan after subtracting the duration
		 */
		inline constexpr TimeSpan& operator-=( const TimeSpan& other ) noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get tick count
		 * @return The number of 100-nanosecond intervals in this TimeSpan
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline constexpr std::int64_t ticks() const noexcept;

		/**
		 * @brief Get total days
		 * @return The total number of days represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] double totalDays() const noexcept;

		/**
		 * @brief Get total hours
		 * @return The total number of hours represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] double totalHours() const noexcept;

		/**
		 * @brief Get total minutes
		 * @return The total number of minutes represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] double totalMinutes() const noexcept;

		/**
		 * @brief Get total seconds
		 * @return The total number of seconds represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] double totalSeconds() const noexcept;

		/**
		 * @brief Get total milliseconds
		 * @return The total number of milliseconds represented by this TimeSpan (fractional)
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] double totalMilliseconds() const noexcept;

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/**
		 * @brief Convert to ISO 8601 duration string
		 * @return String representation in ISO 8601 duration format (e.g., "PT1H30M45S")
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Create a TimeSpan from a number of days
		 * @param days The number of days for the TimeSpan duration
		 * @return TimeSpan representing the specified number of days
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan fromDays( double days ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of hours
		 * @param hours The number of hours for the TimeSpan duration
		 * @return TimeSpan representing the specified number of hours
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan fromHours( double hours ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of minutes
		 * @param minutes The number of minutes for the TimeSpan duration
		 * @return TimeSpan representing the specified number of minutes
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan fromMinutes( double minutes ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of seconds
		 * @param seconds The number of seconds for the TimeSpan duration
		 * @return TimeSpan representing the specified number of seconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan fromSeconds( double seconds ) noexcept;

		/**
		 * @brief Create a TimeSpan from a number of milliseconds
		 * @param milliseconds The number of milliseconds for the TimeSpan duration
		 * @return TimeSpan representing the specified number of milliseconds
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan fromMilliseconds( double milliseconds ) noexcept;

		//----------------------------------------------
		// String parsing
		//----------------------------------------------

		/**
		 * @brief Parse a TimeSpan from a string representation
		 * @details Supports the following formats:
		 *          - ISO 8601 duration subset: "PT1H30M45S", "P1DT2H", "-PT45S", "PT0.5S"
		 *          - Numeric seconds (convenience): "123.45", "-60" (interpreted as seconds)
		 *
		 *          Supported ISO 8601 components: Days (D), Hours (H), Minutes (M), Seconds (S)
		 *          NOT supported: Years (Y), Months (M in date part), Weeks (W)
		 *
		 * @param iso8601DurationString The string to parse
		 * @return TimeSpan parsed from the string
		 * @throws std::invalid_argument if the string format is invalid or contains unsupported components
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @par Valid ISO 8601 Examples:
		 * @code
		 * auto ts1{ TimeSpan::parse("PT1H30M45S") };  // ✓ 1 hour, 30 minutes, 45 seconds
		 * auto ts2{ TimeSpan::parse("P1DT12H") };     // ✓ 1 day, 12 hours
		 * auto ts3{ TimeSpan::parse("-PT2H") };       // ✓ Negative: -2 hours
		 * auto ts4{ TimeSpan::parse("PT0.5S") };      // ✓ Fractional: 0.5 seconds
		 * auto ts5{ TimeSpan::parse("123.45") };      // ✓ Convenience: 123.45 seconds
		 * @endcode
		 *
		 * @par Invalid Examples (will throw std::invalid_argument):
		 * @code
		 * TimeSpan::parse("P3Y");       // ✗ Years not supported (variable duration)
		 * TimeSpan::parse("P6M");       // ✗ Months not supported (variable duration)
		 * TimeSpan::parse("P2W");       // ✗ Weeks not supported
		 * TimeSpan::parse("P1Y2M3D");   // ✗ Contains unsupported Y and M components
		 * @endcode
		 */
		[[nodiscard]] static TimeSpan parse( std::string_view iso8601DurationString );

		/**
		 * @brief Try to parse a TimeSpan from a string without throwing exceptions
		 * @details Supports the following formats:
		 *          - ISO 8601 duration (standard): "PT1H30M45S", "P1DT2H", "-PT45S", "PT0.5S"
		 *          - Numeric seconds (convenience): "123.45", "-60" (interpreted as seconds)
		 * @param iso8601DurationString The string to parse
		 * @param result Reference to store the parsed TimeSpan if successful
		 * @return true if parsing succeeded, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 *
		 * @par Format Requirements
		 * ISO 8601 format must start with 'P' and contain at least one component (H, M, S, or D).
		 * Numeric format accepts only digits, decimal point, and optional leading minus sign.
		 * Empty strings and invalid formats return false.
		 */
		[[nodiscard]] static bool tryParse( std::string_view iso8601DurationString, TimeSpan& result ) noexcept;

		//----------------------------------------------
		// std::chrono interoperability
		//----------------------------------------------

		/**
		 * @brief Convert to std::chrono::system_clock::duration
		 * @details Converts this TimeSpan to a std::chrono duration type.
		 *          Values outside the representable range of std::chrono::system_clock::duration
		 *          will be clamped to the nearest representable value.
		 * @return A system_clock::duration representing this TimeSpan
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::chrono::system_clock::duration toChrono() const noexcept;

		/**
		 * @brief Create TimeSpan from std::chrono::system_clock::duration
		 * @details Converts a std::chrono duration to a TimeSpan.
		 *          Values outside the representable range of TimeSpan
		 *          will be clamped to the nearest valid TimeSpan value.
		 * @param duration The system_clock duration to convert
		 * @return A TimeSpan representing the same time duration
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static TimeSpan fromChrono( const std::chrono::system_clock::duration& duration ) noexcept;

	private:
		/** @brief 100-nanosecond ticks since January 1, 0001 UTC */
		std::int64_t m_ticks;
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator for TimeSpan (ISO 8601 duration format)
	 * @param os Output stream to write to
	 * @param timeSpan TimeSpan value to format and output
	 * @return Reference to the output stream for chaining
	 */
	std::ostream& operator<<( std::ostream& os, const TimeSpan& timeSpan );

	/**
	 * @brief Input stream operator for TimeSpan (ISO 8601 duration format)
	 * @param is Input stream to read from
	 * @param timeSpan TimeSpan reference to store parsed value
	 * @return Reference to the input stream for chaining
	 */
	std::istream& operator>>( std::istream& is, TimeSpan& timeSpan );
} // namespace nfx::datetime

#include "nfx/detail/datetime/TimeSpan.inl"
