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
- **流支持**: 通过自定义流适配器实现灵活的 I/O
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
- `src/jsonpp.hpp`
- `src/jsonpp.cpp`
- `src/jsonexception.hpp`
- `src/json_stream_adaptor.hpp`
- `src/stream_traits.hpp`

## 快速开始

### 解析 JSON

```cpp
#include "jsonpp.hpp"
#include <iostream>

int main() {
    // 解析 JSON 字符串
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

该库使用在 `jsonexception.hpp` 中定义的自定义异常：

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

## 流支持

该库包含用于灵活 I/O 的自定义流适配器：

```cpp
#include "json_stream_adaptor.hpp"
#include "stream_traits.hpp"

// 自定义流处理
// 有关实现自定义流的详细信息，请参见 json_stream_adaptor.hpp
```

## 测试

项目包含全面的测试套件：

- **可用性测试**: 位于 `tests/usability/`
  - 测试常见的使用模式
  - 通过 `.generateUsabilityTestFiles.sh` 生成
  
- **错误处理测试**: 位于 `tests/error_handling/`
  - 测试无效 JSON 和错误条件
  - 通过 `.generateErrorHandlingTestFiles.sh` 生成

运行测试：

```bash
cd cmake-build-debug
./JSONpp
```

## 项目结构

```
JSONpp/
├── src/
│   ├── jsonpp.hpp                 # 主头文件
│   ├── jsonpp.cpp               # 实现文件
│   ├── jsonexception.hpp          # 异常定义
│   ├── json_stream_adaptor.hpp    # 流适配器工具
│   └── stream_traits.hpp          # 流特征
├── tests/
│   ├── test.cpp               # 测试实现
│   ├── .generateUsabilityTestFiles.sh      # 可用性测试用例生成脚本
│   ├── .generateErrorHandlingTestFiles.sh  # 错误处理测试用例生成脚本
│   ├── usability/             # 可用性测试用例
│   └── error_handling/        # 错误处理测试用例
├── CMakeLists.txt           # 构建配置
├── LICENSE                  # Apache License 2.0
├── README.md                # 英文版说明文件
└── README.zh.md             # 当前文件
```

## 性能考虑

- 使用 `std::variant` 实现类型安全的值存储
- 在可能的情况下使用零拷贝的字符串视图进行解析
- 利用移动语义实现高效的值传递
- 使用 `std::unordered_map` 实现快速的对象键查找

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
答：可以，先将文件读入字符串，然后使用 `JSONpp::parse()`：
```cpp
std::ifstream file("data.json");
std::string content((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
json j = JSONpp::parse(content);
```

**问：如何处理大数字？**  
答：该库对整数使用 `std::int64_t`，对浮点数使用 `double`。对于任意精度，您可能需要将数字存储为字符串。

**问：支持 JSON 注释吗？**  
答：不支持，纯 JSON 不支持注释。如果需要注释支持，请使用 JSONC 或 JSON5 解析器。

## 致谢

使用现代 C++17 特性构建，提供简洁高效的 JSON 处理体验。

