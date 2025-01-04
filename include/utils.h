#ifndef UTILS_HEADER
#define UTILS_HEADER

#include <string>
#include <JSONlang.h>
#include <map>
#include <vector>

void findBracePairs(const string &s, map<int,int> &bracePairs)
{
    // finds all brace pairs in a JSON string
    vector<int> stack;

    int n = s.length();

    for(int i = 0; i < n; i++)
    {
        if(s[i]=='[' || s[i] == '{')
        {
            stack.push_back(i);
        }
        else if(s[i] == ']' || s[i] == '}')
        {
            bracePairs[stack.back()] = i;
            stack.pop_back();
        }
    }

}

bool isWhitespace(char c){
    return c == ' ' || c == '\n';
}

bool isDouble(const string &s)
{
    int i = 0;
    if(s[0]=='+' || s[0]=='-')
    {
        i++;
    }

    bool dotSeen = 0;

    while(i < s.length())
    {
        if(!isdigit(s[i]) && s[i]!='.')
        {
            return false;
        }

        if(s[i]=='.')
        {
            if(dotSeen)
            {
                return false;
            }
            dotSeen = true;
        }
        i++;
    }

    return true;
}

bool isInteger(const string &s)
{
    int i = 0;
    if(s[0]=='+' || s[0]=='-')
    {
        i++;
    }

    while(i < s.length())
    {
        if(!isdigit(s[i]))
        {
            return false;
        }
        i++;
    }

    return true;
}

JsonValue getValue(const string &s){

    int i = 0; int j = s.length()-1;
    while(isWhitespace(s[i]))
    {
        i++;
    }

    while(isWhitespace(s[j]))
    {
        j--;
    }
    string temp = s.substr(i, j-i + 1);

    if(temp[0]=='"')
    {
        return JsonValue(temp.substr(1, temp.length()-2));
    }

    if(temp == "true" || temp == "false")
    {
        return JsonValue(s=="true");
    }

    if(temp == "null")
    {
        return JsonValue();
    }

    if(isDouble(temp)){
        return JsonValue(stod(temp));
    }

    if(isInteger(temp))
    {
        return JsonValue(stoi(temp));
    }

    return JsonValue(temp);
}


JsonValue parseAssign(const string& lvalue, JsonValue root) {
    JsonValue current = root; // Start from the root and work on a copy
    size_t i = 0;

    while (i < lvalue.size()) {
        // Ignore whitespace
        while (i < lvalue.size() && isWhitespace(lvalue[i])) {
            i++;
        }

        if (i >= lvalue.size()) break;

        // If it's an object key access (e.g., ["key"])
        if (lvalue[i] == '[') {
            i++; // Skip the '['
            if (lvalue[i] == '"') {
                i++; // Skip the '"'
                string key = "";
                while (i < lvalue.size() && lvalue[i] != '"') {
                    key += lvalue[i++];
                }
                if (i >= lvalue.size() || lvalue[i] != '"') {
                    throw runtime_error("Invalid JSON lvalue: missing closing quote for key");
                }
                i++; // Skip the closing '"'
                if (i >= lvalue.size() || lvalue[i] != ']') {
                    throw runtime_error("Invalid JSON lvalue: missing closing ]");
                }
                i++; // Skip the ']'

                // Move to the object
                if (!current.isObject()) {
                    throw runtime_error("Cannot access key '" + key + "' in a non-object value");
                }
                current = current[key]; // Access the key in the object
            } else {
                // If it's an array index access (e.g., [0])
                string indexStr = "";
                while (i < lvalue.size() && lvalue[i] != ']') {
                    indexStr += lvalue[i++];
                }
                if (i >= lvalue.size() || lvalue[i] != ']') {
                    throw runtime_error("Invalid JSON lvalue: missing closing ]");
                }
                i++; // Skip the ']'

                // Move to the array
                if (!current.isArray()) {
                    throw runtime_error("Cannot access index in a non-array value");
                }
                int index = stoi(indexStr);
                current = current[index]; // Access the index in the array
            }
        } else if (lvalue[i] == '.') {
            // Skip the '.' for nested object traversal
            i++;
        } else {
            throw runtime_error("Invalid JSON lvalue format at position " + to_string(i));
        }
    }

    return current; // Return a copy of the resolved node
}

vector<string> splitByTopLevelComma(const string& s) {

    auto trim = [&](const string &s) -> string {
        const char *whitespace = " \t\n\r\f\v";
        size_t start = s.find_first_not_of(whitespace);
        if (start == string::npos) {
            return "";
        }
        size_t end = s.find_last_not_of(whitespace);
        return s.substr(start, end - start + 1);
    };

    vector<string> result;
    int bracketLevel = 0;  // For [ ]
    int braceLevel = 0;    // For { }
    string current;

    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];

        // Update levels for brackets and braces
        if (c == '[') bracketLevel++;
        if (c == ']') bracketLevel--;
        if (c == '{') braceLevel++;
        if (c == '}') braceLevel--;

        // Split when encountering a comma at the top level
        if (c == ',' && bracketLevel == 0 && braceLevel == 0) {
            result.push_back(trim(current));
            current.clear();
        } else {
            current += c;
        }
    }

    // Add the last element, if any
    if (!current.empty()) {
        result.push_back(trim(current));
    }

    return result;
}

#endif