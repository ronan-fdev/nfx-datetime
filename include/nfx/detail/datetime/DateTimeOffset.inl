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
 * @file DateTimeOffset.inl
 * @brief Inline implementations for DateTimeOffset class performance-critical methods
 * @details Contains inline implementations of constructors, operators, and accessor methods
 *          for timezone-aware datetime operations. These are separated from the main header
 *          to improve compilation times while maintaining zero-cost abstractions and optimal
 *          performance for UTC conversion and comparison operations.
 */

namespace nfx::datetime
{
	//=====================================================================
	// DateTimeOffset class - Inline implementations
	//=====================================================================

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool DateTimeOffset::operator==( const DateTimeOffset& other ) const noexcept
	{
		// Compare UTC times
		return utcTicks() == other.utcTicks();
	}

	inline bool DateTimeOffset::operator!=( const DateTimeOffset& other ) const noexcept
	{
		return !( *this == other );
	}

	inline bool DateTimeOffset::operator<( const DateTimeOffset& other ) const noexcept
	{
		// Compare UTC times
		return utcTicks() < other.utcTicks();
	}

	inline bool DateTimeOffset::operator<=( const DateTimeOffset& other ) const noexcept
	{
		return utcTicks() <= other.utcTicks();
	}

	inline bool DateTimeOffset::operator>( const DateTimeOffset& other ) const noexcept
	{
		return utcTicks() > other.utcTicks();
	}

	inline bool DateTimeOffset::operator>=( const DateTimeOffset& other ) const noexcept
	{
		return utcTicks() >= other.utcTicks();
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	inline DateTimeOffset DateTimeOffset::operator+( const TimeSpan& duration ) const noexcept
	{
		return DateTimeOffset{ m_dateTime + duration, m_offset };
	}

	inline DateTimeOffset DateTimeOffset::operator-( const TimeSpan& duration ) const noexcept
	{
		return DateTimeOffset{ m_dateTime - duration, m_offset };
	}

	inline TimeSpan DateTimeOffset::operator-( const DateTimeOffset& other ) const noexcept
	{
		// Compare UTC times by converting both to UTC ticks and subtracting
		return TimeSpan{ utcTicks() - other.utcTicks() };
	}

	inline DateTimeOffset& DateTimeOffset::operator+=( const TimeSpan& duration ) noexcept
	{
		m_dateTime += duration;

		return *this;
	}

	inline DateTimeOffset& DateTimeOffset::operator-=( const TimeSpan& duration ) noexcept
	{
		m_dateTime -= duration;

		return *this;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	inline constexpr const DateTime& DateTimeOffset::dateTime() const noexcept
	{
		return m_dateTime;
	}

	inline constexpr const TimeSpan& DateTimeOffset::offset() const noexcept
	{
		return m_offset;
	}

	inline constexpr std::int64_t DateTimeOffset::ticks() const noexcept
	{
		return m_dateTime.ticks();
	}

	inline std::int64_t DateTimeOffset::utcTicks() const noexcept
	{
		return m_dateTime.ticks() - m_offset.ticks();
	}

	inline std::int32_t DateTimeOffset::year() const noexcept
	{
		return m_dateTime.year();
	}

	inline std::int32_t DateTimeOffset::month() const noexcept
	{
		return m_dateTime.month();
	}

	inline std::int32_t DateTimeOffset::day() const noexcept
	{
		return m_dateTime.day();
	}

	inline std::int32_t DateTimeOffset::hour() const noexcept
	{
		return m_dateTime.hour();
	}

	inline std::int32_t DateTimeOffset::minute() const noexcept
	{
		return m_dateTime.minute();
	}

	inline std::int32_t DateTimeOffset::second() const noexcept
	{
		return m_dateTime.second();
	}

	inline std::int32_t DateTimeOffset::millisecond() const noexcept
	{
		return m_dateTime.millisecond();
	}

	inline std::int32_t DateTimeOffset::microsecond() const noexcept
	{
		// Extract microseconds from ticks (1 microsecond = 10 ticks)
		const auto remainderTicks{ m_dateTime.ticks() % 10000 };

		return static_cast<std::int32_t>( remainderTicks / 10 );
	}

	inline std::int32_t DateTimeOffset::nanosecond() const noexcept
	{
		// Extract nanosecond component (in hundreds of nanoseconds since ticks are 100ns units)
		const auto remainderTicks{ m_dateTime.ticks() % 10 };

		return static_cast<std::int32_t>( remainderTicks * 100 );
	}

	inline std::int32_t DateTimeOffset::dayOfWeek() const noexcept
	{
		return m_dateTime.dayOfWeek();
	}

	inline std::int32_t DateTimeOffset::dayOfYear() const noexcept
	{
		return m_dateTime.dayOfYear();
	}

	inline std::int32_t DateTimeOffset::totalOffsetMinutes() const noexcept
	{
		return static_cast<std::int32_t>( m_offset.totalMinutes() );
	}

	//----------------------------------------------
	// Conversion methods
	//----------------------------------------------

	inline std::int64_t DateTimeOffset::toUnixSeconds() const noexcept
	{
		return utcDateTime().toUnixSeconds();
	}

	inline std::int64_t DateTimeOffset::toUnixMilliseconds() const noexcept
	{
		return utcDateTime().toUnixMilliseconds();
	}

	inline TimeSpan DateTimeOffset::timeOfDay() const noexcept
	{
		return m_dateTime.timeOfDay();
	}

	//----------------------------------------------
	// Arithmetic methods
	//----------------------------------------------

	inline DateTimeOffset DateTimeOffset::add( const TimeSpan& value ) const noexcept
	{
		return *this + value;
	}

	inline TimeSpan DateTimeOffset::subtract( const DateTimeOffset& value ) const noexcept
	{
		return *this - value;
	}

	inline DateTimeOffset DateTimeOffset::subtract( const TimeSpan& value ) const noexcept
	{
		return *this - value;
	}

	inline DateTimeOffset DateTimeOffset::addTicks( std::int64_t ticks ) const noexcept
	{
		return *this + TimeSpan{ ticks };
	}

	//----------------------------------------------
	// Comparison methods
	//----------------------------------------------

	inline std::int32_t DateTimeOffset::compareTo( const DateTimeOffset& other ) const noexcept
	{
		const auto utcThis{ utcTicks() };
		const auto utcOther{ other.utcTicks() };

		if ( utcThis < utcOther )
		{
			return -1;
		}
		if ( utcThis > utcOther )
		{
			return 1;
		}

		return 0;
	}

	inline bool DateTimeOffset::equals( const DateTimeOffset& other ) const noexcept
	{
		return *this == other;
	}

	inline bool DateTimeOffset::equalsExact( const DateTimeOffset& other ) const noexcept
	{
		// Check both local time and offset are identical
		return m_dateTime == other.m_dateTime && m_offset == other.m_offset;
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	inline std::int32_t DateTimeOffset::compare( const DateTimeOffset& left, const DateTimeOffset& right ) noexcept
	{
		return left.compareTo( right );
	}
} // namespace nfx::datetime
