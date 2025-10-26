#include <iomanip>
#include <iostream>
#include <string>
#include "jsonpp.h"
int main()
{
    std::cout << std::boolalpha;

    std::string input(R"JSON(  )JSON");
    if (auto json = JSONpp::parse(input); json.has_value())
    {
        auto json_value = json.value();
        std::cout << json_value.as_string() << std::endl;
    }
    else
    {
        std::cout << "document is empty.";
    }


    return 0;
}