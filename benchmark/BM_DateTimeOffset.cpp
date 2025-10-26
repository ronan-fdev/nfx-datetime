/**
 * @file BM_DateTimeOffset.cpp
 * @brief Benchmark DateTimeOffset parsing, formatting, and timezone operations
 */

#include <benchmark/benchmark.h>

#include <nfx/datetime/DateTimeOffset.h>

namespace nfx::datetime::benchmark
{
	//=====================================================================
	// DateTimeOffset benchmark suite
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	static void BM_DateTimeOffset_Construct( ::benchmark::State& state )
	{
		auto offset{ TimeSpan::fromHours( 5.5 ) };

		for ( auto _ : state )
		{
			auto dto{ DateTimeOffset{ 2024, 10, 23, 15, 30, 45, offset } };
			::benchmark::DoNotOptimize( dto );
		}
	}

	BENCHMARK( BM_DateTimeOffset_Construct );

	static void BM_DateTimeOffset_Now( ::benchmark::State& state )
	{
		for ( auto _ : state )
		{
			auto dto{ DateTimeOffset::now() };
			::benchmark::DoNotOptimize( dto );
		}
	}

	BENCHMARK( BM_DateTimeOffset_Now );

	//----------------------------------------------
	// Parsing
	//----------------------------------------------

	static void BM_DateTimeOffset_Parse( ::benchmark::State& state )
	{
		const std::string iso{ "2024-10-23T15:30:45+05:30" };

		for ( auto _ : state )
		{
			auto dto{ DateTimeOffset::parse( iso ) };
			::benchmark::DoNotOptimize( dto );
		}
	}

	BENCHMARK( BM_DateTimeOffset_Parse );

	static void BM_DateTimeOffset_ParseZ( ::benchmark::State& state )
	{
		const std::string iso{ "2024-10-23T15:30:45Z" };

		for ( auto _ : state )
		{
			auto dto{ DateTimeOffset::parse( iso ) };
			::benchmark::DoNotOptimize( dto );
		}
	}

	BENCHMARK( BM_DateTimeOffset_ParseZ );

	//----------------------------------------------
	// Conversion
	//----------------------------------------------

	static void BM_DateTimeOffset_ToUniversalTime( ::benchmark::State& state )
	{
		auto dto{ DateTimeOffset::now() };

		for ( auto _ : state )
		{
			auto utc{ dto.toUniversalTime() };
			::benchmark::DoNotOptimize( utc );
		}
	}

	BENCHMARK( BM_DateTimeOffset_ToUniversalTime );

	static void BM_DateTimeOffset_ToOffset( ::benchmark::State& state )
	{
		auto dto{ DateTimeOffset::now() };
		auto newOffset{ TimeSpan::fromHours( -8 ) };

		for ( auto _ : state )
		{
			auto result{ dto.toOffset( newOffset ) };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTimeOffset_ToOffset );

	static void BM_DateTimeOffset_UtcDateTime( ::benchmark::State& state )
	{
		auto dto{ DateTimeOffset::now() };

		for ( auto _ : state )
		{
			auto utcDt{ dto.utcDateTime() };
			::benchmark::DoNotOptimize( utcDt );
		}
	}

	BENCHMARK( BM_DateTimeOffset_UtcDateTime );

	//----------------------------------------------
	// Formatting
	//----------------------------------------------

	static void BM_DateTimeOffset_ToString( ::benchmark::State& state )
	{
		auto dto{ DateTimeOffset::now() };

		for ( auto _ : state )
		{
			auto str{ dto.toString() };
			::benchmark::DoNotOptimize( str );
		}
	}

	BENCHMARK( BM_DateTimeOffset_ToString );

	//----------------------------------------------
	// Arithmetic
	//----------------------------------------------

	static void BM_DateTimeOffset_Add( ::benchmark::State& state )
	{
		auto dto{ DateTimeOffset::now() };
		auto ts{ TimeSpan::fromHours( 24 ) };

		for ( auto _ : state )
		{
			auto result{ dto + ts };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTimeOffset_Add );

	static void BM_DateTimeOffset_Subtract( ::benchmark::State& state )
	{
		auto dto1{ DateTimeOffset::now() };
		auto dto2{ DateTimeOffset{ 2024, 1, 1, TimeSpan::fromHours( 0 ) } };

		for ( auto _ : state )
		{
			auto result{ dto1 - dto2 };
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTimeOffset_Subtract );

	//----------------------------------------------
	// Comparison
	//----------------------------------------------

	static void BM_DateTimeOffset_Comparison( ::benchmark::State& state )
	{
		auto dto1{ DateTimeOffset{ 2024, 10, 23, 15, 0, 0, TimeSpan::fromHours( 5 ) } };
		auto dto2{ DateTimeOffset{ 2024, 10, 23, 10, 0, 0, TimeSpan::fromHours( 0 ) } };

		for ( auto _ : state )
		{
			bool result{ dto1 == dto2 }; // Should be true - same UTC time
			::benchmark::DoNotOptimize( result );
		}
	}

	BENCHMARK( BM_DateTimeOffset_Comparison );
} // namespace nfx::datetime::benchmark

//=====================================================================
// Benchmarks registration
//=====================================================================

BENCHMARK_MAIN();
