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
 * @file DateTimeOffset.cpp
 * @brief Implementation of DateTimeOffset class methods for timezone-aware datetime operations
 * @details Provides parsing logic for ISO 8601 format with timezone offsets, UTC/local time
 *          conversions, timezone offset calculations, and cross-platform time handling using
 *          thread-safe functions. Implements arithmetic operations and string formatting for
 *          timezone-aware datetime values with 100-nanosecond precision.
 */

#include <istream>
#include <stdexcept>
#include <sstream>

#include "nfx/datetime/DateTimeOffset.h"

#include "Constants.h"

//----------------------------------------------
// Cross-platform time functions
//----------------------------------------------

/** @brief Cross-platform thread-safe time conversion functions */
#if defined( _WIN32 )
// Windows uses gmtime_s and localtime_s (different parameter order than POSIX)
/** @brief Thread-safe UTC time conversion wrapper for Windows gmtime_s */
#	define NFX_DATETIME_GMTIME_R( timer, result ) ( gmtime_s( ( result ), ( timer ) ) == 0 )
/** @brief Thread-safe local time conversion wrapper for Windows localtime_s */
#	define NFX_DATETIME_LOCALTIME_R( timer, result ) ( localtime_s( ( result ), ( timer ) ) == 0 )
#elif defined( __GNUC__ ) || defined( __clang__ )
// POSIX systems use gmtime_r and localtime_r
/** @brief Thread-safe UTC time conversion wrapper for POSIX gmtime_r */
#	define NFX_DATETIME_GMTIME_R( timer, result ) ( gmtime_r( ( timer ), ( result ) ) != nullptr )
/** @brief Thread-safe local time conversion wrapper for POSIX localtime_r */
#	define NFX_DATETIME_LOCALTIME_R( timer, result ) ( localtime_r( ( timer ), ( result ) ) != nullptr )
#else
// Fallback to non-thread-safe versions
/** @brief Fallback UTC time conversion using non-thread-safe gmtime */
#	define NFX_DATETIME_GMTIME_R( timer, result ) ( ( *( result ) = *gmtime( timer ) ), true )
/** @brief Fallback local time conversion using non-thread-safe localtime */
#	define NFX_DATETIME_LOCALTIME_R( timer, result ) ( ( *( result ) = *localtime( timer ) ), true )
#endif

namespace nfx::datetime
{
	namespace internal
	{
		//=====================================================================
		//  Internal helper methods
		//=====================================================================

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/** @brief Validate timezone offset is within valid range (±14:00:00) */
		static bool isValidOffset( const TimeSpan& offset ) noexcept
		{
			// Use integer tick comparison to avoid floating-point precision issues
			const auto offsetTicks{ offset.ticks() };

			// ±14 hours = ±840 minutes = ±50,400 seconds = ±504,000,000,000 ticks
			constexpr std::int64_t MAX_OFFSET_TICKS{ 14LL * 60LL * 60LL * datetime::constants::TICKS_PER_SECOND };

			return offsetTicks >= -MAX_OFFSET_TICKS && offsetTicks <= MAX_OFFSET_TICKS;
		}

		//----------------------------------------------
		// Timezone calculation
		//----------------------------------------------

		/** @brief Calculate system timezone offset for a given DateTime using platform-specific APIs */
		static TimeSpan systemTimezoneOffset( const DateTime& dateTime ) noexcept
		{
			// Get system timezone offset for the given UTC time
			// MinGW/GCC on Windows doesn't support C++20 timezone features
#if defined( _MSC_VER ) && _MSC_VER >= 1929 && __cpp_lib_chrono >= 201907L
			// C++20 timezone support - MSVC only
			try
			{
				using namespace std::chrono;
				const auto sysTp{ sys_time<nanoseconds>( nanoseconds( dateTime.ticks() * 100 ) ) };

				if ( const std::chrono::time_zone* zone = std::chrono::current_zone() )
				{
					const auto info{ zone->get_info( sysTp ) };
					return TimeSpan::fromSeconds( static_cast<double>( info.offset.count() ) );
				}
			}
			catch ( ... )
			{
				// Fall through to legacy implementation
			}
#endif

			// Legacy implementation for MinGW/GCC and fallback for MSVC
			const auto timePoint{ std::chrono::system_clock::time_point(
				std::chrono::duration_cast<std::chrono::system_clock::duration>(
					std::chrono::nanoseconds( dateTime.ticks() * 100 ) ) ) };

			const auto localTime{ std::chrono::system_clock::to_time_t( timePoint ) };

			// Use thread-safe time functions
			std::tm utcTm{};
			std::tm localTm{};

			if ( !NFX_DATETIME_GMTIME_R( &localTime, &utcTm ) || !NFX_DATETIME_LOCALTIME_R( &localTime, &localTm ) )
			{
				// Fallback to UTC on error
				return TimeSpan{ 0 };
			}

			// Calculate offset in seconds
			const auto utcSeconds{ utcTm.tm_hour * 3600 + utcTm.tm_min * 60 + utcTm.tm_sec };
			const auto localSeconds{ localTm.tm_hour * 3600 + localTm.tm_min * 60 + localTm.tm_sec };

			// Handle day boundary crossings
			auto offsetSeconds{ localSeconds - utcSeconds };
			if ( localTm.tm_mday != utcTm.tm_mday )
			{
				if ( localTm.tm_mday > utcTm.tm_mday )
				{
					offsetSeconds += 24 * 3600;
				}
				else
				{
					offsetSeconds -= 24 * 3600;
				}
			}

			return TimeSpan::fromSeconds( offsetSeconds );
		}

		//----------------------------------------------
		// String formatting
		//----------------------------------------------

		/** @brief Append timezone offset to output stream */
		static void appendOffset( std::ostringstream& oss, std::int32_t offsetMinutes )
		{
			if ( offsetMinutes == 0 )
			{
				oss << 'Z';
			}
			else
			{
				const auto absMinutes{ std::abs( offsetMinutes ) };
				const auto offsetHours{ absMinutes / 60 };
				const auto offsetMins{ absMinutes % 60 };
				oss << ( offsetMinutes >= 0 ? '+' : '-' )
					<< std::setw( 2 ) << offsetHours << ':'
					<< std::setw( 2 ) << offsetMins;
			}
		}

		/** @brief Format ISO 8601 datetime with offset */
		static std::string formatIso8601( const DateTimeOffset& dto, DateTime::Format format )
		{
			std::ostringstream oss;
			oss << std::setfill( '0' );

			// Date part
			oss << std::setw( 4 ) << dto.year() << '-'
				<< std::setw( 2 ) << dto.month() << '-'
				<< std::setw( 2 ) << dto.day() << 'T';

			// Time part
			oss << std::setw( 2 ) << dto.hour() << ':'
				<< std::setw( 2 ) << dto.minute() << ':'
				<< std::setw( 2 ) << dto.second();

			// Add fractional seconds for extended format
			if ( format == DateTime::Format::Iso8601Extended )
			{
				const auto ms{ dto.millisecond() };
				const auto us{ dto.microsecond() };
				const auto ns{ dto.nanosecond() };
				if ( ms > 0 || us > 0 || ns > 0 )
				{
					oss << '.' << std::setw( 3 ) << ms;
					if ( us > 0 || ns > 0 )
					{
						oss << std::setw( 3 ) << us;
						if ( ns > 0 )
						{
							oss << std::setw( 1 ) << ( ns / 100 );
						}
					}
				}
			}

			// Offset part
			appendOffset( oss, dto.totalOffsetMinutes() );
			return oss.str();
		}

		/** @brief Format date only */
		static std::string formatDateOnly( const DateTimeOffset& dto )
		{
			std::ostringstream oss;
			oss << std::setfill( '0' )
				<< std::setw( 4 ) << dto.year() << '-'
				<< std::setw( 2 ) << dto.month() << '-'
				<< std::setw( 2 ) << dto.day();
			return oss.str();
		}

		/** @brief Format time only with offset */
		static std::string formatTimeOnly( const DateTimeOffset& dto )
		{
			std::ostringstream oss;
			oss << std::setfill( '0' )
				<< std::setw( 2 ) << dto.hour() << ':'
				<< std::setw( 2 ) << dto.minute() << ':'
				<< std::setw( 2 ) << dto.second();

			appendOffset( oss, dto.totalOffsetMinutes() );
			return oss.str();
		}

	} // namespace internal

	//=====================================================================
	// DateTimeOffset class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DateTimeOffset::DateTimeOffset() noexcept
		: m_dateTime{ DateTime::minValue() },
		  m_offset{ 0 }
	{
	}

	DateTimeOffset::DateTimeOffset( const DateTime& dateTime, const TimeSpan& offset ) noexcept
		: m_dateTime{ dateTime },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( const DateTime& dateTime ) noexcept
		: m_dateTime{ dateTime },
		  m_offset{ internal::systemTimezoneOffset( dateTime ) }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int64_t ticks, const TimeSpan& offset ) noexcept
		: m_dateTime{ ticks },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day,
		std::int32_t hour, std::int32_t minute, std::int32_t second, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day, hour, minute, second },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
		std::int32_t millisecond, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day, hour, minute, second, millisecond },
		  m_offset{ offset }
	{
	}

	DateTimeOffset::DateTimeOffset( std::int32_t year, std::int32_t month, std::int32_t day, std::int32_t hour, std::int32_t minute, std::int32_t second,
		std::int32_t millisecond, std::int32_t microsecond, const TimeSpan& offset ) noexcept
		: m_dateTime{ year, month, day, hour, minute, second, millisecond },
		  m_offset{ offset }
	{
		// Add microseconds to the DateTime (1 microsecond = 10 ticks)
		m_dateTime += TimeSpan{ microsecond * 10 };
	}

	DateTimeOffset::DateTimeOffset( std::string_view iso8601String )
		: m_dateTime{ DateTime::minValue() },
		  m_offset{ 0 }
	{
		if ( !tryParse( iso8601String, *this ) )
		{
			throw std::invalid_argument{ "Invalid ISO 8601 DateTimeOffset string format" };
		}
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	DateTime DateTimeOffset::utcDateTime() const noexcept
	{
		return DateTime{ utcTicks() };
	}

	DateTime DateTimeOffset::localDateTime() const noexcept
	{
		// Return the local DateTime component as-is
		return m_dateTime;
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	DateTimeOffset DateTimeOffset::date() const noexcept
	{
		return DateTimeOffset{ m_dateTime.date(), m_offset };
	}

	DateTimeOffset DateTimeOffset::toOffset( const TimeSpan& newOffset ) const noexcept
	{
		// Convert to UTC, then apply new offset
		const auto utcTime{ utcDateTime() };

		return DateTimeOffset{ utcTime + newOffset, newOffset };
	}

	DateTimeOffset DateTimeOffset::toUniversalTime() const noexcept
	{
		return DateTimeOffset{ utcDateTime(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::toLocalTime() const noexcept
	{
		const auto utcTime{ utcDateTime() };
		const auto localOffset{ internal::systemTimezoneOffset( utcTime ) };

		return DateTimeOffset{ utcTime + localOffset, localOffset };
	}

	std::int64_t DateTimeOffset::toFILETIME() const noexcept
	{
		// Windows file time: 100-nanosecond intervals since January 1, 1601 UTC
		const auto utcTicksValue{ utcTicks() };

		// Guard against underflow for dates before FILETIME epoch (Jan 1, 1601)
		if ( utcTicksValue < datetime::constants::MICROSOFT_FILETIME_EPOCH_TICKS )
		{
			return 0; // Return 0 for dates before FILETIME epoch
		}

		return utcTicksValue - datetime::constants::MICROSOFT_FILETIME_EPOCH_TICKS;
	}

	//----------------------------------------------
	// Arithmetic methods
	//----------------------------------------------

	DateTimeOffset DateTimeOffset::addDays( double days ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromDays( days ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addHours( double hours ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromHours( hours ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addMilliseconds( double milliseconds ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromMilliseconds( milliseconds ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addMinutes( double minutes ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromMinutes( minutes ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addMonths( std::int32_t months ) const noexcept
	{
		// Extract date components
		auto year{ m_dateTime.year() };
		auto month{ m_dateTime.month() };
		const auto day{ m_dateTime.day() };
		const auto timeOfDay{ m_dateTime.timeOfDay() };

		// Add months with proper year overflow handling
		month += months;
		while ( month > 12 )
		{
			month -= 12;
			year++;
		}
		while ( month < 1 )
		{
			month += 12;
			year--;
		}

		// Handle day overflow (e.g., Jan 31 + 1 month = Feb 28/29)
		const auto daysInTargetMonth{ DateTime::daysInMonth( year, month ) };
		const auto adjustedDay{ std::min( day, daysInTargetMonth ) };

		// Create new DateTime and add back the time of day
		const auto newDate{ DateTime{ year, month, adjustedDay } + timeOfDay };

		return DateTimeOffset{ newDate, m_offset };
	}

	DateTimeOffset DateTimeOffset::addSeconds( double seconds ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + TimeSpan::fromSeconds( seconds ), m_offset };
	}

	DateTimeOffset DateTimeOffset::addYears( std::int32_t years ) const noexcept
	{
		return addMonths( years * 12 );
	}

	//----------------------------------------------
	// String formatting
	//----------------------------------------------

	std::string DateTimeOffset::toString() const
	{
		return toString( DateTime::Format::Iso8601Basic );
	}

	std::string DateTimeOffset::toString( DateTime::Format format ) const
	{
		switch ( format )
		{
			case DateTime::Format::Iso8601Basic:
			case DateTime::Format::Iso8601Extended:
			case DateTime::Format::Iso8601WithOffset:
			{
				return internal::formatIso8601( *this, format );
			}
			case DateTime::Format::DateOnly:
			{
				return internal::formatDateOnly( *this );
			}
			case DateTime::Format::TimeOnly:
			{
				return internal::formatTimeOnly( *this );
			}
			case DateTime::Format::UnixSeconds:
			{
				return std::to_string( toUnixSeconds() );
			}
			case DateTime::Format::UnixMilliseconds:
			{
				return std::to_string( toUnixMilliseconds() );
			}
			default:
			{
				return toString( DateTime::Format::Iso8601Basic );
			}
		}
	}

	std::string DateTimeOffset::toIso8601Extended() const
	{
		return toString( DateTime::Format::Iso8601Extended );
	}

	//----------------------------------------------
	// Validation methods
	//----------------------------------------------

	bool DateTimeOffset::isValid() const noexcept
	{
		return m_dateTime.isValid() && internal::isValidOffset( m_offset );
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	DateTimeOffset DateTimeOffset::now() noexcept
	{
		const auto utcNow{ DateTime::now() };
		const auto localOffset{ internal::systemTimezoneOffset( utcNow ) };

		return DateTimeOffset{ utcNow + localOffset, localOffset };
	}

	DateTimeOffset DateTimeOffset::utcNow() noexcept
	{
		return DateTimeOffset{ DateTime::now(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::today() noexcept
	{
		// Get current local time, then extract just the date part at midnight
		const auto localNow{ DateTimeOffset::now() };

		// Extract local date components
		const auto year{ localNow.year() };
		const auto month{ localNow.month() };
		const auto day{ localNow.day() };

		// Create local midnight for today with the same offset
		return DateTimeOffset{ year, month, day, 0, 0, 0, localNow.offset() };
	}

	DateTimeOffset DateTimeOffset::minValue() noexcept
	{
		return DateTimeOffset{ DateTime::minValue(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::maxValue() noexcept
	{
		return DateTimeOffset{ DateTime::maxValue(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::unixEpoch() noexcept
	{
		return DateTimeOffset{ DateTime::epoch(), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::parse( std::string_view iso8601String )
	{
		DateTimeOffset result;
		if ( !tryParse( iso8601String, result ) )
		{
			throw std::invalid_argument{ "Invalid ISO 8601 DateTimeOffset string format" };
		}
		return result;
	}

	bool DateTimeOffset::tryParse( std::string_view iso8601String, DateTimeOffset& result ) noexcept
	{
		/*
			ISO 8601 compliant parser supporting:
			- Local time without timezone (valid but ambiguous per ISO 8601)
			- UTC indicator: Z
			- Timezone offsets: ±HH:MM (extended), ±HHMM (basic), ±HH (basic)
			- Maximum offset: ±14:00 (±840 minutes)

			Note: ISO 8601 allows local time without timezone designator, though it's
			ambiguous for cross-timezone communication. When no timezone is provided,
			offset defaults to zero (treated as unspecified/local time).
		*/
		try
		{
			// ISO 8601 parsing with comprehensive offset format support
			std::string s{ iso8601String };
			TimeSpan offset{ 0 };
			DateTime dateTime;

			// Track if we successfully parsed an offset
			bool offsetParsed{ false };

			// Find timezone indicator - search from right to avoid matching negative years
			auto offsetPos{ std::string::npos };
			for ( auto i{ s.length() }; i > 10; --i )
			{
				// Skip date part (YYYY-MM-DD = 10 chars)
				if ( s[i - 1] == 'Z' || s[i - 1] == '+' || s[i - 1] == '-' )
				{
					offsetPos = i - 1;
					break;
				}
			}

			if ( offsetPos != std::string::npos )
			{
				if ( s[offsetPos] == 'Z' )
				{
					// UTC indicator
					offset = TimeSpan{ 0 };
					offsetParsed = true;
					s = s.substr( 0, offsetPos );
				}
				else
				{
					// Parse offset: supports +HH:MM, +HHMM, +HH formats
					auto offsetStr{ s.substr( offsetPos ) };
					s = s.substr( 0, offsetPos );

					// Minimum: +H or -H
					if ( offsetStr.length() >= 2 )
					{
						const bool isNegative{ offsetStr[0] == '-' };
						const auto numericPart{ offsetStr.substr( 1 ) };

						std::int32_t hours{ 0 };
						std::int32_t minutes{ 0 };

						auto colonPos{ numericPart.find( ':' ) };
						if ( colonPos != std::string::npos )
						{
							// Format: +HH:MM or +H:MM
							if ( colonPos > 0 && colonPos < numericPart.length() - 1 )
							{
								hours = std::stoi( numericPart.substr( 0, colonPos ) );
								minutes = std::stoi( numericPart.substr( colonPos + 1 ) );
							}
						}
						else if ( numericPart.length() == 4 )
						{
							// Format: +HHMM
							hours = std::stoi( numericPart.substr( 0, 2 ) );
							minutes = std::stoi( numericPart.substr( 2, 2 ) );
						}
						else if ( numericPart.length() == 2 || numericPart.length() == 1 )
						{
							// Format: +HH or +H
							hours = std::stoi( numericPart );
							minutes = 0;
						}

						// Validate offset components - ISO 8601 allows ±14:00 maximum
						if ( hours >= 0 && minutes >= 0 && minutes <= 59 )
						{
							const auto totalMinutes{ hours * 60 + minutes };
							// Maximum valid offset is ±14:00 (840 minutes) exactly
							if ( totalMinutes <= 840 )
							{
								offset = TimeSpan::fromMinutes( isNegative
																	? -totalMinutes
																	: totalMinutes );
								offsetParsed = true;
							}
						}
					}
				}
			}
			else
			{
				// No timezone indicator found - assume this is valid for DateTime parsing
				offsetParsed = true;
			}

			// Parse the datetime part
			if ( DateTime::tryParse( s, dateTime ) && offsetParsed )
			{
				// ( isValidOffset( offset ) )
				{
					result = DateTimeOffset{ dateTime, offset };
					return true;
				}
			}
		}
		catch ( ... )
		{
			// Parsing failed - tryParse methods should not throw exceptions
		}

		return false;
	}

	DateTimeOffset DateTimeOffset::fromUnixTimeSeconds( std::int64_t seconds ) noexcept
	{
		return DateTimeOffset{ DateTime::sinceEpochSeconds( seconds ), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::fromUnixTimeMilliseconds( std::int64_t milliseconds ) noexcept
	{
		return DateTimeOffset{ DateTime::sinceEpochMilliseconds( milliseconds ), TimeSpan{ 0 } };
	}

	DateTimeOffset DateTimeOffset::fromFILETIME( std::int64_t FILETIME ) noexcept
	{
		// Windows file time: 100-nanosecond intervals since January 1, 1601 UTC
		const auto ticks{ FILETIME + datetime::constants::MICROSOFT_FILETIME_EPOCH_TICKS };

		return DateTimeOffset{ DateTime{ ticks }, TimeSpan{ 0 } };
	}

	//=====================================================================
	// Stream operators
	//=====================================================================

	std::ostream& operator<<( std::ostream& os, const DateTimeOffset& dateTimeOffset )
	{
		return os << dateTimeOffset.toString();
	}

	std::istream& operator>>( std::istream& is, DateTimeOffset& dateTimeOffset )
	{
		std::string str;
		is >> str;

		if ( !DateTimeOffset::tryParse( str, dateTimeOffset ) )
		{
			is.setstate( std::ios::failbit );
		}

		return is;
	}
} // namespace nfx::datetime
