#include <gtest/gtest.h>
#include "jsonpp.hpp"

using namespace JSONpp;

// 这是一个针对复杂真实数据的"抽查"测试 (Spot Check)
// 目的：验证解析后的 C++ 对象结构是否严格符合预期，作为"基准真理" (Ground Truth)
// 数据来源：tests/test_cases_roundtrip/roundtrip032.json

TEST(SpotCheckTest, ComplexRealWorld_UserProfile) {
    const char* json_content = R"(
    [
      {
        "_id": "5b5d3b7c5f4d2c1a3b6e8f9a",
        "index": 0,
        "guid": "1a2b3c4d-5e6f-7g8h-9i0j-1k2l3m4n5o6p",
        "isActive": true,
        "balance": "$3,456.78",
        "picture": "http://placehold.it/32x32",
        "age": 32,
        "eyeColor": "brown",
        "name": "John Doe",
        "gender": "male",
        "company": "ACME",
        "email": "john.doe@acme.com",
        "phone": "+1 (555) 123-4567",
        "fax": null,
        "address": "123 Main St, Anytown, USA 12345",
        "about": "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
        "registered": "2018-07-29T12:34:56 -08:00",
        "latitude": 37.7749,
        "longitude": -122.4194,
        "tags": [
          "tag1",
          "tag2",
          "tag3"
        ],
        "friends": [
          {
            "id": 0,
            "name": "Jane Smith"
          },
          {
            "id": 1,
            "name": "Bob Johnson"
          },
          {
            "id": 2,
            "name": "Alice Williams"
          }
        ],
        "greeting": "Hello, John Doe! You have 5 unread messages.",
        "favoriteFruit": "apple"
      }
    ]
    )";

    // 2. 执行解析
    json root = json::parse(json_content);

    // 3. 验证根结构 (Root Structure)
    ASSERT_TRUE(root.is_array()) << "Root should be an array";
    ASSERT_EQ(root.as_array().size(), 1) << "Root array should contain exactly 1 element";

    // 4. 获取用户对象 (User Object)
    const auto& user = root[0];
    ASSERT_TRUE(user.is_object()) << "First element should be an object";

    // 5. 验证基础字段 (Basic Fields)
    // Null
    EXPECT_TRUE(user["fax"].is_null()) << "fax should be null";
    EXPECT_EQ(user["fax"], null) << "fax should be json::null";

    // String
    EXPECT_EQ(user["_id"].as_string(), "5b5d3b7c5f4d2c1a3b6e8f9a");
    EXPECT_EQ(user["name"].as_string(), "John Doe");
    EXPECT_EQ(user["company"].as_string(), "ACME");
    
    // Integer
    EXPECT_EQ(user["age"].as_int(), 32);
    EXPECT_EQ(user["index"].as_int(), 0);

    // Boolean
    EXPECT_TRUE(user["isActive"].is_bool());
    EXPECT_TRUE(user["isActive"].as_bool()) << "isActive should be true";

    // Float / Double
    // 注意：使用 DOUBLE_EQ 来处理浮点数精度
    EXPECT_DOUBLE_EQ(user["latitude"].as_float(), 37.7749);
    EXPECT_DOUBLE_EQ(user["longitude"].as_float(), -122.4194);

    // 6. 验证嵌套数组 (Nested Arrays - Strings)
    const auto& tags = user["tags"];
    ASSERT_TRUE(tags.is_array());
    ASSERT_EQ(tags.as_array().size(), 3);
    
    EXPECT_EQ(tags[0].as_string(), "tag1");
    EXPECT_EQ(tags[1].as_string(), "tag2");
    EXPECT_EQ(tags[2].as_string(), "tag3");

    // 7. 验证嵌套对象数组 (Nested Array of Objects)
    const auto& friends = user["friends"];
    ASSERT_TRUE(friends.is_array());
    ASSERT_EQ(friends.as_array().size(), 3);

    // 抽查朋友列表中的第二个人 (Index 1)
    const auto& bob = friends[1];
    ASSERT_TRUE(bob.is_object());
    EXPECT_EQ(bob["id"].as_int(), 1);
    EXPECT_EQ(bob["name"].as_string(), "Bob Johnson");

    // 8. 验证不存在的 Key (安全性检查)
    if (user.is_object()) {
        auto& obj = user.as_object();
        EXPECT_EQ(obj.find("non_existent_key"), obj.end());
    }
}