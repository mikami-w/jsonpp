//
// Created on 2025/10/30.
//

#include <algorithm>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include "../src/jsonpp.hpp"

namespace fs = std::filesystem;

namespace Test
{
    static constexpr char const* testFilePath_usability = "../tests/usability/";
    static constexpr char const* testFilePath_errorHandling = "../tests/error_handling/";

    struct TestDirectoryEntries
    {
        std::vector<fs::directory_entry>& usability;
        std::vector<fs::directory_entry>& error_handling;
    };

    inline bool is_hidden(fs::directory_entry const& file)
    {
        return file.path().filename().string().front() == '.';
    }

    TestDirectoryEntries getSortedDirectoryEntries()
    {
        static std::vector<fs::directory_entry> usa;
        static std::vector<fs::directory_entry> err;
        static bool initialized = false;

        if (!initialized)
        {
            for (auto& file : fs::directory_iterator(testFilePath_usability))
            {
                usa.push_back(file);
            }
            std::sort(usa.begin(), usa.end());

            for (auto& file : fs::directory_iterator(testFilePath_errorHandling))
            {
                err.push_back(file);
            }
            std::sort(err.begin(), err.end());

            initialized = true;
        }

        return {usa, err};
    }

    std::string readFileToString(std::string const& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
            return "";
        }

        // 一次性构造字符串
        //    - std::istreambuf_iterator<char>(file) : 指向文件流开始的迭代器
        //    - std::istreambuf_iterator<char>()     : 默认构造函数，表示 "end-of-stream"
        std::string content(
            (std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>())
        );

        file.close();

        return content;
    }

    void test_usability()
    {
        std::cout << "========== USABILITY TEST ==========" << std::endl;
        for (auto const& file: getSortedDirectoryEntries().usability)
        {
            std::cout << "-------- " << file.path().filename() << " --------" << std::endl;
            try
            {
                std::string file_content = readFileToString(file.path().string());
                auto j = JSONpp::parse(file_content);

                std::cout << "File contents:\t" << file_content;
                std::cout << "Parsed JSON:\t" << j << std::endl;
            } catch (std::exception& e)
            {
                std::cout << "Error occurred in file " << file.path().filename() << std::endl;
                std::cout << e.what() << std::endl;
            }
            std::cout << "END----- " << file.path().filename() << " --------" << std::endl;
        }
        std::cout << "========== USABILITY TEST END ==========" << std::endl;
    }

    void test_error_handling()
    {
        std::cout << "========== ERROR HANDLING TEST ==========" << std::endl;
        int errors = 0;
        for (auto const& file: getSortedDirectoryEntries().error_handling)
        {
            std::cout << "-------- " << file.path().filename() << " --------" << std::endl;
            try
            {
                std::string file_content = readFileToString(file.path().string());
                auto j = JSONpp::parse(file_content);
            } catch (std::exception& e)
            {
                std::cout << "Error occurred in file " << file.path().filename();
                std::cout << ": " << e.what() << std::endl;
                ++errors;
            }
            std::cout << "END----- " << file.path().filename() << " --------" << std::endl;
        }
        std::cout << std::endl << errors << " errors was found." << std::endl;
        std::cout << "========== ERROR HANDLING TEST END ==========" << std::endl;
    }

    void temp_test()
    {
        using namespace JSONpp;
        // std::cout << "😀你好" << std::endl;
        json j = parse(R"("\uD83D\uDE00")");
        std::cout << j.as_string() << std::endl;
    }
}

int main()
{
    //std::cout << "JSONpp::isContiguousStream_v<JSONpp::StringViewStream>: " << JSONpp::traits::isContiguousStream_v<JSONpp::StringViewStream> << std::endl;

    Test::temp_test();
    std::cout << std::endl;

    // std::cout << "========== Running tests... ==========" << std::endl;
    // Test::test_usability();
    // std::cout << std::endl;
    // Test::test_error_handling();
    // std::cout << std::endl;

    return 0;
}

