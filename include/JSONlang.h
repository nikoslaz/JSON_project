#ifndef JSONLANG_H
#define JSONLANG_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include <stdexcept>

using namespace std;

using JsonObject = unordered_map<string, class JsonValue>;
using JsonArray = vector<class JsonValue>;

enum class JsonType : short {
    NUMBER,
    STRING,
    NULLT,
    OBJECT,
    ARRAY,
    BOOL
};

class JsonValue {
private:
    JsonType type;
    JsonObject object;
    JsonArray array;

    // Replace union with variant
    using JsonValueType = variant<monostate, string, double, bool>;
    JsonValueType j_value;

    void limitToObject() {
        if (!isObject()) {
            throw runtime_error("this operation is only available to object node");
        }
    }

    void limitToArray() {
        if (!isArray()) {
            throw runtime_error("this operation is only available to array node");
        }
    }

public:
    // Constructors
    JsonValue(JsonType type) : type(type) {}

    JsonValue() : type(JsonType::NULLT), j_value(monostate{}) {}

    JsonValue(nullptr_t) : JsonValue() {}

    JsonValue(const vector<JsonValue>& nodes) : type(JsonType::ARRAY), array(nodes) {}

    JsonValue(double value) : type(JsonType::NUMBER), j_value(value) {}

    JsonValue(const string& value) : type(JsonType::STRING), j_value(value) {}

    JsonValue(bool value) : type(JsonType::BOOL), j_value(value) {}

    // Copy constructor
    JsonValue(const JsonValue& node) : type(node.type), object(node.object), array(node.array), j_value(node.j_value) {}

    // Copy assignment operator
    JsonValue& operator=(const JsonValue& node) {
        if (this != &node) {
            type = node.type;
            object = node.object;
            array = node.array;
            j_value = node.j_value;
        }
        return *this;
    }

    JsonValue& operator=(const JsonObject& node) {
            object = node;
        return *this;
    }

    // Member functions
    bool isValue() const {
        return type == JsonType::BOOL || type == JsonType::NUMBER || type == JsonType::STRING || type == JsonType::NULLT;
    }

    bool isObject() const {
        return type == JsonType::OBJECT;
    }

    bool isArray() const {
        return type == JsonType::ARRAY;
    }

    bool isNULL() const {
        return type == JsonType::NULLT;
    }

    void appendArray(const JsonValue& node) {
        if (!isArray()) {
            throw runtime_error("appendArray is only available for array nodes");
        }
        array.push_back(node);
    }

    template<typename T>
    T get() const {
        if (!isValue()) {
            throw runtime_error("unable to get value for this type");
        }
        return get<T>(j_value);
    }

    // Index operator overloads
    JsonValue& operator[](int index) {
        limitToArray();
        return array[index];
    }

    JsonValue& operator[](const string& key) {
        limitToObject();
        return object[key];
    }

    JsonValue& operator[](const char* key) {
        return (*this)[string(key)];
    }

    // Conversion operator overloads
    operator string() const {
        return get<string>();
    }

    operator int() const {
        return static_cast<int>(get<double>());
    }

    operator double() const {
        return get<double>();
    }

    operator bool() const {
        return get<bool>();
    }
};

#endif // JSONLANG_H
