#include <JSONlang.h>
#include <utils.h>

JsonValue parseObject(const string &s, int start, int end, map<int,int> &bracePair);
JsonValue parseArray(const string &s, int start, int end, map<int,int> &bracePair);

JsonValue parseObject(const string &s, int start, int end, map<int, int> &bracePair) {
    JsonObject ans;
    int i = start + 1; // Skip the opening '{'

    while (i < end) {
        // Skip whitespace
        while (i < end && isWhitespace(s[i])) {
            i++;
        }

        // Ensure we start at a key
        if (i >= end || s[i] != '"') {
            throw runtime_error("Expected a key starting with '\"'");
        }

        // Parse the key
        i++;
        string key;
        while (i < end && s[i] != '"') {
            key += s[i++];
        }
        if (i >= end || s[i] != '"') {
            throw runtime_error("Expected closing '\"' for key");
        }
        i++; // Skip closing '"'

        // Skip whitespace and the ':' separator
        while (i < end && (isWhitespace(s[i]) || s[i] == ':')) {
            i++;
        }

        // Parse the value
        if (i >= end) {
            throw runtime_error("Expected a value for key: " + key);
        }

        if (s[i] == '{') {
            // Nested object
            ans[key] = parseObject(s, i, bracePair[i], bracePair);
            i = bracePair[i] + 1;
        } else if (s[i] == '[') {
            // Nested array
            ans[key] = parseArray(s, i, bracePair[i], bracePair);
            i = bracePair[i] + 1;
        } else {
            // Primitive value
            string value;
            while (i < end && s[i] != ',' && s[i] != '}') {
                value += s[i++];
            }
            ans[key] = getValue(value);
        }

        // Skip whitespace and the ',' separator
        while (i < end && (isWhitespace(s[i]) || s[i] == ',')) {
            i++;
        }
    }

    return ans;
}


JsonValue parseArray(const string &s, int start, int end, map<int, int> &bracePair) {
    JsonArray ans;
    int i = start + 1; // Skip the opening '['

    while (i < end) {
        // Skip whitespace
        while (i < end && isWhitespace(s[i])) {
            i++;
        }

        if (i >= end) break;

        if (s[i] == '{') {
            // Nested object
            ans.push_back(parseObject(s, i, bracePair[i], bracePair));
            i = bracePair[i] + 1;
        } else if (s[i] == '[') {
            // Nested array
            ans.push_back(parseArray(s, i, bracePair[i], bracePair));
            i = bracePair[i] + 1;
        } else {
            // Primitive value
            string value;
            while (i < end && s[i] != ',' && s[i] != ']') {
                value += s[i++];
            }
            ans.push_back(getValue(value));
        }

        // Skip whitespace and the ',' separator
        while (i < end && (isWhitespace(s[i]) || s[i] == ',')) {
            i++;
        }
    }

    return ans;
}


JsonValue JsonValue::parse(const string &s)
{
    map<int,int> bracePairs;
    findBracePairs(s, bracePairs);

    int i = 0;

    while(isWhitespace(s[i]))
    {
        i++;
    }

    if(s[i]=='[')
    {
        return parseArray(s, i, bracePairs[i], bracePairs);
    }

    return parseObject(s, i, bracePairs[i], bracePairs);
}


string JsonValue::parseInput(const string& input) 
{
    // We'll define several lambda helpers **inside** this function.
    // In normal code, you might split these out, but here it's "all in one".

    // 1) Trim leading/trailing whitespace.
    auto trim = [&](const string &s) -> string {
        const char *whitespace = " \t\n\r\f\v";
        size_t start = s.find_first_not_of(whitespace);
        if (start == string::npos) {
            return "";
        }
        size_t end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    };

    // 2) Split a string by top-level commas (commas not within braces/brackets).
    //    This helps parse OBJECT{} or ARRAY[] contents.
    auto splitByTopLevelComma = [&](const string &s) -> vector<string> {
        vector<string> result;
        int bracketLevel = 0;  // for [ ]
        int braceLevel   = 0;  // for { }
        string current;

        for (size_t i = 0; i < s.size(); i++) {
            char c = s[i];
            if (c == '[') bracketLevel++;
            if (c == ']') bracketLevel--;
            if (c == '{') braceLevel++;
            if (c == '}') braceLevel--;

            // If we encounter a comma at "top level", we split.
            if (c == ',' && bracketLevel == 0 && braceLevel == 0) {
                result.push_back(trim(current));
                current.clear();
            } else {
                current.push_back(c);
            }
        }

        if (!current.empty()) {
            result.push_back(trim(current));
        }
        return result;
    };

    // 3) We'll use function so that our lambdas can refer to each other recursively.
    function<string(const string&)> parseValue;
    function<string(const string&)> parseObject;
    function<string(const string&)> parseArray;

    // 4) parseKey: handle KEY(something). 
    //    Returns just the string inside KEY(...).
    auto parseKey = [&](const string &inp) -> string {
        // Example pattern: KEY("title") or KEY(title)
        // We'll use a normal (escaped) pattern: ^KEY\("?([^"]+)"?\)$
        static const regex keyPattern("^KEY\\(\"?([^\\\"]+)\"?\\)$");
        smatch match;
        if (regex_match(inp, match, keyPattern)) {
            // match[1] is the part inside KEY(...)
            return match[1].str();
        }
        return "UNKNOWN_KEY";
    };

    // 5) parseObject: handle OBJECT{ ... }.
    //    We assume the format: OBJECT{ KEY(...): something, KEY(...): something, ... }
    parseObject = [&](const string &inp) -> string {
        const string prefix = "OBJECT{";
        const string suffix = "}";
        if (inp.size() < prefix.size() + suffix.size()) {
            return "{}";
        }
        // Extract the stuff inside OBJECT{...}
        string inner = inp.substr(prefix.size(), inp.size() - prefix.size() - suffix.size());
        inner = trim(inner);

        if (inner.empty()) {
            return "{}";  // empty object
        }

        // Split by top-level commas to get key-value pairs
        vector<string> pairs = splitByTopLevelComma(inner);

        // Build JSON-like output
        string result = "{";
        bool first = true;

        for (auto &p : pairs) {
            // We expect something like: KEY(title): STRING("C++11")
            // So we need to find the top-level colon:
            int bracketLevel = 0;
            int braceLevel   = 0;
            size_t colonPos  = string::npos;
            for (size_t i = 0; i < p.size(); i++) {
                if (p[i] == '[') bracketLevel++;
                if (p[i] == ']') bracketLevel--;
                if (p[i] == '{') braceLevel++;
                if (p[i] == '}') braceLevel--;
                if (p[i] == ':' && bracketLevel == 0 && braceLevel == 0) {
                    colonPos = i;
                    break;
                }
            }
            if (colonPos == string::npos) {
                continue; // skip or handle error
            }

            string left  = trim(p.substr(0, colonPos));   
            string right = trim(p.substr(colonPos + 1));  

            // left is KEY(...)
            string key = parseKey(left);
            // right is a value (STRING(...), NUMBER(...), ARRAY[...], OBJECT{...}, etc.)
            string val = parseValue(right);

            if (!first) {
                result += ", ";
            }
            first = false;
            // For JSON: "key": value
            result += "\"" + key + "\": " + val;
        }

        result += "}";
        return result;
    };

    // 6) parseArray: handle ARRAY[ ... ].
    parseArray = [&](const string &inp) -> string {
        const string prefix = "ARRAY[";
        const string suffix = "]";
        if (inp.size() < prefix.size() + suffix.size()) {
            return "[]";
        }
        // Extract the stuff inside ARRAY[...]
        string inner = inp.substr(prefix.size(), inp.size() - prefix.size() - suffix.size());
        inner = trim(inner);
        if (inner.empty()) {
            return "[]";  // empty array
        }

        // Split by top-level commas to get array elements
        vector<string> elements = splitByTopLevelComma(inner);

        // Build JSON-like output
        string result = "[";
        bool first = true;
        for (auto &elem : elements) {
            if (!first) {
                result += ", ";
            }
            first = false;
            result += parseValue(elem);
        }
        result += "]";
        return result;
    };

    // 7) parseValue: the central “dispatcher”.
    parseValue = [&](const string &raw) -> string {
        string val = trim(raw);

        // Regex for basic types
        static const regex stringPattern("^STRING\\(\"([^\"]*)\"\\)$");
        static const regex numberPattern("^NUMBER\\(([^()]*)\\)$");
        static const regex truePattern("^TRUE$");
        static const regex falsePattern("^FALSE$");
        static const regex nullPattern("^NULL$");
        static const regex setAssignPattern(R"(^SET\s+(.+)\s+ASSIGN\s+(.+)$)");
        static const regex setAppendPattern(R"(^SET\s+(.+)\s+APPEND\s+(.+)$)");

        smatch match;
        // STRING("...")
        if (regex_match(val, match, stringPattern)) {
            return "\"" + match[1].str() + "\"";
        }
        // NUMBER(...)
        else if (regex_match(val, match, numberPattern)) {
            return match[1].str(); // no quotes
        }
        // TRUE
        else if (regex_match(val, match, truePattern)) {
            return "true";
        }
        // FALSE
        else if (regex_match(val, match, falsePattern)) {
            return "false";
        }
        // NULL
        else if (regex_match(val, match, nullPattern)) {
            return "null";
        }
        // OBJECT{...}
        else if (val.rfind("OBJECT{", 0) == 0 && !val.empty() && val.back() == '}') {
            return parseObject(val);
        }
        // ARRAY[...]
        else if (val.rfind("ARRAY[", 0) == 0 && !val.empty() && val.back() == ']') {
            return parseArray(val);
        }
        // SET ... ASSIGN ...
        else if (regex_match(val, match, setAssignPattern)) {
            return "S";
        }
        // SET ... APPEND ...
        else if (regex_match(val, match, setAppendPattern)) {
            return "A";
        }


        // If none match, return "UNKNOWN_TYPE"
        return "UNKNOWN_TYPE";
    };

    // Finally, we just call parseValue on the user's `input`.
    return parseValue(input);
}

string JsonValue::executeSet(JsonValue& root, const string& command) {
    // Patterns for ASSIGN and APPEND operations
    static const regex setAssignPattern(R"(^SET\s+(.+)\s+ASSIGN\s+(.+)$)");
    static const regex setAppendPattern(R"(^SET\s+(.+)\s+APPEND\s+(.+)$)");
    smatch match;

    if (regex_match(command, match, setAssignPattern)) {
        // Extract the target and value
        string target = match[1].str();
        string valueStr = match[2].str();

        // Resolve the target node as a reference
        JsonValue targetNode = parseAssign(target, root);
        JsonValue newValue = JsonValue::parse(valueStr);

        // Assign the new value
        targetNode = newValue;
        return "ASSIGN_OK";
    } 
    else if (regex_match(command, match, setAppendPattern)) {
        // Extract the target and values
        string target = match[1].str();
        string valuesStr = match[2].str();

        // Resolve the target node as a reference
        JsonValue targetNode = parseAssign(target, root);
        if (!targetNode.isArray()) {
            throw runtime_error("APPEND operation is only valid for arrays");
        }

        // Append each value to the array
        JsonArray targetArray = get<JsonArray>(targetNode.getData());
        vector<string> valueParts = splitByTopLevelComma(valuesStr);

        for (const auto& valuePart : valueParts) {
            JsonValue newValue = JsonValue::parse(valuePart);
            targetArray.push_back(newValue);
        }
        return "APPEND_OK";
    }

    throw runtime_error("Invalid SET command");
}

void JsonValue::printJsonObject(const JsonObject& obj, int indent) {
    for (const auto& [key, value] : obj) {
        std::cout << std::string(indent, ' ') << key << ": ";
        printJsonValue(value, indent + 2);
    }
}

void JsonValue::printJsonValue(const JsonValue& value, int indent) {
    if (std::holds_alternative<std::string>(value.data)) {
        std::cout << std::get<std::string>(value.data) << std::endl;
    } else if (std::holds_alternative<double>(value.data)) {
        std::cout << std::get<double>(value.data) << std::endl;
    } else if (std::holds_alternative<bool>(value.data)) {
        std::cout << (std::get<bool>(value.data) ? "true" : "false") << std::endl;
    } else if (std::holds_alternative<JsonObject>(value.data)) {
        std::cout << "{" << std::endl;
        printJsonObject(std::get<JsonObject>(value.data), indent);
        std::cout << std::string(indent - 2, ' ') << "}" << std::endl;
    } else if (std::holds_alternative<JsonArray>(value.data)) {
        std::cout << "[" << std::endl;
        for (const auto& item : std::get<JsonArray>(value.data)) {
            std::cout << std::string(indent, ' ');
            printJsonValue(item, indent + 2);
        }
        std::cout << std::string(indent - 2, ' ') << "]" << std::endl;
    } else {
        std::cout << "null" << std::endl;
    }
}