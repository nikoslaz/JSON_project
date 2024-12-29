#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

enum class JSONType : short
{
    NUMBER,
    STRING,
    NULLT,
    OBJECT,
    ARRAY,
    BOOL
};

class JsonValue {
    JSONType d_type;
    unordered_map<string, JsonValue> d_object;
    vector <JsonValue> d_array;

    // union to hold the value if there is a value.
    union value
    {
        string d_string;
        double d_number;
        bool d_bool;
        value() {}
        ~value() {}

        // conversion operators for the union
        operator string()
        {
            return d_string;
        }

        operator double()
        {
            return d_number;
        }

        operator int()
        {
            return d_number;
        }

        operator bool()
        {
            return d_bool;
        }

    } d_value;

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

    JsonValue(JSONType type) : d_type(type) {}

    // default constructor creates null value node
    JsonValue() : d_type(JSONType::NULLT) {}

    // constructors for implicit conversion from nullptr
    JsonValue(nullptr_t value) : JsonValue() {}

    JsonValue(double value) : d_type(JSONType::NUMBER)
    {
        d_value.d_number = value;
    }

    JsonValue(const vector<JsonValue> &nodes) : d_type(JSONType::ARRAY), d_array(nodes) {}

    JsonValue(int value) : d_type(JSONType::NUMBER)
    {
        d_value.d_number = value;
    }

    JsonValue(const string &value) : d_type(JSONType::STRING)
    {
        d_value.d_string = value;
    }

    JsonValue(const char *value) : d_type(JSONType::STRING)
    {
        d_value.d_string = value;
    }

    JsonValue(bool value) : d_type(JSONType::BOOL)
    {
        d_value.d_bool = value;
    }

    // copy constructor
    JsonValue(const JsonValue &node)
    {
        d_type = node.d_type;
        d_object = node.d_object;
        d_array = node.d_array;

        d_value.d_bool = node.d_value.d_bool;
        d_value.d_string = node.d_value.d_string;
        d_value.d_number = node.d_value.d_number;
    }

    // copy assignment operator 
    JsonValue &operator=(const JsonValue &node)
    {
        d_type = node.d_type;
        d_object = node.d_object;
        d_array = node.d_array;

        d_value.d_bool = node.d_value.d_bool;
        d_value.d_string = node.d_value.d_string;
        d_value.d_number = node.d_value.d_number;

        return *this;
    }


};