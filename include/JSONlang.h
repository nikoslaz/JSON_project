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
    // New fields to track parent and position
    JsonValue* parentValue = nullptr;
    string parentKey;
    size_t parentIndex;
    bool hasParentKey = false;  // true if we're tracking an object key
    bool hasParentIndex = false; // true if we're tracking an array index

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
    JsonValue(size_t value); // Add new constructor for size_t

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

    // Overload -- to erase from parent container
    JsonValue& operator--();

    JsonValue operator+(const JsonValue& other) const;

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

//Class JsonObject
class JsonObject {
    friend class JsonValue;  // Allow JsonValue to access private members
    // Replace map with vector of pairs
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

    bool hasKey(const string& key) const {
        // Scan the vector for matching key
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
        valRef.parentValue = reinterpret_cast<JsonValue*>(this);     // ‘this’ is a pointer to the JsonValue holding this object
        valRef.parentKey = key;
        valRef.hasParentKey = true;
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

    void removeIndex(size_t index);

    // Clears all elements
    void clear() {
        values.clear();
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
    data.stringValue = nullptr;
}

JsonValue::JsonValue(Type t) {
    type = t;
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

JsonValue::JsonValue(size_t value) {
    type = Type::Number;
    data.numberValue = static_cast<double>(value);
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
    // If this JsonValue contains a Type value (from TYPE_OF)
    if (type == Type::String && data.stringValue == nullptr) {
        cout << typeToString(Type::String);
        return;
    }
    else if(type == Type::Object && data.stringValue == nullptr) {
        cout << typeToString(Type::Object);
        return;
    }
    else if(type == Type::Array && data.stringValue == nullptr) {
        cout << typeToString(Type::Array);
        return;
    }
    else if(type == Type::Number && data.stringValue == nullptr) {
        cout << typeToString(Type::Number);
        return;
    }
    else if(type == Type::Boolean && data.stringValue == nullptr) {
        cout << typeToString(Type::Boolean);
        return;
    }
    else if(type == Type::Null && data.stringValue == nullptr) {
        cout << typeToString(Type::Null);
        return;
    }
    
    switch (type) {
        case Type::String: cout << "\"" << *data.stringValue << "\""; break;
        case Type::Number: cout << data.numberValue; break;
        case Type::Boolean: cout << (data.boolValue ? "true" : "false"); break;
        case Type::Null: cout << "null"; break;
        case Type::Object: data.objectValue->print(); break;
        case Type::Array: data.arrayValue->print(); break;
    }
}

bool JsonValue::hasKey(const string& key) const {
    if (type != Type::Object) {
        return false;  // Only objects can have keys
    }
    return data.objectValue->hasKey(key);
}

JsonValue& JsonValue::operator[](const string& key) {
    if (type != Type::Object) {
        type = Type::Object;
        data.objectValue = new JsonObject();
    }
    auto& valRef = (*data.objectValue)[key];
    valRef.parentValue = this;     // ‘this’ is a pointer to the JsonValue holding this object
    valRef.parentKey = key;
    valRef.hasParentKey = true;
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
    valRef.parentValue = this;     // ‘this’ is a pointer to the JsonValue holding this array
    valRef.parentIndex = index;
    valRef.hasParentIndex = true;
    return valRef;
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

JsonValue& JsonValue::operator--() {
    if (parentValue && parentValue->type == Type::Object && hasParentKey) {
        parentValue->data.objectValue->removeKey(parentKey);
    } 
    else if (parentValue && parentValue->type == Type::Array && hasParentIndex) {
        parentValue->data.arrayValue->removeIndex(parentIndex);
    } 
    // If there's NO parent, just clear ourselves if we're an object or array
    else if (!parentValue) {
        if (type == Type::Object) {
            data.objectValue->clear();
        } else if (type == Type::Array) {
            data.arrayValue->clear();
        }
    }
    return *this;
}

JsonValue JsonValue::operator+(const JsonValue& other) const {
    // Handle string concatenation
    if (type == Type::String && other.type == Type::String) {
        return JsonValue(*data.stringValue + *other.data.stringValue);
    }
    
    // Handle array merging
    if (type == Type::Array && other.type == Type::Array) {
        JsonArray result(*data.arrayValue);
        result.append(*other.data.arrayValue);
        return JsonValue(result);
    }
    
    // Handle object merging
    if (type == Type::Object && other.type == Type::Object) {
        JsonObject result(*data.objectValue);
        // Now we can access data directly since JsonValue is a friend
        for (const auto& pair : other.data.objectValue->data) {
            result.add(pair.first, *pair.second);
        }
        return JsonValue(result);
    }
    
    throw runtime_error("Error: operator+ only supports String+String, Array+Array, or Object+Object");
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
    valRef.parentValue = reinterpret_cast<JsonValue*>(this);     // ‘this’ is a pointer to the JsonValue holding this array
    valRef.parentIndex = index;
    valRef.hasParentIndex = true;
    return valRef;
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

void JsonArray::removeIndex(size_t index) {
    if (index < values.size()) {
        values.erase(values.begin() + index);
    }
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
#define ERASE ; --

// Utility macros
#define SIZE_OF(value) JsonValue(value.size())
#define IS_EMPTY(value) value.size() == 0
#define HAS_KEY(object, key) object.hasKey(key)
#define TYPE_OF(value) value.getType()

#define KEY(key) JsonValue(#key), 1 < 0 ? JsonValue(#key) 
#define PRINT(value) ; value.print(); cout << "\n"; cout.flush();

#define PROGRAM_BEGIN ; int main() {;
#define PROGRAM_END ; return 0; }

//-------------------------------------------------------------------------------------------------------------------------------------------//

#endif