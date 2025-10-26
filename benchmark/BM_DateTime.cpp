/**
 * @file BM_DateTime.cpp
 * @brief Benchmark DateTime parsing, formatting, and arithmetic operations
 */

#include <benchmark/benchmark.h>

#include <nfx/datetime/DateTime.h>

namespace nfx::datetime::benchmark
{
	//=====================================================================
	// DateTime benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	static void BM_DateTime_Construct_YMD( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto dt{ DateTime{ 2024, 10, 23 } };
			::benchmark::DoNotOptimize( dt );
		}
	}

	BENCHMARK( BM_DateTime_Construct_YMD );

	static void BM_DateTime_Construct_YMDHMS( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto dt{ DateTime{ 2024, 10, 23, 15, 30, 45 } };
			::benchmark::DoNotOptimize( dt );
		}
	}

	BENCHMARK( BM_DateTime_Construct_YMDHMS );

	static void BM_DateTime_Now( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto dt{ DateTime::now() };
			::benchmark::DoNotOptimize( dt );
		}
	}

	BENCHMARK( BM_DateTime_Now );

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	static void BM_DateTime_Parse( ::benchmark::State& state )
	{
		const std::string iso{ "2024-10-23T15:30:45Z" };

		for ( auto _ : state )
		{
			auto dt{ DateTime::parse( iso ) };
			::benchmark::DoNotOptimize( dt );
		}
	}

	BENCHMARK( BM_DateTime_Parse );

	static void BM_DateTime_ParseExtended( ::benchmark::State& state )
	{
		const std::string iso{ "2024-10-23T15:30:45.1234567Z" };

		for ( auto _ : state )
		{
			auto dt{ DateTime::parse( iso ) };
			::benchmark::DoNotOptimize( dt );
		}
	}

	BENCHMARK( BM_DateTime_ParseExtended );

	//----------------------------------------------
	// Formatting
	//----------------------------------------------

	static void BM_DateTime_ToString_ISO8601( ::benchmark::State& state )
	{
		auto dt{ DateTime::now() };

		for ( auto _ : state )
		{
			auto str{ dt.toString() };
			::benchmark::DoNotOptimize( str );
		}
	}

	BENCHMARK( BM_DateTime_ToString_ISO8601 );

	static void BM_DateTime_toIso8601Extended( ::benchmark::State& state )
	{
		auto dt{ DateTime::now() };

		for ( auto _ : state )
		{
			auto str{ dt.toIso8601Extended() };
			::benchmark::DoNotOptimize( str );
		}
	}

	BENCHMARK( BM_DateTime_toIso8601Extended );

	//----------------------------------------------
	// Arithmetic
	//----------------------------------------------

	static void BM_DateTime_Add_TimeSpan( ::benchmark::State& state )
	{
		auto dt{ DateTime::now() };
		auto ts{ TimeSpan::fromHours( 24 ) };

		for ( auto _ : state )
		{
			auto result{ dt + ts };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTime_Add_TimeSpan );

	static void BM_DateTime_Subtract_TimeSpan( ::benchmark::State& state )
	{
		auto dt{ DateTime::now() };
		auto ts{ TimeSpan::fromHours( 24 ) };

		for ( auto _ : state )
		{
			auto result{ dt - ts };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTime_Subtract_TimeSpan );

	static void BM_DateTime_Subtract_DateTime( ::benchmark::State& state )
	{
		auto dt1{ DateTime{ 2024, 10, 23, 15, 0, 0 } };
		auto dt2{ DateTime{ 2024, 1, 1 } };

		for ( auto _ : state )
		{
			auto result{ dt1 - dt2 };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTime_Subtract_DateTime );

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	static void BM_DateTime_ToUnixSeconds( ::benchmark::State& state )
	{
		auto dt{ DateTime::now() };

		for ( auto _ : state )
		{
			auto unix{ dt.toUnixSeconds() };
			::benchmark::DoNotOptimize( unix );
		}
	}

	BENCHMARK( BM_DateTime_ToUnixSeconds );

	static void BM_DateTime_sinceEpochSeconds( ::benchmark::State& state )
	{
		std::int64_t unix{ 1729699845 };

		for ( auto _ : state )
		{
			auto dt{ DateTime::sinceEpochSeconds( unix ) };
			::benchmark::DoNotOptimize( dt );
		}
	}

	BENCHMARK( BM_DateTime_sinceEpochSeconds );

	//----------------------------------------------
	// Component extraction
	//----------------------------------------------

	static void BM_DateTime_GetComponents( ::benchmark::State& state )
	{
		auto dt{ DateTime::now() };

		for ( auto _ : state )
		{
			auto y{ dt.year() };
			auto m{ dt.month() };
			auto d{ dt.day() };
			auto h{ dt.hour() };
			auto min{ dt.minute() };
			auto s{ dt.second() };
			::benchmark::DoNotOptimize( y );
			::benchmark::DoNotOptimize( m );
			::benchmark::DoNotOptimize( d );
			::benchmark::DoNotOptimize( h );
			::benchmark::DoNotOptimize( min );
			::benchmark::DoNotOptimize( s );
		}
	}

	BENCHMARK( BM_DateTime_GetComponents );

	//----------------------------------------------
	// Comparison
	//----------------------------------------------

	static void BM_DateTime_Comparison( ::benchmark::State& state )
	{
		auto dt1{ DateTime{ 2024, 10, 23, 15, 0, 0 } };
		auto dt2{ DateTime{ 2024, 10, 23 } };

		for ( auto _ : state )
		{
			bool result{ dt1 > dt2 };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTime_Comparison );
} // namespace nfx::datetime::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

BENCHMARK_MAIN();
