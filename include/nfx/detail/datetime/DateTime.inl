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
 * @file DateTime.inl
 * @brief Inline implementations for DateTime class performance-critical methods
 * @details Contains inline implementations of constructors, operators, and accessor methods
 *          that benefit from inlining for optimal performance. These are separated from the
 *          main header to improve compilation times while maintaining zero-cost abstractions.
 */

namespace nfx::datetime
{
	//=====================================================================
	// DateTime class - inline implementations
	//=====================================================================

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline bool DateTime::operator==( const DateTime& other ) const noexcept
	{
		return m_ticks == other.m_ticks;
	}

	inline bool DateTime::operator!=( const DateTime& other ) const noexcept
	{
		return m_ticks != other.m_ticks;
	}

	inline bool DateTime::operator<( const DateTime& other ) const noexcept
	{
		return m_ticks < other.m_ticks;
	}

	inline bool DateTime::operator<=( const DateTime& other ) const noexcept
	{
		return m_ticks <= other.m_ticks;
	}

	inline bool DateTime::operator>( const DateTime& other ) const noexcept
	{
		return m_ticks > other.m_ticks;
	}

	inline bool DateTime::operator>=( const DateTime& other ) const noexcept
	{
		return m_ticks >= other.m_ticks;
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	inline DateTime DateTime::operator+( const TimeSpan& duration ) const noexcept
	{
		return DateTime{ m_ticks + duration.ticks() };
	}

	inline DateTime DateTime::operator-( const TimeSpan& duration ) const noexcept
	{
		return DateTime{ m_ticks - duration.ticks() };
	}

	inline TimeSpan DateTime::operator-( const DateTime& other ) const noexcept
	{
		return TimeSpan{ m_ticks - other.m_ticks };
	}

	inline DateTime& DateTime::operator+=( const TimeSpan& duration ) noexcept
	{
		m_ticks += duration.ticks();

		return *this;
	}

	inline DateTime& DateTime::operator-=( const TimeSpan& duration ) noexcept
	{
		m_ticks -= duration.ticks();

		return *this;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	inline constexpr std::int64_t DateTime::ticks() const noexcept
	{
		return m_ticks;
	}

	//----------------------------------------------
	// Validation methods
	//----------------------------------------------

	inline constexpr bool DateTime::isLeapYear( std::int32_t year ) noexcept
	{
		return ( year % 4 == 0 && year % 100 != 0 ) || ( year % 400 == 0 );
	}

	inline constexpr std::int32_t DateTime::daysInMonth( std::int32_t year, std::int32_t month ) noexcept
	{
		if ( month < 1 || month > 12 )
		{
			return 0;
		}

		if ( month == 2 && isLeapYear( year ) )
		{
			return 29;
		}

		switch ( month )
		{
			case 1:
			{
				return 31;
			}
			case 2:
			{
				return 28; // Non-leap year February
			}
			case 3:
			{
				return 31;
			}
			case 4:
			{
				return 30;
			}
			case 5:
			{
				return 31;
			}
			case 6:
			{
				return 30;
			}
			case 7:
			{
				return 31;
			}
			case 8:
			{
				return 31;
			}
			case 9:
			{
				return 30;
			}
			case 10:
			{
				return 31;
			}
			case 11:
			{
				return 30;
			}
			case 12:
			{
				return 31;
			}
			default:
			{
				return 0;
			}
		}
	}
} // namespace nfx::datetime
