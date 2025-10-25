#include <iostream>
#include <string>
#include "jsonpp.h"
int main()
{
    std::cout << std::boolalpha;

    std::string input("null");
    auto json = JSONpp::parse(input);
    std::cout << json.is_null() << std::endl;


    return 0;
}