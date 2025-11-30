#include <algorithm>
#include <filesystem>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <map>
#include <chrono>

#include "../src/jsonpp.hpp"

namespace fs = std::filesystem;

namespace Test
{
    unsigned result[6]{}; // Passed, IThrew, SThrew, Inequal, SFailed, IFailed

    static constexpr char const* testFilePath = "../tests/";

    using TestDirectoryEntries = std::unordered_map<std::string, std::vector<fs::directory_entry>>;

    enum TestStatus: std::uint8_t
    {
        Passed = 0,
        IThrew,
        SThrew,
        Inequal,
        SFailed,
        IFailed
    };

    std::string getDateTimeString()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&now_c), "%Y%m%d_%H%M%S");
        return oss.str();
    }

    inline bool is_hidden(fs::directory_entry const& file)
    {
        return file.path().filename().string().front() == '.';
    }

    TestDirectoryEntries& getSortedDirectoryEntries()
    {
        static TestDirectoryEntries entries;
        static bool initialized = false;

        if (!initialized)
        {
            for (auto& dir : fs::directory_iterator(testFilePath))
            {
                if (is_hidden(dir)) continue;
                if (!dir.is_directory()) continue;

                for (auto& file: fs::directory_iterator(dir))
                {
                    entries[dir.path().filename()].push_back(file);
                }
            }
            for (auto& [key, vec] : entries)
            {
                std::sort(vec.begin(), vec.end());
            }

            // std::cout << "====" << entries.size() << "====" << std::endl;
            initialized = true;
        }

        return entries;
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

    class Testee
    {
        fs::directory_entry filedir;
    public:
        std::string getFileName() const { return filedir.path().filename().string(); }
        std::string getString() const { return readFileToString(filedir.path().string()); }
        std::unique_ptr<std::istream> getIStreamPtr() const { return std::make_unique<std::ifstream>(filedir.path()); }

        explicit Testee(fs::directory_entry dir) : filedir(std::move(dir))
        {
            if (!dir.is_regular_file())
                throw std::invalid_argument("Testee must be initialized with a directory entry representing a file.");
        }
    };

    void doTestFor(Testee const& t, std::ostream& logger = std::cout)
    {
        logger << "\nTestee: " << t.getFileName() << std::endl;
        logger << "Content:\n" << t.getString();

        bool passed = true;
        try
        {
            auto js = JSONpp::parse(t.getString());
            logger << "Parsed JSON from string:\t" << js << std::endl;
        } catch (std::runtime_error& e)
        {
            passed = false;
            ++result[SThrew];
            logger << "String parsing threw an exception for file " << t.getFileName() << std::endl;
            logger << e.what() << std::endl;
        } catch (std::exception& e)
        {
            passed = false;
            ++result[SFailed];
            logger << "Process was aborted while parsing string. Test failed for file " << t.getFileName() << std::endl;
            logger << e.what() << std::endl;
        }

        try
        {
            auto jis = JSONpp::parse(*t.getIStreamPtr());
            logger << "Parsed JSON from istream:\t" << jis << std::endl;

            auto js = JSONpp::parse(t.getString());
            if (js != jis)
            {
                passed = false;
                ++result[Inequal];
                logger << "Inequal JSONs parsed from string and istream in file " << t.getFileName() << std::endl;
            }
        } catch (std::runtime_error& e)
        {
            passed = false;
            ++result[IThrew];
            logger << "IStream parsing threw an exception for file " << t.getFileName() << std::endl;
            logger << e.what() << std::endl;
        } catch (std::exception& e)
        {
            passed = false;
            ++result[IFailed];
            logger << "Process was aborted while parsing istream. Test failed for file " << t.getFileName() << std::endl;
            logger << e.what() << std::endl;
        }

        if (passed)
        {
            logger << "Test passed for file " << t.getFileName() << std::endl;
            ++result[Passed];
        }
    }

    void testAll(std::ostream& logger = std::cout)
    {
        for (auto const& [dname, dirs] : getSortedDirectoryEntries())
        {
            for (auto const& file : dirs)
            {
                Testee t(file);
                doTestFor(t, logger);
            }
        }
        auto& dirs = getSortedDirectoryEntries();
        int total = 0;
        for (auto const& [dname, dirs] : dirs)
            total += dirs.size();

        logger << "\nAll tests finished." << std::endl;
        logger << "Total files: " << total << std::endl;
        logger << "Passed: " << result[Passed] << std::endl;
        logger << "IStream threw: " << result[IThrew] << std::endl;
        logger << "String threw: " << result[SThrew] << std::endl;
        logger << "Inequal: " << result[Inequal] << std::endl;
        logger << "SFailed: " << result[SFailed] << std::endl;
        logger << "IFailed: " << result[IFailed] << std::endl;
    }

    void temp_test()
    {
        using namespace JSONpp;
        using namespace Test;

        std::string testdir = "error_handling";
        std::string testnum = "010";
        fs::directory_entry testfile("../tests/" + testdir + "/" + testdir + testnum + ".json");
        doTestFor(Testee(testfile));
    }
}

int main()
{
    using namespace Test;
    //std::cout << "JSONpp::isContiguousStream_v<JSONpp::StringViewStream>: " << JSONpp::traits::isContiguousStream_v<JSONpp::StringViewStream> << std::endl;

    constexpr bool choice = 1;
    if constexpr (choice)
    {
        std::ofstream logfile("/mnt/d/works/jsontests/test_log_" + getDateTimeString() + ".log"
           , std::ios::out | std::ios::trunc);
        if (!logfile.is_open())
            throw std::runtime_error("Could not open logfile");
        testAll(logfile);
    }
    else
    {
        temp_test();
    }
    return 0;
}

