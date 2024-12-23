#include "../include/JSONlang.h"

JsonObject object;
JsonArray  array;

#define JSON(name) JsonValue name

#define STRING(value) value;
#define NUMBER(value) value;
#define KEY(value) #value
#define OBJECT JsonObject{};


int main() {


    return 0;
}
