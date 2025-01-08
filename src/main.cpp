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
PRINT("Empty object: ")
PRINT(emptyObj)
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
PRINT("Special case of +: ")
PRINT(conc)
PRINT("\n")

// TODO
// JSON(concarr) = ARRAY[NUMBER(1), NUMBER(2)] + ARRAY[NUMBER(3), NUMBER(4)]
// PRINT("Special case of +: ")
// PRINT(concarr)
// PRINT("\n")

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

//TODO ADD (&&, ||, !)

// 5. String concatenation
// JSON(fullName) = STRING("Nikos ") + STRING("Nikolaou")
// JSON(studentMessage) = STRING("Student: ") + students[0]["name"] + STRING(", ID: ") + NUMBER(4444)

// 6. Logical operators
// JSON(isAdult) = students[0]["author"]["age"] >= NUMBER(18)
// JSON(containsGrades) = HAS_KEY(students[0], "grades")

// // 7. Array and object operations
// JSON(isWeekTemperaturesEmpty) = IS_EMPTY(emptyObj)
// JSON(studentCount) = SIZE_OF(book)
// JSON(weekTemperatureType) = TYPE_OF(book)
// JSON(studentNameType) = TYPE_OF(students[0]["name"])

// 8. Use of SIZE_OF
// JSON(studentGradesSize) = SIZE_OF(students[0]["grades"])
// JSON(bookSize) = SIZE_OF(book)

// 9. Comparing JSON objects and arrays
// JSON(areArraysEqual) = (week_temperatures == ARRAY [NUMBER(20), NUMBER(19.5), NUMBER(19), NUMBER(20),
//                                                     NUMBER(19), NUMBER(18.5), NUMBER(19)])

// 10. Print statements
// PRINT(book)
// PRINT(week_temperatures)
//PRINT(students)
// PRINT(emptyObj)
// PRINT(fullName)
// PRINT(studentMesIsage)
// PRINT(isAdult)
//PRINT(containsGrades)
// PRINT(studentCount)
// PRINT(isWeekTemperaturesEmpty)
// PRINT(weekTemperatureType)
// PRINT(studentNameType)
// PRINT(studentGradesSize)
// PRINT(bookSize)
// PRINT(areArraysEqual)
//PRINT(totalGrades)
//PRINT(hy352_nik)
// PRINT book["title"] //prints:Gone Girl
// PRINT book["author"] //prints:{firstname:”Gillian”,sirname:“Flynn”,age: 45}
// PRINT book //prints: the whole json for book
// PRINT HAS_KEY(book, "author") //z
// PRINT STRING("Book has key author? "), HAS_KEY(book, "author")
//prints: Book has key author? True
// PRINT STRING("Book has key author? "), HAS_KEY(book, "author")


PROGRAM_END