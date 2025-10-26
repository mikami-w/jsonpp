#include <iostream>
#include <string>
#include "jsonpp.h"
int main()
{
    std::cout << std::boolalpha;

    std::string input("3e-5");
    auto json = JSONpp::parse(input);
    std::cout << json.is_number() << std::endl;


    return 0;
}