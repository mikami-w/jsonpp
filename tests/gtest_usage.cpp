#include <gtest/gtest.h>

#include "jsonpp.hpp"
using namespace JSONpp;

// Basic assignment and type switching
TEST(JsonUsageTest, AssignmentAndTypeSwitching) {
    // 1. 初始化为 empty
    json j;
    EXPECT_TRUE(j.empty());

    // 2. 赋值为整数
    j = 42;
    EXPECT_TRUE(j.is_int());
    EXPECT_EQ(j.as_int(), 42);

    // 3. 赋值为浮点数 (类型切换)
    j = 3.14159;
    EXPECT_TRUE(j.is_float());
    EXPECT_DOUBLE_EQ(j.as_float(), 3.14159);

    // 4. 赋值为字符串
    j = "Hello World";
    EXPECT_TRUE(j.is_string());
    EXPECT_EQ(j.as_string(), "Hello World");

    // 5. 赋值为布尔
    j = true;
    EXPECT_TRUE(j.is_bool());
    EXPECT_TRUE(j.as_bool());

    // 6. 赋值回 null
    j = nullptr;
    EXPECT_TRUE(j.is_null());
}

// Object manipulation: inserting, modifying, and accessing keys
TEST(JsonUsageTest, ObjectManipulation) {
    json j_obj; // 默认为 empty

    // 1. 自动转换为 Object 并插入 Key
    j_obj["name"] = "Mikami";
    j_obj["age"] = 25;
    j_obj["is_student"] = true;

    EXPECT_TRUE(j_obj.is_object());
    EXPECT_EQ(j_obj["name"].as_string(), "Mikami");
    EXPECT_EQ(j_obj["age"].as_int(), 25);

    // 2. 修改现有 Key
    j_obj["age"] = 26;
    EXPECT_EQ(j_obj["age"].as_int(), 26);

    // 3. 嵌套对象赋值
    j_obj["address"]["city"] = "Tokyo";
    j_obj["address"]["zip"] = 1000001;

    EXPECT_TRUE(j_obj["address"].is_object());
    EXPECT_EQ(j_obj["address"]["city"].as_string(), "Tokyo");

    // 4. const 访问 (如果你保留了 const 重载)
    const json j_const = j_obj;
    // 注意: 这里取决于你是否实现了 const operator[] 或者需要用 .at()
    // 假设你使用了 .at() 策略:
    EXPECT_EQ(j_const.as_object().at("name").as_string(), "Mikami");
}

// Array manipulation: constructing, modifying, and iterating
TEST(JsonUsageTest, ArrayManipulation) {
    // 1. 通过 vector 构造
    std::vector<json> vec = {1, 2, 3};
    json j_arr = vec;

    EXPECT_TRUE(j_arr.is_array());
    EXPECT_EQ(j_arr.as_array().size(), 3);
    EXPECT_EQ(j_arr[0].as_int(), 1);

    // 2. 修改元素
    j_arr[1] = 42;
    EXPECT_EQ(j_arr[1].as_int(), 42);

    // 3. 使用底层容器方法 (push_back)
    // 因为 basic_json 没有直接暴露 push_back，我们需要通过 as_array()
    j_arr.as_array().push_back("new element");
    EXPECT_EQ(j_arr.as_array().size(), 4);
    EXPECT_EQ(j_arr[3].as_string(), "new element");

    // 4. 遍历测试 (Range-based for)
    int sum = 0;
    // 重新构造一个纯整数组
    json num_arr = std::vector<json>{10, 20, 30};
    for (auto& item : num_arr.as_array()) {
        sum += item.as_int();
    }
    EXPECT_EQ(sum, 60);
}

// Copy and Move semantics
TEST(JsonUsageTest, CopyAndMove) {
    // --- 深拷贝测试 ---
    json j1;
    j1["key"] = "original";

    json j2 = j1; // 拷贝构造
    j2["key"] = "modified";

    // j1 应该保持不变
    EXPECT_EQ(j1["key"].as_string(), "original");
    EXPECT_EQ(j2["key"].as_string(), "modified");

    // --- 移动语义测试 ---
    json j3 = std::vector<json>{1, 2, 3};
    json j4 = std::move(j3); // 移动构造

    // j4 应该拥有数据
    EXPECT_TRUE(j4.is_array());
    EXPECT_EQ(j4.as_array().size(), 3);

    // j3 应该处于有效但未定义状态 (通常是 null 或空)
    // 对于 variant 实现，通常移动后源对象仍然持有某种状态，但这里我们只关心 j4 拿到了数据
}

// Equality comparison tests
TEST(JsonUsageTest, EqualityComparison) {
    json j1 = 10;
    json j2 = 10;
    json j3 = 20;

    EXPECT_TRUE(j1 == j2);
    EXPECT_FALSE(j1 == j3);
    EXPECT_TRUE(j1 != j3);

    // 复杂结构比较
    json o1;
    o1["a"] = 1;
    o1["b"] = 2;

    json o2;
    o2["b"] = 2;
    o2["a"] = 1; // 顺序不同，但在 JSON 对象逻辑中应视为相等 (取决于底层 map 实现)

    EXPECT_TRUE(o1 == o2);

    json arr1 = std::vector<json>{1, 2};
    json arr2 = std::vector<json>{1, 2};
    EXPECT_TRUE(arr1 == arr2);
}

// Exception safety tests
TEST(JsonUsageTest, ExceptionSafety) {
    json j = "I am a string";

    // 1. 错误的类型转换 (as_xxx)
    EXPECT_THROW({
        int val = j.as_int();
        (void)val;
    }, JsonTypeError);

    // 2. 错误的下标访问 (对非对象/非数组使用下标)
    // 注意：如果是非 const 引用，operator[] 可能会抛出异常或断言失败，取决于你的实现
    // 如果你的实现是 as_object()[key]，那么对 string 调用 as_object() 应该抛出类型错误
    EXPECT_THROW({
        j["key"] = 1;
    }, JsonTypeError);

    // 3. const 对象访问不存在的 key (如果你实现了 .at() 或抛异常的 const operator[])
    const json obj_const = json::object{{"a", 1}};
    EXPECT_THROW({
        auto val = obj_const["b"]; // 假设这会抛异常
        (void)val;
    }, JsonTypeError); // 或具体的错误类型
}

// 1. 测试编译期模板版本 set_type<T>()
TEST(JsonSetTypeTest, CompileTimeTypeSwitching) {
    json j; // 默认为 empty

    // empty -> Array
    j.set_type<json::Type::array>();
    EXPECT_TRUE(j.is_array());
    EXPECT_TRUE(j.as_array().empty()); // 新构造的 array 应为空

    // Array -> Object
    j.set_type<json::Type::object>();
    EXPECT_TRUE(j.is_object());
    EXPECT_TRUE(j.as_object().empty());

    // Object -> String
    j.set_type<json::Type::string>();
    EXPECT_TRUE(j.is_string());
    EXPECT_EQ(j.as_string(), ""); // string 默认构造为空

    // String -> Boolean
    j.set_type<json::Type::boolean>();
    EXPECT_TRUE(j.is_bool());
    EXPECT_FALSE(j.as_bool()); // boolean 默认构造为 false

    // Boolean -> Number Int
    j.set_type<json::Type::number_int>();
    EXPECT_TRUE(j.is_int());
    EXPECT_EQ(j.as_int(), 0); // number 默认构造为 0
}

// 2. 测试运行期参数版本 set_type(type)
TEST(JsonSetTypeTest, RuntimeTypeSwitching) {
    json j = "initial string";

    // String -> Number Float
    j.set_type(json::Type::number_float);
    EXPECT_TRUE(j.is_float());
    EXPECT_DOUBLE_EQ(j.as_float(), 0.0);

    // Number -> Null
    j.set_type(json::Type::null);
    EXPECT_TRUE(j.is_null());
}

// 3. 核心逻辑测试：同类型下的 clear_content = false (应保留数据)
TEST(JsonSetTypeTest, ContentPreservation) {
    // 准备数据
    json j_arr = json::array({1, 2, 3});
    json j_str = "hello world";
    json j_int = 12345;

    // 尝试设置为相同类型，且不清除内容
    j_arr.set_type<json::Type::array>(false);
    j_str.set_type<json::Type::string>(false);
    j_int.set_type(json::Type::number_int, false); // 测试运行期版本接口

    // 验证数据未丢失
    EXPECT_EQ(j_arr.as_array().size(), 3);
    EXPECT_EQ(j_arr[0].as_int(), 1);

    EXPECT_EQ(j_str.as_string(), "hello world");

    EXPECT_EQ(j_int.as_int(), 12345);
}

// 4. 核心逻辑测试：同类型下的 clear_content = true (应重置数据)
TEST(JsonSetTypeTest, ContentClearing) {
    // 准备数据
    json j_arr = json::array({1, 2, 3});
    json j_str = "hello world";
    json j_int = 12345;

    // 尝试设置为相同类型，但强制清除内容
    j_arr.set_type<json::Type::array>(true);
    j_str.set_type<json::Type::string>(true);
    j_int.set_type(json::Type::number_int, true);

    // 验证数据已重置为默认值
    EXPECT_TRUE(j_arr.is_array());
    EXPECT_TRUE(j_arr.as_array().empty());

    EXPECT_TRUE(j_str.is_string());
    EXPECT_EQ(j_str.as_string(), "");

    EXPECT_TRUE(j_int.is_int());
    EXPECT_EQ(j_int.as_int(), 0);
}

// 5. 核心逻辑测试：不同类型切换 (clear_content 参数应被忽略，总是重置)
// 逻辑依据：当类型改变时，底层 variant 必须重新构造，旧数据必然丢失
TEST(JsonSetTypeTest, TypeChangeAlwaysResets) {
    json j = json::array({1, 2, 3});

    // 切换为 Object，即使传入 false，因为类型不同，也应该变成空的 Object
    j.set_type<json::Type::object>(false);

    EXPECT_TRUE(j.is_object());
    EXPECT_TRUE(j.as_object().empty()); // 此时绝不应该保留 Array 的数据
}