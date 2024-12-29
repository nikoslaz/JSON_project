#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

enum class JsonType : short
{
    NUMBER,
    STRING,
    NULLT,
    OBJECT,
    ARRAY,
    BOOL
};

class JsonValue {
    JsonType d_type;
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

    JsonValue(JsonType type) : d_type(type) {}

    // default constructor creates null value node
    JsonValue() : d_type(JsonType::NULLT) {}

    // constructors for implicit conversion from nullptr
    JsonValue(nullptr_t value) : JsonValue() {}

    JsonValue(double value) : d_type(JsonType::NUMBER)
    {
        d_value.d_number = value;
    }

    JsonValue(const vector<JsonValue> &nodes) : d_type(JsonType::ARRAY), d_array(nodes) {}

    JsonValue(int value) : d_type(JsonType::NUMBER)
    {
        d_value.d_number = value;
    }

    JsonValue(const string &value) : d_type(JsonType::STRING)
    {
        d_value.d_string = value;
    }

    JsonValue(const char *value) : d_type(JsonType::STRING)
    {
        d_value.d_string = value;
    }

    JsonValue(bool value) : d_type(JsonType::BOOL)
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

    bool isValue()
    {
        return d_type == JsonType::BOOL ||
               d_type == JsonType::NUMBER ||
               d_type == JsonType::STRING ||
               d_type == JsonType::NULLT;
    }

    // helper methods
    bool isObject()
    {
        return d_type == JsonType::OBJECT;
    }

    bool isArray()
    {
        return d_type == JsonType::ARRAY;
    }

    bool isNUll()
    {
        return d_type == JsonType::NULLT;
    }

    void appendArray(const JsonValue &node)
    {
        d_array.push_back(node);
    }

    template <typename T>
    T get()
    {
        if (!isValue())
        {
            throw runtime_error("unable to get value for this type");
        }
        return static_cast<T>(d_value);
    }

    
    // index operator overloads
    JsonValue &operator[](int index)
    {
        limitToArray();
        return d_array[index];
    }

    JsonValue &operator[](const string &key)
    {
        limitToObject();
        return d_object[key];
    }

    JsonValue &operator[](const char *key)
    {
        limitToObject();
        return d_object[key];
    }

    // conversion operator oveloads
    operator string()
    {
        return d_value.d_string;
    }

    operator int()
    {
        return d_value.d_number;
    }

    operator double()
    {
        return d_value.d_number;
    }

    operator bool()
    {
        return d_value.d_bool;
    }    

    JsonValue parse(const std::string &s);
};