#include <iostream>
#include <JSONlang.h>

#define JSON(value) JsonValue value
#define OBJECT  JsonValue(JsonType::OBJECT);
#define ARRAY   JsonValue(JsonType::ARRAY);
#define STRING  JsonValue(JsonType::STRING);
#define NUMBER  JsonValue(JsonType::NUMBER);
#define BOOL    JsonValue(JsonType::BOOL);
#define NULL    JsonValue(JsonType::NULLT);

int main() {
    JSON(emptyObj) = OBJECT{}
    JSON(emptyArray) = ARRAY
}