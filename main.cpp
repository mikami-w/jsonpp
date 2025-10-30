#include <iomanip>
#include <iostream>
#include <string>
#include "jsonpp.h"
#include "tests/test.h"
#include "print.h"
#include "print_ref.h"

int main()
{
    Test::test_usability();
    std::cerr << std::endl;
    Test::test_error_handling();

    return 0;
}