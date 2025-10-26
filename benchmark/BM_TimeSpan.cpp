/**
 * @file BM_TimeSpan.cpp
 * @brief Benchmark TimeSpan parsing, formatting, and arithmetic operations
 */

#include <benchmark/benchmark.h>

#include <nfx/datetime/TimeSpan.h>

namespace nfx::datetime::benchmark
{
	//=====================================================================
	// TimeSpan benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	static void BM_TimeSpan_FromHours( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto ts{ TimeSpan::fromHours( 24.5 ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_FromHours );

	static void BM_TimeSpan_FromMinutes( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto ts{ TimeSpan::fromMinutes( 90 ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_FromMinutes );

	static void BM_TimeSpan_FromSeconds( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto ts{ TimeSpan::fromSeconds( 3661.5 ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_FromSeconds );

	static void BM_TimeSpan_FromMilliseconds( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto ts{ TimeSpan::fromMilliseconds( 123456.789 ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_FromMilliseconds );

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	static void BM_TimeSpan_ParseISO( ::benchmark::State& state )
	{
		const std::string duration{ "PT1H30M45S" };

		for ( auto _ : state )
		{
			auto ts{ TimeSpan::parse( duration ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_ParseISO );

	static void BM_TimeSpan_ParseISOComplex( ::benchmark::State& state )
	{
		const std::string duration{ "P5DT12H30M" };

		for ( auto _ : state )
		{
			auto ts{ TimeSpan::parse( duration ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_ParseISOComplex );

	static void BM_TimeSpan_Parse( ::benchmark::State& state )
	{
		const std::string duration{ "3600.5" };

		for ( auto _ : state )
		{
			auto ts{ TimeSpan::parse( duration ) };
			::benchmark::DoNotOptimize( ts );
		}
	}

	BENCHMARK( BM_TimeSpan_Parse );

	//----------------------------------------------
	// Formatting
	//----------------------------------------------

	static void BM_TimeSpan_ToString_ISO8601( ::benchmark::State& state )
	{
		auto ts{ TimeSpan::fromHours( 25.5 ) };

		for ( auto _ : state )
		{
			auto str{ ts.toString() };
			::benchmark::DoNotOptimize( str );
		}
	}

	BENCHMARK( BM_TimeSpan_ToString_ISO8601 );

	//----------------------------------------------
	// Arithmetic
	//----------------------------------------------

	static void BM_TimeSpan_Add( ::benchmark::State& state )
	{
		auto ts1{ TimeSpan::fromHours( 1 ) };
		auto ts2{ TimeSpan::fromMinutes( 30 ) };
		for ( auto _ : state )
		{
			auto result{ ts1 + ts2 };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_TimeSpan_Add );

	static void BM_TimeSpan_Subtract( ::benchmark::State& state )
	{
		auto ts1{ TimeSpan::fromHours( 5 ) };
		auto ts2{ TimeSpan::fromMinutes( 30 ) };
		for ( auto _ : state )
		{
			auto result{ ts1 - ts2 };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_TimeSpan_Subtract );

	static void BM_TimeSpan_Negate( ::benchmark::State& state )
	{
		auto ts{ TimeSpan::fromHours( 5 ) };

		for ( auto _ : state )
		{
			auto result{ -ts };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_TimeSpan_Negate );

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	static void BM_TimeSpan_TotalHours( ::benchmark::State& state )
	{
		auto ts{ TimeSpan::fromMinutes( 150 ) };

		for ( auto _ : state )
		{
			auto hours{ ts.totalHours() };
			::benchmark::DoNotOptimize( hours );
		}
	}

	BENCHMARK( BM_TimeSpan_TotalHours );

	static void BM_TimeSpan_TotalSeconds( ::benchmark::State& state )
	{
		auto ts{ TimeSpan::fromHours( 2.5 ) };

		for ( auto _ : state )
		{
			auto seconds{ ts.totalSeconds() };
			::benchmark::DoNotOptimize( seconds );
		}
	}

	BENCHMARK( BM_TimeSpan_TotalSeconds );

	static void BM_TimeSpan_TotalMilliseconds( ::benchmark::State& state )
	{
		auto ts{ TimeSpan::fromSeconds( 1.5 ) };

		for ( auto _ : state )
		{
			auto ms{ ts.totalMilliseconds() };
			::benchmark::DoNotOptimize( ms );
		}
	}

	BENCHMARK( BM_TimeSpan_TotalMilliseconds );

	//----------------------------------------------
	// Comparison
	//----------------------------------------------

	static void BM_TimeSpan_Comparison( ::benchmark::State& state )
	{
		auto ts1{ TimeSpan::fromHours( 2 ) };
		auto ts2{ TimeSpan::fromMinutes( 90 ) };
		for ( auto _ : state )
		{
			bool result{ ts1 > ts2 };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_TimeSpan_Comparison );
} // namespace nfx::datetime::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

BENCHMARK_MAIN();
