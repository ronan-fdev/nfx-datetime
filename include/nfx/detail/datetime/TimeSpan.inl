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
 * @file TimeSpan.inl
 * @brief Inline implementations for TimeSpan class performance-critical methods
 * @details Contains inline implementations of constructors, operators, and accessor methods
 *          for time duration operations. These are separated from the main header to improve
 *          compilation times while maintaining zero-cost abstractions and optimal performance
 *          for arithmetic operations and time unit conversions.
 */

namespace nfx::datetime
{
	//=====================================================================
	// TimeSpan class - inline implementations
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline constexpr TimeSpan::TimeSpan( std::int64_t ticks ) noexcept
		: m_ticks{ ticks }
	{
	}

	inline TimeSpan::TimeSpan( std::chrono::system_clock::duration duration ) noexcept
		: m_ticks{ std::chrono::duration_cast<std::chrono::nanoseconds>( duration ).count() / 100 }
	{
	}

	//----------------------------------------------
	// Comparison operators
	//----------------------------------------------

	inline constexpr bool TimeSpan::operator==( const TimeSpan& other ) const noexcept
	{
		return m_ticks == other.m_ticks;
	}

	inline constexpr bool TimeSpan::operator!=( const TimeSpan& other ) const noexcept
	{
		return m_ticks != other.m_ticks;
	}

	inline constexpr bool TimeSpan::operator<( const TimeSpan& other ) const noexcept
	{
		return m_ticks < other.m_ticks;
	}

	inline constexpr bool TimeSpan::operator<=( const TimeSpan& other ) const noexcept
	{
		return m_ticks <= other.m_ticks;
	}

	inline constexpr bool TimeSpan::operator>( const TimeSpan& other ) const noexcept
	{
		return m_ticks > other.m_ticks;
	}

	inline constexpr bool TimeSpan::operator>=( const TimeSpan& other ) const noexcept
	{
		return m_ticks >= other.m_ticks;
	}

	//----------------------------------------------
	// Arithmetic operators
	//----------------------------------------------

	inline constexpr TimeSpan TimeSpan::operator+( const TimeSpan& other ) const noexcept
	{
		return TimeSpan{ m_ticks + other.m_ticks };
	}

	inline constexpr TimeSpan TimeSpan::operator-( const TimeSpan& other ) const noexcept
	{
		return TimeSpan{ m_ticks - other.m_ticks };
	}

	inline constexpr TimeSpan TimeSpan::operator-() const noexcept
	{
		return TimeSpan{ -m_ticks };
	}

	inline constexpr TimeSpan& TimeSpan::operator+=( const TimeSpan& other ) noexcept
	{
		m_ticks += other.m_ticks;

		return *this;
	}

	inline constexpr TimeSpan& TimeSpan::operator-=( const TimeSpan& other ) noexcept
	{
		m_ticks -= other.m_ticks;

		return *this;
	}

	//----------------------------------------------
	// Property accessors
	//----------------------------------------------

	inline constexpr std::int64_t TimeSpan::ticks() const noexcept
	{
		return m_ticks;
	}
} // namespace nfx::datetime
