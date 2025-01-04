#include <JSONlang.h>

KeyValue kv;

#define JSON(value) JsonValue value
#define OBJECT JsonValue
#define STRING(x) kv.ret(JsonValue{x})
#define NUMBER(x) kv.ret(JsonValue{x})
#define KEY(x) (kv.create(#x)) ? JsonValue{}
#define TRUE kv.ret(JsonValue{true})
#define FALSE kv.ret(JsonValue{false})
#define NULL kv.ret(JsonValue{nullptr})

int main() {
    // Case 1: String
    JsonValue stringTest = STRING("example");
    std::cout << "String Test: " << std::get<std::string>(stringTest.data) << std::endl;

    // Case 2: Number
    JsonValue intTest = NUMBER(23);
    JsonValue floatTest = NUMBER(3.14);
    std::cout << "Integer Test: " << std::get<double>(intTest.data) << std::endl;
    std::cout << "Float Test: " << std::get<double>(floatTest.data) << std::endl;

    // Case 3: Boolean
    JsonValue trueTest = TRUE;
    JsonValue falseTest = FALSE;
    std::cout << "True Test: " << std::get<bool>(trueTest.data) << std::endl;
    std::cout << "False Test: " << std::get<bool>(falseTest.data) << std::endl;

    // Case 5: Object
    JSON(objectTest) = OBJECT{KEY(title) : STRING("C++11")};
    std::cout << "Object Test:" << std::endl;
    for (const auto& [key, value] : std::get<JsonObject>(objectTest.data)) {
        std::cout << key << ": ";
        if (std::holds_alternative<std::string>(value.data)) {
            std::cout << std::get<std::string>(value.data);
        } else if (std::holds_alternative<double>(value.data)) {
            std::cout << std::get<double>(value.data);
        } else if (std::holds_alternative<bool>(value.data)) {
            std::cout << std::get<bool>(value.data);
        } else if (value.data.index() == 0) {
            std::cout << "null";
        }
        std::cout << std::endl;
    }

    return 0;
}
