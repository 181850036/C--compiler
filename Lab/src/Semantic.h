#include "syntaxTree.h"
#include <string>
#include <vector>
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct structure_* Structure;
typedef struct Function_* Function;
enum BASIC
{
    INT,
    FLOAT
};
struct field{
    std::string name;
    Type type = nullptr;
};
struct structure_{
    std::string name;
    std::vector<field> fieldList;
};
struct Type_
{
    enum{
        BASIC,
        ARRAY,
        STRUCTURE,
        ERROR
    } kind;
    union 
    {
        enum BASIC basic;
        struct 
        {
            Type elem;
            int size;
        }   array;
         Structure structure;
    } u;
    enum{left,right,two}side;
    
};
struct FieldList_
{
    char* name;
    Type type;
    FieldList tail;
};
struct Function_
{
    Type returnType= nullptr;
    std::vector<field> varLi;
};
