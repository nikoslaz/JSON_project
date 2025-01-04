#include <JSONlang.h>

KeyValue kv{};
JsonValue val;

#define JSON(value) JsonValue value
#define OBJECT JsonValue
#define ARRAY JsonValue 
#define VALUE val
#define STRING(x) kv.ret(VALUE = x)
#define NUMBER(x) kv.ret(VALUE = x)
#define TRUE      kv.ret(VALUE = true)
#define FALSE     kv.ret(VALUE = false)
#define NULL      kv.ret(JsonValue{nullptr})
#define KEY(x) (kv.create(#x)) ? JsonValue{}

int main() {

    JSON(book) = OBJECT {
    KEY(title) : STRING("Gone Girl"),
    KEY(published) : NUMBER(2012), 
    KEY(type) : STRING("Thriller"), 
    KEY(author) : OBJECT{
            KEY(firstname) : STRING("GILLIAN"), 
            KEY(sirname) : STRING("FLYNN"), 
            KEY(age) : NUMBER(45)
        }
    };

     // Print the JSON object
    std::cout << "JSON(book):" << std::endl;
    for (const auto& [key, value] : std::get<JsonObject>(book.data)) {
        std::cout << key << ": ";
        if (std::holds_alternative<std::string>(value.data)) {
            std::cout << std::get<std::string>(value.data);
        } else if (std::holds_alternative<double>(value.data)) {
            std::cout << std::get<double>(value.data);
        } else if (std::holds_alternative<bool>(value.data)) {
            std::cout << std::get<bool>(value.data);
        } else if (std::holds_alternative<JsonObject>(value.data)) {
            std::cout << "{" << std::endl;
            for (const auto& [subKey, subValue] : std::get<JsonObject>(value.data)) {
                std::cout << "  " << subKey << ": ";
                if (std::holds_alternative<std::string>(subValue.data)) {
                    std::cout << std::get<std::string>(subValue.data);
                } else if (std::holds_alternative<double>(subValue.data)) {
                    std::cout << std::get<double>(subValue.data);
                } else if (std::holds_alternative<bool>(subValue.data)) {
                    std::cout << std::get<bool>(subValue.data);
                } else if (subValue.data.index() == 0) {
                    std::cout << "null";
                }
                std::cout << std::endl;
            }
            std::cout << "}";
        } else if (value.data.index() == 0) {
            std::cout << "null";
        }
        std::cout << std::endl;
    }

    return 0;
}