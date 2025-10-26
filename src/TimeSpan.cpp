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
 * @file TimeSpan.cpp
 * @brief Implementation of TimeSpan class methods for time duration operations
 * @details Provides parsing logic for ISO 8601 duration format, H:M:S time format,
 *          and numeric seconds format. Implements string conversion and formatting
 *          functionality for time intervals with 100-nanosecond precision.
 */

#include <charconv>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "nfx/datetime/TimeSpan.h"
#include "Constants.h"

namespace nfx::datetime
{
	//=====================================================================
	// TimeSpan class
	//=====================================================================

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	double TimeSpan::totalDays() const noexcept
	{
		return static_cast<double>( m_ticks ) / datetime::constants::TICKS_PER_DAY;
	}

	double TimeSpan::totalHours() const noexcept
	{
		return static_cast<double>( m_ticks ) / datetime::constants::TICKS_PER_HOUR;
	}

	double TimeSpan::totalMinutes() const noexcept
	{
		return static_cast<double>( m_ticks ) / datetime::constants::TICKS_PER_MINUTE;
	}

	double TimeSpan::totalSeconds() const noexcept
	{
		return static_cast<double>( m_ticks ) / datetime::constants::TICKS_PER_SECOND;
	}

	double TimeSpan::totalMilliseconds() const noexcept
	{
		return static_cast<double>( m_ticks ) / datetime::constants::TICKS_PER_MILLISECOND;
	}

	//----------------------------------------------
	// String formatting
	//----------------------------------------------

	std::string TimeSpan::toString() const
	{
		std::ostringstream oss;

		// Handle negative durations
		bool isNegative{ m_ticks < 0 };
		if ( isNegative )
		{
			oss << "-";
		}

		oss << "P";

		std::int64_t absTicks{ std::abs( m_ticks ) };
		std::int64_t totalSeconds{ absTicks / 10000000 };

		// Break down into days, hours, minutes, seconds, milliseconds
		std::int32_t days{ static_cast<std::int32_t>( totalSeconds / 86400 ) };
		std::int32_t remainingSeconds{ static_cast<std::int32_t>( totalSeconds % 86400 ) };
		std::int32_t hours{ remainingSeconds / 3600 };
		std::int32_t minutes{ ( remainingSeconds % 3600 ) / 60 };
		std::int32_t seconds{ remainingSeconds % 60 };
		std::int32_t milliseconds{ static_cast<std::int32_t>( ( absTicks / 10000 ) % 1000 ) };

		// Output days if present
		if ( days > 0 )
		{
			oss << days << "D";
		}

		// Time component
		oss << "T";

		bool hasTimeComponent{ false };

		if ( hours > 0 )
		{
			oss << hours << "H";
			hasTimeComponent = true;
		}

		if ( minutes > 0 )
		{
			oss << minutes << "M";
			hasTimeComponent = true;
		}

		// Include seconds (with or without milliseconds), or if nothing else, show "0S"
		if ( seconds > 0 || milliseconds > 0 || !hasTimeComponent )
		{
			if ( milliseconds > 0 )
			{
				// Format with fractional seconds: "1.500S"
				oss << seconds << "." << std::setfill( '0' ) << std::setw( 3 ) << milliseconds << "S";
			}
			else
			{
				oss << seconds << "S";
			}
		}

		return oss.str();
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	TimeSpan TimeSpan::fromDays( double days ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( days * datetime::constants::TICKS_PER_DAY ) };
	}

	TimeSpan TimeSpan::fromHours( double hours ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( hours * datetime::constants::TICKS_PER_HOUR ) };
	}

	TimeSpan TimeSpan::fromMinutes( double minutes ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( minutes * datetime::constants::TICKS_PER_MINUTE ) };
	}

	TimeSpan TimeSpan::fromSeconds( double seconds ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( seconds * datetime::constants::TICKS_PER_SECOND ) };
	}

	TimeSpan TimeSpan::fromMilliseconds( double milliseconds ) noexcept
	{
		return TimeSpan{ static_cast<std::int64_t>( milliseconds * datetime::constants::TICKS_PER_MILLISECOND ) };
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TimeSpan TimeSpan::parse( std::string_view iso8601DurationString )
	{
		TimeSpan result;
		if ( !tryParse( iso8601DurationString, result ) )
		{
			throw std::invalid_argument{ "Invalid TimeSpan string format" };
		}
		return result;
	}

	bool TimeSpan::tryParse( std::string_view iso8601DurationString, TimeSpan& result ) noexcept
	{
		try
		{
			if ( iso8601DurationString.empty() )
			{
				return false;
			}

			// Handle numeric seconds format (convenience)
			if ( iso8601DurationString.find_first_not_of( "0123456789.-" ) == std::string_view::npos )
			{
				double seconds{};
				auto [ptr, ec] = std::from_chars( iso8601DurationString.data(), iso8601DurationString.data() + iso8601DurationString.size(), seconds );
				if ( ec == std::errc{} && ptr == iso8601DurationString.data() + iso8601DurationString.size() )
				{
					result = TimeSpan::fromSeconds( seconds );
					return true;
				}
				return false;
			}

			// Check for negative sign
			bool isNegative{ false };
			std::string_view parseStr{ iso8601DurationString };
			if ( !parseStr.empty() && parseStr[0] == '-' )
			{
				isNegative = true;
				parseStr = parseStr.substr( 1 );
			}

			if ( parseStr.length() > 1 && parseStr[0] == 'P' )
			{
				double days{ 0.0 };
				double totalSeconds{ 0.0 };
				bool foundComponent{ false };

				// Parse days component (P[n]D)
				auto tPos{ parseStr.find( 'T' ) };

				// Parse date part (before T, or entire string if no T)
				auto datePart{ tPos != std::string_view::npos ? parseStr.substr( 1, tPos - 1 ) : parseStr.substr( 1 ) };
				if ( !datePart.empty() )
				{
					auto dPos{ datePart.find( 'D' ) };
					if ( dPos != std::string_view::npos )
					{
						auto dayStr{ datePart.substr( 0, dPos ) };
						auto [ptr, ec] = std::from_chars( dayStr.data(), dayStr.data() + dayStr.size(), days );
						if ( ec == std::errc{} && ptr == dayStr.data() + dayStr.size() )
						{
							foundComponent = true;
						}
						else
						{
							return false;
						}
					}
				}

				// Parse time part after T (if present)
				if ( tPos != std::string_view::npos )
				{
					auto timePart{ parseStr.substr( tPos + 1 ) };

					// Must have at least one component after T (not just "PT")
					if ( timePart.empty() )
					{
						return false;
					}

					// Parse hours
					auto hPos{ timePart.find( 'H' ) };
					if ( hPos != std::string_view::npos )
					{
						auto hourStr{ timePart.substr( 0, hPos ) };
						double hours{};
						auto [ptr, ec] = std::from_chars( hourStr.data(), hourStr.data() + hourStr.size(), hours );
						if ( ec == std::errc{} && ptr == hourStr.data() + hourStr.size() )
						{
							totalSeconds += hours * 3600.0;
							timePart = timePart.substr( hPos + 1 );
							foundComponent = true;
						}
						else
						{
							return false;
						}
					}

					// Parse minutes
					auto mPos{ timePart.find( 'M' ) };
					if ( mPos != std::string::npos )
					{
						auto minuteStr{ timePart.substr( 0, mPos ) };
						double minutes{};
						auto [ptr, ec] = std::from_chars( minuteStr.data(), minuteStr.data() + minuteStr.size(), minutes );
						if ( ec == std::errc{} && ptr == minuteStr.data() + minuteStr.size() )
						{
							totalSeconds += minutes * 60.0;
							timePart = timePart.substr( mPos + 1 );
							foundComponent = true;
						}
						else
						{
							return false;
						}
					}

					// Parse seconds
					auto sPos{ timePart.find( 'S' ) };
					if ( sPos != std::string::npos )
					{
						auto secondStr{ timePart.substr( 0, sPos ) };
						double seconds{};
						auto [ptr, ec] = std::from_chars( secondStr.data(), secondStr.data() + secondStr.size(), seconds );
						if ( ec == std::errc{} && ptr == secondStr.data() + secondStr.size() )
						{
							totalSeconds += seconds;
							foundComponent = true;
						}
						else
						{
							return false;
						}
					}
				}

				// Must have parsed at least one valid component (D, H, M, or S)
				if ( !foundComponent )
				{
					return false;
				}

				// Convert days to seconds
				totalSeconds += days * 86400.0;

				// Apply negative sign if present
				if ( isNegative )
				{
					totalSeconds = -totalSeconds;
				}

				result = TimeSpan::fromSeconds( totalSeconds );

				return true;
			}

			return false;
		}
		catch ( ... )
		{
			return false;
		}
	}

	//----------------------------------------------
	// std::chrono interoperability
	//----------------------------------------------

	std::chrono::system_clock::duration TimeSpan::toChrono() const noexcept
	{
		return std::chrono::duration_cast<std::chrono::system_clock::duration>(
			std::chrono::nanoseconds{ m_ticks * 100 } );
	}

	TimeSpan TimeSpan::fromChrono( const std::chrono::system_clock::duration& duration ) noexcept
	{
		return TimeSpan{ std::chrono::duration_cast<std::chrono::nanoseconds>( duration ).count() / 100 };
	}

	//=====================================================================
	// Stream operators
	//=====================================================================

	std::ostream& operator<<( std::ostream& os, const TimeSpan& timeSpan )
	{
		os << timeSpan.toString();

		return os;
	}

	std::istream& operator>>( std::istream& is, TimeSpan& timeSpan )
	{
		std::string str;
		is >> str;
		if ( !TimeSpan::tryParse( str, timeSpan ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
} // namespace nfx::datetime
