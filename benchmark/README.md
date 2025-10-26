# Benchmarks

---

## Test Environment

### Hardware Configuration

| Component                | Specification                                                 |
| ------------------------ | ------------------------------------------------------------- |
| **CPU**                  | 12th Gen Intel Core i7-12800H (20 logical, 14 physical cores) |
| **Base Clock**           | 2.80 GHz                                                      |
| **Turbo Clock**          | 4.80 GHz                                                      |
| **L1 Data Cache**        | 48 KiB (×6 P-cores) + 32 KiB (×8 E-cores)                     |
| **L1 Instruction Cache** | 32 KiB (×6 P-cores) + 64 KiB (×2 E-core clusters)             |
| **L2 Unified Cache**     | 1.25 MiB (×6 P-cores) + 2 MiB (×2 E-core clusters)            |
| **L3 Unified Cache**     | 24 MiB (×1 shared)                                            |
| **RAM**                  | DDR4-3200 (32GB)                                              |
| **GPU**                  | NVIDIA RTX A2000 4GB GDDR6                                    |

### Software Configuration

| Platform    | Benchmark Framework     | C++ Compiler              | nfx-datetime Version |
| ----------- | ----------------------- | ------------------------- | -------------------- |
| **Linux**   | Google Benchmark v1.9.4 | GCC 14.2.0-x64            | v1.0.0               |
| **Linux**   | Google Benchmark v1.9.4 | Clang 19.1.7-x64          | v1.0.0               |
| **Windows** | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64      | v1.0.0               |
| **Windows** | Google Benchmark v1.9.4 | Clang-GNU-CLI 19.1.5-x64  | v1.0.0               |
| **Windows** | Google Benchmark v1.9.4 | Clang-MSVC-CLI 19.1.5-x64 | v1.0.0               |
| **Windows** | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64    | v1.0.0               |

---

# Performance Results

## DateTime Operations

| Operation                      | Linux GCC    | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------------ | ------------ | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| **Construct from Y-M-D**       | 2,191 ns     | **1,947 ns** | 2,863 ns          | 2,304 ns              | 2,252 ns               | 2,232 ns     |
| **Construct from Y-M-D-H-M-S** | 2,197 ns     | **1,951 ns** | 2,621 ns          | 2,284 ns              | 2,209 ns               | 2,208 ns     |
| **DateTime::now()**            | **13.2 ns**  | 13.7 ns      | 37.1 ns           | 20.8 ns               | 20.5 ns                | 23.0 ns      |
| **Parse ISO 8601 Basic**       | 2,178 ns     | **1,946 ns** | 2,532 ns          | 2,321 ns              | 2,244 ns               | 2,048 ns     |
| **Parse ISO 8601 Extended**    | 2,201 ns     | **1,958 ns** | 2,587 ns          | 2,283 ns              | 2,264 ns               | 2,146 ns     |
| **ToString ISO 8601**          | **1,526 ns** | 2,116 ns     | 1,977 ns          | 3,628 ns              | 3,617 ns               | 4,604 ns     |
| **toIso8601Extended**          | **1,555 ns** | 2,117 ns     | 2,044 ns          | 4,147 ns              | 3,937 ns               | 5,055 ns     |
| **Add TimeSpan**               | **0.108 ns** | 0.211 ns     | 0.111 ns          | 0.248 ns              | 0.460 ns               | 0.858 ns     |
| **Subtract TimeSpan**          | **0.106 ns** | 0.211 ns     | 0.116 ns          | 0.252 ns              | 0.244 ns               | 0.867 ns     |
| **Subtract DateTime**          | 0.215 ns     | **0.214 ns** | 0.241 ns          | 0.270 ns              | 0.258 ns               | 0.883 ns     |
| **ToUnixSeconds**              | **0.107 ns** | 0.214 ns     | 0.112 ns          | 0.565 ns              | 0.511 ns               | 1.10 ns      |
| **FromUnixSeconds**            | **0.330 ns** | 0.433 ns     | 0.403 ns          | 0.560 ns              | 0.532 ns               | 1.13 ns      |
| **GetComponents**              | **3,919 ns** | 5,884 ns     | 4,490 ns          | 7,018 ns              | 6,662 ns               | 8,087 ns     |
| **Comparison**                 | 0.355 ns     | 0.318 ns     | **0.255 ns**      | 0.394 ns              | 0.368 ns               | 0.868 ns     |

---

## DateTimeOffset Operations

| Operation                       | Linux GCC    | Linux Clang  | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------------- | ------------ | ------------ | ----------------- | --------------------- | ---------------------- | ------------ |
| **Construct with Offset**       | 2,201 ns     | **1,935 ns** | 3,039 ns          | 2,365 ns              | 2,296 ns               | 2,020 ns     |
| **DateTimeOffset::now()**       | 425 ns       | 437 ns       | **91.6 ns**       | 23,303 ns             | 20,795 ns              | 23,244 ns    |
| **Parse with Offset**           | 2,246 ns     | **1,980 ns** | 2,914 ns          | 2,495 ns              | 2,357 ns               | 2,348 ns     |
| **Parse UTC**                   | 2,215 ns     | **1,979 ns** | 2,813 ns          | 2,350 ns              | 2,311 ns               | 2,248 ns     |
| **ToUTC**                       | 0.643 ns     | **0.433 ns** | 0.597 ns          | 0.566 ns              | 0.541 ns               | 1.11 ns      |
| **ToOffset**                    | 0.879 ns     | **0.443 ns** | 0.772 ns          | 0.658 ns              | 0.654 ns               | 1.16 ns      |
| **GetUTCDateTime**              | **0.279 ns** | 0.430 ns     | 0.352 ns          | 0.518 ns              | 0.522 ns               | 1.14 ns      |
| **ToString**                    | **4,072 ns** | 6,151 ns     | 5,304 ns          | 8,221 ns              | 8,515 ns               | 9,950 ns     |
| **Add TimeSpan**                | 0.212 ns     | **0.211 ns** | 0.233 ns          | 0.319 ns              | 0.250 ns               | 0.855 ns     |
| **Subtract DateTimeOffset**     | **0.212 ns** | 0.269 ns     | 0.262 ns          | 0.413 ns              | 0.418 ns               | 0.899 ns     |
| **Comparison_DifferentOffsets** | 0.292 ns     | 0.352 ns     | **0.262 ns**      | 0.426 ns              | 0.480 ns               | 0.909 ns     |

---

## TimeSpan Operations

| Operation                    | Linux GCC    | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ---------------------------- | ------------ | ----------- | ----------------- | --------------------- | ---------------------- | ------------ |
| **FromHours**                | **0.106 ns** | 0.213 ns    | 0.112 ns          | 0.227 ns              | 0.231 ns               | 0.800 ns     |
| **FromMinutes**              | **0.106 ns** | 0.210 ns    | 0.109 ns          | 0.230 ns              | 0.227 ns               | 0.805 ns     |
| **FromSeconds**              | **0.107 ns** | 0.214 ns    | 0.108 ns          | 0.231 ns              | 0.228 ns               | 0.785 ns     |
| **FromMilliseconds**         | **0.107 ns** | 0.215 ns    | 0.108 ns          | 0.230 ns              | 0.225 ns               | 0.798 ns     |
| **Parse ISO 8601 Simple**    | 30.1 ns      | **28.1 ns** | 53.5 ns           | 80.0 ns               | 78.5 ns                | 130 ns       |
| **Parse ISO 8601 with Days** | 30.9 ns      | **27.8 ns** | 56.6 ns           | 85.6 ns               | 82.4 ns                | 133 ns       |
| **Parse Numeric**            | 22.3 ns      | **12.3 ns** | 40.9 ns           | 60.7 ns               | 59.6 ns                | 86.8 ns      |
| **ToString ISO 8601**        | **156 ns**   | 163 ns      | 198 ns            | 932 ns                | 904 ns                 | 1,013 ns     |
| **Add**                      | **0.106 ns** | 0.210 ns    | 0.109 ns          | 0.229 ns              | 0.226 ns               | 0.793 ns     |
| **Subtract**                 | **0.107 ns** | 0.210 ns    | 0.113 ns          | 0.228 ns              | 0.226 ns               | 0.781 ns     |
| **Negate**                   | **0.106 ns** | 0.212 ns    | 0.110 ns          | 0.231 ns              | 0.229 ns               | 0.778 ns     |
| **TotalHours**               | **0.107 ns** | 0.211 ns    | 0.108 ns          | 0.230 ns              | 0.227 ns               | 0.801 ns     |
| **TotalSeconds**             | **0.107 ns** | 0.210 ns    | 0.109 ns          | 0.230 ns              | 0.232 ns               | 0.800 ns     |
| **TotalMilliseconds**        | **0.108 ns** | 0.214 ns    | 0.108 ns          | 0.230 ns              | 0.228 ns               | 0.788 ns     |
| **Comparison**               | **0.107 ns** | 0.212 ns    | 0.108 ns          | 0.235 ns              | 0.235 ns               | 0.824 ns     |

---

_Benchmarks executed on October 26, 2025_
