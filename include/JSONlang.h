#ifndef JSON_LANG_H
#define JSON_LANG_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <sstream>
#include <functional>
#include <regex>

using namespace std;

class JsonValue;

using JsonObject = map<string, JsonValue>;
using JsonArray = vector<JsonValue>;
using JsonVariant = variant<monostate, bool, double, string, JsonObject, JsonArray>;

class JsonValue {
private:
    JsonVariant data;

    void limitToArray()
    {
        if (!isArray())
        {
            throw runtime_error("this operation is only available to array node");
        }
    }

    void limitToObject()
    {
        if (!isObject())
        {
            throw runtime_error("this operation is only available to object node");
        }
    }

public:
    JsonValue() : data(monostate{}) {}
    JsonValue(bool value) : data(value) {}
    JsonValue(double value) : data(value) {}
    JsonValue(int value) : data(static_cast<double>(value)) {}
    JsonValue(const string& value) : data(value) {}
    JsonValue(const char* value) : data(string(value)) {}
    JsonValue(const JsonObject& value) : data(value) {}
    JsonValue(const JsonArray& value) : data(value) {}


    // Copy constructor
    JsonValue(const JsonValue &value) {
        data = value.data;
    }

    JsonValue &operator=(const JsonValue &value) {
        data = value.data;
        return *this;
    }

    bool isValue() {
        return holds_alternative<double>(this->data) ||
               holds_alternative<string>(this->data) ||
               holds_alternative<bool>(this->data);
    }

    bool isObject() {
        return holds_alternative<JsonObject>(this->data);
    }

    bool isArray() {
        return holds_alternative<JsonArray>(this->data);
    }

    bool isNULL() {
        return holds_alternative<monostate>(this->data);
    }

    // index operator overloads
    JsonValue &operator[](int index)
    {
        limitToArray();
        return get<JsonArray>(this->data)[index];
    }

    JsonValue &operator[](const string &key)
    {
        limitToObject();
        return get<JsonObject>(this->data)[key];
    }

    JsonValue &operator[](const char *key)
    {
        limitToObject();
        return get<JsonObject>(this->data)[key];
    }

    JsonVariant getData() {
        return this->data;
    }

    // Conversion operator overloads
    operator string() {
        if (holds_alternative<string>(data)) {
            return get<string>(data);
        }
        throw runtime_error("JsonValue is not a string");
    }

    operator int() {
        if (holds_alternative<double>(data)) {
            return static_cast<int>(get<double>(data));
        }
        throw runtime_error("JsonValue is not a number");
    }

    operator double() {
        if (holds_alternative<double>(data)) {
            return get<double>(data);
        }
        throw runtime_error("JsonValue is not a number");
    }

    operator bool() {
        if (holds_alternative<bool>(data)) {
            return get<bool>(data);
        }
        throw runtime_error("JsonValue is not a boolean");
    }

    static string parseInput(const string& input);
    static JsonValue parse(const string &s);

};

#endif