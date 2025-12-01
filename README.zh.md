# JSONpp

一个现代化的 C++17 JSON 解析和序列化库，提供简洁直观的 API。

[English](README.md)

## 特性

- **C++17 标准**: 利用现代 C++ 特性实现类型安全和高性能
- **头文件设计**: 易于集成到您的项目中
- **完整的 JSON 支持**: 解析和生成所有 JSON 数据类型
  - `null`（空值）
  - `boolean`（布尔值）
  - `number`（数字：整数和浮点数）
  - `string`（字符串）
  - `array`（数组）
  - `object`（对象）
- **类型安全的 API**: 使用模板进行编译时类型检查
- **基于异常的错误处理**: 为无效 JSON 提供清晰的错误信息
- **流式解析支持**: 支持从 `std::string_view` 和 `std::istream` 解析，提供自定义流适配器
- **灵活的流架构**: 支持不同类型的流（连续、可寻址、可获取大小或顺序流）
- **Unicode 支持**: 完整的 UTF-8 编码/解码，正确处理代理对
- **零依赖**: 仅需要 C++17 标准库

## 环境要求

- 支持 C++17 的编译器（GCC 7+、Clang 5+、MSVC 2017+）
- CMake 3.10 或更高版本（用于构建）

## 安装

### 使用 CMake

```bash
git clone https://github.com/yourusername/JSONpp.git
cd JSONpp
mkdir build && cd build
cmake ..
make
```

### 头文件集成

只需将 `src/` 文件夹中的以下文件复制到您的项目中：
- `src/jsonpp.hpp` - 主 JSON 类和公共 API
- `src/jsonpp.cpp` - 实现文件
- `src/jsonexception.hpp` - 异常定义
- `src/parser.hpp` - JSON 解析逻辑
- `src/serializer.hpp` - JSON 序列化逻辑
- `src/json_stream_adaptor.hpp` - 流适配器实现
- `src/stream_traits.hpp` - 流类型特征

## 快速开始

### 解析 JSON

```cpp
#include "jsonpp.hpp"
#include <iostream>
#include <fstream>

int main() {
    // 方法 1：从字符串解析（使用零拷贝的 StringViewStream）
    auto j = JSONpp::parse(R"({
        "name": "张三",
        "age": 30,
        "is_student": false,
        "courses": ["数学", "物理", "化学"],
        "address": {
            "city": "北京",
            "zip": "100000"
        }
    })");
    
    // 方法 2：从文件流解析（使用 IStreamStream）
    std::ifstream file("data.json");
    auto j2 = JSONpp::parse(file);
    
    // 访问值
    std::cout << j["name"].as_string() << std::endl;        // "张三"
    std::cout << j["age"].as_int() << std::endl;             // 30
    std::cout << j["is_student"].as_bool() << std::endl;     // false
    std::cout << j["courses"][0].as_string() << std::endl;   // "数学"
    std::cout << j["address"]["city"].as_string() << std::endl; // "北京"
    
    return 0;
}
```

### 创建 JSON

```cpp
#include "jsonpp.hpp"

int main() {
    using namespace JSONpp;
    
    // 创建 JSON 对象
    json obj = object{
        {"name", "小红"},
        {"age", 25},
        {"is_active", true},
        {"balance", 1234.56}
    };
    
    // 创建 JSON 数组
    json arr = array{1, 2, 3, 4, 5};
    
    // 嵌套结构
    json data = object{
        {"user", object{
            {"id", 1001},
            {"username", "xiaohong123"}
        }},
        {"scores", array{95, 87, 92, 88}}
    };
    
    // 输出为 JSON 字符串
    std::cout << data.stringify() << std::endl;
    
    return 0;
}
```

### 类型检查

```cpp
using namespace JSONpp;

json j = parse(R"({"value": 42})");

// 访问前检查类型
if (j.is_object()) {
    auto& obj = j.as_object();
    if (obj.count("value") && obj["value"].is_int()) {
        std::cout << "值: " << obj["value"].as_int() << std::endl;
    }
}

// 使用 get_if 进行安全访问
if (auto* num = j["value"].get_if_int()) {
    std::cout << "整数值: " << *num << std::endl;
} else if (auto* str = j["value"].get_if_string()) {
    std::cout << "字符串值: " << *str << std::endl;
}
```

## API 参考

### JSON 类型

- `JSONpp::null` - 表示 JSON null 值
- `JSONpp::array` - JSON 数组（`std::vector<json>`）
- `JSONpp::object` - JSON 对象（`std::unordered_map<std::string, json>`）

### 构造函数

```cpp
json j1;                          // 空 JSON
json j2 = nullptr;                // null
json j3 = true;                   // 布尔值
json j4 = 42;                     // 整数
json j5 = 3.14;                   // 浮点数
json j6 = "你好";                  // 字符串
json j7 = array{1, 2, 3};         // 数组
json j8 = object{{"key", "val"}}; // 对象
```

### 类型检查方法

- `bool empty()` - 如果 JSON 值未初始化则返回 true
- `bool is_null()` - 检查值是否为 null
- `bool is_bool()` - 检查值是否为布尔值
- `bool is_number()` - 检查值是否为数字（整数或浮点数）
- `bool is_int()` - 检查值是否为整数
- `bool is_float()` - 检查值是否为浮点数
- `bool is_string()` - 检查值是否为字符串
- `bool is_array()` - 检查值是否为数组
- `bool is_object()` - 检查值是否为对象

### 安全访问器（返回指针或 nullptr）

```cpp
bool* get_if_bool()
std::int64_t* get_if_int()
double* get_if_float()
std::string* get_if_string()
array* get_if_array()
object* get_if_object()
```

### 断言访问器（类型不匹配时抛出异常）

```cpp
bool as_bool()
std::int64_t as_int()
double as_float()
std::string& as_string()
array& as_array()
object& as_object()
```

### 数组和对象访问

```cpp
json arr = array{1, 2, 3};
arr[0].as_int();  // 通过索引访问数组元素

json obj = object{{"key", "value"}};
obj["key"].as_string();  // 通过键访问对象值
```

### 解析和序列化

```cpp
// 解析 JSON 字符串
json j = JSONpp::parse(json_string);

// 检查解析结果是否为空
if (j.empty()) {
    std::cout << "输入为空或仅包含空白字符" << std::endl;
}

// 转换为 JSON 字符串
std::string json_str = j.stringify();

// 流输出（自动进行适当的转义）
std::cout << j << std::endl;
```

### 赋值

```cpp
json j;
j = 42;                    // 赋值整数
j = "你好";                 // 赋值字符串
j = array{1, 2, 3};        // 赋值数组
j = object{{"k", "v"}};    // 赋值对象
```

## 异常处理

该库使用在 `jsonexception.hpp` 中定义的自定义异常。

- JSONpp::JSONParseError - 解析发生错误时抛出
- JSONpp::JSONTypeError - 断言式访问发生类型不匹配时抛出

本库执行严格的异常分类策略：解析操作仅会抛出`JSONParseError`，而带类型检查的访问器（例如 as_string）仅会抛出`JSONTypeError`。

```cpp
#include "jsonpp.hpp"
#include "jsonexception.hpp"

try {
    auto j = JSONpp::parse(R"({"invalid": json})");
} catch (const JSONpp::JSONParseError& e) {
    std::cerr << "JSON 错误: " << e.what() << std::endl;
}

try {
    json j = 42;
    std::string s = j.as_string();  // 类型不匹配！
} catch (const JSONpp::JSONTypeError& e) {
    std::cerr << "类型错误: " << e.what() << std::endl;
}
```

## 流式解析支持

该库提供灵活的流式解析功能，包含多种内置流适配器：

### 从字符串视图解析（零拷贝）

```cpp
#include "jsonpp.hpp"

std::string_view json_data = R"({"key": "value"})";
json j = JSONpp::parse(json_data);  // 零拷贝解析
```

### 从 std::istream 解析

```cpp
#include "jsonpp.hpp"
#include <fstream>

std::ifstream file("data.json");
json j = JSONpp::parse(file);  // 直接流式解析
file.close();
```

### 流适配器架构

该库使用基于特征的流系统，支持不同的流能力：

- **基础流**: 提供 `peek()`、`advance()`、`tell_pos()` 和 `eof()`
- **可获取大小流**: 额外提供 `size()` 获取流总大小
- **可寻址流**: 额外提供 `seek()` 用于随机访问
- **连续流**: 额外提供 `get_chunk()` 和 `read_chunk_until()` 用于高效批量操作

#### 内置流适配器

1. **StringViewStream**: 从 `std::string_view` 零拷贝解析
   - 连续、可获取大小、可寻址
   - 内存中 JSON 的最佳性能

2. **IStreamStream**: 从任意 `std::istream` 解析
   - 仅支持顺序访问
   - 支持文件流、字符串流、网络流等

### 自定义流实现

您可以通过提供必需的接口实现自定义流适配器：

```cpp
class CustomStream {
public:
    char peek() const noexcept;      // 查看当前字符
    char advance() noexcept;         // 消费并返回当前字符
    std::size_t tell_pos() const noexcept;  // 获取当前位置
    bool eof() const noexcept;       // 检查是否到达流末尾
    
    // 可选：用于可获取大小的流
    std::size_t size() const noexcept;
    
    // 可选：用于可寻址的流
    void seek(std::size_t step) noexcept;
    
    // 可选：用于连续流
    std::string_view get_chunk(std::size_t begin, std::size_t length) const noexcept;
    template <typename FunctorT>
    std::string_view read_chunk_until(FunctorT predicate) &;
};

// 与解析器配合使用
CustomStream stream(...);
json j = JSONpp::parse(stream);
```

有关实现细节，请参见 `src/json_stream_adaptor.hpp` 和 `src/stream_traits.hpp`。

## 测试

项目包含全面的测试套件：

- **可用性测试**: 位于 `tests/usability/`
  - 测试常见的使用模式
  - 通过 `.generateUsabilityTestFiles.sh` 生成
  
- **错误处理测试**: 位于 `tests/error_handling/`
  - 测试无效 JSON 和错误条件
  - 通过 `.generateErrorHandlingTestFiles.sh` 生成

构建并运行测试：

```bash
mkdir build && cd build
cmake ..
make
./JSONpp
```

## 项目结构

```
JSONpp/
├── src/
│   ├── jsonpp.hpp                 # 主 JSON 类和公共 API
│   ├── jsonpp.cpp                 # json 类方法实现
│   ├── jsonexception.hpp          # 异常定义（JSONParseError、JSONTypeError 等）
│   ├── parser.hpp                 # JSON 解析逻辑，支持模板化流
│   ├── serializer.hpp             # JSON 序列化和字符串化
│   ├── json_stream_adaptor.hpp    # 流适配器实现（StringViewStream、IStreamStream）
│   └── stream_traits.hpp          # 流类型特征和概念
├── tests/
│   ├── test.cpp                   # 测试运行器实现
│   ├── .generateUsabilityTestFiles.sh      # 生成可用性测试用例的脚本
│   └── .generateErrorHandlingTestFiles.sh  # 生成错误处理测试用例的脚本
├── CMakeLists.txt                 # 构建配置
├── LICENSE                        # Apache License 2.0
├── README.md                      # 英文文档
└── README.zh.md                   # 中文文档（当前文件）
```


## 架构

### 核心组件

1. **json 类（`jsonpp.hpp`）**：主 JSON 值容器
   - 使用 `std::variant` 存储不同的 JSON 类型
   - 提供类型安全的访问器和类型检查方法
   - 支持运算符重载以实现自然语法

2. **解析器（`parser.hpp`）**：基于模板的解析引擎
   - `Parser<StreamT>`：接受任何流类型的主解析器模板
   - `JSONStringParser<StreamT>`：支持 Unicode 的专用字符串解析器
   - `ParserBase<StreamT>`：提供流接口抽象的基类

3. **序列化器（`serializer.hpp`）**：JSON 到字符串的转换
   - 实现 `operator<<` 用于流输出
   - 处理正确的字符串转义和 Unicode 编码
   - 支持所有 JSON 数据类型

4. **流系统**：灵活的输入处理
   - **特征（`stream_traits.hpp`）**：基于 SFINAE 的类型特征，用于检测流能力
   - **适配器（`json_stream_adaptor.hpp`）**：内置流实现
     - `StringViewStream`：连续、零拷贝访问
     - `IStreamStream`：顺序流访问

### 设计原则

- **基于模板的流**：解析器按流类型模板化，允许编译时优化
- **基于特征的调度**：使用 SFINAE 根据流能力选择最优解析策略
- **异常安全**：对解析错误提供强异常保证
- **以头文件为主的设计**：易于集成，编译开销最小

## 性能考虑

- **类型安全存储**: 使用 `std::variant` 实现零开销的类型安全值存储
- **零拷贝解析**: `StringViewStream` 提供从 `std::string_view` 的零拷贝解析
- **优化的字符串解析**: 
  - 连续流使用基于块的解析，通过 `read_chunk_until()` 快速提取字符串
  - 顺序流回退到逐字符解析
- **移动语义**: 整个 API 中高效的值传递
- **快速对象访问**: 使用 `std::unordered_map` 实现 O(1) 平均情况下的对象键查找
- **Unicode 处理**: 高效的 UTF-8 编码/解码，正确支持代理对
- **流适配器**: 基于模板的流架构允许编译器根据流能力进行优化

## 限制

- 数字存储为 `std::int64_t` 或 `double`
- 对象键必须是字符串（符合 JSON 规范）
- 支持字符串中的 Unicode 转义序列
- 通过标准容器自动管理内存

## 许可证

本项目采用 Apache License 2.0 许可证 - 详见 [LICENSE](LICENSE) 文件。

## 贡献

欢迎贡献！请随时提交问题和拉取请求。

## 示例

### 使用数组

```cpp
using namespace JSONpp;

json arr = array{1, 2, 3, 4, 5};

// 遍历数组
if (arr.is_array()) {
    for (const auto& elem : arr.as_array()) {
        std::cout << elem.as_int() << " ";
    }
}

// 修改数组
arr.as_array().push_back(json(6));
arr.as_array()[0] = 10;
```

### 使用对象

```cpp
using namespace JSONpp;

json obj = object{
    {"name", "小明"},
    {"age", 35},
    {"city", "上海"}
};

// 遍历对象
if (obj.is_object()) {
    for (const auto& [key, value] : obj.as_object()) {
        std::cout << key << ": " << value.stringify() << std::endl;
    }
}

// 添加/修改字段
obj.as_object()["email"] = "xiaoming@example.com";
obj.as_object()["age"] = 36;
```

### 复杂的嵌套结构

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

## 常见问题

**问：这个库是线程安全的吗？**  
答：单个 `json` 对象不是线程安全的。如果在线程之间共享对象，请使用外部同步。

**问：我可以从文件解析 JSON 吗？**  
答：可以！该库支持直接流式解析：
```cpp
// 方法 1：直接流式解析（推荐）
std::ifstream file("data.json");
json j = JSONpp::parse(file);

// 方法 2：先读入字符串（利用 string_view 优势）
std::ifstream file("data.json");
std::string content((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
json j = JSONpp::parse(content);  // 使用零拷贝的 StringViewStream
```

**问：字符串解析和流式解析的性能差异是什么？**  
答：`StringViewStream`（与 `std::string_view` 配合使用）由于零拷贝和连续内存访问提供最佳性能。`IStreamStream` 适合需要流式输入（网络、大文件）的情况，但由于逐字符读取速度略慢。

**问：如何处理大数字？**  
答：该库对整数使用 `std::int64_t`（范围：-2^63 到 2^63-1），对浮点数使用 `double`。对于任意精度，您可能需要将数字存储为字符串。

**问：支持 JSON 注释吗？**  
答：不支持，纯 JSON 不支持注释。如果需要注释支持，请使用 JSONC 或 JSON5 解析器。

**问：如何处理 Unicode？**  
答：该库完全支持 UTF-8 编码，并正确处理 Unicode 转义序列（例如 `\uXXXX`），包括用于基本多文种平面之外字符的代理对。

## 致谢

使用现代 C++17 特性构建，提供简洁高效的 JSON 处理体验。

