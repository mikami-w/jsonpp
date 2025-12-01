# JSONpp

A modern C++17 JSON parsing and serialization library with a clean and intuitive API.

[简体中文](README.zh.md)

## Features

- **C++17 Standard**: Leverages modern C++ features for type safety and performance
- **Header-based Design**: Easy integration into your projects
- **Complete JSON Support**: Parse and generate all JSON data types
  - `null`
  - `boolean`
  - `number` (integer and floating-point)
  - `string`
  - `array`
  - `object`
- **Type-safe API**: Compile-time type checking with templates
- **Exception-based Error Handling**: Clear error messages for invalid JSON
- **Stream Parsing Support**: Parse from `std::string_view` and `std::istream` with custom stream adaptors
- **Flexible Stream Architecture**: Support for different stream types (contiguous, seekable, sized, or sequential)
- **Unicode Support**: Full UTF-8 encoding/decoding with proper surrogate pair handling
- **Zero Dependencies**: Only requires C++17 standard library

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10 or higher (for building)

## Installation

### Using CMake

```bash
git clone https://github.com/yourusername/JSONpp.git
cd JSONpp
mkdir build && cd build
cmake ..
make
```

### Header-only Integration

Simply copy the following files from the `src/` folder to your project:
- `src/jsonpp.hpp` - Main JSON class and API
- `src/jsonpp.cpp` - Implementation
- `src/jsonexception.hpp` - Exception definitions
- `src/parser.hpp` - JSON parsing logic
- `src/serializer.hpp` - JSON serialization logic
- `src/json_stream_adaptor.hpp` - Stream adaptor implementations
- `src/stream_traits.hpp` - Stream type traits

## Quick Start

### Parsing JSON

```cpp
#include "jsonpp.hpp"
#include <iostream>
#include <fstream>

int main() {
    // Method 1: Parse from string (uses zero-copy StringViewStream)
    auto j = JSONpp::parse(R"({
        "name": "John Doe",
        "age": 30,
        "is_student": false,
        "courses": ["Math", "Physics", "Chemistry"],
        "address": {
            "city": "New York",
            "zip": "10001"
        }
    })");
    
    // Method 2: Parse from file stream (uses IStreamStream)
    std::ifstream file("data.json");
    auto j2 = JSONpp::parse(file);
    
    // Access values
    std::cout << j["name"].as_string() << std::endl;        // "John Doe"
    std::cout << j["age"].as_int() << std::endl;             // 30
    std::cout << j["is_student"].as_bool() << std::endl;     // false
    std::cout << j["courses"][0].as_string() << std::endl;   // "Math"
    std::cout << j["address"]["city"].as_string() << std::endl; // "New York"
    
    return 0;
}
```

### Creating JSON

```cpp
#include "jsonpp.hpp"

int main() {
    using namespace JSONpp;
    
    // Create JSON object
    json obj = object{
        {"name", "Alice"},
        {"age", 25},
        {"is_active", true},
        {"balance", 1234.56}
    };
    
    // Create JSON array
    json arr = array{1, 2, 3, 4, 5};
    
    // Nested structures
    json data = object{
        {"user", object{
            {"id", 1001},
            {"username", "alice123"}
        }},
        {"scores", array{95, 87, 92, 88}}
    };
    
    // Output as JSON string
    std::cout << data.stringify() << std::endl;
    
    return 0;
}
```

### Type Checking

```cpp
using namespace JSONpp;

json j = parse(R"({"value": 42})");

// Check type before access
if (j.is_object()) {
    auto& obj = j.as_object();
    if (obj.count("value") && obj["value"].is_int()) {
        std::cout << "Value: " << obj["value"].as_int() << std::endl;
    }
}

// Safe access with get_if
if (auto* num = j["value"].get_if_int()) {
    std::cout << "Integer value: " << *num << std::endl;
} else if (auto* str = j["value"].get_if_string()) {
    std::cout << "String value: " << *str << std::endl;
}
```

## API Reference

### JSON Types

- `JSONpp::null` - Represents JSON null value
- `JSONpp::array` - JSON array (`std::vector<json>`)
- `JSONpp::object` - JSON object (`std::unordered_map<std::string, json>`)

### Constructors

```cpp
json j1;                          // Empty JSON
json j2 = nullptr;                // null
json j3 = true;                   // boolean
json j4 = 42;                     // integer
json j5 = 3.14;                   // floating-point
json j6 = "hello";                // string
json j7 = array{1, 2, 3};         // array
json j8 = object{{"key", "val"}}; // object
```

### Type Checking Methods

- `bool empty()` - Returns true if JSON value is uninitialized
- `bool is_null()` - Check if value is null
- `bool is_bool()` - Check if value is boolean
- `bool is_number()` - Check if value is a number (int or float)
- `bool is_int()` - Check if value is integer
- `bool is_float()` - Check if value is floating-point
- `bool is_string()` - Check if value is string
- `bool is_array()` - Check if value is array
- `bool is_object()` - Check if value is object

### Safe Accessors (return pointer or nullptr)

```cpp
bool* get_if_bool()
std::int64_t* get_if_int()
double* get_if_float()
std::string* get_if_string()
array* get_if_array()
object* get_if_object()
```

### Asserted Accessors (throw exception on type mismatch)

```cpp
bool as_bool()
std::int64_t as_int()
double as_float()
std::string& as_string()
array& as_array()
object& as_object()
```

### Array and Object Access

```cpp
json arr = array{1, 2, 3};
arr[0].as_int();  // Access array element by index

json obj = object{{"key", "value"}};
obj["key"].as_string();  // Access object value by key
```

### Parsing and Serialization

```cpp
// Parse JSON string
json j = JSONpp::parse(json_string);

// Check if parse result is empty
if (j.empty()) {
    std::cout << "Empty or whitespace-only input" << std::endl;
}

// Convert to JSON string
std::string json_str = j.stringify();

// Stream output (with proper escaping)
std::cout << j << std::endl;
```

### Assignment

```cpp
json j;
j = 42;                    // Assign integer
j = "hello";               // Assign string
j = array{1, 2, 3};        // Assign array
j = object{{"k", "v"}};    // Assign object
```

## Exception Handling

The library uses custom exceptions defined in `jsonexception.hpp`.

- `JSONpp::JSONParseError` - Thrown on parsing errors
- `JSONpp::JSONTypeError` - Thrown on type mismatch during asserted access

The library enforces strict exception types: parsing operations throw only `JSONParseError`, while type-checked accessors (e.g., as_string) throw only `JSONTypeError`.

```cpp
#include "jsonpp.hpp"
#include "jsonexception.hpp"

try {
    auto j = JSONpp::parse(R"({"invalid": json})");
} catch (const JSONpp::JSONParseError& e) {
    std::cerr << "JSON Error: " << e.what() << std::endl;
}

try {
    json j = 42;
    std::string s = j.as_string();  // Type mismatch!
} catch (const JSONpp::JSONTypeError& e) {
    std::cerr << "Type error: " << e.what() << std::endl;
}
```

## Stream Parsing Support

The library provides flexible stream parsing capabilities with multiple built-in stream adaptors:

### Parsing from String View (Zero-copy)

```cpp
#include "jsonpp.hpp"

std::string_view json_data = R"({"key": "value"})";
json j = JSONpp::parse(json_data);  // Zero-copy parsing
```

### Parsing from std::istream

```cpp
#include "jsonpp.hpp"
#include <fstream>

std::ifstream file("data.json");
json j = JSONpp::parse(file);  // Direct stream parsing
file.close();
```

### Stream Adaptor Architecture

The library uses a trait-based stream system that supports different stream capabilities:

- **Basic Stream**: Provides `peek()`, `advance()`, `tell_pos()`, and `eof()`
- **Sized Stream**: Additionally provides `size()` to get total stream size
- **Seekable Stream**: Additionally provides `seek()` for random access
- **Contiguous Stream**: Additionally provides `get_chunk()` and `read_chunk_until()` for efficient bulk operations

#### Built-in Stream Adaptors

1. **StringViewStream**: Zero-copy parsing from `std::string_view`
   - Contiguous, sized, and seekable
   - Optimal performance for in-memory JSON

2. **IStreamStream**: Parse from any `std::istream`
   - Sequential access only
   - Supports file streams, string streams, network streams, etc.

### Custom Stream Implementation

You can implement custom stream adaptors by providing the required interface:

```cpp
class CustomStream {
public:
    char peek() const noexcept;      // Peek current character
    char advance() noexcept;         // Consume and return current character
    std::size_t tell_pos() const noexcept;  // Get current position
    bool eof() const noexcept;       // Check if at end of stream
    
    // Optional: for sized streams
    std::size_t size() const noexcept;
    
    // Optional: for seekable streams
    void seek(std::size_t step) noexcept;
    
    // Optional: for contiguous streams
    std::string_view get_chunk(std::size_t begin, std::size_t length) const noexcept;
    template <typename FunctorT>
    std::string_view read_chunk_until(FunctorT predicate) &;
};

// Use with parser
CustomStream stream(...);
json j = JSONpp::parse(stream);
```

See `src/json_stream_adaptor.hpp` and `src/stream_traits.hpp` for implementation details.

## Testing

The project includes comprehensive test suites:

- **Usability Tests**: Located in `tests/usability/`
  - Tests for common usage patterns
  - Generated via `.generateUsabilityTestFiles.sh`
  
- **Error Handling Tests**: Located in `tests/error_handling/`
  - Tests for invalid JSON and error conditions
  - Generated via `.generateErrorHandlingTestFiles.sh`

To build and run tests:

```bash
mkdir build && cd build
cmake ..
make
./JSONpp
```

## Project Structure

```
JSONpp/
├── src/
│   ├── jsonpp.hpp                 # Main JSON class and public API
│   ├── jsonpp.cpp                 # Implementation of json class methods
│   ├── jsonexception.hpp          # Exception definitions (JSONParseError, JSONTypeError, etc.)
│   ├── parser.hpp                 # JSON parsing logic with template stream support
│   ├── serializer.hpp             # JSON serialization and stringification
│   ├── json_stream_adaptor.hpp    # Stream adaptor implementations (StringViewStream, IStreamStream)
│   └── stream_traits.hpp          # Stream type traits and concepts
├── tests/
│   ├── test.cpp                   # Test runner implementation
│   ├── .generateUsabilityTestFiles.sh      # Script to generate usability test cases
│   └── .generateErrorHandlingTestFiles.sh  # Script to generate error handling test cases
├── CMakeLists.txt                 # Build configuration
├── LICENSE                        # Apache License 2.0
├── README.md                      # English documentation (this file)
└── README.zh.md                   # Chinese documentation
```


## Architecture

### Core Components

1. **json Class (`jsonpp.hpp`)**: The main JSON value container
   - Uses `std::variant` to store different JSON types
   - Provides type-safe accessors and type checking methods
   - Supports operator overloading for natural syntax

2. **Parser (`parser.hpp`)**: Template-based parsing engine
   - `Parser<StreamT>`: Main parser template accepting any stream type
   - `JSONStringParser<StreamT>`: Specialized string parser with Unicode support
   - `ParserBase<StreamT>`: Base class providing stream interface abstraction

3. **Serializer (`serializer.hpp`)**: JSON to string conversion
   - Implements `operator<<` for stream output
   - Handles proper string escaping and Unicode encoding
   - Supports all JSON data types

4. **Stream System**: Flexible input handling
   - **Traits (`stream_traits.hpp`)**: SFINAE-based type traits to detect stream capabilities
   - **Adaptors (`json_stream_adaptor.hpp`)**: Built-in stream implementations
     - `StringViewStream`: Contiguous, zero-copy access
     - `IStreamStream`: Sequential stream access

### Design Principles

- **Template-based Streams**: Parsers are templated on stream type, allowing compile-time optimization
- **Trait-based Dispatch**: Uses SFINAE to select optimal parsing strategy based on stream capabilities
- **Exception Safety**: Strong exception guarantee for parsing errors
- **Header-mostly Design**: Easy integration with minimal compilation overhead

## Performance Considerations

- **Type-safe Storage**: Uses `std::variant` for type-safe value storage with zero overhead
- **Zero-copy Parsing**: `StringViewStream` provides zero-copy parsing from `std::string_view`
- **Optimized String Parsing**: 
  - Contiguous streams use chunk-based parsing with `read_chunk_until()` for fast string extraction
  - Sequential streams fall back to character-by-character parsing
- **Move Semantics**: Efficient value transfers throughout the API
- **Fast Object Access**: `std::unordered_map` for O(1) average-case object key lookups
- **Unicode Handling**: Efficient UTF-8 encoding/decoding with proper surrogate pair support
- **Stream Adaptors**: Template-based stream architecture allows compiler optimizations based on stream capabilities

## Limitations

- Numbers are stored as `std::int64_t` or `double`
- Object keys must be strings (as per JSON specification)
- Unicode escape sequences are supported in strings
- Memory is managed automatically via standard containers

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## Examples

### Working with Arrays

```cpp
using namespace JSONpp;

json arr = array{1, 2, 3, 4, 5};

// Iterate through array
if (arr.is_array()) {
    for (const auto& elem : arr.as_array()) {
        std::cout << elem.as_int() << " ";
    }
}

// Modify array
arr.as_array().push_back(json(6));
arr.as_array()[0] = 10;
```

### Working with Objects

```cpp
using namespace JSONpp;

json obj = object{
    {"name", "Bob"},
    {"age", 35},
    {"city", "Boston"}
};

// Iterate through object
if (obj.is_object()) {
    for (const auto& [key, value] : obj.as_object()) {
        std::cout << key << ": " << value.stringify() << std::endl;
    }
}

// Add/modify fields
obj.as_object()["email"] = "bob@example.com";
obj.as_object()["age"] = 36;
```

### Complex Nested Structures

```cpp
using namespace JSONpp;

json config = object{
    {"server", object{
        {"host", "localhost"},
        {"port", 8080},
        {"ssl", true}
    }},
    {"database", object{
        {"connections", array{
            object{{"name", "primary"}, {"url", "db1.example.com"}},
            object{{"name", "replica"}, {"url", "db2.example.com"}}
        }}
    }},
    {"features", array{"auth", "logging", "caching"}}
};

std::cout << config.stringify() << std::endl;
```

## FAQ

**Q: Is this library thread-safe?**  
A: Individual `json` objects are not thread-safe. Use external synchronization if sharing objects between threads.

**Q: Can I parse JSON from a file?**  
A: Yes! The library supports direct stream parsing:
```cpp
// Method 1: Direct stream parsing (recommended)
std::ifstream file("data.json");
json j = JSONpp::parse(file);

// Method 2: Read to string first (for string_view benefits)
std::ifstream file("data.json");
std::string content((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
json j = JSONpp::parse(content);  // Uses zero-copy StringViewStream
```

**Q: What's the performance difference between string and stream parsing?**  
A: `StringViewStream` (used with `std::string_view`) provides the best performance due to zero-copy and contiguous memory access. `IStreamStream` is suitable for situations where you need streaming input (network, large files) but is slightly slower due to character-by-character reading.

**Q: How do I handle large numbers?**  
A: The library uses `std::int64_t` for integers (range: -2^63 to 2^63-1) and `double` for floating-point. For arbitrary precision, you may need to store numbers as strings.

**Q: Does it support JSON comments?**  
A: No, pure JSON does not support comments. Use JSONC or JSON5 parsers if you need comment support.

**Q: How is Unicode handled?**  
A: The library fully supports UTF-8 encoding and properly handles Unicode escape sequences (e.g., `\uXXXX`) including surrogate pairs for characters beyond the Basic Multilingual Plane.

## Acknowledgments

Built with modern C++17 features for a clean and efficient JSON handling experience.

