#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <functional>
#include "JSONlang.h"

int main()
{
    // A few demo calls to json.parseInput(...)
    JsonValue json;
    string input;

    std::cout << json.parseInput("STRING(\"Hello\")") << std::endl;   
    // -> "Hello"

    std::cout << json.parseInput("NUMBER(123)") << std::endl;         
    // -> 123

    std::cout << json.parseInput("TRUE") << std::endl;                
    // -> true

    std::cout << json.parseInput("FALSE") << std::endl;               
    // -> false

    std::cout << json.parseInput("NULL") << std::endl;                
    // -> null

    // OBJECT
    std::cout << json.parseInput("OBJECT{ KEY(x): NUMBER(42), KEY(msg): STRING(\"Hi\") }") 
              << std::endl;
    // -> {"x": 42, "msg": "Hi"}

    // ARRAY
    std::cout << json.parseInput("ARRAY[ NUMBER(10), STRING(\"abc\"), TRUE ]") 
              << std::endl;
    // -> [10, "abc", true]

    // Nested example
    std::cout << json.parseInput(
        "OBJECT{ KEY(data): ARRAY[ NUMBER(1), OBJECT{ KEY(foo): STRING(\"bar\") } ] }"
    ) << std::endl;
    // -> {"data": [1, {"foo": "bar"}]}

    // Unknown
    std::cout << json.parseInput("FOO(999)") << std::endl; 
    // -> UNKNOWN_TYPE

    input = json.parseInput("OBJECT{KEY(title): STRING(\"Gone Girl\"),KEY(year): NUMBER(2011),KEY(available): TRUE}");
    string jsonObject1 = input;
    std::cout << "jsonObject1: " << jsonObject1 << std::endl;

    string jsonObjectStr = R"({"title": "Gone Girl", "year": 2011, "available": true})";
    string jsonArrayStr = R"(["First", "Second", "Third"])";

    // Parse the JSON object string
    try {
        JsonValue parsedObject = JsonValue::parse(jsonObjectStr);
        cout << "Parsed JSON Object:" << endl;
        cout << "Title: " << static_cast<string>(parsedObject["title"]) << endl;
        cout << "Year: " << static_cast<int>(parsedObject["year"]) << endl;
        cout << "Available: " << static_cast<bool>(parsedObject["available"]) << endl;
    } catch (const exception &e) {
        cerr << "Error parsing JSON object: " << e.what() << endl;
    }

    // Parse the JSON array string
    try {
        JsonValue parsedArray = JsonValue::parse(jsonArrayStr);
        cout << "Parsed JSON Array:" << endl;
        cout << "First Element: " << static_cast<string>(parsedArray[0]) << endl;
        cout << "Second Element: " << static_cast<string>(parsedArray[1]) << endl;
        cout << "Third Element: " << static_cast<string>(parsedArray[2]) << endl;
    } catch (const exception &e) {
        cerr << "Error parsing JSON array: " << e.what() << endl;
    }

    return 0;
}