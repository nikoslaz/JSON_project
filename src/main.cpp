#include "JSONlang.h"

//define emptyObj json with empty object
JSON(emptyObj) = OBJECT{}

//define emptyArray json with empty array
JSON(emptyArray) = ARRAY

//define book json with an object containing data for a book
JSON(book) = OBJECT {
    KEY(title) : STRING("Gone Girl"),
    KEY(published) : NUMBER(2012),
    KEY(type) : STRING("Thriller"),
    KEY(author) : OBJECT {
        KEY(firstname) : STRING("Gillian"),
        KEY(sirname) : STRING("Flynn"),
        KEY(age) : NUMBER(45)
    }
}

//define week_temperatures json with an array of numbers
JSON(week_temperatures) = ARRAY [
    NUMBER(20), NUMBER(19.5), NUMBER(19), NUMBER(20),
    NUMBER(19), NUMBER(18.5), NUMBER(19)
];

//define students json with an array of objects representing students
JSON(students) = ARRAY [
    OBJECT {
        KEY(name) : STRING("Nikos Nikolaou"),
        KEY(id) : NUMBER(4444),
        KEY(grades) : ARRAY [
            OBJECT { KEY(hy100) : NUMBER(7.5) },
            OBJECT { KEY(hy150) : NUMBER(8.0) }
        ]
    },
    OBJECT {
        KEY(name) : STRING("Giannis Zaroliagkis"),
        KEY(id) : NUMBER(5555),
        KEY(grades) : ARRAY [
            OBJECT { KEY(hy100) : NUMBER(10) },
            OBJECT { KEY(hy150) : NUMBER(9.5) }
        ]
    },
    OBJECT {
        KEY(name) : STRING("Nikos Lazaridis"),
        KEY(id) : NUMBER(6666),
        KEY(grades) : ARRAY [
            OBJECT { KEY(hy100) : NUMBER(9.5) },
            OBJECT { KEY(hy150) : NUMBER(10) }
        ]
    },
    OBJECT {
        KEY(name) : STRING("Maria Papadopoulou"),
        KEY(id) : NUMBER(7777),
        KEY(grades) : ARRAY [
            OBJECT { KEY(hy100) : NUMBER(9.0) },
            OBJECT { KEY(hy150) : NUMBER(8.5) }
        ]
    }
]

PROGRAM_BEGIN

//Print emptyObj
PRINT("Empty object: ")
PRINT(emptyObj)
PRINT("\n")

//Print emptyArray
PRINT("Empty array: ")
PRINT(emptyArray)
PRINT("\n")

//Print book
PRINT("Book: ")
PRINT(book)
PRINT("\n")

//Print week temperatures
PRINT("Week temperatures: ")
PRINT(week_temperatures)
PRINT("\n")

//Print students
PRINT("Students: ")
PRINT(students)
PRINT("\n")

//1) SET ASSIGN
//change 3rd day temperature from 19 to 22
SET week_temperatures[2] ASSIGN NUMBER(22)
PRINT("Week temperatures after ASSIGN: ")
PRINT(week_temperatures)
PRINT("\n")

//add email address for 1st student
SET students[0]["email"] ASSIGN STRING("csd444@csd.uoc.gr")
PRINT("Students after ASSIGN: ")
PRINT(students)
PRINT("\n")

//assign new object in emptyObj json
SET emptyObj ASSIGN OBJECT { KEY(a) : STRING("alpha") }
PRINT("Empty object after ASSIGN: ")
PRINT(emptyObj)
PRINT("\n")

//2) SET APPEND
//appends values 23, 22, 20 to the end of the temperature array
SET week_temperatures APPEND NUMBER(23), NUMBER(22), NUMBER(20)
PRINT("Week temperatures after APPEND: ")
PRINT(week_temperatures)
PRINT("\n")

//appends a grade for course hy255
SET students[0]["grades"] APPEND OBJECT { KEY(hy255) : NUMBER(9) }
PRINT("Students after APPEND: ")
PRINT(students)
PRINT("\n")

//3) ERASE
ERASE book["author"]["age"] //removes age from author object of book
PRINT("Book after first ERASE: ")
PRINT(book)
PRINT("\n")

ERASE book["type"] //removes type of book
PRINT("Book after second ERASE: ")
PRINT(book)
PRINT("\n")

ERASE book //removes all book data, now book is an empty object
PRINT("Book after third ERASE: ")
PRINT(book)
PRINT("\n")

//4) Arithmetic operators
PRINT("Arithmetic operators:\n")
JSON(plus) = NUMBER(10) + NUMBER(5) //15
PRINT("NUMBER(10) + NUMBER(5) = ")
PRINT(plus)

JSON(sub) = NUMBER(10) - NUMBER(5)  //5
PRINT("NUMBER(10) - NUMBER(5) = ")
PRINT(sub)

JSON(mul) = NUMBER(10) * NUMBER(5)  //50
PRINT("NUMBER(10) * NUMBER(5) = ")
PRINT(mul)

JSON(div) = NUMBER(10) / NUMBER(5)  //2
PRINT("NUMBER(10) / NUMBER(5) = ")
PRINT(div)

JSON(mod) = NUMBER(10) % NUMBER(3)  //1
PRINT("NUMBER(10) % NUMBER(5) = ")
PRINT(mod)
PRINT("\n")

//5) Special case of +
JSON(conc) = STRING("hello") + STRING(" world")
PRINT("Special case of + (1): ")
PRINT(conc)
PRINT("\n")

JSON(fullName) = STRING("Nikos ") + STRING("Nikolaou")
JSON(studentMessage) = STRING("Student: ")
JSON(studentFullName) = studentMessage + students[0]["name"] + STRING(", ID: ") + STRING("4444")
PRINT(studentFullName)
PRINT("\n")

//7) Array concatenation example
JSON(arr1) = ARRAY[NUMBER(1), NUMBER(2)]
JSON(arr2) = ARRAY[NUMBER(3), NUMBER(4)]
JSON(combinedArray) = arr1 + arr2
PRINT("Array concatenation example: ")
PRINT(combinedArray)  //[ 1, 2, 3, 4 ]
PRINT("\n")

//8) Object concatenation example
JSON(obj1) = OBJECT {
    KEY(name) : STRING("John"),
    KEY(age) : NUMBER(30)
}
JSON(obj2) = OBJECT {
    KEY(city) : STRING("New York"),
    KEY(country) : STRING("USA")
}
JSON(combinedObject) = obj1 + obj2
PRINT("Object concatenation example: ")
PRINT(combinedObject)  //{ "name": "John", "age": 30, "city": "New York", "country": "USA" }
PRINT("\n")

JSON(hy352_nik) = OBJECT{ KEY(exam):NUMBER(7), KEY(project):NUMBER(8) }
JSON(students) = ARRAY [ 
    OBJECT{
        KEY(name) : STRING("Nikos ") + STRING("Nikolaou"),
        KEY(id) : NUMBER(4444),
        KEY(grades) : ARRAY[ 
            OBJECT {
                KEY(hy352):
                hy352_nik["exam"] * NUMBER(0.75) + hy352_nik["project"] * NUMBER(0.25)  
            }
        ]
    }
]
PRINT("Students after arithmetic operators: ")
PRINT(students)
PRINT("\n")

//5) Locical operators
JSON(logh) = STRING("hello") == STRING("hello") // true
PRINT("Comparasion 'hello' == 'hello' = ")
PRINT(logh)
PRINT("\n")

JSON(logn) = NUMBER(42) != NUMBER(43)  // true
PRINT("Comparasion '42' != '43' = ")
PRINT(logn)
PRINT("\n")

JSON(loga) = ARRAY[NUMBER(1), NUMBER(2)] == ARRAY[NUMBER(1), NUMBER(2)]  // true
PRINT("Comparasion ARRAY[NUMBER(1), NUMBER(2)] == ARRAY[NUMBER(1), NUMBER(2)] = ")
PRINT(loga)
PRINT("\n")

JSON(logo) = OBJECT{KEY(a): NUMBER(1)} == OBJECT{KEY(a): NUMBER(1)}  // true
PRINT("OBJECT{KEY(a): NUMBER(1)} == OBJECT{KEY(a): NUMBER(1)} = ")
PRINT(logo)
PRINT("\n")

JSON(and_result) = (NUMBER(5) > NUMBER(3)) && (NUMBER(10) < NUMBER(20))  // true
PRINT("Logical AND example (5>3 && 10<20) = ")
PRINT(and_result)
PRINT("\n")

JSON(or_result) = (NUMBER(5) < NUMBER(3)) || (NUMBER(10) < NUMBER(20))  // true
PRINT("Logical OR example (5<3 || 10<20) = ")
PRINT(or_result)
PRINT("\n")

JSON(not_result) = !(NUMBER(5) < NUMBER(3))  // true
PRINT("Logical NOT example !(5<3) = ")
PRINT(not_result)
PRINT("\n")

JSON(complex_logic) = ((NUMBER(5) > NUMBER(3)) && (NUMBER(10) < NUMBER(20))) || !(NUMBER(7) == NUMBER(8))  // true
PRINT("Complex logical expression ((5>3 && 10<20) || !(7==8)) = ")
PRINT(complex_logic)
PRINT("\n")

// 6) Utility functions examples
// SIZE_OF examples
JSON(arraySize) = SIZE_OF(week_temperatures)
PRINT("Size of week_temperatures array: ")
PRINT(arraySize)
PRINT("\n")

JSON(objectSize) = SIZE_OF(book)
PRINT("Size of book object: ")
PRINT(objectSize)
PRINT("\n")

// IS_EMPTY examples
JSON(isEmptyArray) = IS_EMPTY(emptyArray)
PRINT("Is emptyArray empty? ")
PRINT(isEmptyArray)
PRINT("\n")

JSON(isEmptyObject) = IS_EMPTY(emptyObj)
PRINT("Is emptyObj empty? ")
PRINT(isEmptyObject)
PRINT("\n")

// HAS_KEY examples
JSON(hasTitle) = HAS_KEY(book, "title")
PRINT("Does book have 'title' key? ")
PRINT(hasTitle)
PRINT("\n")

JSON(hasAuthor) = HAS_KEY(book, "author")
PRINT("Does book have 'author' key? ")
PRINT(hasAuthor)
PRINT("\n")

// TYPE_OF examples
JSON(tempType) = TYPE_OF(week_temperatures)
PRINT("Type of week_temperatures: ")
PRINT(tempType)
PRINT("\n")

JSON(bookType) = TYPE_OF(book)
PRINT("Type of book: ")
PRINT(bookType)
PRINT("\n")

JSON(nameType) = TYPE_OF(students[0]["name"])
PRINT("Type of student name: ")
PRINT(nameType)
PRINT("\n")

//6) Print example
JSON(book2) = OBJECT {
    KEY(title) : STRING("Gone Girl"),
    KEY(published) : NUMBER(2012),
    KEY(type) : STRING("Thriller"),
    KEY(author) : OBJECT {
        KEY(firstname) : STRING("Gillian"),
        KEY(sirname) : STRING("Flynn"),
        KEY(age) : NUMBER(45)
    }
}

PRINT book2["title"] //prints:Gone Girl
PRINT("\n")
PRINT book2["author"] //prints:{firstname:”Gillian”,sirname:“Flynn”,age: 45}
PRINT("\n")
PRINT book2 //prints: the whole json for book
PRINT("\n")
PRINT HAS_KEY(book2, "author") //prints: true
PRINT("\n")
//prints: Book has key author? True
PRINT STRING("Book has key author? "), HAS_KEY(book2, "author")

PROGRAM_END