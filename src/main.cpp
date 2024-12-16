#include "../include/JSONlang.h"

using namespace std;

int main() {
    // Define JSON variables
    JSON(book) = OBJECT{
        {KEY("title"), STRING("Gone Girl")},
        {KEY("published"), NUMBER(2012)},
        {KEY("type"), STRING("Thriller")},
        {KEY("author"), OBJECT{
            {KEY("firstname"), STRING("Gillian")},
            {KEY("sirname"), STRING("Flynn")},
            {KEY("age"), NUMBER(45)}
        }}
    };
 
    
    return 0;
}
