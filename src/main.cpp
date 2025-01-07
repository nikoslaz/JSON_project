#include "JSONlang.h"
 
JSON(book) = OBJECT {
    KEY(title) : STRING("Gone Girl"),
    KEY(published) : NUMBER(2012),
    KEY(type) : STRING("Thriller"),
    KEY(author) : OBJECT {
        KEY(firstname) : STRING("Gillian"),
        KEY(sirname) : STRING("Flynn"),
        KEY(age) : NUMBER(45)
    }
};


JSON(week_temperatures) = ARRAY [
    NUMBER(20), NUMBER(19.5), NUMBER(19), NUMBER(20),
    NUMBER(19), NUMBER(18.5), NUMBER(19)
];

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
        KEY(name) : STRING("Maria Papadopoulou"),
        KEY(id) : NUMBER(5555),
        KEY(grades) : ARRAY [
            OBJECT { KEY(hy100) : NUMBER(9.0) },
            OBJECT { KEY(hy150) : NUMBER(8.5) }
        ]
    }
];

JSON(emptyObj) = OBJECT {}
JSON(emptyArray) = ARRAY

JSON(numbers) = ARRAY [NUMBER(1), NUMBER(2), NUMBER(3)]
JSON(mixedArray) = ARRAY [STRING("hello"), NUMBER(25), TRUE]

PROGRAM_BEGIN

// 1. SET and object manipulation
SET week_temperatures[7] ASSIGN NUMBER(22)
SET book["author"]["age"] ASSIGN NUMBER(50)
SET book["author"]["email"] ASSIGN STRING("gillian@example.com")
//SET emptyObj ASSIGN OBJECT { KEY(a) : STRING("alpha")}

// 2. APPEND values to arrays
SET week_temperatures APPEND NUMBER(23), NUMBER(22), NUMBER(20)
SET students[0]["grades"] APPEND OBJECT { KEY(hy255) : NUMBER(9) }

// 3. ERASE keys from objects and arrays
ERASE book["author"]["age"] 
ERASE week_temperatures[2]   
ERASE book["type"]
ERASE students[1]["grades"][1]
//ERASE book


// 4. Use of arithmetic operators
// JSON(totalGrades) = OBJECT {
//     KEY(total_hy100) : students[0]["grades"][0]["hy100"] + students[1]["grades"][0]["hy100"]
// }

// 5. String concatenation
// JSON(fullName) = STRING("Nikos ") + STRING("Nikolaou")
// JSON(studentMessage) = STRING("Student: ") + students[0]["name"] + STRING(", ID: ") + NUMBER(4444)

// 6. Logical operators
// JSON(isAdult) = students[0]["author"]["age"] >= NUMBER(18)
JSON(containsGrades) = HAS_KEY(students[0], "grades")

// 7. Array and object operations
JSON(isWeekTemperaturesEmpty) = IS_EMPTY(emptyObj)
JSON(studentCount) = SIZE_OF(book)
JSON(weekTemperatureType) = TYPE_OF(book)
JSON(studentNameType) = TYPE_OF(students[0]["name"])

// 8. Use of SIZE_OF
// JSON(studentGradesSize) = SIZE_OF(students[0]["grades"])
// JSON(bookSize) = SIZE_OF(book)

// 9. Comparing JSON objects and arrays
// JSON(areArraysEqual) = (week_temperatures == ARRAY [NUMBER(20), NUMBER(19.5), NUMBER(19), NUMBER(20),
//                                                     NUMBER(19), NUMBER(18.5), NUMBER(19)])


// 10. Print statements
// PRINT(book)
// PRINT(week_temperatures)
// PRINT(students)
// PRINT(emptyObj)
// PRINT(fullName)
// PRINT(studentMessage)
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

PROGRAM_END