#include <iomanip>
#include <iostream>
#include <string>
#include "jsonpp.h"
#include "print.h"
#include "print_ref.h"

int main()
{
    // test_print_h();

    std::cout << std::boolalpha;

    std::string input(R"JSON(
{
  "type": "minecraft:entity",
  "pools": [
    {
      "bonus_rolls": 0.0,
      "entries": [
        {
          "type": "minecraft:item",
          "functions": [
            {
              "add": false,
              "count": {
                "type": "minecraft:uniform",
                "max": 1.0,
                "min": -1.0
              },
              "function": "minecraft:set_count"
            },
            {
              "count": {
                "type": "minecraft:uniform",
                "max": 1.0,
                "min": 0.0
              },
              "function": "minecraft:looting_enchant"
            }
          ],
          "name": "minecraft:coal"
        }
      ],
      "rolls": 1.0
    },
    {
      "bonus_rolls": 0.0,
      "entries": [
        {
          "type": "minecraft:item",
          "functions": [
            {
              "add": false,
              "count": {
                "type": "minecraft:uniform",
                "max": 2.0,
                "min": 0.0
              },
              "function": "minecraft:set_count"
            },
            {
              "count": {
                "type": "minecraft:uniform",
                "max": 1.0,
                "min": 0.0
              },
              "function": "minecraft:looting_enchant"
            }
          ],
          "name": "minecraft:bone"
        }
      ],
      "rolls": 1.0
    },
    {
      "bonus_rolls": 0.0,
      "conditions": [
        {
          "condition": "minecraft:killed_by_player"
        },
        {
          "chance": 0.025,
          "condition": "minecraft:random_chance_with_looting",
          "looting_multiplier": 0.01
        }
      ],
      "entries": [
        {
          "type": "minecraft:item",
          "name": "minecraft:wither_skeleton_skull"
        }
      ],
      "rolls": 1.0
    }
  ],
  "random_sequence": "minecraft:entities/wither_skeleton"
}
)JSON");
    if (auto json = JSONpp::parse(input); json.has_value())
    {
        // auto json_value = json.value();
        JSONpp::JSONValue json_value("2222");
        std::cout << json_value.stringify() << std::endl;
    }
    else
    {
        std::cout << "document is empty." << std::endl;
    }


    // JSONpp::JSONValue json("Hello World");
    // std::cout << json.as_string() << std::endl;
    // json = 123;
    // std::cout << json.as_int64() << std::endl;
    // json = (void*)0;
    // json = {1, 1};

    return 0;
}