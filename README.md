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
- **Stream Support**: Flexible I/O with custom stream adaptors
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

Simply copy the following files to your project:
- `jsonpp.h`
- `jsonpp.cpp`
- `jsonexception.h`
- `json_stream_adaptor.h`
- `stream_traits.h`

## Quick Start

### Parsing JSON

```cpp
#include "jsonpp.h"
#include <iostream>

int main() {
    // Parse JSON string
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
#include "jsonpp.h"

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

The library uses custom exceptions defined in `jsonexception.h`:

```cpp
#include "jsonpp.h"
#include "jsonexception.h"

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

## Stream Support

The library includes custom stream adaptors for flexible I/O:

```cpp
#include "json_stream_adaptor.h"
#include "stream_traits.h"

// Custom stream handling
// See json_stream_adaptor.h for details on implementing custom streams
```

## Testing

The project includes comprehensive test suites:

- **Usability Tests**: Located in `tests/usability/`
  - Tests for common usage patterns
  - Generated via `.generateUsabilityTestFiles.sh`
  
- **Error Handling Tests**: Located in `tests/error_handling/`
  - Tests for invalid JSON and error conditions
  - Generated via `.generateErrorHandlingTestFiles.sh`

To run tests:

```bash
cd cmake-build-debug
./JSONpp
```

## Project Structure

```
JSONpp/
├── jsonpp.h                 # Main header file
├── jsonpp.cpp               # Implementation
├── jsonexception.h          # Exception definitions
├── json_stream_adaptor.h    # Stream adaptor utilities
├── stream_traits.h          # Stream traits
├── CMakeLists.txt           # Build configuration
├── LICENSE                  # Apache License 2.0
├── tests/
│   ├── test.cpp               # Test implementations
│   ├── .generateUsabilityTestFiles.sh      # Usability test cases generator
│   ├── .generateErrorHandlingTestFiles.sh  # Error handling test cases generator
│   ├── usability/             # Usability test cases
│   └── error_handling/        # Error handling test cases
├── README.md                # This file
└── README.zh.md             # This file in Chinese
```

## Performance Considerations

- Uses `std::variant` for type-safe value storage
- Zero-copy string views for parsing where possible
- Move semantics for efficient value transfers
- `std::unordered_map` for fast object key lookups

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
A: Yes, read the file into a string first, then use `JSONpp::parse()`:
```cpp
std::ifstream file("data.json");
std::string content((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
json j = JSONpp::parse(content);
```

**Q: How do I handle large numbers?**  
A: The library uses `std::int64_t` for integers and `double` for floating-point. For arbitrary precision, you may need to store numbers as strings.

**Q: Does it support JSON comments?**  
A: No, pure JSON does not support comments. Use JSONC or JSON5 parsers if you need comment support.

## Acknowledgments

Built with modern C++17 features for a clean and efficient JSON handling experience.

