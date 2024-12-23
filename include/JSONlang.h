#ifndef JSONLANG.H
#define JSONLANG.H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>

using namespace std;
using JsonObject = unordered_map<string, JsonNode>;
using JsonArray = vector<JsonNode>;

enum class JsonType : short {
    NUMBER,
    STRING,
    NULLT,
    OBJECT,
    ARRAY,
    BOOL
};

class JsonNode {
    /* Member Variables */
    private:
        JsonType   type;
        JsonObject object;
        JsonArray array;

        union JsonValue {
            string j_string;
            double j_number;
            bool  j_bool;
            JsonValue()  {}
            ~JsonValue() {}

            /* Conversion operators for the union */
            operator string() {
                return j_string;
            }

            operator double() {
                return j_number;
            }

            operator int() {
                return j_number;
            }

            operator bool() {
                return j_bool;
            }

        } JsonValue;

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

        JsonNode(JsonType) : type(type) {}

        /* Default constructor creates null value node */
        JsonNode() : type(JsonType::NULLT) {}

        /* Constructor for implicit conversion from nullptr */
        JsonNode(nullptr_t value) : JsonNode() {}

        JsonNode(const vector<JsonNode> &nodes) : type(JsonType::ARRAY), array(nodes) {}

        JsonNode(double value) : type(JsonType::NUMBER) {
            JsonValue.j_number = value;
        }

        JsonNode(const string value) : type(JsonType::STRING) {
            JsonValue.j_string = value;
        }

        JsonNode(bool value) : type(JsonType::BOOL) {
            JsonValue.j_bool = value;
        }

        /* Copy constructor */
        JsonNode (const JsonNode &node) {
            type   = node.type;
            object = node.object;
            array  = node.array;

            JsonValue.j_bool   = node.JsonValue.j_bool;
            JsonValue.j_string = node.JsonValue.j_string;
            JsonValue.j_number = node.JsonValue.j_number;
        } 

        /* Copy assignment operator */
        JsonNode &operator=(const JsonNode &node) {
            type   = node.type;
            object = node.object;
            array  = node.array;

            JsonValue.j_bool   = node.JsonValue.j_bool;
            JsonValue.j_string = node.JsonValue.j_string;
            JsonValue.j_number = node.JsonValue.j_number;
        } 

        bool isValue() {
            return type == JsonType :: BOOL   ||
                   type == JsonType :: NUMBER ||
                   type == JsonType :: STRING ||
                   type == JsonType :: NULLT;
        }

        bool isObject() {
            return type == JsonType :: OBJECT;
        }

        bool isArray() {
            return type == JsonType :: ARRAY;
        }

        bool isNULL() {
            return type == JsonType :: NULLT;
        }

        void appendArray(const JsonNode &node) {
            array.push_back(node);
        }

        template<typename T>
        T get() {
            if (!isValue()) {
                throw std::runtime_error("unable to get value for this type");
            }
            return static_cast<T>(JsonValue);
        }

        /* Index operator overloads*/
        JsonNode &operator[](int index) {
            limitToArray();
            return array[index];
        }

        JsonNode &operator[](const string &key) {
            limitToObject();
            return object[key];
        }

        JsonNode &operator[](const char *key) {
            limitToObject();
            return object[key];
        }

        /* Conversion operator overloads */
        operator string() {
            return JsonValue.j_string;
        }

        operator int() {
            return JsonValue.j_number;
        }

        operator double() {
            return JsonValue.j_number;
        }

        operator bool() {
            return JsonValue.j_bool;
        }
};

#endif //JSONLANG.H