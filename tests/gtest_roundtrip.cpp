#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "jsonpp.hpp"
#include "test_utils.hpp"

using namespace JSONpp;

// 测试文件目录常量
static constexpr char const* kTestFileDir = "../tests/test_cases_roundtrip/";

class RoundTripTest : public ::testing::TestWithParam<std::string> {
protected:
    std::string GetFilePath() const {
        return std::string(kTestFileDir) + GetParam();
    }
};

TEST_P(RoundTripTest, FullIOCombinationCoverage) {
    const std::string filename = GetFilePath();
    std::string original_content;

    // -------------------------------------------------------
    // Phase 0: 读取文件
    // -------------------------------------------------------
    try {
        original_content = readFileToString(filename);
    } catch (const std::exception& e) {
        FAIL() << "Setup failed: " << e.what();
    }

    // -------------------------------------------------------
    // Phase 1: 测试两种解析接口 (Parsing)
    // -------------------------------------------------------

    // 1.1 基于 string_view 的解析 (High Performance Path)
    json j_from_sv;
    try {
        // 使用 static basic_json parse(std::string_view)
        j_from_sv = json::parse(original_content);
    } catch (const std::exception& e) {
        FAIL() << "parse(string_view) failed for " << filename << ": " << e.what();
    }

    // 1.2 基于 istream 的解析 (Stream Path)
    json j_from_stream;
    try {
        std::stringstream ss(original_content);
        // 使用 static basic_json parse(std::istream&)
        j_from_stream = json::parse(ss);
    } catch (const std::exception& e) {
        FAIL() << "parse(istream) failed for " << filename << ": " << e.what();
    }

    // 1.3 [关键] 验证一致性：两种解析方式必须产生相等的对象
    ASSERT_EQ(j_from_sv, j_from_stream)
        << "Mismatch! parse(string_view) and parse(istream) produced different objects for " << filename;


    // -------------------------------------------------------
    // Phase 2: 测试两种序列化接口 (Dumping)
    // -------------------------------------------------------
    // 我们使用 j_from_sv 作为基准对象进行测试

    // 2.1 Dump 到 std::string (Memory Buffer Path)
    std::string dump_result_str;
    try {
        // 使用 void dump(std::string&) const
        j_from_sv.dump(dump_result_str);
    } catch (const std::exception& e) {
        FAIL() << "dump(std::string&) failed: " << e.what();
    }

    // 2.2 Dump 到 std::ostream (Stream Path)
    std::stringstream ss_out;
    try {
        // 使用 void dump(std::ostream&) const
        j_from_sv.dump(ss_out);
    } catch (const std::exception& e) {
        FAIL() << "dump(std::ostream&) failed: " << e.what();
    }
    std::string dump_result_stream = ss_out.str();

    // 2.3 [可选] 验证输出一致性 (通常应该完全一致，除非 map 顺序不确定)
    // 如果使用的是 std::map，输出应该是确定性的
    EXPECT_EQ(dump_result_str, dump_result_stream)
        << "dump(string) and dump(ostream) produced different outputs string.";


    // -------------------------------------------------------
    // Phase 3: 往返验证 (Round-Trip Verification)
    // -------------------------------------------------------
    // 验证序列化后的字符串是否有效，且能否还原为原始对象

    // 3.1 验证 dump(string) 的结果
    try {
        json j_roundtrip = json::parse(dump_result_str);
        EXPECT_EQ(j_from_sv, j_roundtrip)
            << "Round-trip failed for dump(std::string&). Data corrupted after serialization.";
    } catch (const std::exception& e) {
        FAIL() << "Failed to re-parse output from dump(std::string&): " << e.what();
    }

    // 3.2 验证 dump(ostream) 的结果
    try {
        json j_roundtrip = json::parse(dump_result_stream);
        EXPECT_EQ(j_from_sv, j_roundtrip)
            << "Round-trip failed for dump(std::ostream&). Data corrupted after serialization.";
    } catch (const std::exception& e) {
        FAIL() << "Failed to re-parse output from dump(std::ostream&): " << e.what();
    }
}

// -------------------------------------------------------
// 测试参数实例化
// -------------------------------------------------------
const std::vector<std::string> roundtrip_files = {
    // --- A. 简单类型 (001-007) ---
    "roundtrip001.json", "roundtrip002.json", "roundtrip003.json", "roundtrip004.json",
    "roundtrip005.json", "roundtrip006.json", "roundtrip007.json",
    // --- B. 数组 (008-012) ---
    "roundtrip008.json", "roundtrip009.json", "roundtrip010.json", "roundtrip011.json",
    "roundtrip012.json",
    // --- C. 对象 (013-017) ---
    "roundtrip013.json", "roundtrip014.json", "roundtrip015.json", "roundtrip016.json",
    "roundtrip017.json",
    // --- D. 嵌套结构 (018-022) ---
    "roundtrip018.json", "roundtrip019.json", "roundtrip020.json", "roundtrip021.json",
    "roundtrip022.json",
    // --- E. 特殊字符串 (023-028) ---
    "roundtrip023.json", "roundtrip024.json", "roundtrip025.json", "roundtrip026.json",
    "roundtrip027.json", "roundtrip028.json",
    // --- F. 复杂数据 (029-033) ---
    "roundtrip029.json", "roundtrip030.json", "roundtrip031.json", "roundtrip032.json",
    "roundtrip033.json",
    // --- G. 数字边缘情况 (034-037) ---
    "roundtrip034.json", // 0, -0, 0.0
    "roundtrip035.json", // 科学计数法
    "roundtrip036.json", // int64 边界
    "roundtrip037.json", // double 边界
    // --- H. 高级 Unicode (038-039) ---
    "roundtrip038.json", // 代理对 Emoji
    "roundtrip039.json", // 原始 UTF-8
    // --- I. 结构边缘情况 (040-042) ---
    "roundtrip040.json", // 空 Key
    "roundtrip041.json", // 深度嵌套
    "roundtrip042.json", // 大量空白
    // --- J. 特殊转义增强 (043) ---
    "roundtrip043.json"  // 正斜杠
};

INSTANTIATE_TEST_SUITE_P(
    GeneratedRoundTrips,
    RoundTripTest,
    ::testing::ValuesIn(roundtrip_files),
    [](const testing::TestParamInfo<RoundTripTest::ParamType>& info) {
        std::string name = info.param;
        // 移除非法字符，让 GTest 名字更干净
        std::replace(name.begin(), name.end(), '.', '_');
        return name;
    }
);