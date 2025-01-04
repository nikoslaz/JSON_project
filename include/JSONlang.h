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
class KeyValue;

using JsonObject = map<string, JsonValue>;
using JsonArray = vector<JsonValue>;
using JsonVariant = variant<monostate, bool, double, string, JsonObject, JsonArray>;

class JsonValue {
public:
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

    JsonValue() : data(monostate{}) {}
    JsonValue(bool value) : data(value) {}
    JsonValue(double value) : data(value) {}
    JsonValue(int value) : data(static_cast<double>(value)) {}
    JsonValue(const string& value) : data(value) {}
    JsonValue(const char* value) : data(string(value)) {}
    JsonValue(const JsonObject& value) : data(value) {}
    JsonValue(const JsonArray& value) : data(value) {}
    JsonValue(nullptr_t) : data(monostate{}) {}
    JsonValue(const std::initializer_list<JsonValue>& list)
        : data(JsonObject())
    {
        auto& obj = get<JsonObject>(data);
        for (const auto& item : list) {
            if (item.isObject()) {
                const auto& itemObj = get<JsonObject>(item.data);
                obj.insert(itemObj.begin(), itemObj.end());
            }
        }
    }

    // Copy constructor
    JsonValue(const JsonValue &value) {
        data = value.data;
    }

    JsonValue &operator=(const JsonValue &value) {
        data = value.data;
        return *this;
    }

    bool isValue() const {
        return holds_alternative<double>(this->data) ||
               holds_alternative<string>(this->data) ||
               holds_alternative<bool>(this->data);
    }

    bool isObject() const {
        return holds_alternative<JsonObject>(this->data);
    }

    bool isArray() const {
        return holds_alternative<JsonArray>(this->data);
    }

    bool isNULL() const {
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
        } else if (holds_alternative<monostate>(data)) {
            return "null";
        }
        throw runtime_error("JsonValue is not a string or null");
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


    void append(const vector<JsonValue>& values) {
        if (!isArray()) {
            throw runtime_error("APPEND operation is only valid for arrays");
        }

        auto& arr = get<JsonArray>(data);

        for (const auto& value : values) {
            arr.push_back(value);
        }
    }

    static string parseInput(const string& input);
    static JsonValue parse(const string &s);
    static string executeSet(JsonValue& root, const string& command);
    static void printJsonValue(const JsonValue& value, int indent);
    static void printJsonObject(const JsonObject& obj, int indent);
    
};

class KeyValue {
public:
    string key;
    JsonValue value;

    KeyValue() : value(nullptr) {}

    // Create sets the key and returns false for chaining in macros
    bool create(const string& k) {
        key = k;
        return false;
    }

    // Ret returns either a JsonObject or a plain JsonValue
    JsonValue ret(const JsonValue& val) {
        if (key.empty()) {
            return val; // If no key is set, return the value directly
        }
        
        value = val;
        JsonObject obj;
        obj.insert({key, val});

        return JsonValue{obj};
    }

};

#endif