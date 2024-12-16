#ifndef JSONLANG.H
#define JSONLANG.H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <variant>

using namespace std;

struct JsonObject {
    map<string, JsonValue> JsonMap;
};

struct JsonArray {
    vector<JsonValue> JsonVector;
};

struct JsonValue {
    variant<string, double, JsonObject, JsonArray, bool, nullptr_t> JsonValue;
};

#endif //JSONLANG.H