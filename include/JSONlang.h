#ifndef JSONLANG.H
#define JSONLANG.H

#define JSON(name) JsonValue name
#define STRING(value) std::string(value)
#define NUMBER(value) 
#define TRUE true
#define FALSE false
#define NULLVALUE nullptr
#define OBJECT JsonObject
#define ARRAY JsonArray
#define KEY(key) key


#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <variant>

using namespace std;

/* Map of key-value pairs */
struct JsonObject;

/* Vector of JSON values */
struct JsonArray;

/* Define types for JSON values */
typedef variant<string, double, bool, nullptr_t, struct JsonObject, struct JsonArray> JsonValue;

template<typename... Args>
void PRINT(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

#endif //JSONLANG.H