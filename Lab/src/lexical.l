%{
    #include "syntax.tab.h"
    #include <stdio.h>
    #include<stdarg.h>
    #include "stdlib.h"
    #include<string.h>
    //#include "syntaxTree.h"
    extern struct syntaxTree *createNode(char *name, int brotherNum,...);
    extern struct syntaxTree *node;
%}

%option yylineno
ws [ \t\r\n]+
letter [_A-Za-z]
digit [0-9]
id ({letter}|\_)({letter}|{digit}|\_)*
integer 0|([1-9]{digit}*)|(0[1-7][0-7]*)|(0[xX][0-9A-Fa-f]+)
float ((0|([1-9]{digit}*))\.{digit}+)|((({digit}*\.{digit}+)|({digit}+\.{digit}*))[Ee][+-]?{digit}+)
sigleLineNotes \/\/.*
relop >=?|<=?|==|!=
%%
"\n" {}
"/*" {
    char c = input();
    char temp='$';
    while(!(c=='/' && temp=='*'))
    {
        temp=c;
        c=input();
    }
}
";" {yylval.node = createNode("SEMI",0,yylineno);return SEMI;}
"," {yylval.node = createNode("COMMA",0,yylineno);return COMMA;}
"=" {yylval.node = createNode("ASSIGNOP",0,yylineno);return ASSIGNOP;}
{relop} {yylval.node = createNode("RELOP",0,yylineno);return RELOP;}
"+" {yylval.node = createNode("PLUS",0,yylineno);return PLUS;}
"-" {yylval.node = createNode("MINUS",0,yylineno);return MINUS;}
"*" {yylval.node = createNode("STAR",0,yylineno);return STAR;}
"/" {yylval.node = createNode("DIV",0,yylineno);return DIV;}
"&&" {yylval.node = createNode("AND",0,yylineno);return AND;}
"||" {yylval.node = createNode("OR",0,yylineno);return OR;}
"." {yylval.node = createNode("DOT",0,yylineno);return DOT;}
"!" {yylval.node = createNode("NOT",0,yylineno);return NOT;}
int|float {yylval.node = createNode("TYPE",0,yylineno);return TYPE;}
"(" {yylval.node = createNode("LP",0,yylineno);return LP;}
")" {yylval.node = createNode("RP",0,yylineno);return RP;}
"[" {yylval.node = createNode("LB",0,yylineno);return LB;}
"]" {yylval.node = createNode("RB",0,yylineno);return RB;}
"{" {yylval.node = createNode("LC",0,yylineno);return LC;}
"}" {yylval.node = createNode("RC",0,yylineno);return RC;}
struct {yylval.node = createNode("STRUCT",0,yylineno);return STRUCT;}
return {yylval.node = createNode("RETURN",0,yylineno);return RETURN;}
if {yylval.node = createNode("IF",0,yylineno);return IF;}
else {yylval.node = createNode("ELSE",0,yylineno);return ELSE;}
while {yylval.node = createNode("WHILE",0,yylineno);return WHILE;}
{ws} {}
{sigleLineNotes} {}
{id} {yylval.node = createNode("ID",0,yylineno);return ID;}
{integer} {
    yylval.node = createNode("INT",0,yylineno);
    return INT;}
{float} {
    yylval.node = createNode("FLOAT",0,yylineno);
    return FLOAT;}
. {return ERROR;}
%%
