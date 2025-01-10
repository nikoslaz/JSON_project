#ifndef JSON_LANG_H
#define JSON_LANG_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>
#include <cmath>

using namespace std;

class JsonArrayHelper;

//Class JsonValue
class JsonValue {
public:
    JsonValue* parent_value = nullptr;
    string parent_key;
    size_t parent_index;
    bool hasparent_key = false;   // if it's true then we're tracking an object key
    bool hasparent_index = false; // if it's true then we're tracking an array index

    enum class Type {String = 0, Number = 1, Boolean = 2, Null = 3, Object = 4, Array = 5} type;

    union {
        string* string_value;
        double number_value;
        bool bool_value;
        class JsonObject* object_value;
        class JsonArray* arrayValue;  
    } data;

    // Constructors
    JsonValue();
    JsonValue(Type t);
    JsonValue(const string& value);
    JsonValue(const char* value);
    JsonValue(double value);
    JsonValue(bool value);
    JsonValue(nullptr_t);
    JsonValue(const JsonObject& obj);
    JsonValue(const JsonArray& arr);
    JsonValue(const JsonValue& other);
    JsonValue(const JsonArrayHelper& jah);
    JsonValue(size_t value);

    //Destructor
    ~JsonValue();

    //Methods
    void print() const;
    size_t size() const;
    string asString() const;
    bool hasKey(const string& key) const;
    Type getType() const { return type; }

    // Overloaded operators
    JsonValue& operator=(const JsonValue& other);
    JsonValue& operator=(const JsonObject& other);
    JsonValue& operator=(const JsonArray& other);

    JsonValue& operator[](const string& key);
    JsonValue& operator[](size_t index);

    template <typename DataType>
    JsonArray& operator+=(const DataType& dt);

    template <typename DataType>
    JsonArray operator,(const DataType& dt);

    JsonValue& operator--();

    JsonValue operator+(const JsonValue& other) const;
    JsonValue operator-(const JsonValue& other) const;
    JsonValue operator*(const JsonValue& other) const;
    JsonValue operator/(const JsonValue& other) const;
    JsonValue operator%(const JsonValue& other) const;

    bool operator>(const JsonValue& other) const;
    bool operator>=(const JsonValue& other) const;
    bool operator<(const JsonValue& other) const;
    bool operator<=(const JsonValue& other) const;

    JsonValue operator&&(const JsonValue& other) const;
    JsonValue operator||(const JsonValue& other) const;
    JsonValue operator!() const;

    bool operator==(const JsonValue& other) const;
    bool operator!=(const JsonValue& other) const;

    friend ostream& operator<<(ostream& os, const JsonValue& jv) {
        jv.print();
        return os << endl;
    }

private:
    string typeToString(Type t) const {
        switch(t) {
            case Type::String: return "String";
            case Type::Number: return "Number";
            case Type::Boolean: return "Boolean";
            case Type::Null: return "Null";
            case Type::Object: return "Object";
            case Type::Array: return "Array";
            default: return "Unknown";
        }
    }
};

// Add operator== declarations for comparing with JsonObject and JsonArray directly
bool operator==(const JsonObject& lhs, const JsonObject& rhs);
bool operator==(const JsonArray& lhs, const JsonArray& rhs);
bool operator!=(const JsonObject& lhs, const JsonObject& rhs);
bool operator!=(const JsonArray& lhs, const JsonArray& rhs);

//Class JsonObject
class JsonObject {
    friend class JsonValue;  // With friend allows JsonValue to access private members
    vector<pair<string, shared_ptr<JsonValue>>> data;

public:
    JsonObject();
    JsonObject(const JsonObject& other);
    JsonObject(initializer_list<JsonValue> list);

    void add(const string& key, const JsonValue& value);

    // Print each pair
    void print() const {
        cout << "{ ";
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it != data.begin()) cout << ", ";
            cout << "\"" << it->first << "\": ";
            it->second->print();
        }
        cout << " }";
    }

    size_t size() const {
        return data.size();
    }

    // Try to find key
    bool hasKey(const string& key) const {
        for (auto& pair : data) {
            if (pair.first == key) return true;
        }
        return false;
    }

    // If key not found, create a new pair
    JsonValue& operator[](const string& key) {
        for (auto& pair : data) {
            if (pair.first == key) {
                return *pair.second;
            }
        }
        data.push_back({ key, make_shared<JsonValue>() });
        auto& valRef = *data.back().second;
        valRef.parent_value = reinterpret_cast<JsonValue*>(this); // convert one pointer type to another pointer type
        valRef.parent_key = key;
        valRef.hasparent_key = true;
        return valRef;
    }

    // Removes a key if it exists
    void removeKey(const string& key) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->first == key) {
                data.erase(it);
                break;
            }
        }
    }

    // Clears all pairs
    void clear() {
        data.clear();
    }

    JsonObject& operator=(const JsonObject& other);

    JsonArray operator,(const JsonValue& jv);
    JsonArray operator,(const JsonArray& ja);
    JsonArray operator,(const JsonObject& jo);
};

//Class JsonArray
class JsonArray {
    vector<shared_ptr<JsonValue>> values;
    friend class JsonValue;  // Allow JsonValue to access private members

public:
    JsonArray();
    JsonArray(const JsonArray& other);
    JsonArray(const JsonArrayHelper& jah);

    template <typename DataType> 
    void add(const DataType& dt);
    void add(const JsonArray& arr);
    void append(const JsonArray& arr);
    void append(const JsonValue& value);

    JsonValue& operator[](size_t index);
    JsonArrayHelper operator[]( JsonArray ja);
    JsonArray& operator=(const JsonArray& other);
 
    template <typename DataType>
    JsonArray& operator+=(const DataType& dt);

    template <typename DataType>
    JsonArray operator,(const DataType& dt);

    JsonArray operator[](const JsonObject& obj);

    JsonValue operator[](initializer_list<JsonValue> init_list) {
        JsonArray tmp;
        for (auto &v : init_list) {
            tmp.add(v);
        }
        return JsonValue(tmp);
    }

    void print() const;
    size_t size() const;

    void removeIndex(size_t index);

    // Clears all elements
    void clear() {
        values.clear();
    }

    JsonArray operator+(const JsonArray& other) const {
        JsonArray result(*this);
        result.append(other);
        return result;
    }
};

//Helper class that will enable chaining via operator[]
class JsonArrayHelper {
public:
    JsonArray& obj;
    JsonArrayHelper(JsonArray& obj);
};

//-------------------------------------------------------------------------------------------------------------------------------------------//

//JsonValue implementation
JsonValue::JsonValue() {
    type = Type::Null;
    data.string_value = nullptr;
}

// Update JsonValue constructor for Type
JsonValue::JsonValue(Type t) {
    type = Type::String;  // We'll store the type name as a string
    switch(t) {
        case Type::String:
            data.string_value = new string("String");
            break;
        case Type::Number:
            data.string_value = new string("Number");
            break;
        case Type::Boolean:
            data.string_value = new string("Boolean");
            break;
        case Type::Object:
            data.string_value = new string("Object");
            break;
        case Type::Array:
            data.string_value = new string("Array");
            break;
        case Type::Null:
            data.string_value = new string("Null");
            break;
        default:
            data.string_value = new string("Unknown");
            break;
    }
}

JsonValue::JsonValue(const string& value) {
    type = Type::String;
    data.string_value = new string(value);
}

JsonValue::JsonValue(const char* value) {
    type = Type::String;
    data.string_value = new string(value);
}

JsonValue::JsonValue(double value) {
    type = Type::Number;
    data.number_value = value;
}

JsonValue::JsonValue(bool value) {
    type = Type::Boolean;
    data.bool_value = value;
}

JsonValue::JsonValue(nullptr_t) {
    type = Type::Null;
    data.string_value = nullptr;
}

JsonValue::JsonValue(const JsonObject& obj) {
    type = Type::Object;
    data.object_value = new JsonObject(obj);
}

JsonValue::JsonValue(const JsonArray& arr) {
    type = Type::Array;
    data.arrayValue = new JsonArray(arr);
}

JsonValue::JsonValue(const JsonArrayHelper& jah)
{
    type = Type::Array;
    data.arrayValue = new JsonArray(jah.obj);
}    

JsonValue::JsonValue(size_t value) {
    type = Type::Number;
    data.number_value = static_cast<double>(value);
}

JsonValue::JsonValue(const JsonValue& other) : type(other.type) {
    switch (type) {
        case Type::String:
            data.string_value = new string(*other.data.string_value);
            break;
        case Type::Number:
            data.number_value = other.data.number_value;
            break;
        case Type::Boolean:
            data.bool_value = other.data.bool_value;
            break;
        case Type::Object:
            data.object_value = new JsonObject(*other.data.object_value);
            break;
        case Type::Array:
            data.arrayValue = new JsonArray(*other.data.arrayValue);
            break;
        case Type::Null:
            data.string_value = nullptr;
            break;
    }
}

// Destructor for JsonValue
JsonValue::~JsonValue() {
    switch (type) {
        case Type::String:
            if (data.string_value) {
                delete data.string_value;
                data.string_value = nullptr;
            }
            break;
        case Type::Object:
            if (data.object_value) {
                delete data.object_value;
                data.object_value = nullptr;
            }
            break;
        case Type::Array:
            if (data.arrayValue) {
                delete data.arrayValue;
                data.arrayValue = nullptr;
            }
            break;
        default:
            break;
    }
    
    type = Type::Null;
}

string JsonValue::asString() const {
    if (type == Type::String) {
        return *data.string_value;
    }
    return "";
}

size_t JsonValue::size() const {
    switch (type) {
        case Type::Object: return data.object_value->size();
        case Type::Array: return data.arrayValue->size();
        default: return 1;
    }
}

// Update print method
void JsonValue::print() const {
    if (type == Type::String && data.string_value) {
        cout << *data.string_value;  // Print the string value directly
        return;
    }
    
    // Handle TYPE_OF cases
    if (type == Type::String && data.string_value == nullptr) {
        cout << typeToString(Type::String);
        return;
    }
    else if(type == Type::Object && data.string_value == nullptr) {
        cout << typeToString(Type::Object);
        return;
    }
    else if(type == Type::Array && data.string_value == nullptr) {
        cout << typeToString(Type::Array);
        return;
    }
    else if(type == Type::Number && data.string_value == nullptr) {
        cout << typeToString(Type::Number);
        return;
    }
    else if(type == Type::Boolean && data.string_value == nullptr) {
        cout << typeToString(Type::Boolean);
        return;
    }
    else if(type == Type::Null && data.string_value == nullptr) {
        cout << typeToString(Type::Null);
        return;
    }
    
    // Normal value printing
    switch (type) {
        case Type::String: 
            cout << "\"" << *data.string_value << "\""; 
            break;
        case Type::Number: 
            // Print integers without decimal point
            if (floor(data.number_value) == data.number_value) {
                cout << static_cast<long long>(data.number_value);
            } else {
                cout << data.number_value;
            }
            break;
        case Type::Boolean: 
            cout << (data.bool_value ? "true" : "false"); 
            break;
        case Type::Null: 
            cout << "null"; 
            break;
        case Type::Object: 
            data.object_value->print(); 
            break;
        case Type::Array: 
            data.arrayValue->print(); 
            break;
    }
}

bool JsonValue::hasKey(const string& key) const {
    if (type != Type::Object) {
        return false; // We search objects for key
    }
    return data.object_value->hasKey(key);
}

JsonValue& JsonValue::operator[](const string& key) {
    if (type != Type::Object) {
        type = Type::Object;
        data.object_value = new JsonObject();
    }
    auto& valRef = (*data.object_value)[key];
    valRef.parent_value = this;     // ‘this’ is a pointer to the JsonValue holding this object
    valRef.parent_key = key;
    valRef.hasparent_key = true;
    return valRef;
}

JsonValue& JsonValue::operator[](size_t index) {
    if (type != Type::Array) {
        type = Type::Array;
        data.arrayValue = new JsonArray();
    }
    auto& arr = *data.arrayValue;

    // Auto-resize if index is out of range
    while (arr.size() <= index) {
        arr.add(JsonValue());
    }
    auto& valRef = arr[index];
    valRef.parent_value = this;     // ‘this’ is a pointer to the JsonValue holding this array
    valRef.parent_index = index;
    valRef.hasparent_index = true;
    return valRef;
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
    cout << "~\n"; // Standard output
    if (this == &other) {
        return *this; // Handle self-assignment
    }

    cout << (int) type;
    cout.flush();

    // Clean up the current object
    JsonValue::~JsonValue();

    // Copy the data from the other
    type = other.type;
    switch (type) {
        case Type::String:
            data.string_value = new string(*other.data.string_value);
            break;
        case Type::Number:
            data.number_value = other.data.number_value;
            break;
        case Type::Boolean:
            data.bool_value = other.data.bool_value;
            break;
        case Type::Object:
            data.object_value = new JsonObject(*other.data.object_value);
            break;
        case Type::Array:
            data.arrayValue = new JsonArray(*other.data.arrayValue);
            break;
        case Type::Null:
            data.string_value = nullptr;
            break;
    }

    return *this;
}

JsonValue& JsonValue::operator=(const JsonObject& other) {

    // Clean up the current object
    JsonValue::~JsonValue();

    // Copy the data from the other
    type = Type::Object;
    data.object_value = new JsonObject(other);

    return *this;
}

JsonValue& JsonValue::operator=(const JsonArray& other) {

    // Clean up the current object
    JsonValue::~JsonValue();

    // Copy the data from the other
    type = Type::Array;
    data.arrayValue = new JsonArray(other);

    return *this;
}

template <typename DataType>    
JsonArray& JsonValue::operator+=(const DataType& dt)
{
    if( type != JsonValue::Type::Array)
        throw runtime_error("Error at line " + to_string(__LINE__) + ": operator+= data type does not support append");    
    data.arrayValue->add(dt);
    return *(data.arrayValue);
}

template <typename DataType>    
JsonArray JsonValue::operator,(const DataType& dt)
{
    JsonArray ja;
    ja.append(*this);
    ja.add(dt);
    return ja;
}

JsonValue& JsonValue::operator--() {
    if (parent_value && parent_value->type == Type::Object && hasparent_key) {
        parent_value->data.object_value->removeKey(parent_key);
    } 
    else if (parent_value && parent_value->type == Type::Array && hasparent_index) {
        parent_value->data.arrayValue->removeIndex(parent_index);
    } 
    // If there's NO parent, just clear ourselves if we're an object or array
    else if (!parent_value) {
        if (type == Type::Object) {
            data.object_value->clear();
        } else if (type == Type::Array) {
            data.arrayValue->clear();
        }
    }
    return *this;
}

JsonValue JsonValue::operator+(const JsonValue& other) const {
    // Handle number addition
    if (type == Type::Number && other.type == Type::Number) {
        return JsonValue(data.number_value + other.data.number_value);
    }
    
    // Handle string concatenation
    if (type == Type::String && other.type == Type::String) {
        return JsonValue(*data.string_value + *other.data.string_value);
    }
    
    // Handle array merging
    if (type == Type::Array && other.type == Type::Array) {
        JsonArray result(*data.arrayValue);
        result.append(*other.data.arrayValue);
        return JsonValue(result);
    }
    
    // Handle object merging
    if (type == Type::Object && other.type == Type::Object) {
        JsonObject result(*data.object_value);
        for (const auto& pair : other.data.object_value->data) {
            result.add(pair.first, *pair.second);
        }
        return JsonValue(result);
    }
    
    throw runtime_error("Error: operator+ only supports Number+Number, String+String, Array+Array, or Object+Object");
}

JsonValue JsonValue::operator-(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator- only supports Number-Number");
    }
    return JsonValue(data.number_value - other.data.number_value);
}

JsonValue JsonValue::operator*(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator* only supports Number-Number");
    }
    return JsonValue(data.number_value * other.data.number_value);
}

JsonValue JsonValue::operator/(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator/ only supports Number-Number");
    }
    if (other.data.number_value == 0) {
        throw runtime_error("Error: division by zero");
    }
    return JsonValue(data.number_value / other.data.number_value);
}

JsonValue JsonValue::operator%(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator% only supports Number-Number");
    }
    if (other.data.number_value == 0) {
        throw runtime_error("Error: modulo by zero");
    }
    return JsonValue(fmod(data.number_value, other.data.number_value));
}

bool JsonValue::operator>(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator> only supports Number-Number");
    }
    return data.number_value > other.data.number_value;
}

bool JsonValue::operator>=(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator>= only supports Number-Number");
    }
    return data.number_value >= other.data.number_value;
}

bool JsonValue::operator<(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator< only supports Number-Number");
    }
    return data.number_value < other.data.number_value;
}

bool JsonValue::operator<=(const JsonValue& other) const {
    if (type != Type::Number || other.type != Type::Number) {
        throw runtime_error("Error: operator<= only supports Number-Number");
    }
    return data.number_value <= other.data.number_value;
}

JsonValue JsonValue::operator&&(const JsonValue& other) const {
    if (type != Type::Boolean || other.type != Type::Boolean) {
        throw runtime_error("Error: operator&& only supports Boolean&&Boolean");
    }
    return JsonValue(data.bool_value && other.data.bool_value);
}

JsonValue JsonValue::operator||(const JsonValue& other) const {
    if (type != Type::Boolean || other.type != Type::Boolean) {
        throw runtime_error("Error: operator|| only supports Boolean||Boolean");
    }
    return JsonValue(data.bool_value || other.data.bool_value);
}

JsonValue JsonValue::operator!() const {
    if (type != Type::Boolean) {
        throw runtime_error("Error: operator! only supports Boolean values");
    }
    return JsonValue(!data.bool_value);
}

bool JsonValue::operator==(const JsonValue& other) const {
    if (type != other.type) {
        return false;
    }

    switch (type) {
        case Type::String:
            return *data.string_value == *other.data.string_value;
        case Type::Number:
            return data.number_value == other.data.number_value;
        case Type::Boolean:
            return data.bool_value == other.data.bool_value;
        case Type::Null:
            return true;
        case Type::Array:
            if (data.arrayValue->size() != other.data.arrayValue->size()) {
                return false;
            }
            // Compare each element recursively
            for (size_t i = 0; i < data.arrayValue->size(); i++) {
                if (!(*data.arrayValue->values[i] == *other.data.arrayValue->values[i])) {
                    return false;
                }
            }
            return true;
        case Type::Object:
            if (data.object_value->size() != other.data.object_value->size()) {
                return false;
            }
            // Compare each key-value pair recursively
            for (const auto& pair : data.object_value->data) {
                bool found = false;
                for (const auto& other_pair : other.data.object_value->data) {
                    if (pair.first == other_pair.first) {
                        if (!(*pair.second == *other_pair.second)) {
                            return false;
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
            return true;
    }
    return false;
}

bool JsonValue::operator!=(const JsonValue& other) const {
    return !(*this == other);
}

bool operator==(const JsonObject& lhs, const JsonObject& rhs) {
    return JsonValue(lhs) == JsonValue(rhs);
}

bool operator==(const JsonArray& lhs, const JsonArray& rhs) {
    return JsonValue(lhs) == JsonValue(rhs);
}

bool operator!=(const JsonObject& lhs, const JsonObject& rhs) {
    return !(lhs == rhs);
}

bool operator!=(const JsonArray& lhs, const JsonArray& rhs) {
    return !(lhs == rhs);
}

//-------------------------------------------------------------------------------------------------------------------------------------------//

//JsonObject implementation
JsonObject::JsonObject() {}

JsonObject::JsonObject(initializer_list<JsonValue> list)
{
    auto it = list.begin();
    while (it != list.end()) {
        const auto& key = *it;
        ++it;  
        if (it != list.end()) {
            const auto& value = *it;
           data.push_back({ key.asString(), make_shared<JsonValue>(value) });
        }
        ++it; 
    }
}

JsonObject::JsonObject(const JsonObject& other) {
    data = other.data;  
}

void JsonObject::add(const string& key, const JsonValue& value) {
    data.push_back({ key, make_shared<JsonValue>(value) });
}

JsonObject& JsonObject::operator=(const JsonObject& other) {
    data = other.data;
    return *this;
}

JsonArray JsonObject::operator,(const JsonValue& jvl)
{
    JsonArray ja;
    ja.add(JsonValue(*this));
    ja.add(jvl);
    return ja;
}

JsonArray JsonObject::operator,(const JsonObject& job)
{
    JsonArray ja;
    ja.add(JsonValue(*this));
    ja.add(JsonValue(job));
    return ja;
}

JsonArray JsonObject::operator,(const JsonArray& jar)
{
    JsonArray ja;
    ja.add(JsonValue(*this));
    ja.add(JsonValue(jar));
    return ja;
}

//-------------------------------------------------------------------------------------------------------------------------------------------//

//JsonArray implemantation
JsonArrayHelper::JsonArrayHelper(JsonArray& ja) : obj(ja) {}


JsonArray::JsonArray() {}

JsonArray::JsonArray(const JsonArray& other) {
    values = other.values;
}

JsonArray::JsonArray( const JsonArrayHelper& jah)
{
    values = jah.obj.values;
}    

void JsonArray::append(const JsonArray& arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        values.push_back(arr.values[i]);
    }
}       

void JsonArray::append(const JsonValue& value) {
    if (value.type == JsonValue::Type::Array) {
        const JsonArray& nestedArray = *value.data.arrayValue;
        for (size_t i = 0; i < nestedArray.size(); ++i) {
            values.push_back(nestedArray.values[i]);
        }
    } else {
        add(value);
    }
}

template <typename DataType> 
void JsonArray::add(const DataType& dt) {
    values.push_back(make_shared<JsonValue>(dt));
}

void JsonArray::add(const JsonArray& arr) {
    values.push_back(make_shared<JsonValue>(arr));
}

void JsonArray::print() const {
    cout << "[ ";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i != 0) cout << ", ";
        values[i]->print();
    }
    cout << " ]";
}

size_t JsonArray::size() const {
    return values.size();
}

JsonValue& JsonArray::operator[](size_t index) {
    auto& valRef = *values[index];
    valRef.parent_value = reinterpret_cast<JsonValue*>(this);     // ‘this’ is a pointer to the JsonValue holding this array
    valRef.parent_index = index;
    valRef.hasparent_index = true;
    return valRef;
}

// Overload operator[] to accept a JsonValue object and return the helper object for chaining
JsonArrayHelper JsonArray::operator[]( JsonArray ja) {
    return JsonArrayHelper(ja);
}    

JsonArray JsonArray::operator[](const JsonObject& obj) {
    JsonArray result;
    result.add(obj);
    return result;
}

template <typename DataType> 
JsonArray JsonArray::operator,(const DataType& dt)
{
    JsonArray ja;
    ja.append(*this);
    ja.add(dt);
    return ja;
}

JsonArray& JsonArray::operator=(const JsonArray& other) {
    values = other.values;
    return *this;
}

template <typename DataType>    
JsonArray& JsonArray::operator+=(const DataType& dt)
{
    add(dt);
    return *this;
}

void JsonArray::removeIndex(size_t index) {
    if (index < values.size()) {
        values.erase(values.begin() + index);
    }
}

JsonArray createArray() {
    return JsonArray();
}

//-------------------------------------------------------------------------------------------------------------------------------------------//

//Handle << overload with all cases
inline ostream& operator<<(ostream& os, const pair<JsonValue, bool>& p) {
    p.first.print();
    os << (p.second ? "True" : "False") << endl;
    return os;
}

inline ostream& operator,(ostream& os, const pair<JsonValue, bool>& p) {
    p.first.print();
    os << (p.second ? "True" : "False") << endl;
    return os;
}

inline pair<JsonValue, bool> operator,(const JsonValue& str, bool b) {
    return make_pair(str, b);
}

ostream& operator,(ostream& os, const JsonValue& str) {
    str.print();
    os << endl;
    return os;
}

ostream& operator,(ostream& os, const char* str) {
    os << str << endl;
    return os;
}

//-------------------------------------------------------------------------------------------------------------------------------------------//

#define JSON(name) ;JsonValue name
#define STRING(value) JsonValue(value)
#define NUMBER(value) JsonValue(static_cast<double>(value))
#define TRUE JsonValue(true)
#define FALSE JsonValue(false)
#define NULL_VALUE JsonValue(nullptr)
#define OBJECT JsonObject
#define ARRAY createArray()

// SET macros for assigning values
#define SET ;
#define ASSIGN =
#define APPEND +=
#define ERASE ; --

// Utility macros
#define SIZE_OF(value) JsonValue(static_cast<double>(value.size()))  // Return NUMBER type
#define IS_EMPTY(value) (value.size() == 0 ? "true" : "false")  // Return proper boolean JSON
#define HAS_KEY(object, key) (object.hasKey(key) ? "true" : "false")  // Return proper boolean JSON
// Update TYPE_OF macro
#define TYPE_OF(value) JsonValue((value).getType())

#define KEY(key) JsonValue(#key), 1 < 0 ? JsonValue(#key) 
#define PRINT ;cout <<

#define PROGRAM_BEGIN ; int main() {;
#define PROGRAM_END ; return 0; }

//-------------------------------------------------------------------------------------------------------------------------------------------//

#endif