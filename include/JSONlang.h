#ifndef JSONLANG.H
#define JSONLANG.H

// #define JSON(name) JsonValue name
// #define STRING(value) sstring(value)
// #define NUMBER(value) double(value)
// #define TRUE true
// #define FALSE false
// #define NULLVALUE nullptr
// #define OBJECT JsonObject{{}}
// #define ARRAY JsonArray{}
// #define KEY(key) key

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <variant>

using namespace std;

/* Map of key-value pairs */
struct JsonObject {
    map<string, JsonValue> values;

    JsonValue& operator[](const string& key) {
        return values[key];
    }
};

/* Vector of JSON values */
struct JsonArray {
    vector<JsonValue> values;

    JsonValue& operator[](size_t index) {
        return values.at(index);
    }

    void append(const JsonValue& value) {
        values.push_back(value);
    }
};

/* Define types for JSON values */
typedef variant<string, double, bool, nullptr_t, struct JsonObject, struct JsonArray> JsonValue;

template<typename... Args>
void PRINT(Args... args) {
    ((cout << args << " "), ...);
    cout << std:: endl;
}

#endif //JSONLANG.H