#include <iomanip>
#include <iostream>
#include <string>
#include "jsonpp.h"
#include "tests/test.h"
#include "basic_json_stream.h"
// #include "print.h"
// #include "print_ref.h"

int main()
{
    std::cerr << JSONpp::isContiguousStream_v<JSONpp::StringViewStream> << std::endl;

    Test::test_usability();
    std::cerr << std::endl;
    Test::test_error_handling();
    std::cerr << std::endl;

    return 0;
}