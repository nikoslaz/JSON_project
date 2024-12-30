#include <JSONlang.h>
#include <utils.h>

JsonValue parseObject(const string &s, int start, int end, map<int,int> &bracePair);
JsonValue parseArray(const string &s, int start, int end, map<int,int> &bracePair);

JsonValue parseObject(const string &s, int start, int end, map<int,int> &bracePair){
    /**
     * s: the json string
     * start: the index of the opening curly brace for this object
     * end: the index of the closing curly brace for this object
     * bracePair: an map that maps index of opening brackets to their corresponding closing brackets.
     */

    int i = start;
    JsonObject ans;

    while(i < end)
    {
        // ignore all characters before the first quote for the key string
        while(s[i] != '"')
        {
            i++;
        }
        i++;

        // add all characters between the quotes to the key
        string key = "";

        while(s[i]!='"')
        {
            key+=s[i];
            i++;
        }

        // find the semicolon between the key and value
        while(s[i]!=':')
        {
            i++;
        }
        i++;

        // ignore all whitespaces till you get to the start of the value
        while(isWhitespace(s[i]))
        {
            i++;
        }
        string value = "";
        
        // if value starts with a curly brace, it means its an object. 
        // parse that object.
        if(s[i]=='{')
        {
            ans[key] = parseObject(s, i, bracePair[i], bracePair);
            i = bracePair[i]+1;
            continue;
        }
        // if value starts with a square bracket, it is an array
        // parse the array
        else if(s[i]=='[')
        {
             ans[key] = parseArray(s, i, bracePair[i], bracePair);
            i = bracePair[i]+1;
            continue;
        }
        // it is a JSON Value. Parse it and store it. 
        // stop when you get to a comma or reach end of this JSON.
        while(i < end && s[i] !=',')
        {
            value += s[i];
            i++;
        }
        

        ans[key] = getValue(value);
    }

    return ans;
}

JsonValue parseArray(const string &s, int start, int end, map<int,int> &bracePair){
    /**
     * s: the json string
     * start: the index of the opening square bracket for this object
     * end: the index of the closing square bracket for this object
     * bracePair: an map that maps index of opening brackets to their corresponding closing brackets.
     */
    int i = start;
    JsonArray ans;
    i++;
    while(i < end)
    {
        //ignore all whitespace
        while(isWhitespace(s[i]))
        {
            i++;
        }
        // find a value
        string value = "";

        //stop when you see a comma or reach end of the array.
        while(i < end && s[i]!=',')
        {
            value += s[i];
            i++;
        }
        i++;
        ans.push_back(getValue(value));
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

// The one function you can call with any "custom" string. 
// It returns a JSON-like string or "UNKNOWN_TYPE" if it doesn't recognize the format.
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

        // If none match, return "UNKNOWN_TYPE"
        return "UNKNOWN_TYPE";
    };

    // Finally, we just call parseValue on the user's `input`.
    return parseValue(input);
}