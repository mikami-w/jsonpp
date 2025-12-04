#include <gtest/gtest.h>
#include "jsonpp.hpp"

using namespace JSONpp;

// 1. 基础字面量与数值测试 (Primitives)
TEST(CorrectnessTest, Primitives) {
    // Null
    auto j_null = json::parse("null");
    EXPECT_TRUE(j_null.is_null());
    EXPECT_EQ(j_null, json(nullptr));

    // Boolean
    EXPECT_EQ(json::parse("true"), json(true));
    EXPECT_EQ(json::parse("false"), json(false));

    // Integer (常规)
    auto j_int = json::parse("123");
    EXPECT_TRUE(j_int.is_int());
    EXPECT_EQ(j_int.as_int(), 123);
    
    // Integer (负数边界)
    auto j_neg = json::parse("-9223372036854775807"); 
    EXPECT_EQ(j_neg.as_int(), -9223372036854775807LL);

    // Float (定点)
    auto j_float = json::parse("3.14159");
    EXPECT_TRUE(j_float.is_float());
    EXPECT_DOUBLE_EQ(j_float.as_float(), 3.14159);

    // Float (科学计数法)
    auto j_sci = json::parse("1.5e3"); // 1500.0
    EXPECT_DOUBLE_EQ(j_sci.as_float(), 1500.0);
    
    auto j_sci_neg = json::parse("2e-2"); // 0.02
    EXPECT_DOUBLE_EQ(j_sci_neg.as_float(), 0.02);
}

// 2. 字符串与复杂转义测试 (Strings & Escapes)
TEST(CorrectnessTest, Strings) {
    // 普通字符串
    EXPECT_EQ(json::parse(R"("hello world")").as_string(), "hello world");
    
    // 转义字符检查 (换行、引号、斜杠)
    // 输入 JSON: "line\nbreak\t\r\\\""
    auto j_esc = json::parse(R"("line\nbreak\t\r\\\"")");
    EXPECT_EQ(j_esc.as_string(), "line\nbreak\t\r\\\"");

    // Unicode (BMP 平面): \u4F60\u597D -> "你好"
    auto j_uni = json::parse(R"("\u4F60\u597D")");
    EXPECT_EQ(j_uni.as_string(), "你好");

    // Unicode (代理对 Surrogate Pair): \uD83D\uDE00 -> "😀"
    // 这是验证你 parse_u_escape 逻辑是否正确的终极测试
    auto j_emoji = json::parse(R"("\uD83D\uDE00")");
    EXPECT_EQ(j_emoji.as_string(), "😀");
}

// 3. 结构化数据测试 (Arrays & Objects)
TEST(CorrectnessTest, Structures) {
    // 数组
    auto j_arr = json::parse("[1, 2, 3]");
    EXPECT_TRUE(j_arr.is_array());
    ASSERT_EQ(j_arr.as_array().size(), 3);
    EXPECT_EQ(j_arr[0].as_int(), 1);
    EXPECT_EQ(j_arr[2].as_int(), 3);

    // 空数组
    EXPECT_TRUE(json::parse("[]").as_array().empty());

    // 对象
    auto j_obj = json::parse(R"({"key": "value", "id": 42})");
    EXPECT_TRUE(j_obj.is_object());
    EXPECT_EQ(j_obj["key"].as_string(), "value");
    EXPECT_EQ(j_obj["id"].as_int(), 42);

    // 空对象
    EXPECT_TRUE(json::parse("{}").as_object().empty());
}

// 4. 空白字符处理 (Whitespace Tolerance)
TEST(CorrectnessTest, WhitespaceHandling) {
    // 验证你的 skip_whitespace 是否在各个角落都工作正常
    const char* json_str = R"(
        {
            "a" :  1 ,
            "b": [ 
                2, 
                3 
            ] 
        }
    )";
    auto j = json::parse(json_str);
    EXPECT_EQ(j["a"].as_int(), 1);
    EXPECT_EQ(j["b"][0].as_int(), 2);
    EXPECT_EQ(j["b"][1].as_int(), 3);
}

// 5. 混合嵌套结构 (Complex Nested)
TEST(CorrectnessTest, ComplexNested) {
    // 测试对象套数组，数组套对象
    auto j = json::parse(R"({"meta":{"count":2},"data":[{"id":1},{"id":2}]})");
    
    ASSERT_TRUE(j["meta"].is_object());
    EXPECT_EQ(j["meta"]["count"].as_int(), 2);
    
    ASSERT_TRUE(j["data"].is_array());
    EXPECT_EQ(j["data"][0]["id"].as_int(), 1);
    EXPECT_EQ(j["data"][1]["id"].as_int(), 2);
}