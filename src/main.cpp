#include "../include/JSONlang.h"

using namespace std;

#define JSON(name) JsonValue name
#define STRING(value) string(value)
#define NUMBER(value) double(value)
#define TRUE true
#define FALSE false
#define NULLVALUE nullptr
#define OBJECT JsonObject{};
#define ARRAY JsonArray
#define KEY(key) key

int main() {
    
    JSON(emptyObj) = OBJECT{}


    return 0;
}
