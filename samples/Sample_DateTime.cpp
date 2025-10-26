/**
 * @file Sample_DateTime.cpp
 * @brief Comprehensive demonstration of NFX C++ Core time utilities
 * @details This sample shows practical usage of DateTime, TimeSpan, and DateTimeOffset
 *          for high-precision temporal operations, ISO 8601 parsing, and timezone handling.
 *
 *          Demonstrates:
 *          - DateTime: UTC-only datetime operations
 *          - TimeSpan: Duration calculations with ISO 8601 format
 *          - DateTimeOffset: Timezone-aware datetime handling
 *          - Arithmetic operations, parsing, formatting, and conversions
 */

#include <iostream>
#include <iomanip>

#include <nfx/datetime/DateTime.h>
#include <nfx/datetime/DateTimeOffset.h>
#include <nfx/datetime/TimeSpan.h>

using namespace nfx::datetime;

// Helper function to print section headers
void printSection( const std::string& title )
{
	std::cout << "\n"
			  << std::string( 70, '=' ) << "\n";
	std::cout << "  " << title << "\n";
	std::cout << std::string( 70, '=' ) << "\n";
}

void printSubsection( const std::string& title )
{
	std::cout << "\n--- " << title << " ---\n";
}

//=====================================================================
// DateTime Examples - UTC-only datetime operations
//=====================================================================

void demonstrateDateTime()
{
	printSection( "DateTime - UTC-only Datetime Operations" );

	// 1. Creating DateTime objects
	printSubsection( "Creating DateTime Objects" );
	{
		// Current UTC time
		DateTime now{ DateTime::now() };
		std::cout << "Current UTC time:           " << now.toString() << "\n";

		// Today at midnight UTC
		DateTime today{ DateTime::today() };
		std::cout << "Today at midnight UTC:      " << today.toString() << "\n";

		// From date and time components
		DateTime specific{ 2024, 6, 15, 14, 30, 45 };
		std::cout << "Specific date/time:         " << specific.toString() << "\n";

		// With milliseconds
		DateTime precise{ 2024, 6, 15, 14, 30, 45, 123 };
		std::cout << "With milliseconds:          " << precise.toIso8601Extended() << "\n";

		// Special values
		DateTime minVal{ DateTime::minValue() };
		DateTime maxVal{ DateTime::maxValue() };
		DateTime epoch{ DateTime::epoch() };
		std::cout << "Minimum DateTime:           " << minVal.toString() << "\n";
		std::cout << "Maximum DateTime:           " << maxVal.toString() << "\n";
		std::cout << "Unix Epoch:                 " << epoch.toString() << "\n";
	}

	// 2. Parsing from strings
	printSubsection( "Parsing from ISO 8601 Strings" );
	{
		// Basic ISO 8601
		DateTime dt1{ DateTime::parse( "2024-06-15T14:30:45Z" ) };
		std::cout << "Parsed basic format:        " << dt1.toString() << "\n";

		// Extended with fractional seconds
		DateTime dt2{ DateTime::parse( "2024-06-15T14:30:45.123Z" ) };
		std::cout << "Parsed with milliseconds:   " << dt2.toIso8601Extended() << "\n";

		// Date only
		DateTime dt3{ DateTime::parse( "2024-06-15" ) };
		std::cout << "Parsed date only:           " << dt3.toString() << "\n";

		// Safe parsing with tryParse
		DateTime result;
		if ( DateTime::tryParse( "2024-12-25T00:00:00Z", result ) )
		{
			std::cout << "tryParse success:           " << result.toString() << "\n";
		}
	}

	// 3. Accessing components
	printSubsection( "Accessing Date/Time Components" );
	{
		DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };
		std::cout << "Full DateTime:              " << dt.toIso8601Extended() << "\n";
		std::cout << "  Year:                     " << dt.year() << "\n";
		std::cout << "  Month:                    " << dt.month() << "\n";
		std::cout << "  Day:                      " << dt.day() << "\n";
		std::cout << "  Hour:                     " << dt.hour() << "\n";
		std::cout << "  Minute:                   " << dt.minute() << "\n";
		std::cout << "  Second:                   " << dt.second() << "\n";
		std::cout << "  Millisecond:              " << dt.millisecond() << "\n";
		std::cout << "  Day of week (0=Sun):      " << dt.dayOfWeek() << "\n";
		std::cout << "  Day of year:              " << dt.dayOfYear() << "\n";
	}

	// 4. Arithmetic operations
	printSubsection( "Arithmetic Operations" );
	{
		DateTime start{ 2024, 6, 15, 10, 0, 0 };
		std::cout << "Start time:                 " << start.toString() << "\n";

		// Add duration
		DateTime later{ start + TimeSpan::fromHours( 2.5 ) };
		std::cout << "After +2.5 hours:           " << later.toString() << "\n";

		// Subtract duration
		DateTime earlier{ start - TimeSpan::fromMinutes( 30.0 ) };
		std::cout << "After -30 minutes:          " << earlier.toString() << "\n";

		// Difference between DateTimes
		TimeSpan difference{ later - start };
		std::cout << "Difference:                 " << difference.toString() << "\n";
		std::cout << "  In hours:                 " << difference.totalHours() << "\n";
	}

	// 5. Unix timestamp conversions
	printSubsection( "Unix Timestamp Conversions" );
	{
		// To Unix timestamp
		DateTime dt{ 2024, 1, 1, 0, 0, 0 };
		std::cout << "DateTime:                   " << dt.toString() << "\n";
		std::cout << "Unix seconds:               " << dt.toUnixSeconds() << "\n";
		std::cout << "Unix milliseconds:          " << dt.toUnixMilliseconds() << "\n";

		// From Unix timestamp
		DateTime fromUnix{ DateTime::sinceEpochSeconds( 1704067200 ) };
		std::cout << "From Unix 1704067200:       " << fromUnix.toString() << "\n";
	}

	// 6. Date and time extraction
	printSubsection( "Date and Time Extraction" );
	{
		DateTime dt{ 2024, 6, 15, 14, 30, 45 };
		std::cout << "Full DateTime:              " << dt.toString() << "\n";

		// Extract date only (time = 00:00:00)
		DateTime dateOnly{ dt.date() };
		std::cout << "Date only:                  " << dateOnly.toString() << "\n";

		// Extract time of day
		TimeSpan timeOfDay{ dt.timeOfDay() };
		std::cout << "Time of day:                " << timeOfDay.toString() << "\n";
	}

	// 7. Formatting options
	printSubsection( "String Formatting Options" );
	{
		DateTime dt{ 2024, 6, 15, 14, 30, 45, 123 };
		std::cout << "ISO 8601 Basic:             " << dt.toString() << "\n";
		std::cout << "ISO 8601 Extended:          " << dt.toIso8601Extended() << "\n";
		std::cout << "Date Only:                  " << dt.toString( DateTime::Format::DateOnly ) << "\n";
		std::cout << "Time Only:                  " << dt.toString( DateTime::Format::TimeOnly ) << "\n";
	}

	// 8. Comparisons
	printSubsection( "Comparison Operations" );
	{
		DateTime dt1{ 2024, 6, 15, 10, 0, 0 };
		DateTime dt2{ 2024, 6, 15, 14, 0, 0 };
		DateTime dt3{ 2024, 6, 15, 10, 0, 0 };

		std::cout << std::boolalpha;
		std::cout << "dt1 == dt3:                 " << ( dt1 == dt3 ) << "\n";
		std::cout << "dt1 != dt2:                 " << ( dt1 != dt2 ) << "\n";
		std::cout << "dt1 < dt2:                  " << ( dt1 < dt2 ) << "\n";
		std::cout << "dt2 > dt1:                  " << ( dt2 > dt1 ) << "\n";
	}

	// 9. Leap year handling
	printSubsection( "Leap Year Handling" );
	{
		std::cout << std::boolalpha;
		std::cout << "2024 is leap year:          " << DateTime::isLeapYear( 2024 ) << "\n";
		std::cout << "2023 is leap year:          " << DateTime::isLeapYear( 2023 ) << "\n";
		std::cout << "Days in Feb 2024:           " << DateTime::daysInMonth( 2024, 2 ) << "\n";
		std::cout << "Days in Feb 2023:           " << DateTime::daysInMonth( 2023, 2 ) << "\n";

		// February 29 in leap year
		DateTime leapDay{ 2024, 2, 29, 12, 0, 0 };
		std::cout << "Leap day Feb 29, 2024:      " << leapDay.toString() << "\n";
	}
}

//=====================================================================
// TimeSpan Examples - Duration calculations
//=====================================================================

void demonstrateTimeSpan()
{
	printSection( "TimeSpan - Duration Calculations" );

	// 1. Creating TimeSpan objects
	printSubsection( "Creating TimeSpan Objects" );
	{
		// From various units
		TimeSpan days{ TimeSpan::fromDays( 1.5 ) };
		TimeSpan hours{ TimeSpan::fromHours( 2.5 ) };
		TimeSpan minutes{ TimeSpan::fromMinutes( 90.0 ) };
		TimeSpan seconds{ TimeSpan::fromSeconds( 3600.0 ) };
		TimeSpan millis{ TimeSpan::fromMilliseconds( 1500.0 ) };

		std::cout << "1.5 days:                   " << days.toString() << "\n";
		std::cout << "2.5 hours:                  " << hours.toString() << "\n";
		std::cout << "90 minutes:                 " << minutes.toString() << "\n";
		std::cout << "3600 seconds:               " << seconds.toString() << "\n";
		std::cout << "1500 milliseconds:          " << millis.toString() << "\n";

		// From ticks (100-nanosecond units)
		TimeSpan fromTicks{ 10000000 }; // 1 second = 10 million ticks
		std::cout << "10,000,000 ticks:           " << fromTicks.toString() << "\n";
	}

	// 2. Parsing ISO 8601 duration strings
	printSubsection( "Parsing ISO 8601 Duration Strings" );
	{
		// Compact format
		TimeSpan ts1{ TimeSpan::parse( "PT1H" ) };
		std::cout << "PT1H:                       " << ts1.totalHours() << " hours\n";

		// Verbose format
		TimeSpan ts2{ TimeSpan::parse( "PT1H30M45S" ) };
		std::cout << "PT1H30M45S:                 " << ts2.totalSeconds() << " seconds\n";

		// With days
		TimeSpan ts3{ TimeSpan::parse( "P1DT12H" ) };
		std::cout << "P1DT12H:                    " << ts3.totalHours() << " hours\n";

		// Fractional seconds
		TimeSpan ts4{ TimeSpan::parse( "PT1.5S" ) };
		std::cout << "PT1.5S:                     " << ts4.totalMilliseconds() << " ms\n";

		// Negative duration
		TimeSpan ts5{ TimeSpan::parse( "-PT2H30M" ) };
		std::cout << "-PT2H30M:                   " << ts5.totalMinutes() << " minutes\n";

		// Convenience: numeric seconds
		TimeSpan ts6{ TimeSpan::parse( "123.45" ) };
		std::cout << "123.45 (numeric):           " << ts6.totalSeconds() << " seconds\n";
	}

	// 3. Converting to different units
	printSubsection( "Converting to Different Units" );
	{
		TimeSpan duration{ TimeSpan::fromHours( 2.5 ) };
		std::cout << "Duration: 2.5 hours\n";
		std::cout << "  Total days:               " << duration.totalDays() << "\n";
		std::cout << "  Total hours:              " << duration.totalHours() << "\n";
		std::cout << "  Total minutes:            " << duration.totalMinutes() << "\n";
		std::cout << "  Total seconds:            " << duration.totalSeconds() << "\n";
		std::cout << "  Total milliseconds:       " << duration.totalMilliseconds() << "\n";
		std::cout << "  Ticks:                    " << duration.ticks() << "\n";
	}

	// 4. Arithmetic operations
	printSubsection( "Arithmetic Operations" );
	{
		TimeSpan ts1{ TimeSpan::fromHours( 2.0 ) };
		TimeSpan ts2{ TimeSpan::fromMinutes( 30.0 ) };

		std::cout << "ts1 (2 hours):              " << ts1.toString() << "\n";
		std::cout << "ts2 (30 minutes):           " << ts2.toString() << "\n";

		// Addition
		TimeSpan sum{ ts1 + ts2 };
		std::cout << "ts1 + ts2:                  " << sum.toString() << "\n";

		// Subtraction
		TimeSpan diff{ ts1 - ts2 };
		std::cout << "ts1 - ts2:                  " << diff.toString() << "\n";

		// Negation
		TimeSpan neg{ -ts1 };
		std::cout << "-ts1:                       " << neg.toString() << "\n";

		// Compound operations
		TimeSpan complex{ TimeSpan::fromDays( 1.0 ) + TimeSpan::fromHours( 3.0 ) + TimeSpan::fromMinutes( 30.0 ) };
		std::cout << "1 day + 3 hours + 30 min:   " << complex.toString() << "\n";
	}

	// 5. Comparisons
	printSubsection( "Comparison Operations" );
	{
		TimeSpan short_duration{ TimeSpan::fromMinutes( 30.0 ) };
		TimeSpan long_duration{ TimeSpan::fromHours( 2.0 ) };

		std::cout << std::boolalpha;
		std::cout << "30 min == 30 min:           " << ( short_duration == TimeSpan::fromMinutes( 30.0 ) ) << "\n";
		std::cout << "30 min < 2 hours:           " << ( short_duration < long_duration ) << "\n";
		std::cout << "2 hours > 30 min:           " << ( long_duration > short_duration ) << "\n";
	}

	// 6. Negative durations
	printSubsection( "Negative Durations" );
	{
		TimeSpan negative{ TimeSpan::fromHours( -2.5 ) };
		std::cout << "Negative duration:          " << negative.toString() << "\n";
		std::cout << "Total hours:                " << negative.totalHours() << "\n";

		// Going backwards in time
		DateTime now{ DateTime::now() };
		DateTime past{ now + negative };
		std::cout << "Now:                        " << now.toString() << "\n";
		std::cout << "2.5 hours ago:              " << past.toString() << "\n";
	}
}

//=====================================================================
// DateTimeOffset Examples - Timezone-aware datetime
//=====================================================================

void demonstrateDateTimeOffset()
{
	printSection( "DateTimeOffset - Timezone-Aware DateTime" );

	// 1. Creating DateTimeOffset objects
	printSubsection( "Creating DateTimeOffset Objects" );
	{
		// Current local time with system timezone
		DateTimeOffset now{ DateTimeOffset::now() };
		std::cout << "Current local time:         " << now.toString() << "\n";

		// Current UTC time
		DateTimeOffset utcNow{ DateTimeOffset::utcNow() };
		std::cout << "Current UTC time:           " << utcNow.toString() << "\n";

		// Specific timezone offsets
		DateTimeOffset tokyo{ 2024, 6, 15, 14, 30, 0, TimeSpan::fromHours( 9.0 ) };		// UTC+9
		DateTimeOffset newYork{ 2024, 6, 15, 14, 30, 0, TimeSpan::fromHours( -5.0 ) };	// UTC-5
		DateTimeOffset india{ 2024, 6, 15, 14, 30, 0, TimeSpan::fromMinutes( 330.0 ) }; // UTC+5:30

		std::cout << "Tokyo (UTC+9):              " << tokyo.toString() << "\n";
		std::cout << "New York (UTC-5):           " << newYork.toString() << "\n";
		std::cout << "India (UTC+5:30):           " << india.toString() << "\n";

		// UTC (zero offset)
		DateTimeOffset utc{ 2024, 6, 15, 14, 30, 0, TimeSpan::fromHours( 0.0 ) };
		std::cout << "UTC (explicit):             " << utc.toString() << "\n";
	}

	// 2. Parsing with timezone offsets
	printSubsection( "Parsing with Timezone Offsets" );
	{
		// Positive offset
		DateTimeOffset dto1{ DateTimeOffset::parse( "2024-06-15T14:30:00+09:00" ) };
		std::cout << "Parsed +09:00:              " << dto1.toString() << "\n";

		// Negative offset
		DateTimeOffset dto2{ DateTimeOffset::parse( "2024-06-15T14:30:00-05:00" ) };
		std::cout << "Parsed -05:00:              " << dto2.toString() << "\n";

		// Zulu time (UTC)
		DateTimeOffset dto3{ DateTimeOffset::parse( "2024-06-15T14:30:00Z" ) };
		std::cout << "Parsed Z (UTC):             " << dto3.toString() << "\n";

		// With fractional seconds
		DateTimeOffset dto4{ DateTimeOffset::parse( "2024-06-15T14:30:00.123+02:00" ) };
		std::cout << "With milliseconds:          " << dto4.toIso8601Extended() << "\n";
	}

	// 3. Timezone offset conversions
	printSubsection( "Timezone Offset Conversions" );
	{
		// Same moment in different timezones
		DateTimeOffset tokyo{ 2024, 6, 15, 18, 0, 0, TimeSpan::fromHours( 9.0 ) };
		std::cout << "Tokyo (UTC+9):              " << tokyo.toString() << "\n";

		// Convert to New York time (UTC-5)
		DateTimeOffset newYork{ tokyo.toOffset( TimeSpan::fromHours( -5.0 ) ) };
		std::cout << "Same moment in NY (UTC-5):  " << newYork.toString() << "\n";

		// Convert to UTC
		DateTimeOffset utc{ tokyo.toUniversalTime() };
		std::cout << "Same moment in UTC:         " << utc.toString() << "\n";

		// Verify all represent the same instant
		std::cout << "\nAll UTC ticks equal: " << std::boolalpha
				  << ( tokyo.utcTicks() == newYork.utcTicks() && newYork.utcTicks() == utc.utcTicks() ) << "\n";
	}

	// 4. Accessing components
	printSubsection( "Accessing Date/Time Components" );
	{
		DateTimeOffset dto{ 2024, 6, 15, 14, 30, 45, TimeSpan::fromHours( 2.0 ) };
		std::cout << "DateTimeOffset:             " << dto.toString() << "\n";
		std::cout << "  Local time:               " << dto.dateTime().toString() << "\n";
		std::cout << "  UTC time:                 " << dto.utcDateTime().toString() << "\n";
		std::cout << "  Offset:                   " << dto.offset().toString() << "\n";
		std::cout << "  Total offset minutes:     " << dto.totalOffsetMinutes() << "\n";
		std::cout << "  Year:                     " << dto.year() << "\n";
		std::cout << "  Month:                    " << dto.month() << "\n";
		std::cout << "  Day:                      " << dto.day() << "\n";
		std::cout << "  Hour:                     " << dto.hour() << "\n";
		std::cout << "  Minute:                   " << dto.minute() << "\n";
	}

	// 5. Arithmetic operations
	printSubsection( "Arithmetic Operations" );
	{
		DateTimeOffset start{ 2024, 6, 15, 10, 0, 0, TimeSpan::fromHours( 2.0 ) };
		std::cout << "Start:                      " << start.toString() << "\n";

		// Add duration (offset remains unchanged)
		DateTimeOffset later{ start + TimeSpan::fromHours( 3.0 ) };
		std::cout << "After +3 hours:             " << later.toString() << "\n";
		std::cout << "  (offset unchanged)\n";

		// Difference between DateTimeOffsets (in UTC)
		DateTimeOffset tokyo{ 2024, 6, 15, 18, 0, 0, TimeSpan::fromHours( 9.0 ) };
		DateTimeOffset newYork{ 2024, 6, 15, 5, 0, 0, TimeSpan::fromHours( -5.0 ) };
		TimeSpan diff{ tokyo - newYork };

		std::cout << "\nTokyo time:                 " << tokyo.toString() << "\n";
		std::cout << "New York time:              " << newYork.toString() << "\n";
		std::cout << "Difference (UTC):           " << diff.toString() << " (" << diff.totalHours() << " hours)\n";
	}

	// 6. Comparisons (based on UTC values)
	printSubsection( "Comparison Operations (UTC-based)" );
	{
		// These represent the SAME moment in time
		DateTimeOffset tokyo{ 2024, 6, 15, 18, 0, 0, TimeSpan::fromHours( 9.0 ) };
		DateTimeOffset newYork{ 2024, 6, 15, 5, 0, 0, TimeSpan::fromHours( -5.0 ) };

		std::cout << "Tokyo:                      " << tokyo.toString() << "\n";
		std::cout << "New York:                   " << newYork.toString() << "\n";
		std::cout << std::boolalpha;
		std::cout << "Equal (same UTC moment):    " << ( tokyo == newYork ) << "\n";
		std::cout << "Exact equal (incl offset):  " << tokyo.equalsExact( newYork ) << "\n";
	}

	// 7. Unix timestamp conversions
	printSubsection( "Unix Timestamp Conversions" );
	{
		// From Unix timestamp (always UTC)
		DateTimeOffset fromUnix{ DateTimeOffset::fromUnixTimeSeconds( 1704067200 ) };
		std::cout << "From Unix timestamp:        " << fromUnix.toString() << "\n";

		// Convert to local timezone
		DateTimeOffset local{ fromUnix.toOffset( TimeSpan::fromHours( 2.0 ) ) };
		std::cout << "In UTC+2:                   " << local.toString() << "\n";

		// Back to Unix timestamp
		std::cout << "Back to Unix seconds:       " << local.toUnixSeconds() << "\n";
	}

	// 8. Real-world scenario: Meeting across timezones
	printSubsection( "Real-World: Cross-Timezone Meeting" );
	{
		// Meeting scheduled for 2PM Tokyo time
		DateTimeOffset meetingTokyo{ 2024, 6, 15, 14, 0, 0, TimeSpan::fromHours( 9.0 ) };
		std::cout << "Meeting in Tokyo (UTC+9):   " << meetingTokyo.toString() << "\n";

		// What time is it in other cities?
		DateTimeOffset meetingNY{ meetingTokyo.toOffset( TimeSpan::fromHours( -4.0 ) ) };	  // EDT
		DateTimeOffset meetingLondon{ meetingTokyo.toOffset( TimeSpan::fromHours( 1.0 ) ) };  // BST
		DateTimeOffset meetingSydney{ meetingTokyo.toOffset( TimeSpan::fromHours( 10.0 ) ) }; // AEST

		std::cout << "Same meeting in NY (EDT):   " << meetingNY.toString() << " (";
		std::cout << meetingNY.hour() << ":00)\n";
		std::cout << "Same meeting in London:     " << meetingLondon.toString() << " (";
		std::cout << meetingLondon.hour() << ":00)\n";
		std::cout << "Same meeting in Sydney:     " << meetingSydney.toString() << " (";
		std::cout << meetingSydney.hour() << ":00)\n";

		std::cout << "\nMeeting UTC time:           " << meetingTokyo.utcDateTime().toString() << "\n";
	}
}

//=====================================================================
// Integration Examples - Combining all three classes
//=====================================================================

void demonstrateIntegration()
{
	printSection( "Integration - Combining DateTime, TimeSpan, DateTimeOffset" );

	// 1. Task scheduling with durations
	printSubsection( "Task Scheduling Example" );
	{
		// Task starts at 9 AM UTC
		DateTime taskStart{ 2024, 6, 15, 9, 0, 0 };
		std::cout << "Task starts (UTC):          " << taskStart.toString() << "\n";

		// Estimated duration: 2h 30m
		TimeSpan estimatedDuration{ TimeSpan::fromHours( 2.0 ) + TimeSpan::fromMinutes( 30.0 ) };
		std::cout << "Estimated duration:         " << estimatedDuration.toString() << "\n";

		// Calculate end time
		DateTime taskEnd{ taskStart + estimatedDuration };
		std::cout << "Expected end (UTC):         " << taskEnd.toString() << "\n";

		// Show in different timezones
		DateTimeOffset endTokyo{ taskEnd, TimeSpan::fromHours( 9.0 ) };
		DateTimeOffset endNewYork{ taskEnd, TimeSpan::fromHours( -4.0 ) };
		std::cout << "End time in Tokyo:          " << endTokyo.toString() << "\n";
		std::cout << "End time in New York:       " << endNewYork.toString() << "\n";
	}

	// 2. Time tracking application
	printSubsection( "Time Tracking Application" );
	{
		// User starts work (with their timezone)
		DateTimeOffset workStart{ 2024, 6, 15, 9, 0, 0, TimeSpan::fromHours( -5.0 ) }; // EST
		std::cout << "Work started (EST):         " << workStart.toString() << "\n";

		// User takes a break after 3h 45m
		TimeSpan workDuration{ TimeSpan::fromHours( 3.0 ) + TimeSpan::fromMinutes( 45.0 ) };
		DateTimeOffset breakTime{ workStart + workDuration };
		std::cout << "Break time (EST):           " << breakTime.toString() << "\n";

		// Break duration
		TimeSpan breakDuration{ TimeSpan::fromMinutes( 30.0 ) };

		// Resume work
		DateTimeOffset workResume{ breakTime + breakDuration };
		std::cout << "Work resumed (EST):         " << workResume.toString() << "\n";

		// Work another 4 hours
		DateTimeOffset workEnd{ workResume + TimeSpan::fromHours( 4.0 ) };
		std::cout << "Work ended (EST):           " << workEnd.toString() << "\n";

		// Total work time (excluding break)
		TimeSpan totalWork{ workDuration + TimeSpan::fromHours( 4.0 ) };
		std::cout << "Total work time:            " << totalWork.toString() << " (";
		std::cout << totalWork.totalHours() << " hours)\n";
	}

	// 3. Event countdown
	printSubsection( "Event Countdown" );
	{
		DateTime now{ DateTime::now() };
		DateTime event{ 2025, 1, 1, 0, 0, 0 }; // New Year 2025

		if ( event > now )
		{
			TimeSpan timeUntil{ event - now };
			std::cout << "Current UTC time:           " << now.toString() << "\n";
			std::cout << "New Year 2025:              " << event.toString() << "\n";
			std::cout << "Time until event:           " << timeUntil.toString() << "\n";
			std::cout << "  Days:                     " << timeUntil.totalDays() << "\n";
			std::cout << "  Hours:                    " << timeUntil.totalHours() << "\n";
		}
		else
		{
			std::cout << "Event has passed!\n";
		}
	}

	// 4. Performance measurement
	printSubsection( "Performance Measurement" );
	{
		DateTime start{ DateTime::now() };
		std::cout << "Operation started:          " << start.toIso8601Extended() << "\n";

		// Simulate some work
		volatile int sum{ 0 };
		for ( int i{ 0 }; i < 10000000; ++i )
		{
			sum += i;
		}

		DateTime end{ DateTime::now() };
		TimeSpan elapsed{ end - start };

		std::cout << "Operation completed:        " << end.toIso8601Extended() << "\n";
		std::cout << "Elapsed time:               " << elapsed.toString() << "\n";
		std::cout << "  Milliseconds:             " << elapsed.totalMilliseconds() << "\n";
		std::cout << "  Microseconds:             " << ( elapsed.ticks() / 10 ) << "\n";
	}
}

//=====================================================================
// Main entry point
//=====================================================================

int main()
{
	try
	{
		std::cout << "\n";
		std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
		std::cout << "║                 nfx Time Library - Sample Program                  ║\n";
		std::cout << "║                                                                    ║\n";
		std::cout << "║  Demonstrates DateTime, TimeSpan, and DateTimeOffset classes       ║\n";
		std::cout << "║  High-precision temporal operations with ISO 8601 support          ║\n";
		std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";

		demonstrateDateTime();
		demonstrateTimeSpan();
		demonstrateDateTimeOffset();
		demonstrateIntegration();

		return 0;
	}
	catch ( const std::exception& e )
	{
		std::cerr << "\n*** ERROR: " << e.what() << "\n\n";
		return 1;
	}
}
