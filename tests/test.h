//
// Created on 2025/10/30.
//

#ifndef JSONPP_TEST_H
#define JSONPP_TEST_H
#include <algorithm>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include "../jsonpp.h"

namespace fs = std::filesystem;

namespace Test
{
    static constexpr char const* testFilePath_usability = "d:/works/cpp/JSONpp/tests/usability/";
    static constexpr char const* testFilePath_errorHandling = "d:/works/cpp/JSONpp/tests/error_handling/";

    inline bool is_hidden(fs::directory_entry const& file)
    {
        return file.path().filename().string().front() == '.';
    }

    inline std::string readFileToString(std::string const& filename) {
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

    inline void test_usability()
    {
        std::cout << "========== USABILITY TEST ==========" << std::endl;
        for (auto const& file
            : fs::directory_iterator(testFilePath_usability))
        {
            try
            {
                auto j = JSONpp::parse(readFileToString(file.path().string()));
                if (j.empty())
                {
                    std::cout << "Error occurred in file " << file.path().filename() << std::endl;
                    std::cout << "Empty Document." << std::endl;
                }
                else
                {
                    std::cout << "File contents:\t" << readFileToString(file.path().string());
                    std::cout << "Parsed JSON:\t" << j << std::endl;
                }

            } catch (std::exception& e)
            {
                std::cout << "Error occurred in file " << file.path().filename() << std::endl;
                std::cout << e.what() << std::endl;
            }
        }
        std::cout << "========== USABILITY TEST END ==========" << std::endl;
    }

    inline void test_error_handling()
    {
        std::cout << "========== ERROR HANDLING TEST ==========" << std::endl;
        int errors = 0;
        for (auto const& file
            : fs::directory_iterator(testFilePath_errorHandling))
        {
            try
            {
                auto j = JSONpp::parse(readFileToString(file.path().string()));
                if (j.empty())
                {
                    std::cout << "Error occurred in file " << file.path().filename() << std::endl;
                    std::cout << "Empty Document." << std::endl;
                    ++errors;
                }

            } catch (std::exception& e)
            {
                std::cout << "Error occurred in file " << file.path().filename();
                std::cout << ": " << e.what() << std::endl;
                ++errors;
            }
        }
        std::cout << std::endl << errors << " errors was found." << std::endl;
        std::cout << "========== ERROR HANDLING TEST END ==========" << std::endl;
    }
};


#endif //JSONPP_TEST_H
