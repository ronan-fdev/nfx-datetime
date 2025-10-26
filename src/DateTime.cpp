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
 * @file DateTime.cpp
 * @brief Implementation of DateTime class methods for UTC datetime operations
 * @details Provides parsing logic for ISO 8601 format, string conversion and formatting,
 *          date/time component extraction, validation logic, and factory methods for
 *          creating DateTime instances. Implements cross-platform time conversions and
 *          arithmetic operations with 100-nanosecond precision.
 */

#include <istream>
#include <stdexcept>
#include <sstream>

#include "nfx/datetime/DateTime.h"

#include "Constants.h"

namespace nfx::datetime
{
	namespace internal
	{
		//=====================================================================
		//  Internal helper methods
		//=====================================================================

		/** @brief Convert ticks to date components */
		static void dateComponents( std::int64_t ticks, std::int32_t& year, std::int32_t& month, std::int32_t& day ) noexcept
		{
			// Extract date components from ticks using simple, reliable iteration
			std::int64_t totalDays{ ticks / datetime::constants::TICKS_PER_DAY };

			// Find the year by iterating from year 1
			year = 1;
			while ( true )
			{
				std::int32_t daysInCurrentYear{ DateTime::isLeapYear( year )
													? 366
													: 365 };
				if ( totalDays < daysInCurrentYear )
				{
					break;
				}

				totalDays -= daysInCurrentYear;
				++year;
			}

			// Find the month by iterating through months
			month = 1;
			while ( month <= 12 )
			{
				std::int32_t daysInCurrentMonth{ DateTime::daysInMonth( year, month ) };
				if ( totalDays < daysInCurrentMonth )
				{
					break;
				}

				totalDays -= daysInCurrentMonth;
				++month;
			}

			// Remaining days is the day of month (add 1 because day is 1-based)
			day = static_cast<std::int32_t>( totalDays ) + 1;
		}

		/** @brief Convert ticks to time components */
		static void timeComponents( std::int64_t ticks, std::int32_t& hour, std::int32_t& minute, std::int32_t& second, std::int32_t& millisecond ) noexcept
		{
			std::int64_t timeTicks{ ticks % datetime::constants::TICKS_PER_DAY };

			hour = static_cast<std::int32_t>( timeTicks / datetime::constants::TICKS_PER_HOUR );
			timeTicks %= datetime::constants::TICKS_PER_HOUR;

			minute = static_cast<std::int32_t>( timeTicks / datetime::constants::TICKS_PER_MINUTE );
			timeTicks %= datetime::constants::TICKS_PER_MINUTE;

			second = static_cast<std::int32_t>( timeTicks / datetime::constants::TICKS_PER_SECOND );
			timeTicks %= datetime::constants::TICKS_PER_SECOND;

			millisecond = static_cast<std::int32_t>( timeTicks / datetime::constants::TICKS_PER_MILLISECOND );
		}

		/** @brief Convert date components to ticks */
		static std::int64_t dateToTicks( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept
		{
			// Calculate total days since January 1, year 1 using simple, reliable method
			std::int64_t totalDays{ 0 };

			// Add days for complete years before the given year
			for ( std::int32_t y = 1; y < year; ++y )
			{
				totalDays += DateTime::isLeapYear( y )
								 ? 366
								 : 365;
			}

			// Add days for complete months in the given year
			for ( std::int32_t m = 1; m < month; ++m )
			{
				totalDays += DateTime::daysInMonth( year, m );
			}

			// Add days in the current month (subtract 1 because day is 1-based)
			totalDays += day - 1;

			return totalDays * datetime::constants::TICKS_PER_DAY;
		}

		/** @brief Convert time components to ticks */
		static std::int64_t timeToTicks( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept
		{
			return ( static_cast<std::int64_t>( hour ) * datetime::constants::TICKS_PER_HOUR ) +
				   ( static_cast<std::int64_t>( minute ) * datetime::constants::TICKS_PER_MINUTE ) +
				   ( static_cast<std::int64_t>( second ) * datetime::constants::TICKS_PER_SECOND ) +
				   ( static_cast<std::int64_t>( millisecond ) * datetime::constants::TICKS_PER_MILLISECOND );
		}

		/** @brief Validate date components */
		static bool isValidDate( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept
		{
			if ( year < datetime::constants::MIN_YEAR || year > datetime::constants::MAX_YEAR )
			{
				return false;
			}
			if ( month < 1 || month > 12 )
			{
				return false;
			}
			if ( day < 1 || day > DateTime::daysInMonth( year, month ) )
			{
				return false;
			}

			return true;
		}

		/** @brief Validate time components */
		static bool isValidTime( std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t millisecond ) noexcept
		{
			return hour >= 0 && hour <= 23 &&
				   minute >= 0 && minute <= 59 &&
				   second >= 0 && second <= 59 &&
				   millisecond >= 0 && millisecond <= 999;
		}
	} // namespace internal

	//=====================================================================
	// DateTime class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DateTime::DateTime() noexcept
		: m_ticks{ datetime::constants::MIN_DATETIME_TICKS }
	{
	}

	DateTime::DateTime( std::int64_t ticks ) noexcept
		: m_ticks{ ticks }
	{
	}

	DateTime::DateTime( std::chrono::system_clock::time_point timePoint ) noexcept
	{
		auto duration{ timePoint.time_since_epoch() };

		using ticks_duration = std::chrono::duration<std::int64_t, std::ratio<1, 10000000>>;
		auto ticks{ std::chrono::duration_cast<ticks_duration>( duration ) };

		m_ticks = datetime::constants::UNIX_EPOCH_TICKS + ticks.count();

		// Clamp to valid range
		if ( m_ticks < datetime::constants::MIN_DATETIME_TICKS )
		{
			m_ticks = datetime::constants::MIN_DATETIME_TICKS;
		}
		else if ( m_ticks > datetime::constants::MAX_DATETIME_TICKS )
		{
			m_ticks = datetime::constants::MAX_DATETIME_TICKS;
		}
	}

	DateTime::DateTime( std::int32_t year, std::int32_t month, std::int32_t day ) noexcept
	{
		if ( !internal::isValidDate( year, month, day ) )
		{
			m_ticks = datetime::constants::MIN_DATETIME_TICKS;
			return;
		}
		m_ticks = internal::dateToTicks( year, month, day );
	}

	DateTime::DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
		std::int32_t hour, std::int32_t minute, std::int32_t second ) noexcept
	{
		if ( !internal::isValidDate( year, month, day ) || !internal::isValidTime( hour, minute, second, 0 ) )
		{
			m_ticks = datetime::constants::MIN_DATETIME_TICKS;
			return;
		}
		m_ticks = internal::dateToTicks( year, month, day ) + internal::timeToTicks( hour, minute, second, 0 );
	}

	DateTime::DateTime( std::int32_t year, std::int32_t month, std::int32_t day,
		std::int32_t hour, std::int32_t minute, std::int32_t second,
		std::int32_t millisecond ) noexcept
	{
		if ( !internal::isValidDate( year, month, day ) || !internal::isValidTime( hour, minute, second, millisecond ) )
		{
			m_ticks = datetime::constants::MIN_DATETIME_TICKS;
			return;
		}
		m_ticks = internal::dateToTicks( year, month, day ) + internal::timeToTicks( hour, minute, second, millisecond );
	}

	DateTime::DateTime( std::string_view iso8601String )
	{
		DateTime result;
		if ( !tryParse( iso8601String, result ) )
		{
			throw std::invalid_argument{ "Invalid ISO 8601 DateTime string" };
		}
		m_ticks = result.m_ticks;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	std::int32_t DateTime::year() const noexcept
	{
		std::int32_t year, month, day;
		internal::dateComponents( m_ticks, year, month, day );

		return year;
	}

	std::int32_t DateTime::month() const noexcept
	{
		std::int32_t year, month, day;
		internal::dateComponents( m_ticks, year, month, day );

		return month;
	}

	std::int32_t DateTime::day() const noexcept
	{
		std::int32_t year, month, day;
		internal::dateComponents( m_ticks, year, month, day );

		return day;
	}

	std::int32_t DateTime::hour() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		internal::timeComponents( m_ticks, hour, minute, second, millisecond );

		return hour;
	}

	std::int32_t DateTime::minute() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		internal::timeComponents( m_ticks, hour, minute, second, millisecond );

		return minute;
	}

	std::int32_t DateTime::second() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		internal::timeComponents( m_ticks, hour, minute, second, millisecond );

		return second;
	}

	std::int32_t DateTime::millisecond() const noexcept
	{
		std::int32_t hour, minute, second, millisecond;
		internal::timeComponents( m_ticks, hour, minute, second, millisecond );

		return millisecond;
	}

	std::int32_t DateTime::dayOfWeek() const noexcept
	{
		// January 1, 0001 was a Monday (day 1), so we need to adjust
		std::int64_t days{ m_ticks / datetime::constants::TICKS_PER_DAY };

		// 0=Sunday, 6=Saturday
		return static_cast<std::int32_t>( ( days + 1 ) % 7 );
	}

	std::int32_t DateTime::dayOfYear() const noexcept
	{
		std::int32_t year, month, day;
		internal::dateComponents( m_ticks, year, month, day );

		std::int32_t dayCount{ 0 };
		for ( std::int32_t m{ 1 }; m < month; ++m )
		{
			dayCount += daysInMonth( year, m );
		}

		return dayCount + day;
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	std::int64_t DateTime::toUnixSeconds() const noexcept
	{
		return ( m_ticks - datetime::constants::UNIX_EPOCH_TICKS ) / datetime::constants::TICKS_PER_SECOND;
	}

	std::int64_t DateTime::toUnixMilliseconds() const noexcept
	{
		return ( m_ticks - datetime::constants::UNIX_EPOCH_TICKS ) / datetime::constants::TICKS_PER_MILLISECOND;
	}

	DateTime DateTime::date() const noexcept
	{
		std::int64_t dayTicks{ ( m_ticks / datetime::constants::TICKS_PER_DAY ) * datetime::constants::TICKS_PER_DAY };

		return DateTime{ dayTicks };
	}

	TimeSpan DateTime::timeOfDay() const noexcept
	{
		std::int64_t timeTicks{ m_ticks % datetime::constants::TICKS_PER_DAY };

		return TimeSpan{ timeTicks };
	}

	//----------------------------------------------
	// String formatting
	//----------------------------------------------

	std::string DateTime::toString() const
	{
		return toString( Format::Iso8601Basic );
	}

	std::string DateTime::toString( Format format ) const
	{
		std::int32_t y, mon, d, h, min, s, ms;
		internal::dateComponents( m_ticks, y, mon, d );
		internal::timeComponents( m_ticks, h, min, s, ms );

		std::ostringstream oss;

		switch ( format )
		{
			case Format::Iso8601Basic:
			{
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d << "T" << std::setw( 2 ) << h << ":"
					<< std::setw( 2 ) << min << ":" << std::setw( 2 ) << s << "Z";

				break;
			}
			case Format::Iso8601Extended:
			{
				std::int32_t fractionalTicks{ static_cast<std::int32_t>( m_ticks % datetime::constants::TICKS_PER_SECOND ) };
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d << "T" << std::setw( 2 ) << h
					<< ":" << std::setw( 2 ) << min << ":" << std::setw( 2 ) << s << "." << std::setw( 7 ) << fractionalTicks << "Z";

				break;
			}
			case Format::Iso8601WithOffset:
			{
				// UTC DateTime always has +00:00 offset
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d << "T" << std::setw( 2 ) << h
					<< ":" << std::setw( 2 ) << min << ":" << std::setw( 2 ) << s << "+00:00";

				break;
			}
			case Format::DateOnly:
			{
				oss << std::setfill( '0' ) << std::setw( 4 ) << y << "-" << std::setw( 2 ) << mon << "-" << std::setw( 2 ) << d;

				break;
			}
			case Format::TimeOnly:
			{
				oss << std::setfill( '0' ) << std::setw( 2 ) << h << ":" << std::setw( 2 ) << min << ":" << std::setw( 2 ) << s;

				break;
			}
			case Format::UnixSeconds:
			{
				oss << toUnixSeconds();

				break;
			}
			case Format::UnixMilliseconds:
			{
				oss << toUnixMilliseconds();

				break;
			}
			default:
			{
				oss << toString( Format::Iso8601Basic );

				break;
			}
		}

		return oss.str();
	}

	std::string DateTime::toIso8601Extended() const
	{
		return toString( Format::Iso8601Extended );
	}

	//----------------------------------------------
	// Validation methods
	//----------------------------------------------

	bool DateTime::isValid() const noexcept
	{
		return m_ticks >= datetime::constants::MIN_DATETIME_TICKS && m_ticks <= datetime::constants::MAX_DATETIME_TICKS;
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	DateTime DateTime::now() noexcept
	{
		return DateTime{ std::chrono::system_clock::now() };
	}

	DateTime DateTime::today() noexcept
	{
		return now().date();
	}

	DateTime DateTime::minValue() noexcept
	{
		return DateTime{ datetime::constants::MIN_DATETIME_TICKS };
	}

	DateTime DateTime::maxValue() noexcept
	{
		return DateTime{ datetime::constants::MAX_DATETIME_TICKS };
	}

	DateTime DateTime::epoch() noexcept
	{
		return DateTime{ datetime::constants::UNIX_EPOCH_TICKS };
	}

	DateTime DateTime::parse( std::string_view iso8601String )
	{
		DateTime result;
		if ( !tryParse( iso8601String, result ) )
		{
			throw std::invalid_argument{ "Invalid ISO 8601 DateTime string" };
		}
		return result;
	}

	bool DateTime::tryParse( std::string_view iso8601String, DateTime& result ) noexcept
	{
		try
		{
			/**
			 * ISO 8601 parser
			 * Supports: YYYY-MM-DD, YYYY-MM-DDTHH:mm:ss, YYYY-MM-DDTHH:mm:ssZ,
			 * YYYY-MM-DDTHH:mm:ss.f, YYYY-MM-DDTHH:mm:ss.fffffffZ, etc.
			 */
			if ( iso8601String.empty() || iso8601String.length() < 4 )
			{
				return false;
			}

			// Remove trailing 'Z' if present
			if ( iso8601String.back() == 'Z' )
			{
				iso8601String.remove_suffix( 1 );
			}

			// Remove timezone offset for DateTime parsing
			auto tzPos{ iso8601String.find_last_of( "+-" ) };

			// Ensure it's not in date part
			if ( tzPos != std::string_view::npos && tzPos > 10 )
			{
				iso8601String = iso8601String.substr( 0, tzPos );
			}

			// Parse year (required)
			if ( iso8601String.length() < 4 )
			{
				return false;
			}

			auto pos{ size_t{ 0 } };
			std::int32_t year{ 0 };
			for ( size_t i{ 0 }; i < 4; ++i )
			{
				char c{ iso8601String[pos++] };
				if ( c < '0' || c > '9' )
				{
					return false;
				}
				year = year * 10 + ( c - '0' );
			}

			// Expect separator
			if ( pos >= iso8601String.length() || iso8601String[pos++] != '-' )
			{
				return false;
			}

			// Parse month (required)
			if ( pos >= iso8601String.length() )
			{
				return false;
			}

			std::int32_t month{ 0 };
			char c{ iso8601String[pos++] };
			if ( c < '0' || c > '9' )
			{
				return false;
			}

			month = c - '0';

			if ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' )
			{
				month = month * 10 + ( iso8601String[pos++] - '0' );
			}

			// Expect separator
			if ( pos >= iso8601String.length() || iso8601String[pos++] != '-' )
			{
				return false;
			}

			// Parse day (required)
			if ( pos >= iso8601String.length() )
			{
				return false;
			}

			std::int32_t day{ 0 };
			c = iso8601String[pos++];
			if ( c < '0' || c > '9' )
			{
				return false;
			}

			day = c - '0';

			if ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' )
			{
				day = day * 10 + ( iso8601String[pos++] - '0' );
			}

			// Time part is optional
			std::int32_t hour{ 0 }, minute{ 0 }, second{ 0 };
			std::int32_t fractionalTicks{ 0 };

			if ( pos < iso8601String.length() && iso8601String[pos] == 'T' )
			{
				++pos; // Skip 'T'

				// Parse hour
				if ( pos + 1 >= iso8601String.length() )
				{
					return false;
				}

				c = iso8601String[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}

				hour = c - '0';

				if ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' )
				{
					hour = hour * 10 + ( iso8601String[pos++] - '0' );
				}

				// Expect colon
				if ( pos >= iso8601String.length() || iso8601String[pos++] != ':' )
				{
					return false;
				}

				// Parse minute
				if ( pos + 1 >= iso8601String.length() )
				{
					return false;
				}

				c = iso8601String[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}

				minute = c - '0';

				if ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' )
				{
					minute = minute * 10 + ( iso8601String[pos++] - '0' );
				}

				// Expect colon
				if ( pos >= iso8601String.length() || iso8601String[pos++] != ':' )
				{
					return false;
				}

				// Parse second
				if ( pos >= iso8601String.length() )
				{
					return false;
				}

				c = iso8601String[pos++];
				if ( c < '0' || c > '9' )
				{
					return false;
				}

				second = c - '0';

				if ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' )
				{
					second = second * 10 + ( iso8601String[pos++] - '0' );
				}

				// Parse fractional seconds if present
				if ( pos < iso8601String.length() && iso8601String[pos] == '.' )
				{
					++pos; // Skip '.'

					std::int32_t fractionDigits{ 0 };
					std::int32_t fractionValue{ 0 };

					while ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' && fractionDigits < 7 )
					{
						fractionValue = fractionValue * 10 + ( iso8601String[pos++] - '0' );
						++fractionDigits;
					}

					// Skip any extra fractional digits
					while ( pos < iso8601String.length() && iso8601String[pos] >= '0' && iso8601String[pos] <= '9' )
					{
						++pos;
					}

					// Convert to ticks (pad to 7 digits)
					while ( fractionDigits < 7 )
					{
						fractionValue *= 10;
						++fractionDigits;
					}

					fractionalTicks = fractionValue;
				}
			}

			// Validate components
			if ( !internal::isValidDate( year, month, day ) || !internal::isValidTime( hour, minute, second, 0 ) )
			{
				return false;
			}

			// Calculate ticks
			std::int64_t ticks{ internal::dateToTicks( year, month, day ) + internal::timeToTicks( hour, minute, second, 0 ) + fractionalTicks };

			result = DateTime{ ticks };

			return true;
		}
		catch ( ... )
		{
			return false;
		}
	}

	DateTime DateTime::sinceEpochSeconds( std::int64_t seconds ) noexcept
	{
		std::int64_t ticks{ datetime::constants::UNIX_EPOCH_TICKS + ( seconds * datetime::constants::TICKS_PER_SECOND ) };

		return DateTime{ ticks };
	}

	DateTime DateTime::sinceEpochMilliseconds( std::int64_t milliseconds ) noexcept
	{
		std::int64_t ticks{ datetime::constants::UNIX_EPOCH_TICKS + ( milliseconds * datetime::constants::TICKS_PER_MILLISECOND ) };

		return DateTime{ ticks };
	}

	//----------------------------------------------
	// std::chrono interoperability
	//----------------------------------------------

	std::chrono::system_clock::time_point DateTime::toChrono() const noexcept
	{
		// Clamp to chrono-safe range
		std::int64_t safeTicks{ m_ticks };
		if ( safeTicks < datetime::constants::MIN_CHRONO_SAFE_TICKS )
		{
			safeTicks = datetime::constants::MIN_CHRONO_SAFE_TICKS;
		}
		else if ( safeTicks > datetime::constants::MAX_CHRONO_SAFE_TICKS )
		{
			safeTicks = datetime::constants::MAX_CHRONO_SAFE_TICKS;
		}

		// Calculate duration since Unix epoch in 100-nanosecond ticks
		std::int64_t ticksSinceEpoch{ safeTicks - datetime::constants::UNIX_EPOCH_TICKS };

		// Convert to std::chrono duration (100ns precision)
		using ticks_duration = std::chrono::duration<std::int64_t, std::ratio<1, 10000000>>;
		auto duration{ ticks_duration{ ticksSinceEpoch } };

		return std::chrono::system_clock::time_point{ duration };
	}

	DateTime DateTime::fromChrono( const std::chrono::system_clock::time_point& timePoint ) noexcept
	{
		return DateTime{ timePoint };
	}

	//=====================================================================
	// Stream operators
	//=====================================================================

	std::ostream& operator<<( std::ostream& os, const DateTime& dateTime )
	{
		os << dateTime.toString();

		return os;
	}

	std::istream& operator>>( std::istream& is, DateTime& dateTime )
	{
		std::string str;
		is >> str;
		if ( !DateTime::tryParse( str, dateTime ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
} // namespace nfx::datetime
