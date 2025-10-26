/**
 * @file TESTS_DateTime.cpp
 * @brief Comprehensive tests for DateTime class
 * @details Tests UTC-only datetime operations with 100-nanosecond precision,
 *          ISO 8601 parsing/formatting, arithmetic operations, and cross-platform compatibility
 */

#include <gtest/gtest.h>

#include <sstream>

#include <nfx/datetime/DateTime.h>

namespace nfx::datetime::test
{
	//=====================================================================
	// DateTime type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( DateTimeConstruction, DefaultConstructor )
	{
		DateTime dt;
		EXPECT_EQ( dt.ticks(), DateTime::minValue().ticks() );
		EXPECT_EQ( dt.year(), 1 );
		EXPECT_EQ( dt.month(), 1 );
		EXPECT_EQ( dt.day(), 1 );
	}

	TEST( DateTimeConstruction, FromTicks )
	{
		std::int64_t ticks{ 638400000000000000LL };
		DateTime dt{ ticks };
		EXPECT_EQ( dt.ticks(), ticks );
	}

	TEST( DateTimeConstruction, FromChrono )
	{
		auto now{ std::chrono::system_clock::now() };
		DateTime dt{ now };
		EXPECT_GT( dt.ticks(), 0 );
	}

	TEST( DateTimeConstruction, FromDateComponents )
	{
		DateTime dt{ 2024, 6, 15 };
		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 6 );
		EXPECT_EQ( dt.day(), 15 );
		EXPECT_EQ( dt.hour(), 0 );
		EXPECT_EQ( dt.minute(), 0 );
		EXPECT_EQ( dt.second(), 0 );
	}

	TEST( DateTimeConstruction, FromDateTimeComponents )
	{
		DateTime dt{ 2024, 12, 25, 18, 45, 30 };
		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 12 );
		EXPECT_EQ( dt.day(), 25 );
		EXPECT_EQ( dt.hour(), 18 );
		EXPECT_EQ( dt.minute(), 45 );
		EXPECT_EQ( dt.second(), 30 );
	}

	TEST( DateTimeConstruction, FromDateTimeWithMilliseconds )
	{
		DateTime dt{ 2024, 3, 10, 9, 15, 22, 500 };
		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 3 );
		EXPECT_EQ( dt.day(), 10 );
		EXPECT_EQ( dt.hour(), 9 );
		EXPECT_EQ( dt.minute(), 15 );
		EXPECT_EQ( dt.second(), 22 );
		EXPECT_EQ( dt.millisecond(), 500 );
	}

	TEST( DateTimeConstruction, FromIso8601String )
	{
		DateTime dt{ "2024-01-15T12:30:45Z" };
		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 1 );
		EXPECT_EQ( dt.day(), 15 );
		EXPECT_EQ( dt.hour(), 12 );
		EXPECT_EQ( dt.minute(), 30 );
		EXPECT_EQ( dt.second(), 45 );
	}

	TEST( DateTimeConstruction, CopyConstructor )
	{
		DateTime dt1{ 2024, 5, 15, 10, 20, 30 };
		DateTime dt2{ dt1 };
		EXPECT_EQ( dt1.ticks(), dt2.ticks() );
	}

	TEST( DateTimeConstruction, MoveConstructor )
	{
		DateTime dt1{ 2024, 5, 15, 10, 20, 30 };
		std::int64_t originalTicks{ dt1.ticks() };
		DateTime dt2{ std::move( dt1 ) };
		EXPECT_EQ( dt2.ticks(), originalTicks );
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	TEST( DateTimeAssignment, CopyAssignment )
	{
		DateTime dt1{ 2024, 5, 15, 10, 20, 30 };
		DateTime dt2;
		dt2 = dt1;
		EXPECT_EQ( dt1.ticks(), dt2.ticks() );
	}

	TEST( DateTimeAssignment, MoveAssignment )
	{
		DateTime dt1{ 2024, 5, 15, 10, 20, 30 };
		std::int64_t originalTicks{ dt1.ticks() };
		DateTime dt2;
		dt2 = std::move( dt1 );
		EXPECT_EQ( dt2.ticks(), originalTicks );
	}

	//----------------------------------------------
	// Comparison Operators
	//----------------------------------------------

	TEST( DateTimeComparison, EqualityOperator )
	{
		DateTime dt1{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt3{ 2024, 1, 15, 13, 0, 0 };

		EXPECT_TRUE( dt1 == dt2 );
		EXPECT_FALSE( dt1 == dt3 );
	}

	TEST( DateTimeComparison, InequalityOperator )
	{
		DateTime dt1{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 13, 0, 0 };

		EXPECT_TRUE( dt1 != dt2 );
		EXPECT_FALSE( dt1 != dt1 );
	}

	TEST( DateTimeComparison, LessThanOperator )
	{
		DateTime dt1{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 13, 0, 0 };

		EXPECT_TRUE( dt1 < dt2 );
		EXPECT_FALSE( dt2 < dt1 );
		EXPECT_FALSE( dt1 < dt1 );
	}

	TEST( DateTimeComparison, LessThanOrEqualOperator )
	{
		DateTime dt1{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 13, 0, 0 };

		EXPECT_TRUE( dt1 <= dt2 );
		EXPECT_TRUE( dt1 <= dt1 );
		EXPECT_FALSE( dt2 <= dt1 );
	}

	TEST( DateTimeComparison, GreaterThanOperator )
	{
		DateTime dt1{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 13, 0, 0 };

		EXPECT_TRUE( dt2 > dt1 );
		EXPECT_FALSE( dt1 > dt2 );
		EXPECT_FALSE( dt1 > dt1 );
	}

	TEST( DateTimeComparison, GreaterThanOrEqualOperator )
	{
		DateTime dt1{ 2024, 1, 15, 12, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 13, 0, 0 };

		EXPECT_TRUE( dt2 >= dt1 );
		EXPECT_TRUE( dt1 >= dt1 );
		EXPECT_FALSE( dt1 >= dt2 );
	}

	//----------------------------------------------
	// Arithmetic Operators
	//----------------------------------------------

	TEST( DateTimeArithmetic, AddTimeSpan )
	{
		DateTime dt{ 2024, 1, 15, 12, 0, 0 };
		TimeSpan duration{ TimeSpan::fromHours( 3.0 ) };
		DateTime result{ dt + duration };

		EXPECT_EQ( result.hour(), 15 );
		EXPECT_EQ( result.day(), 15 );
	}

	TEST( DateTimeArithmetic, SubtractTimeSpan )
	{
		DateTime dt{ 2024, 1, 15, 12, 0, 0 };
		TimeSpan duration{ TimeSpan::fromHours( 2.0 ) };
		DateTime result{ dt - duration };

		EXPECT_EQ( result.hour(), 10 );
		EXPECT_EQ( result.day(), 15 );
	}

	TEST( DateTimeArithmetic, SubtractDateTime )
	{
		DateTime dt1{ 2024, 1, 15, 15, 0, 0 };
		DateTime dt2{ 2024, 1, 15, 12, 0, 0 };
		TimeSpan difference{ dt1 - dt2 };

		EXPECT_EQ( difference.totalHours(), 3.0 );
	}

	TEST( DateTimeArithmetic, AddAssignTimeSpan )
	{
		DateTime dt{ 2024, 1, 15, 12, 0, 0 };
		TimeSpan duration{ TimeSpan::fromMinutes( 45.0 ) };
		dt += duration;

		EXPECT_EQ( dt.hour(), 12 );
		EXPECT_EQ( dt.minute(), 45 );
	}

	TEST( DateTimeArithmetic, SubtractAssignTimeSpan )
	{
		DateTime dt{ 2024, 1, 15, 12, 30, 0 };
		TimeSpan duration{ TimeSpan::fromMinutes( 30.0 ) };
		dt -= duration;

		EXPECT_EQ( dt.hour(), 12 );
		EXPECT_EQ( dt.minute(), 0 );
	}

	//----------------------------------------------
	// Property Accessors
	//----------------------------------------------

	TEST( DateTimeAccessors, ComponentAccessors )
	{
		DateTime dt{ 2024, 3, 15, 14, 30, 45, 123 };

		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 3 );
		EXPECT_EQ( dt.day(), 15 );
		EXPECT_EQ( dt.hour(), 14 );
		EXPECT_EQ( dt.minute(), 30 );
		EXPECT_EQ( dt.second(), 45 );
		EXPECT_EQ( dt.millisecond(), 123 );
	}

	TEST( DateTimeAccessors, TicksAccessor )
	{
		DateTime dt{ 2024, 1, 1, 0, 0, 0 };
		EXPECT_GT( dt.ticks(), 0 );
	}

	TEST( DateTimeAccessors, DayOfWeekAndDayOfYear )
	{
		// January 15, 2024 is a Monday (day 1)
		DateTime dt{ 2024, 1, 15, 12, 0, 0 };

		EXPECT_EQ( dt.dayOfWeek(), 1 );	 // Monday
		EXPECT_EQ( dt.dayOfYear(), 15 ); // 15th day of year
	}

	//----------------------------------------------
	// Conversion Methods
	//----------------------------------------------

	TEST( DateTimeConversion, ToUnixSeconds )
	{
		// Unix epoch: 1970-01-01 00:00:00 UTC
		DateTime epoch{ DateTime::epoch() };
		EXPECT_EQ( epoch.toUnixSeconds(), 0 );

		DateTime dt{ 1970, 1, 1, 1, 0, 0 };
		EXPECT_EQ( dt.toUnixSeconds(), 3600 ); // 1 hour after epoch
	}

	TEST( DateTimeConversion, ToUnixMilliseconds )
	{
		DateTime epoch{ DateTime::epoch() };
		EXPECT_EQ( epoch.toUnixMilliseconds(), 0 );

		DateTime dt{ 1970, 1, 1, 0, 0, 1, 500 };
		EXPECT_EQ( dt.toUnixMilliseconds(), 1500 ); // 1.5 seconds = 1500 ms
	}

	TEST( DateTimeConversion, DateAndTimeOfDay )
	{
		DateTime dt{ 2024, 3, 15, 14, 30, 45 };

		DateTime dateOnly{ dt.date() };
		EXPECT_EQ( dateOnly.hour(), 0 );
		EXPECT_EQ( dateOnly.minute(), 0 );
		EXPECT_EQ( dateOnly.second(), 0 );
		EXPECT_EQ( dateOnly.day(), 15 );

		TimeSpan timeOfDay{ dt.timeOfDay() };
		EXPECT_NEAR( timeOfDay.totalHours(), 14.0 + 30.0 / 60.0 + 45.0 / 3600.0, 0.001 );
	}

	//----------------------------------------------
	// String Formatting
	//----------------------------------------------

	TEST( DateTimeStringFormatting, ToStringIso8601Basic )
	{
		DateTime dt{ 2024, 1, 15, 12, 30, 45 };
		std::string str{ dt.toString() };

		// Should be in format: 2024-01-15T12:30:45Z
		EXPECT_NE( str.find( "2024-01-15" ), std::string::npos );
		EXPECT_NE( str.find( "12:30:45" ), std::string::npos );
		EXPECT_NE( str.find( "Z" ), std::string::npos );
	}

	TEST( DateTimeStringFormatting, toIso8601Extended )
	{
		DateTime dt{ 2024, 3, 10, 9, 15, 22, 123 };
		std::string str{ dt.toIso8601Extended() };

		// Should include fractional seconds
		EXPECT_NE( str.find( "22." ), std::string::npos );
		EXPECT_NE( str.find( "Z" ), std::string::npos );
	}

	TEST( DateTimeStringFormatting, ToStringDateOnly )
	{
		DateTime dt{ 2024, 6, 20, 18, 45, 30 };
		std::string str{ dt.toString( DateTime::Format::DateOnly ) };

		EXPECT_NE( str.find( "2024-06-20" ), std::string::npos );
		EXPECT_EQ( str.find( "T" ), std::string::npos ); // No time component
	}

	TEST( DateTimeStringFormatting, ToStringTimeOnly )
	{
		DateTime dt{ 2024, 6, 20, 18, 45, 30 };
		std::string str{ dt.toString( DateTime::Format::TimeOnly ) };

		EXPECT_NE( str.find( "18:45:30" ), std::string::npos );
		EXPECT_EQ( str.find( "2024" ), std::string::npos ); // No date component
	}

	//----------------------------------------------
	// Validation Methods
	//----------------------------------------------

	TEST( DateTimeValidation, IsValid )
	{
		DateTime dt1{ 2024, 1, 15, 12, 30, 45 };
		EXPECT_TRUE( dt1.isValid() );

		DateTime dt2{ DateTime::minValue() };
		EXPECT_TRUE( dt2.isValid() );

		DateTime dt3{ DateTime::maxValue() };
		EXPECT_TRUE( dt3.isValid() );
	}

	TEST( DateTimeValidation, IsLeapYear )
	{
		EXPECT_TRUE( DateTime::isLeapYear( 2024 ) );  // Divisible by 4
		EXPECT_FALSE( DateTime::isLeapYear( 2023 ) ); // Not divisible by 4
		EXPECT_FALSE( DateTime::isLeapYear( 1900 ) ); // Divisible by 100, not by 400
		EXPECT_TRUE( DateTime::isLeapYear( 2000 ) );  // Divisible by 400
	}

	TEST( DateTimeValidation, DaysInMonth )
	{
		// Regular months
		EXPECT_EQ( DateTime::daysInMonth( 2024, 1 ), 31 );	// January
		EXPECT_EQ( DateTime::daysInMonth( 2024, 4 ), 30 );	// April
		EXPECT_EQ( DateTime::daysInMonth( 2024, 12 ), 31 ); // December

		// February in leap year
		EXPECT_EQ( DateTime::daysInMonth( 2024, 2 ), 29 );

		// February in non-leap year
		EXPECT_EQ( DateTime::daysInMonth( 2023, 2 ), 28 );
	}

	//----------------------------------------------
	// Static Factory Methods
	//----------------------------------------------

	TEST( DateTimeFactory, Now )
	{
		DateTime now{ DateTime::now() };
		EXPECT_GT( now.ticks(), 0 );
		EXPECT_GE( now.year(), 2024 );
	}

	TEST( DateTimeFactory, Today )
	{
		DateTime today{ DateTime::today() };
		EXPECT_EQ( today.hour(), 0 );
		EXPECT_EQ( today.minute(), 0 );
		EXPECT_EQ( today.second(), 0 );
		EXPECT_GE( today.year(), 2024 );
	}

	TEST( DateTimeFactory, MinMaxValues )
	{
		DateTime minVal{ DateTime::minValue() };
		DateTime maxVal{ DateTime::maxValue() };

		EXPECT_EQ( minVal.year(), 1 );
		EXPECT_EQ( minVal.month(), 1 );
		EXPECT_EQ( minVal.day(), 1 );

		EXPECT_EQ( maxVal.year(), 9999 );
		EXPECT_EQ( maxVal.month(), 12 );
		EXPECT_EQ( maxVal.day(), 31 );
	}

	TEST( DateTimeFactory, Epoch )
	{
		DateTime epoch{ DateTime::epoch() };

		EXPECT_EQ( epoch.year(), 1970 );
		EXPECT_EQ( epoch.month(), 1 );
		EXPECT_EQ( epoch.day(), 1 );
		EXPECT_EQ( epoch.hour(), 0 );
		EXPECT_EQ( epoch.minute(), 0 );
		EXPECT_EQ( epoch.second(), 0 );
	}

	TEST( DateTimeFactory, SinceEpochSeconds )
	{
		DateTime dt{ DateTime::sinceEpochSeconds( 3600 ) }; // 1 hour after epoch

		EXPECT_EQ( dt.year(), 1970 );
		EXPECT_EQ( dt.month(), 1 );
		EXPECT_EQ( dt.day(), 1 );
		EXPECT_EQ( dt.hour(), 1 );
	}

	TEST( DateTimeFactory, SinceEpochMilliseconds )
	{
		DateTime dt{ DateTime::sinceEpochMilliseconds( 1500 ) }; // 1.5 seconds

		EXPECT_EQ( dt.second(), 1 );
		EXPECT_EQ( dt.millisecond(), 500 );
	}

	//----------------------------------------------
	// String Parsing
	//----------------------------------------------

	TEST( DateTimeStringParsing, ParseIso8601Basic )
	{
		DateTime dt{ DateTime::parse( "2024-01-15T12:30:45Z" ) };

		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 1 );
		EXPECT_EQ( dt.day(), 15 );
		EXPECT_EQ( dt.hour(), 12 );
		EXPECT_EQ( dt.minute(), 30 );
		EXPECT_EQ( dt.second(), 45 );
	}

	TEST( DateTimeStringParsing, ParseIso8601Extended )
	{
		DateTime dt{ DateTime::parse( "2024-01-15T12:30:45.123Z" ) };

		EXPECT_EQ( dt.second(), 45 );
		EXPECT_EQ( dt.millisecond(), 123 );
	}

	TEST( DateTimeStringParsing, ParseDateOnly )
	{
		DateTime dt{ DateTime::parse( "2024-06-20" ) };

		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.month(), 6 );
		EXPECT_EQ( dt.day(), 20 );
		EXPECT_EQ( dt.hour(), 0 );
		EXPECT_EQ( dt.minute(), 0 );
		EXPECT_EQ( dt.second(), 0 );
	}

	TEST( DateTimeStringParsing, TryParseValid )
	{
		DateTime dt;
		bool success{ DateTime::tryParse( "2024-01-15T12:30:45Z", dt ) };

		EXPECT_TRUE( success );
		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.hour(), 12 );
	}

	TEST( DateTimeStringParsing, TryParseInvalid )
	{
		DateTime dt;
		bool success{ DateTime::tryParse( "invalid-date-string", dt ) };

		EXPECT_FALSE( success );
	}

	TEST( DateTimeStringParsing, ParseThrowsOnInvalid )
	{
		EXPECT_THROW( [[maybe_unused]] auto _ = DateTime::parse( "not-a-date" ), std::invalid_argument );
	}

	//----------------------------------------------
	// std::chrono Interoperability
	//----------------------------------------------

	TEST( DateTimeChrono, ToChrono )
	{
		DateTime dt{ 2024, 6, 15, 12, 30, 45 };
		auto chronoTimePoint{ dt.toChrono() };

		// Convert back and verify
		DateTime roundTrip{ DateTime::fromChrono( chronoTimePoint ) };
		EXPECT_EQ( roundTrip.year(), dt.year() );
		EXPECT_EQ( roundTrip.month(), dt.month() );
		EXPECT_EQ( roundTrip.day(), dt.day() );
	}

	TEST( DateTimeChrono, FromChrono )
	{
		auto now{ std::chrono::system_clock::now() };
		DateTime dt{ DateTime::fromChrono( now ) };

		EXPECT_GT( dt.ticks(), 0 );
		EXPECT_GE( dt.year(), 2024 );
	}

	TEST( DateTimeChrono, RoundTripConversion )
	{
		DateTime original{ 2024, 3, 10, 14, 30, 22 };
		auto chronoTimePoint{ original.toChrono() };
		DateTime roundTrip{ DateTime::fromChrono( chronoTimePoint ) };

		// Should match (may have minor precision differences)
		EXPECT_EQ( roundTrip.year(), original.year() );
		EXPECT_EQ( roundTrip.month(), original.month() );
		EXPECT_EQ( roundTrip.day(), original.day() );
		EXPECT_EQ( roundTrip.hour(), original.hour() );
		EXPECT_EQ( roundTrip.minute(), original.minute() );
		EXPECT_EQ( roundTrip.second(), original.second() );
	}

	//----------------------------------------------
	// Stream Operators
	//----------------------------------------------

	TEST( DateTimeStreamOperators, OutputOperator )
	{
		DateTime dt{ 2024, 1, 15, 12, 30, 45 };
		std::ostringstream oss;
		oss << dt;

		std::string output{ oss.str() };
		EXPECT_NE( output.find( "2024" ), std::string::npos );
		EXPECT_NE( output.find( "12:30:45" ), std::string::npos );
	}

	TEST( DateTimeStreamOperators, InputOperator )
	{
		std::istringstream iss{ "2024-01-15T12:30:45Z" };
		DateTime dt;
		iss >> dt;

		EXPECT_EQ( dt.year(), 2024 );
		EXPECT_EQ( dt.hour(), 12 );
	}

	//----------------------------------------------
	// Edge Cases and Integration Tests
	//----------------------------------------------

	TEST( DateTimeEdgeCases, DateTimeCrossingDayBoundary )
	{
		// 23:00 + 2 hours = next day 01:00
		DateTime dt{ 2024, 1, 15, 23, 0, 0 };
		DateTime result{ dt + TimeSpan::fromHours( 2.0 ) };

		EXPECT_EQ( result.day(), 16 );
		EXPECT_EQ( result.hour(), 1 );
	}

	TEST( DateTimeEdgeCases, DateTimeCrossingMonthBoundary )
	{
		// January 31 + 1 day = February 1
		DateTime dt{ 2024, 1, 31, 12, 0, 0 };
		DateTime result{ dt + TimeSpan::fromDays( 1.0 ) };

		EXPECT_EQ( result.month(), 2 );
		EXPECT_EQ( result.day(), 1 );
	}

	TEST( DateTimeEdgeCases, DateTimeCrossingYearBoundary )
	{
		// December 31, 2023 + 1 day = January 1, 2024
		DateTime dt{ 2023, 12, 31, 23, 59, 59 };
		DateTime result{ dt + TimeSpan::fromSeconds( 1.0 ) };

		EXPECT_EQ( result.year(), 2024 );
		EXPECT_EQ( result.month(), 1 );
		EXPECT_EQ( result.day(), 1 );
		EXPECT_EQ( result.hour(), 0 );
		EXPECT_EQ( result.minute(), 0 );
		EXPECT_EQ( result.second(), 0 );
	}

	TEST( DateTimeEdgeCases, LeapYearFebruary29 )
	{
		// February 29, 2024 (leap year)
		DateTime dt{ 2024, 2, 29, 12, 0, 0 };

		EXPECT_EQ( dt.month(), 2 );
		EXPECT_EQ( dt.day(), 29 );
		EXPECT_TRUE( DateTime::isLeapYear( 2024 ) );
	}

	TEST( DateTimeEdgeCases, MinValueOperations )
	{
		DateTime minVal{ DateTime::minValue() };

		EXPECT_EQ( minVal.year(), 1 );
		EXPECT_EQ( minVal.month(), 1 );
		EXPECT_EQ( minVal.day(), 1 );

		// Adding time should work
		DateTime plusOneDay{ minVal + TimeSpan::fromDays( 1.0 ) };
		EXPECT_EQ( plusOneDay.day(), 2 );
	}

	TEST( DateTimeEdgeCases, MaxValueOperations )
	{
		DateTime maxVal{ DateTime::maxValue() };

		EXPECT_EQ( maxVal.year(), 9999 );
		EXPECT_EQ( maxVal.month(), 12 );
		EXPECT_EQ( maxVal.day(), 31 );

		// Subtracting time should work
		DateTime minusOneDay{ maxVal - TimeSpan::fromDays( 1.0 ) };
		EXPECT_EQ( minusOneDay.day(), 30 );
	}

	//----------------------------------------------
	// Integration Tests
	//----------------------------------------------

	TEST( DateTimeIntegration, RoundTripSerialization )
	{
		DateTime original{ 2024, 3, 15, 14, 30, 45, 123 };

		// Convert to string and back
		std::string serialized{ original.toString() };
		DateTime deserialized{ DateTime::parse( serialized ) };

		// Should match (basic format doesn't include milliseconds)
		EXPECT_EQ( deserialized.year(), original.year() );
		EXPECT_EQ( deserialized.month(), original.month() );
		EXPECT_EQ( deserialized.day(), original.day() );
		EXPECT_EQ( deserialized.hour(), original.hour() );
		EXPECT_EQ( deserialized.minute(), original.minute() );
		EXPECT_EQ( deserialized.second(), original.second() );
	}

	TEST( DateTimeIntegration, RoundTripExtendedSerialization )
	{
		DateTime original{ 2024, 3, 15, 14, 30, 45, 123 };

		// Convert to extended string and back
		std::string serialized{ original.toIso8601Extended() };
		DateTime deserialized{ DateTime::parse( serialized ) };

		// Should match exactly including milliseconds
		EXPECT_EQ( deserialized.year(), original.year() );
		EXPECT_EQ( deserialized.month(), original.month() );
		EXPECT_EQ( deserialized.day(), original.day() );
		EXPECT_EQ( deserialized.hour(), original.hour() );
		EXPECT_EQ( deserialized.minute(), original.minute() );
		EXPECT_EQ( deserialized.second(), original.second() );
		EXPECT_EQ( deserialized.millisecond(), original.millisecond() );
	}

	TEST( DateTimeIntegration, ArithmeticChaining )
	{
		// Test complex arithmetic operations
		DateTime dt{ 2024, 1, 1, 0, 0, 0 };
		DateTime result{ dt + TimeSpan::fromDays( 5.0 ) + TimeSpan::fromHours( 3.0 ) + TimeSpan::fromMinutes( 30.0 ) };

		EXPECT_EQ( result.day(), 6 );
		EXPECT_EQ( result.hour(), 3 );
		EXPECT_EQ( result.minute(), 30 );
	}

	TEST( DateTimeIntegration, ComparisonWithDifferentDates )
	{
		DateTime earlier{ 2023, 12, 31, 23, 59, 59 };
		DateTime later{ 2024, 1, 1, 0, 0, 0 };

		EXPECT_TRUE( earlier < later );
		EXPECT_TRUE( later > earlier );
		EXPECT_TRUE( earlier != later );
		EXPECT_FALSE( earlier == later );
	}

	TEST( DateTimeIntegration, UnixTimestampRoundTrip )
	{
		std::int64_t unixSeconds{ 1704110400 }; // Some arbitrary Unix timestamp
		DateTime dt{ DateTime::sinceEpochSeconds( unixSeconds ) };
		std::int64_t convertedBack{ dt.toUnixSeconds() };

		EXPECT_EQ( convertedBack, unixSeconds );
	}
} // namespace nfx::datetime::test
