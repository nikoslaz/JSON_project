#include "../include/JSONlang.h"

struct JsonObject {
    map<string, JsonValue> values;

    JsonValue& operator[](const std::string& key) {
        return values[key];
    }
};

struct JsonArray {
    vector<JsonValue> values;

    JsonValue& operator[](size_t index) {
        return values.at(index);
    }

    void append(const JsonValue& value) {
        values.push_back(value);
    }
};