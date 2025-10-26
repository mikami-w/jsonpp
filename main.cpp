#include <iostream>
#include <string>
#include "jsonpp.h"
int main()
{
    std::cout << std::boolalpha;

    std::string input("true");
    auto json = JSONpp::parse(input);
    std::cout << json.as_bool() << std::endl;


    return 0;
}