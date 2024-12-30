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

#endif