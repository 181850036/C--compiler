#include "stdio.h"
#include "iostream"
#include<stdarg.h>
#include<string>
#include <fstream>
#include "Semantic.h"
extern "C"
{
    int yylex();
    int yyparse();
    int yyrestart(FILE *F);
}
extern FILE *yyin;
extern char *yytext;
extern int yylineno;
extern int errNum;
extern void getFile(char*);
extern syntaxTree *root;
extern void Program(struct syntaxTree *node);
extern void translate_Program(struct syntaxTree *node);
using namespace std;
//string tokens[] =  {"ERROR", "SEMI","COMMA","ASSIGNOP","RELOP","PLUS","MINUS","STAR","DIV","AND","OR","DOT","NOT","TYPE","LP","RP","LB","RB","LC","RC","STRUCT","RETURN","IF","ELSE","WHILE","ID","INT","FLOAT"};
string intTypeChange(string morpheme);
string floatTypeChange(string morpheme);

int main(int argc, char **argv)
{
    if(argc<=1) return 1;
    FILE* f = fopen(argv[1],"r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    //printTREE(root,0);
    Program(root);
    translate_Program(root);
    getFile(argv[2]);
    return 0;
}


