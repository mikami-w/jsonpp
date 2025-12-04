#ifndef JSONPP_TEST_UTILS_HPP
#define JSONPP_TEST_UTILS_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;
using TestDirectoryEntries = std::unordered_map<std::string, std::vector<fs::directory_entry>>;

inline std::string readFileToString(std::string const& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::string content(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );

    file.close();

    return content;
}

inline std::string getDateTimeString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y%m%d_%H%M%S");
    return std::move(oss.str());
}

inline bool is_hidden(fs::directory_entry const& file)
{
    return file.path().filename().string().front() == '.';
}

#endif //JSONPP_TEST_UTILS_HPP