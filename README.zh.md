**[English](./README.md)** | **[简体中文](./README.zh.md)**

---

# jsonpp (v0.1.4)

jsonpp 是一个现代、仅头文件（Header-only）的 C++17 JSON 库，核心围绕可配置的 `basic_json` 模板构建。

它重点关注：
- 基于 `std::variant` 的类型安全 JSON 值处理。
- 灵活的底层容器定制（如 `std::map`、`std::unordered_map`）。
- 通用的流式解析与序列化抽象。
- 针对连续输入（`std::string_view`）的快速解析路径。

## 目录

- [为什么选择 jsonpp](#why-jsonpp)
- [环境要求](#requirements)
- [快速开始](#quick-start)
- [集成方式](#integration)
- [核心类型与设计](#core-types-and-design)
- [已实现 API（v0.1.4）](#implemented-api-v011)
- [解析与序列化](#parsing-and-serialization)
- [错误处理](#error-handling)
- [性能说明](#performance-notes)
- [已知限制与路线图](#known-limitations-and-roadmap)
- [测试](#testing)
- [项目结构](#project-layout)
- [许可证](#license)

<a id="why-jsonpp"></a>
## 为什么选择 jsonpp

- **仅头文件**：无需构建静态库或动态库，直接包含即可使用。
- **模板优先架构**：`basic_json` 允许你定制 object/array/string/number 等底层类型。
- **IO 抽象**：同时支持 `std::string_view`、`std::istream` 以及自定义流。
- **Round-trip 测试覆盖**：解析与序列化一致性由测试集保障。

<a id="requirements"></a>
## 环境要求

- C++17 或更高版本。
- CMake 3.24+（用于构建并运行本仓库测试）。

<a id="quick-start"></a>
## 快速开始

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

<a id="integration"></a>
## 集成方式

### 方案 1：从 `src/` 目录包含

将本仓库的 `src/` 添加到头文件搜索路径，然后：

```cpp
#include "jsonpp.hpp"
```

### 方案 2：使用单头文件分发版

如果你偏好单文件集成，可使用 `single_include/jsonpp.hpp`。

### 最小 CMake 示例

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app)

add_executable(my_app main.cpp)
target_include_directories(my_app PRIVATE /path/to/jsonpp/src)
```

<a id="core-types-and-design"></a>
## 核心类型与设计

主要别名：

- `jsonpp::json` = `basic_json<>`
- `jsonpp::unordered_json` = `basic_json<std::unordered_map>`
- `jsonpp::null` = JSON null 值

核心值类型由 `std::variant` 承载，支持：

- empty（默认构造状态）
- null
- bool
- int64
- double
- string
- array
- object

### `json` 与 `unordered_json`

- `json`（基于 map 的 object）：序列化时键顺序稳定。
- `unordered_json`（基于哈希表的 object）：通常查找更快，但键顺序不稳定。

<a id="implemented-api-v011"></a>
## 已实现 API（v0.1.4）

本节只列出当前代码中已经实现的 API。

### 构造与类型判断

- 支持从 null/bool/整数/浮点/字符串/数组/对象构造。
- `type()`、`empty()`、`is_null()`、`is_bool()`、`is_number()`、`is_int()`、`is_float()`、`is_string()`、`is_array()`、`is_object()`。
- `set_type<Type::...>(clear_content = false)` 与 `set_type(Type, clear_content = false)`。
- `size()`。

### 访问与类型转换

- 数组访问：`operator[](size_t)`、`at(size_t)`。
- 对象访问：`operator[](const std::string&)`、`at(const std::string&)`、`contains(const std::string&)`。
- 安全指针接口：`get_if_bool/int/float/string/array/object()`。
- 受检转换接口：`as_bool/int/float/string/array/object()`。

行为说明：

- `operator[](size_t)` 在调试构建中使用内部断言进行边界检查；若需要抛异常的受检访问，请使用 `at(size_t)`。
- 非 const 的 `operator[](const std::string&)` 在值为 empty/null 时会先转换为 object 再插入键值。
- const 的 `operator[](const std::string&)` 会委托给 `at(...)`，键不存在时抛异常。

### 修改接口

- 数组：`push_back(...)`、`emplace_back(...)`。
- 对象：`insert(std::pair<string, json>)`、`emplace(...)`。
- 交换：`swap(...)` 与 ADL `swap`。

### 解析与输出

- `parse(std::string_view)`
- `parse(std::istream&)`
- `parse(StreamT&)`（要求 `StreamT` 满足 JSON stream traits）
- `dump(std::string&, pretty = false, indent = "\t")`
- `dump(std::ostream&, pretty = false, indent = "\t")`
- `dump(SerializeHandlerT&, pretty = false, indent = "\t")`
- `stringify()` 与 `pretty(indent)`

### 比较与流输出

- `operator==`、`operator!=`
- `operator<<`

<a id="parsing-and-serialization"></a>
## 解析与序列化

### 从字符串类输入解析

```cpp
auto j = jsonpp::json::parse(R"({"k":1,"arr":[1,2,3]})");
```

说明：空文档或仅包含空白字符的文档，解析结果为库中的 `empty` JSON 状态。

### 从流解析

```cpp
#include <sstream>

std::stringstream ss(R"({"ok":true})");
auto j = jsonpp::json::parse(ss);
```

### 输出到字符串 / 输出流

```cpp
std::string compact = j.stringify();
std::string pretty = j.pretty("  ");

j.dump(std::cout, true, "  ");
```

### 自定义输入流类型

若要被 `parse(StreamT&)` 接受，流类型至少需要提供：

- `int peek()`
- `int advance()`
- `std::size_t tell_pos()`
- `bool eof()`

可选能力（`size`、`seek`、`get_chunk`、`read_chunk_until`）可启用优化解析路径。

### 自定义序列化处理器类型

若要被 `dump(SerializeHandlerT&)` 接受，处理器需要提供：

- `append(char)`
- `append(std::string_view)`
- `append(const char*, std::size_t)`

<a id="error-handling"></a>
## 错误处理

jsonpp 提供了专用异常类型：

- `JsonParseError`：JSON 格式非法、转义非法、数字解析失败、尾随字符等。
- `JsonTypeError`：类型访问不匹配（例如对字符串调用 `as_int()`）。
- `JsonOutOfRange`：数组越界，或通过 `at(...)` 访问不存在的键。
- `JsonDepthLimitExceeded`：嵌套深度超过 `MAX_NESTING_DEPTH`（默认 `1024`）。

示例：

```cpp
try {
	auto j = jsonpp::json::parse("{bad json}");
} catch (const jsonpp::JsonParseError& e) {
	// 处理解析失败
}
```

<a id="performance-notes"></a>
## 性能说明

- `parse(std::string_view)` 走连续流路径（`StringViewStream`），通过分块读取减少逐字符开销。
- `parse(std::istream&)` 使用通用流适配器（`IStreamStream`），适合大输入源。
- 数字解析使用 `std::from_chars`（整数与浮点）。
- 序列化时会在必要时补齐浮点小数部分（例如 `1.0`）。

<a id="known-limitations-and-roadmap"></a>
## 已知限制与路线图

以下能力目前在头文件中有声明，但在 v0.1.4 尚未完全实现：

- 完整迭代器支持（`begin/end/rbegin/...`）及基于迭代器的修改接口。
- 若干 STL 风格辅助/修改接口，如 `max_size`、`capacity`、`reserve`、`shrink_to_fit`、`front`、`back`、`value`、`find`、`count`、`resize`、`erase`、`update` 以及部分 `insert` 重载。
- `basic_json` 中对有状态分配器实例的持有。
- 代码注释中标记的扩展点（如 ADL serializer 定制、binary type 支持）。

如果你需要使用上述方法，请先确认具体实现状态。

<a id="testing"></a>
## 测试

本仓库包含：

- 正确性测试（correctness）
- 用法测试（usage）
- round-trip 测试（多组 JSON 样例）
- 真实复杂结构抽查测试（spot-check）

运行测试：

```bash
cmake -B build -S .
cmake --build build
cd build
ctest --output-on-failure
```

关键测试文件：

- `tests/gtest_correctness.cpp`
- `tests/gtest_usage.cpp`
- `tests/gtest_roundtrip.cpp`
- `tests/gtest_spotcheck.cpp`

<a id="project-layout"></a>
## 项目结构

- `src/`：主头文件目录
- `src/detail/`：解析器、序列化器、traits、流适配器、核心实现
- `single_include/`：单头文件分发版本
- `tests/`：基于 GoogleTest 的测试与测试数据

<a id="license"></a>
## 许可证

Apache License 2.0。详见 [LICENSE](./LICENSE)。