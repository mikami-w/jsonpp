**[English](./README.md)** | **[简体中文](./README.zh.md)**

---

# jsonpp (v0.1.4)

Jsonpp is a modern, header-only C++17 JSON library built around a configurable `basic_json` template.

It focuses on:
- Type-safe JSON value handling via `std::variant`.
- Flexible container customization (`std::map` or `std::unordered_map`, etc.).
- Generic stream-based parsing and serialization.
- A fast path for contiguous input (`std::string_view`).

## Table of Contents

- [Why jsonpp](#why-jsonpp)
- [Requirements](#requirements)
- [Quick Start](#quick-start)
- [Integration](#integration)
- [Core Types and Design](#core-types-and-design)
- [Implemented API (v0.1.4)](#implemented-api-v011)
- [Parsing and Serialization](#parsing-and-serialization)
- [Error Handling](#error-handling)
- [Performance Notes](#performance-notes)
- [Known Limitations and Roadmap](#known-limitations-and-roadmap)
- [Testing](#testing)
- [Project Layout](#project-layout)
- [License](#license)

## Why jsonpp

- **Header-only**: Include and use without building a static/shared library.
- **Template-first architecture**: `basic_json` lets you customize object/array/string/number types.
- **IO abstraction**: Works with `std::string_view`, `std::istream`, and custom streams.
- **Round-trip tested**: Parse/serialize consistency is covered by the test suite.

## Requirements

- C++17 or newer.
- CMake 3.24+ (for building/running tests in this repository).

## Quick Start

```cpp
#include "jsonpp.hpp"
#include <iostream>

int main() {
	using jsonpp::json;

	json j = json::parse(R"({"name":"Mikami","age":25,"tags":["cpp","json"]})");

	std::cout << j.at("name").as_string() << "\n";  // Mikami
	std::cout << j["age"].as_int() << "\n";      // 25

	j["active"] = true;
	j["score"] = 98.5;

	std::cout << j.stringify() << "\n";
	std::cout << j.pretty("  ") << "\n";
}
```

## Integration

### Option 1: Include from `src/`

Add this repository's `src/` to your include path, then:

```cpp
#include "jsonpp.hpp"
```

### Option 2: Use single-header distribution

Use `single_include/jsonpp.hpp` when a single-file drop-in is preferred.

### Minimal CMake example

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app)

add_executable(my_app main.cpp)
target_include_directories(my_app PRIVATE /path/to/jsonpp/src)
```

## Core Types and Design

Primary aliases:

- `jsonpp::json` = `basic_json<>`
- `jsonpp::unordered_json` = `basic_json<std::unordered_map>`
- `jsonpp::null` = JSON null value

The core value type is implemented with `std::variant` and supports:

- empty (default-constructed state)
- null
- bool
- int64
- double
- string
- array
- object

### `json` vs `unordered_json`

- `json` (map-based object): deterministic key order in serialization.
- `unordered_json` (hash-map object): typically faster lookup, but key order is not stable.

## Implemented API (v0.1.4)

This section lists APIs that are implemented in the current codebase.

### Construction and type checks

- Constructors from null/bool/integer/floating/string/array/object.
- `type()`, `empty()`, `is_null()`, `is_bool()`, `is_number()`, `is_int()`, `is_float()`, `is_string()`, `is_array()`, `is_object()`.
- `set_type<Type::...>(clear_content = false)` and `set_type(Type, clear_content = false)`.
- `size()`.

### Access and conversion

- Array access: `operator[](size_t)`, `at(size_t)`.
- Object access: `operator[](const std::string&)`, `at(const std::string&)`, `contains(const std::string&)`.
- Safe pointers: `get_if_bool/int/float/string/array/object()`.
- Checked conversion: `as_bool/int/float/string/array/object()`.

Behavior notes:

- `operator[](size_t)` uses an internal assertion for bounds checking in debug builds; use `at(size_t)` when you need checked access with exceptions.
- Non-const `operator[](const std::string&)` converts an empty/null value to an object before insertion.
- Const `operator[](const std::string&)` delegates to `at(...)` and throws if the key does not exist.

### Mutation

- `push_back(...)`, `emplace_back(...)` for arrays.
- `insert(std::pair<string, json>)`, `emplace(...)` for objects.
- `swap(...)` and ADL `swap`.

### Parse and dump

- `parse(std::string_view)`
- `parse(std::istream&)`
- `parse(StreamT&)` for custom stream types satisfying JSON stream traits.
- `dump(std::string&, pretty = false, indent = "\t")`
- `dump(std::ostream&, pretty = false, indent = "\t")`
- `dump(SerializeHandlerT&, pretty = false, indent = "\t")`
- `stringify()` and `pretty(indent)`

### Comparison and output

- `operator==`, `operator!=`
- `operator<<`

## Parsing and Serialization

### Parse from string-like input

```cpp
auto j = jsonpp::json::parse(R"({"k":1,"arr":[1,2,3]})");
```

Note: parsing an empty/whitespace-only document returns the library's `empty` JSON state.

### Parse from stream

```cpp
#include <sstream>

std::stringstream ss(R"({"ok":true})");
auto j = jsonpp::json::parse(ss);
```

### Dump to string / ostream

```cpp
std::string compact = j.stringify();
std::string pretty = j.pretty("  ");

j.dump(std::cout, true, "  ");
```

### Custom input stream type

To be accepted by `parse(StreamT&)`, a stream type must provide:

- `int peek()`
- `int advance()`
- `std::size_t tell_pos()`
- `bool eof()`

Optional capabilities (`size`, `seek`, `get_chunk`, `read_chunk_until`) enable optimized parsing paths.

### Custom serialization handler type

To be accepted by `dump(SerializeHandlerT&)`, a handler must provide:

- `append(char)`
- `append(std::string_view)`
- `append(const char*, std::size_t)`

## Error Handling

Jsonpp defines dedicated exception types:

- `JsonParseError`: malformed JSON, invalid escapes, number parse errors, trailing tokens, etc.
- `JsonTypeError`: invalid typed access (for example `as_int()` on a string).
- `JsonOutOfRange`: array index out-of-range or missing object key via `at(...)`.
- `JsonDepthLimitExceeded`: nesting exceeds `MAX_NESTING_DEPTH` (default `1024`).

Example:

```cpp
try {
	auto j = jsonpp::json::parse("{bad json}");
} catch (const jsonpp::JsonParseError& e) {
	// handle parse failure
}
```

## Performance Notes

- `parse(std::string_view)` uses a contiguous-stream path (`StringViewStream`) with chunk reads, reducing per-character overhead.
- `parse(std::istream&)` uses a generic stream adapter (`IStreamStream`) suitable for large input sources.
- Number parsing uses `std::from_chars` for both integer and floating-point conversion.
- Serializer emits floating numbers with a decimal suffix when needed (for example `1.0`).

## Known Limitations and Roadmap

The following are currently declared in headers but not fully implemented in v0.1.4:

- Full iterator support (`begin/end/rbegin/...`) and iterator-based modifiers.
- Several STL-like helpers/modifiers such as `max_size`, `capacity`, `reserve`, `shrink_to_fit`, `front`, `back`, `value`, `find`, `count`, `resize`, `erase`, `update`, and some `insert` overloads.
- Stateful allocator instance storage in `basic_json`.
- Planned extension points noted in code comments (such as ADL serializer customization and binary type support).

If you need a method listed above, check implementation status before use.

## Testing

This repository includes:

- correctness tests
- usage tests
- round-trip tests (multiple JSON fixtures)
- spot-check tests for realistic nested data

Run tests:

```bash
cmake -B build -S .
cmake --build build
cd build
ctest --output-on-failure
```

Key test files:

- `tests/gtest_correctness.cpp`
- `tests/gtest_usage.cpp`
- `tests/gtest_roundtrip.cpp`
- `tests/gtest_spotcheck.cpp`

## Project Layout

- `src/` - main headers
- `src/detail/` - parser, serializer, traits, stream adapters, core implementation
- `single_include/` - amalgamated single-header distribution
- `tests/` - GoogleTest-based test suite and test data

## License

Apache License 2.0. See [LICENSE](./LICENSE) for details.
