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


};