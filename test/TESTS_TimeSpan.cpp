/**
 * @file TESTS_TimeSpan.cpp
 * @brief Comprehensive tests for TimeSpan class
 * @details Tests high-precision time duration operations with ISO 8601 duration format support,
 *          arithmetic operations, and conversions between various time units
 */

#include <gtest/gtest.h>

#include <sstream>

#include <nfx/datetime/TimeSpan.h>

namespace nfx::datetime::test
{
	//=====================================================================
	// TimeSpan type tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( TimeSpanConstruction, FromTicks )
	{
		// Default constructor (zero duration)
		TimeSpan ts1;
		EXPECT_EQ( ts1.ticks(), 0 );

		// Constructor from ticks
		TimeSpan ts2{ 10000000 }; // 1 second in ticks
		EXPECT_EQ( ts2.ticks(), 10000000 );
		EXPECT_DOUBLE_EQ( ts2.totalSeconds(), 1.0 );

		// Copy constructor
		TimeSpan ts3{ ts2 };
		EXPECT_EQ( ts3.ticks(), ts2.ticks() );

		// Move constructor
		TimeSpan ts4{ std::move( ts3 ) };
		EXPECT_EQ( ts4.ticks(), 10000000 );
	}

	TEST( TimeSpanConstruction, FromChrono )
	{
		// Construct from std::chrono::seconds
		auto chronoSeconds{ std::chrono::seconds( 60 ) };
		TimeSpan ts1{ chronoSeconds };
		EXPECT_DOUBLE_EQ( ts1.totalSeconds(), 60.0 );

		// Construct from std::chrono::milliseconds
		auto chronoMillis{ std::chrono::milliseconds( 1500 ) };
		TimeSpan ts2{ chronoMillis };
		EXPECT_DOUBLE_EQ( ts2.totalMilliseconds(), 1500.0 );
		EXPECT_DOUBLE_EQ( ts2.totalSeconds(), 1.5 );

		// Construct from std::chrono::hours
		auto chronoHours{ std::chrono::hours( 2 ) };
		TimeSpan ts3{ chronoHours };
		EXPECT_DOUBLE_EQ( ts3.totalHours(), 2.0 );

		// Construct from std::chrono::minutes
		auto chronoMinutes{ std::chrono::minutes( 45 ) };
		TimeSpan ts4{ chronoMinutes };
		EXPECT_DOUBLE_EQ( ts4.totalMinutes(), 45.0 );
	}

	//----------------------------------------------
	// Assignment
	//----------------------------------------------

	TEST( TimeSpanAssignment, Assignment )
	{
		TimeSpan ts1{ 10000000 };
		TimeSpan ts2;

		// Copy assignment
		ts2 = ts1;
		EXPECT_EQ( ts2.ticks(), ts1.ticks() );

		// Move assignment
		TimeSpan ts3;
		ts3 = std::move( ts2 );
		EXPECT_EQ( ts3.ticks(), 10000000 );
	}

	//----------------------------------------------
	// Operations
	//----------------------------------------------

	TEST( TimeSpanOperations, ComparisonOperators )
	{
		TimeSpan ts1{ TimeSpan::fromSeconds( 10 ) };
		TimeSpan ts2{ TimeSpan::fromSeconds( 20 ) };
		TimeSpan ts3{ TimeSpan::fromSeconds( 10 ) };

		// Equality
		EXPECT_TRUE( ts1 == ts3 );
		EXPECT_FALSE( ts1 == ts2 );

		// Inequality
		EXPECT_TRUE( ts1 != ts2 );
		EXPECT_FALSE( ts1 != ts3 );

		// Less than
		EXPECT_TRUE( ts1 < ts2 );
		EXPECT_FALSE( ts2 < ts1 );
		EXPECT_FALSE( ts1 < ts3 );

		// Less than or equal
		EXPECT_TRUE( ts1 <= ts2 );
		EXPECT_TRUE( ts1 <= ts3 );
		EXPECT_FALSE( ts2 <= ts1 );

		// Greater than
		EXPECT_TRUE( ts2 > ts1 );
		EXPECT_FALSE( ts1 > ts2 );
		EXPECT_FALSE( ts1 > ts3 );

		// Greater than or equal
		EXPECT_TRUE( ts2 >= ts1 );
		EXPECT_TRUE( ts1 >= ts3 );
		EXPECT_FALSE( ts1 >= ts2 );
	}

	TEST( TimeSpanOperations, ArithmeticOperators )
	{
		TimeSpan ts1{ TimeSpan::fromMinutes( 30 ) };
		TimeSpan ts2{ TimeSpan::fromMinutes( 15 ) };

		// Addition
		TimeSpan sum{ ts1 + ts2 };
		EXPECT_DOUBLE_EQ( sum.totalMinutes(), 45.0 );

		// Subtraction
		TimeSpan diff{ ts1 - ts2 };
		EXPECT_DOUBLE_EQ( diff.totalMinutes(), 15.0 );

		// Negation
		TimeSpan neg{ -ts1 };
		EXPECT_DOUBLE_EQ( neg.totalMinutes(), -30.0 );

		// In-place addition
		TimeSpan ts3{ TimeSpan::fromHours( 1 ) };
		ts3 += TimeSpan::fromMinutes( 30 );
		EXPECT_DOUBLE_EQ( ts3.totalMinutes(), 90.0 );

		// In-place subtraction
		TimeSpan ts4{ TimeSpan::fromHours( 2 ) };
		ts4 -= TimeSpan::fromMinutes( 30 );
		EXPECT_DOUBLE_EQ( ts4.totalMinutes(), 90.0 );
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	TEST( TimeSpanAccessors, PropertyAccessors )
	{
		// Test various time units
		TimeSpan ts{ TimeSpan::fromHours( 2.5 ) };

		EXPECT_EQ( ts.ticks(), 2.5 * 60 * 60 * 10000000 );
		EXPECT_DOUBLE_EQ( ts.totalDays(), 2.5 / 24.0 );
		EXPECT_DOUBLE_EQ( ts.totalHours(), 2.5 );
		EXPECT_DOUBLE_EQ( ts.totalMinutes(), 150.0 );
		EXPECT_DOUBLE_EQ( ts.totalSeconds(), 9000.0 );
		EXPECT_DOUBLE_EQ( ts.totalMilliseconds(), 9000000.0 );

		// Test negative duration
		TimeSpan negTs{ TimeSpan::fromMinutes( -45 ) };
		EXPECT_DOUBLE_EQ( negTs.totalMinutes(), -45.0 );
		EXPECT_DOUBLE_EQ( negTs.totalHours(), -0.75 );
	}

	//----------------------------------------------
	// String formatting
	//----------------------------------------------

	TEST( TimeSpanStringFormatting, ToStringCompactFormat )
	{
		// Test compact ISO 8601 format (omits zero components)
		EXPECT_EQ( TimeSpan::fromHours( 1 ).toString(), "PT1H" );
		EXPECT_EQ( TimeSpan::fromMinutes( 30 ).toString(), "PT30M" );
		EXPECT_EQ( TimeSpan::fromSeconds( 45 ).toString(), "PT45S" );

		// Combined durations (compact format)
		TimeSpan ts1{ TimeSpan::fromHours( 1 ) + TimeSpan::fromMinutes( 30 ) };
		EXPECT_EQ( ts1.toString(), "PT1H30M" );

		TimeSpan ts2{ TimeSpan::fromHours( 2 ) + TimeSpan::fromSeconds( 15 ) };
		EXPECT_EQ( ts2.toString(), "PT2H15S" );

		TimeSpan ts3{ TimeSpan::fromMinutes( 45 ) + TimeSpan::fromSeconds( 30 ) };
		EXPECT_EQ( ts3.toString(), "PT45M30S" );

		// Full combination
		TimeSpan ts4{ TimeSpan::fromHours( 1 ) + TimeSpan::fromMinutes( 30 ) + TimeSpan::fromSeconds( 45 ) };
		EXPECT_EQ( ts4.toString(), "PT1H30M45S" );

		// Zero duration (minimum required format)
		EXPECT_EQ( TimeSpan().toString(), "PT0S" );

		// Milliseconds (fractional seconds) in compact format
		TimeSpan ts5{ TimeSpan::fromSeconds( 1.5 ) };
		std::string result{ ts5.toString() };
		EXPECT_TRUE( result.find( "PT1." ) != std::string::npos || result.find( "PT1.5" ) != std::string::npos );
		EXPECT_TRUE( result.back() == 'S' );

		TimeSpan ts6{ TimeSpan::fromSeconds( 0.123 ) };
		std::string result2{ ts6.toString() };
		EXPECT_TRUE( result2.find( "PT0." ) != std::string::npos );
		EXPECT_TRUE( result2.back() == 'S' );
	}

	TEST( TimeSpanStringFormatting, ToStringNegativeDurations )
	{
		// Negative durations (sign before P)
		TimeSpan negHour{ TimeSpan::fromHours( -1 ) };
		EXPECT_EQ( negHour.toString(), "-PT1H" );

		TimeSpan negMinute{ TimeSpan::fromMinutes( -30 ) };
		EXPECT_EQ( negMinute.toString(), "-PT30M" );

		TimeSpan negCombined{ TimeSpan::fromHours( -2 ) + TimeSpan::fromMinutes( -15 ) };
		EXPECT_EQ( negCombined.toString(), "-PT2H15M" );
	}

	TEST( TimeSpanStringFormatting, ToStringWithDays )
	{
		// Durations with days component
		TimeSpan days{ TimeSpan::fromDays( 1 ) };
		std::string result{ days.toString() };
		EXPECT_TRUE( result.find( "P1D" ) != std::string::npos || result.find( "PT24H" ) != std::string::npos );

		TimeSpan daysAndTime{ TimeSpan::fromDays( 2 ) + TimeSpan::fromHours( 3 ) + TimeSpan::fromMinutes( 30 ) };
		result = daysAndTime.toString();
		EXPECT_TRUE( result.find( "P2DT3H30M" ) != std::string::npos || result.find( "PT51H30M" ) != std::string::npos );
	}

	//----------------------------------------------
	// String parsing - Compact and Verbose formats
	//----------------------------------------------

	TEST( TimeSpanStringParsing, ParseCompactFormat )
	{
		// Parse compact ISO 8601 format (omits zero components)
		TimeSpan ts1{ TimeSpan::parse( "PT1H" ) };
		EXPECT_DOUBLE_EQ( ts1.totalHours(), 1.0 );

		TimeSpan ts2{ TimeSpan::parse( "PT30M" ) };
		EXPECT_DOUBLE_EQ( ts2.totalMinutes(), 30.0 );

		TimeSpan ts3{ TimeSpan::parse( "PT45S" ) };
		EXPECT_DOUBLE_EQ( ts3.totalSeconds(), 45.0 );

		// Combined compact format
		TimeSpan ts4{ TimeSpan::parse( "PT1H30M" ) };
		EXPECT_DOUBLE_EQ( ts4.totalMinutes(), 90.0 );

		TimeSpan ts5{ TimeSpan::parse( "PT2H15S" ) };
		EXPECT_DOUBLE_EQ( ts5.totalSeconds(), 2 * 3600 + 15 );

		// Zero duration
		TimeSpan ts6{ TimeSpan::parse( "PT0S" ) };
		EXPECT_DOUBLE_EQ( ts6.totalSeconds(), 0.0 );
	}

	TEST( TimeSpanStringParsing, ParseVerboseFormat )
	{
		// Parse verbose ISO 8601 format (includes zero components)
		TimeSpan ts1{ TimeSpan::parse( "PT1H0M0S" ) };
		EXPECT_DOUBLE_EQ( ts1.totalHours(), 1.0 );

		TimeSpan ts2{ TimeSpan::parse( "PT0H30M0S" ) };
		EXPECT_DOUBLE_EQ( ts2.totalMinutes(), 30.0 );

		TimeSpan ts3{ TimeSpan::parse( "PT0H0M45S" ) };
		EXPECT_DOUBLE_EQ( ts3.totalSeconds(), 45.0 );

		// Combined verbose format
		TimeSpan ts4{ TimeSpan::parse( "PT1H30M0S" ) };
		EXPECT_DOUBLE_EQ( ts4.totalMinutes(), 90.0 );

		TimeSpan ts5{ TimeSpan::parse( "PT1H0M30S" ) };
		EXPECT_DOUBLE_EQ( ts5.totalSeconds(), 3600 + 30 );

		TimeSpan ts6{ TimeSpan::parse( "PT0H30M45S" ) };
		EXPECT_DOUBLE_EQ( ts6.totalSeconds(), 30 * 60 + 45 );

		// Full verbose format
		TimeSpan ts7{ TimeSpan::parse( "PT1H30M45S" ) };
		EXPECT_DOUBLE_EQ( ts7.totalSeconds(), 3600 + 30 * 60 + 45 );

		// Zero duration verbose
		TimeSpan ts8{ TimeSpan::parse( "PT0H0M0S" ) };
		EXPECT_DOUBLE_EQ( ts8.totalSeconds(), 0.0 );
	}

	TEST( TimeSpanStringParsing, ParseNegativeFormats )
	{
		// Negative durations (sign before P)
		TimeSpan ts1{ TimeSpan::parse( "-PT1H" ) };
		EXPECT_DOUBLE_EQ( ts1.totalHours(), -1.0 );

		TimeSpan ts2{ TimeSpan::parse( "-PT30M" ) };
		EXPECT_DOUBLE_EQ( ts2.totalMinutes(), -30.0 );

		TimeSpan ts3{ TimeSpan::parse( "-PT1H30M45S" ) };
		EXPECT_DOUBLE_EQ( ts3.totalSeconds(), -( 3600 + 30 * 60 + 45 ) );

		// Verbose negative format
		TimeSpan ts4{ TimeSpan::parse( "-PT1H0M0S" ) };
		EXPECT_DOUBLE_EQ( ts4.totalHours(), -1.0 );
	}

	TEST( TimeSpanStringParsing, ParseFractionalSeconds )
	{
		// Fractional seconds in various formats
		TimeSpan ts1{ TimeSpan::parse( "PT1.5S" ) };
		EXPECT_DOUBLE_EQ( ts1.totalSeconds(), 1.5 );

		TimeSpan ts2{ TimeSpan::parse( "PT0.123S" ) };
		EXPECT_NEAR( ts2.totalSeconds(), 0.123, 0.001 );

		TimeSpan ts3{ TimeSpan::parse( "PT1H30M45.500S" ) };
		EXPECT_NEAR( ts3.totalSeconds(), 3600 + 30 * 60 + 45.5, 0.001 );
	}

	TEST( TimeSpanStringParsing, ParseWithDays )
	{
		// ISO 8601 with days component
		TimeSpan ts1{ TimeSpan::parse( "P1D" ) };
		EXPECT_DOUBLE_EQ( ts1.totalDays(), 1.0 );

		TimeSpan ts2{ TimeSpan::parse( "P1DT12H" ) };
		EXPECT_DOUBLE_EQ( ts2.totalHours(), 36.0 );

		TimeSpan ts3{ TimeSpan::parse( "P2DT3H30M45S" ) };
		EXPECT_DOUBLE_EQ( ts3.totalSeconds(), 2 * 86400 + 3 * 3600 + 30 * 60 + 45 );

		// Days only (no time component)
		TimeSpan ts4{ TimeSpan::parse( "P5D" ) };
		EXPECT_DOUBLE_EQ( ts4.totalDays(), 5.0 );
	}

	TEST( TimeSpanStringParsing, TryParseCompactAndVerbose )
	{
		TimeSpan result;

		// Compact format
		EXPECT_TRUE( TimeSpan::tryParse( "PT1H", result ) );
		EXPECT_DOUBLE_EQ( result.totalHours(), 1.0 );

		EXPECT_TRUE( TimeSpan::tryParse( "PT30M", result ) );
		EXPECT_DOUBLE_EQ( result.totalMinutes(), 30.0 );

		EXPECT_TRUE( TimeSpan::tryParse( "PT45S", result ) );
		EXPECT_DOUBLE_EQ( result.totalSeconds(), 45.0 );

		// Verbose format
		EXPECT_TRUE( TimeSpan::tryParse( "PT1H0M0S", result ) );
		EXPECT_DOUBLE_EQ( result.totalHours(), 1.0 );

		EXPECT_TRUE( TimeSpan::tryParse( "PT0H30M0S", result ) );
		EXPECT_DOUBLE_EQ( result.totalMinutes(), 30.0 );

		EXPECT_TRUE( TimeSpan::tryParse( "PT0H0M45S", result ) );
		EXPECT_DOUBLE_EQ( result.totalSeconds(), 45.0 );

		// Mixed formats
		EXPECT_TRUE( TimeSpan::tryParse( "PT1H30M", result ) );
		EXPECT_DOUBLE_EQ( result.totalMinutes(), 90.0 );

		EXPECT_TRUE( TimeSpan::tryParse( "PT1H0M30S", result ) );
		EXPECT_DOUBLE_EQ( result.totalSeconds(), 3630.0 );

		// Negative formats
		EXPECT_TRUE( TimeSpan::tryParse( "-PT1H", result ) );
		EXPECT_DOUBLE_EQ( result.totalHours(), -1.0 );

		EXPECT_TRUE( TimeSpan::tryParse( "-PT1H0M0S", result ) );
		EXPECT_DOUBLE_EQ( result.totalHours(), -1.0 );
	}

	TEST( TimeSpanStringParsing, RoundTripCompactAndVerbose )
	{
		// Test that parsing both formats produces identical results
		TimeSpan compact1{ TimeSpan::parse( "PT1H" ) };
		TimeSpan verbose1{ TimeSpan::parse( "PT1H0M0S" ) };
		EXPECT_EQ( compact1.ticks(), verbose1.ticks() );

		TimeSpan compact2{ TimeSpan::parse( "PT30M" ) };
		TimeSpan verbose2{ TimeSpan::parse( "PT0H30M0S" ) };
		EXPECT_EQ( compact2.ticks(), verbose2.ticks() );

		TimeSpan compact3{ TimeSpan::parse( "PT45S" ) };
		TimeSpan verbose3{ TimeSpan::parse( "PT0H0M45S" ) };
		EXPECT_EQ( compact3.ticks(), verbose3.ticks() );

		TimeSpan compact4{ TimeSpan::parse( "PT1H30M45S" ) };
		TimeSpan verbose4{ TimeSpan::parse( "PT1H30M45S" ) };
		EXPECT_EQ( compact4.ticks(), verbose4.ticks() );

		// Round-trip: parse verbose, output compact (or vice versa)
		TimeSpan parsed{ TimeSpan::parse( "PT1H0M0S" ) };
		std::string output{ parsed.toString() };
		TimeSpan reparsed{ TimeSpan::parse( output ) };
		EXPECT_EQ( parsed.ticks(), reparsed.ticks() );
	}

	//----------------------------------------------
	// Factory
	//----------------------------------------------

	TEST( TimeSpanFactory, StaticFactoryMethods )
	{
		// fromDays
		TimeSpan days{ TimeSpan::fromDays( 1.5 ) };
		EXPECT_DOUBLE_EQ( days.totalDays(), 1.5 );
		EXPECT_DOUBLE_EQ( days.totalHours(), 36.0 );

		// fromHours
		TimeSpan hours{ TimeSpan::fromHours( 2.5 ) };
		EXPECT_DOUBLE_EQ( hours.totalHours(), 2.5 );
		EXPECT_DOUBLE_EQ( hours.totalMinutes(), 150.0 );

		// fromMinutes
		TimeSpan minutes{ TimeSpan::fromMinutes( 90 ) };
		EXPECT_DOUBLE_EQ( minutes.totalMinutes(), 90.0 );
		EXPECT_DOUBLE_EQ( minutes.totalHours(), 1.5 );

		// fromSeconds
		TimeSpan seconds{ TimeSpan::fromSeconds( 3600 ) };
		EXPECT_DOUBLE_EQ( seconds.totalSeconds(), 3600.0 );
		EXPECT_DOUBLE_EQ( seconds.totalHours(), 1.0 );

		// fromMilliseconds
		TimeSpan milliseconds{ TimeSpan::fromMilliseconds( 5000 ) };
		EXPECT_DOUBLE_EQ( milliseconds.totalMilliseconds(), 5000.0 );
		EXPECT_DOUBLE_EQ( milliseconds.totalSeconds(), 5.0 );

		// Test precision
		TimeSpan precise{ TimeSpan::fromSeconds( 1.5 ) };
		EXPECT_DOUBLE_EQ( precise.totalMilliseconds(), 1500.0 );
	}

	//----------------------------------------------
	// String parsing
	//----------------------------------------------

	TEST( TimeSpanStringParsing, ParseMethod )
	{
		// ISO 8601 duration format
		TimeSpan ts1{ TimeSpan::parse( "PT1H30M45S" ) };
		EXPECT_DOUBLE_EQ( ts1.totalHours(), 1.5 + 45.0 / 3600.0 );

		TimeSpan ts2{ TimeSpan::parse( "PT2H30M" ) };
		EXPECT_DOUBLE_EQ( ts2.totalMinutes(), 150.0 );

		TimeSpan ts3{ TimeSpan::parse( "PT45S" ) };
		EXPECT_DOUBLE_EQ( ts3.totalSeconds(), 45.0 );

		// Numeric seconds format
		TimeSpan ts4{ TimeSpan::parse( "123.45" ) };
		EXPECT_DOUBLE_EQ( ts4.totalSeconds(), 123.45 );

		// Invalid format should throw
		EXPECT_THROW( { [[maybe_unused]] auto _ = TimeSpan::parse( "invalid" ); }, std::invalid_argument );
		EXPECT_THROW( { [[maybe_unused]] auto _ = TimeSpan::parse( "" ); }, std::invalid_argument );
	}

	TEST( TimeSpanStringParsing, TryParseMethod )
	{
		TimeSpan result;

		// Valid ISO 8601 duration
		EXPECT_TRUE( TimeSpan::tryParse( "PT1H30M45S", result ) );
		EXPECT_DOUBLE_EQ( result.totalMinutes(), 90.75 );

		// Valid numeric seconds
		EXPECT_TRUE( TimeSpan::tryParse( "60.5", result ) );
		EXPECT_DOUBLE_EQ( result.totalSeconds(), 60.5 );

		// Invalid formats should return false
		EXPECT_FALSE( TimeSpan::tryParse( "invalid", result ) );
		EXPECT_FALSE( TimeSpan::tryParse( "", result ) );
		EXPECT_FALSE( TimeSpan::tryParse( "PT", result ) );
	}

	//----------------------------------------------
	// std::chrono interoperability
	//----------------------------------------------

	TEST( TimeSpanChrono, ToChrono )
	{
		// Convert TimeSpan to chrono duration
		TimeSpan ts1{ TimeSpan::fromSeconds( 90 ) };
		auto chronoDuration{ ts1.toChrono() };
		auto chronoSeconds{ std::chrono::duration_cast<std::chrono::seconds>( chronoDuration ) };
		EXPECT_EQ( chronoSeconds.count(), 90 );

		// Test with fractional seconds
		TimeSpan ts2{ TimeSpan::fromSeconds( 1.5 ) };
		auto chronoDuration2{ ts2.toChrono() };
		auto chronoMillis{ std::chrono::duration_cast<std::chrono::milliseconds>( chronoDuration2 ) };
		EXPECT_EQ( chronoMillis.count(), 1500 );

		// Test with hours
		TimeSpan ts3{ TimeSpan::fromHours( 2 ) };
		auto chronoDuration3{ ts3.toChrono() };
		auto chronoHours{ std::chrono::duration_cast<std::chrono::hours>( chronoDuration3 ) };
		EXPECT_EQ( chronoHours.count(), 2 );
	}

	TEST( TimeSpanChrono, FromChrono )
	{
		// Create TimeSpan from chrono seconds
		auto chronoSeconds{ std::chrono::seconds( 120 ) };
		TimeSpan ts1{ TimeSpan::fromChrono( chronoSeconds ) };
		EXPECT_DOUBLE_EQ( ts1.totalSeconds(), 120.0 );

		// Create TimeSpan from chrono milliseconds
		auto chronoMillis{ std::chrono::milliseconds( 2500 ) };
		TimeSpan ts2{ TimeSpan::fromChrono( chronoMillis ) };
		EXPECT_DOUBLE_EQ( ts2.totalMilliseconds(), 2500.0 );
		EXPECT_DOUBLE_EQ( ts2.totalSeconds(), 2.5 );

		// Create TimeSpan from chrono hours
		auto chronoHours{ std::chrono::hours( 3 ) };
		TimeSpan ts3{ TimeSpan::fromChrono( chronoHours ) };
		EXPECT_DOUBLE_EQ( ts3.totalHours(), 3.0 );

		// Create TimeSpan from chrono minutes
		auto chronoMinutes{ std::chrono::minutes( 90 ) };
		TimeSpan ts4{ TimeSpan::fromChrono( chronoMinutes ) };
		EXPECT_DOUBLE_EQ( ts4.totalMinutes(), 90.0 );

		// Round-trip conversion
		TimeSpan original{ TimeSpan::fromSeconds( 45.5 ) };
		auto chronoDuration{ original.toChrono() };
		TimeSpan roundTrip{ TimeSpan::fromChrono( chronoDuration ) };
		EXPECT_NEAR( roundTrip.totalSeconds(), original.totalSeconds(), 0.001 );
	}

	//----------------------------------------------
	// Edge cases
	//----------------------------------------------

	TEST( TimeSpanEdgeCases, ZeroDuration )
	{
		TimeSpan zero;
		EXPECT_EQ( zero.ticks(), 0 );
		EXPECT_DOUBLE_EQ( zero.totalSeconds(), 0.0 );
		EXPECT_EQ( zero.toString(), "PT0S" ); // Compact format preferred

		// Verify verbose format is also parseable
		TimeSpan parsed;
		EXPECT_TRUE( TimeSpan::tryParse( "PT0H0M0S", parsed ) );
		EXPECT_EQ( parsed.ticks(), 0 );
	}

	TEST( TimeSpanEdgeCases, NegativeDurations )
	{
		TimeSpan neg{ TimeSpan::fromHours( -2.5 ) };
		EXPECT_DOUBLE_EQ( neg.totalHours(), -2.5 );
		EXPECT_LT( neg.ticks(), 0 );

		// Arithmetic with negative durations
		TimeSpan pos{ TimeSpan::fromHours( 3 ) };
		TimeSpan result{ pos + neg };
		EXPECT_DOUBLE_EQ( result.totalHours(), 0.5 );
	}

	TEST( TimeSpanEdgeCases, VeryLargeDurations )
	{
		// Test near the limits (±10,675,199 days ≈ ±29,247 years)
		TimeSpan large{ TimeSpan::fromDays( 10000000 ) };
		EXPECT_DOUBLE_EQ( large.totalDays(), 10000000.0 );
		EXPECT_GT( large.ticks(), 0 );
	}

	//----------------------------------------------
	// Stream operators
	//----------------------------------------------

	TEST( TimeSpanStreamOperators, OutputOperator )
	{
		TimeSpan ts{ TimeSpan::fromHours( 1.5 ) };
		std::ostringstream oss;
		oss << ts;

		std::string output{ oss.str() };
		EXPECT_FALSE( output.empty() );
		EXPECT_TRUE( output.find( "PT" ) != std::string::npos );
	}

	TEST( TimeSpanStreamOperators, InputOperator )
	{
		std::istringstream iss{ "PT1H30M45S" };
		TimeSpan ts;
		iss >> ts;

		EXPECT_FALSE( iss.fail() );
		EXPECT_DOUBLE_EQ( ts.totalMinutes(), 90.75 );

		// Invalid input should set failbit
		std::istringstream iss2{ "invalid" };
		TimeSpan ts2;
		iss2 >> ts2;
		EXPECT_TRUE( iss2.fail() );
	}

	//----------------------------------------------
	// Integration tests
	//----------------------------------------------

	TEST( TimeSpanIntegration, RoundTripSerialization )
	{
		TimeSpan original{ TimeSpan::fromHours( 2 ) + TimeSpan::fromMinutes( 30 ) + TimeSpan::fromSeconds( 45.5 ) };

		// Convert to string and back
		std::string serialized{ original.toString() };
		TimeSpan deserialized{ TimeSpan::parse( serialized ) };

		// Should be approximately equal (may have rounding in milliseconds)
		EXPECT_NEAR( deserialized.totalSeconds(), original.totalSeconds(), 0.001 );
	}

	TEST( TimeSpanIntegration, ArithmeticChaining )
	{
		// Test complex arithmetic operations
		TimeSpan result{ TimeSpan::fromHours( 5 ) + TimeSpan::fromMinutes( 30 ) - TimeSpan::fromSeconds( 45 ) + TimeSpan::fromMilliseconds( 500 ) };

		double expectedSeconds{ 5 * 3600 + 30 * 60 - 45 + 0.5 };
		EXPECT_NEAR( result.totalSeconds(), expectedSeconds, 0.001 );
	}
} // namespace nfx::datetime::test
