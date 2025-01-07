#ifndef JSON_LANG_H
#define JSON_LANG_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>

using namespace std;

class JsonArrayHelper;

//Class JsonValue
class JsonValue {
public:
    enum class Type {String = 0, Number = 1, Boolean = 2, Null = 3, Object = 4, Array = 5} type;

    union {
        string* stringValue;
        double numberValue;
        bool boolValue;
        class JsonObject* objectValue;
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

    //Destructor
    ~JsonValue();

    //Methods
    void print() const;
    size_t size() const;
    string asString() const;

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
};

//Class JsonObject
class JsonObject {
    map<string, shared_ptr<JsonValue>> data;

public:
    JsonObject();
    JsonObject(const JsonObject& other);
    JsonObject(initializer_list<JsonValue> list);

    void add(const string& key, const JsonValue& value);
    void print() const;
    size_t size() const;
    bool hasKey(const string& key) const;

    JsonValue& operator[](const string& key);
    JsonObject& operator=(const JsonObject& other);

    JsonArray operator,(const JsonValue& jv);
    JsonArray operator,(const JsonArray& ja);
    JsonArray operator,(const JsonObject& jo);
};

//Class JsonArray
class JsonArray {
    vector<shared_ptr<JsonValue>> values;

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

    void print() const;
    size_t size() const;
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
    data.stringValue = nullptr;
}

JsonValue::JsonValue(const string& value) {
    type = Type::String;
    data.stringValue = new string(value);
}

JsonValue::JsonValue(const char* value) {
    type = Type::String;
    data.stringValue = new string(value);
}

JsonValue::JsonValue(double value) {
    type = Type::Number;
    data.numberValue = value;
}

JsonValue::JsonValue(bool value) {
    type = Type::Boolean;
    data.boolValue = value;
}

JsonValue::JsonValue(nullptr_t) {
    type = Type::Null;
    data.stringValue = nullptr;
}

JsonValue::JsonValue(const JsonObject& obj) {
    type = Type::Object;
    data.objectValue = new JsonObject(obj);
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

JsonValue::JsonValue(const JsonValue& other) : type(other.type) {
    switch (type) {
        case Type::String:
            data.stringValue = new string(*other.data.stringValue);
            break;
        case Type::Number:
            data.numberValue = other.data.numberValue;
            break;
        case Type::Boolean:
            data.boolValue = other.data.boolValue;
            break;
        case Type::Object:
            data.objectValue = new JsonObject(*other.data.objectValue);
            break;
        case Type::Array:
            data.arrayValue = new JsonArray(*other.data.arrayValue);
            break;
        case Type::Null:
            data.stringValue = nullptr;
            break;
    }
}

// Destructor for JsonValue
JsonValue::~JsonValue() {
    switch (type) {
        case Type::String:
            if (data.stringValue) {
                delete data.stringValue;
                data.stringValue = nullptr;  // Set to nullptr after deletion
            }
            break;
        case Type::Object:
            if (data.objectValue) {
                delete data.objectValue;
                data.objectValue = nullptr;  // Set to nullptr after deletion
            }
            break;
        case Type::Array:
            if (data.arrayValue) {
                delete data.arrayValue;
                data.arrayValue = nullptr;  // Set to nullptr after deletion
            }
            break;
        default:
            break;
    }
    
    type = Type::Null;
}

string JsonValue::asString() const {
    if (type == Type::String) {
        return *data.stringValue;
    }
    return "";
}

size_t JsonValue::size() const {
    switch (type) {
        case Type::Object: return data.objectValue->size();
        case Type::Array: return data.arrayValue->size();
        default: return 1;
    }
}

void JsonValue::print() const {
    switch (type) {
        case Type::String: cout << "\"" << *data.stringValue << "\""; break;
        case Type::Number: cout << data.numberValue; break;
        case Type::Boolean: cout << (data.boolValue ? "true" : "false"); break;
        case Type::Null: cout << "null"; break;
        case Type::Object: data.objectValue->print(); break;
        case Type::Array: data.arrayValue->print(); break;
    }
}

JsonValue& JsonValue::operator[](const string& key) {
    if (type != Type::Object) {
        type = Type::Object;
        data.objectValue = new JsonObject();
    }
    return (*data.objectValue)[key];
}

JsonValue& JsonValue::operator[](size_t index) {
    if (type != Type::Array) {
        type = Type::Array;
        data.arrayValue = new JsonArray();
    }
    return (*data.arrayValue)[index];
}

// Overloading the assignment operator
JsonValue& JsonValue::operator=(const JsonValue& other) {
    cout << "~\n";
    if (this == &other) {
        return *this; // Handle self-assignment
    }

    cout << (int) type;
    cout.flush();
    // First, clean up the current object
    JsonValue::~JsonValue();

    // Copy the data from `other`
    type = other.type;
    switch (type) {
        case Type::String:
            data.stringValue = new string(*other.data.stringValue);
            break;
        case Type::Number:
            data.numberValue = other.data.numberValue;
            break;
        case Type::Boolean:
            data.boolValue = other.data.boolValue;
            break;
        case Type::Object:
            data.objectValue = new JsonObject(*other.data.objectValue);
            break;
        case Type::Array:
            data.arrayValue = new JsonArray(*other.data.arrayValue);
            break;
        case Type::Null:
            data.stringValue = nullptr;
            break;
    }

    return *this;
}

// Overloading the assignment operator
JsonValue& JsonValue::operator=(const JsonObject& other) {

    // First, clean up the current object
    JsonValue::~JsonValue();

    // Copy the data from `other`
    type = Type::Object;
    data.objectValue = new JsonObject(other);

    return *this;
}

// Overloading the assignment operator
JsonValue& JsonValue::operator=(const JsonArray& other) {

    // First, clean up the current object
    JsonValue::~JsonValue();

    // Copy the data from `other`
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
           data[key.asString()] = make_shared<JsonValue>(value);
        }
        ++it; 
    }
}

JsonObject::JsonObject(const JsonObject& other) {
    data = other.data;  
}

void JsonObject::add(const string& key, const JsonValue& value) {
    data[key] = make_shared<JsonValue>(value);
}

void JsonObject::print() const {
    cout << "{ ";
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (it != data.begin()) cout << ", ";
        cout << "\"" << it->first << "\": ";
        it->second->print();
    }
    cout << " }";
}

size_t JsonObject::size() const {
    return data.size();
}

bool JsonObject::hasKey(const string& key) const {
    return data.find(key) != data.end();
}

JsonValue& JsonObject::operator[](const string& key) {
    return *data[key];
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
    return *values[index];
}

// Overload operator[] to accept a JsonValue object and return the helper object for chaining
JsonArrayHelper JsonArray::operator[]( JsonArray ja) {
    return JsonArrayHelper(ja);
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

JsonArray createArray() {
    return JsonArray();
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
#define ERASE(variable) ;variable = JsonValue(nullptr)

// Utility macros
#define SIZE_OF(value) value.size()
#define IS_EMPTY(value) value.size() == 0
#define HAS_KEY(object, key) object.hasKey(key)
#define TYPE_OF(value) value.getType()

#define KEY(key) JsonValue(#key), 1 < 0 ? JsonValue(#key) 
#define PRINT(value) ; value.print(); cout << "\n"; cout.flush();

#define PROGRAM_BEGIN ; int main() {;
#define PROGRAM_END ; return 0; }

//-------------------------------------------------------------------------------------------------------------------------------------------//

#endif