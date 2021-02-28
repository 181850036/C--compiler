%{
    //void yyerror(const char* msg);
    int yyparse(void);
    //#include "syntaxTree.h"
    #include "stdio.h"
    #include<unistd.h>
	extern struct syntaxTree *node;
	extern struct syntaxTree *createNode(char *name, int brotherNum,...);
	extern void printTREE(struct syntaxTree *a,int level);
	int errNum = 0;
	struct syntaxTree * root;

%}
%union{
struct syntaxTree * node;
double d;
}
%token <node> SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE ID INT FLOAT ERROR
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args


%nonassoc LOWER_THAN_ERROR
%nonassoc error


%start Program
%right ASSIGNOP
%left OR
%left AND 
%left  RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left LB RB LP RP DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%


/*High-Level Definations*/
Program: ExtDefList 	{$$=createNode("Program",1,$1);
						root = $$;
							}
    ;
ExtDefList:ExtDef ExtDefList 	{$$=createNode("ExtDefList",2,$1,$2);}  
	| 	{$$=createNode("ExtDefList",0,-1);}
	;
ExtDef:Specifier ExtDecList SEMI    {$$=createNode("ExtDef",3,$1,$2,$3);}    
	|Specifier SEMI	{$$=createNode("ExtDef",2,$1,$2);}
	|Specifier FunDec CompSt	{$$=createNode("ExtDef",3,$1,$2,$3);}
	|error SEMI{}
	;
ExtDecList:VarDec {$$=createNode("ExtDecList",1,$1);}
	|VarDec COMMA ExtDecList {$$=createNode("ExtDecList",3,$1,$2,$3);}
	;
/*Specifier*/
Specifier:TYPE {$$=createNode("Specifier",1,$1);}
	|StructSpecifier {$$=createNode("Specifier",1,$1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {$$=createNode("StructSpecifier",5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=createNode("StructSpecifier",2,$1,$2);}
	|STRUCT OptTag LC error RC {}
	;
OptTag:ID {$$=createNode("OptTag",1,$1);}
	|{$$=createNode("OptTag",0,-1);}
	;
Tag:ID {$$=createNode("Tag",1,$1);}
	;


/*Declaratots*/
VarDec:ID {$$=createNode("VarDec",1,$1);}
	| VarDec LB INT RB {$$=createNode("VarDec",4,$1,$2,$3,$4);}
	|VarDec LB error RB{}
	;


FunDec:ID LP VarList RP {$$=createNode("FunDec",4,$1,$2,$3,$4);}
	|ID LP RP {$$=createNode("FunDec",3,$1,$2,$3);}
	|ID LP error RP{ }
	;

VarList:ParamDec COMMA VarList {$$=createNode("VarList",3,$1,$2,$3);}
	|ParamDec {$$=createNode("VarList",1,$1);}
	;
ParamDec:Specifier VarDec {$$=createNode("ParamDec",2,$1,$2);}
    ;


/*Statement*/
CompSt:LC DefList StmtList RC {$$=createNode("CompSt",4,$1,$2,$3,$4);}
	| LC error RC{}
	;
StmtList:Stmt StmtList{$$=createNode("StmtList",2,$1,$2);}
	| {$$=createNode("StmtList",0,-1);}
	;
Stmt:Exp SEMI {$$=createNode("Stmt",2,$1,$2);}
	|CompSt {$$=createNode("Stmt",1,$1);}
	|RETURN Exp SEMI {$$=createNode("Stmt",3,$1,$2,$3);}
	|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=createNode("Stmt",5,$1,$2,$3,$4,$5);}
	|IF LP Exp RP Stmt ELSE Stmt %prec ELSE {$$=createNode("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
	|WHILE LP Exp RP Stmt {$$=createNode("Stmt",5,$1,$2,$3,$4,$5);}
	|error SEMI {}
	;


/*Local Definations*/
DefList:Def DefList{$$=createNode("DefList",2,$1,$2);}
	|  %prec LOWER_THAN_ERROR{$$=createNode("DefList",0,-1);}
	;
Def:Specifier DecList SEMI {$$=createNode("Def",3,$1,$2,$3);}
	|error SEMI{}
	;


DecList:Dec {$$=createNode("DecList",1,$1);}
	|Dec COMMA DecList {$$=createNode("DecList",3,$1,$2,$3);}
	;
Dec:VarDec {$$=createNode("Dec",1,$1);}
	|VarDec ASSIGNOP Exp {$$=createNode("Dec",3,$1,$2,$3);}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp AND Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp OR Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp RELOP Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp PLUS Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp MINUS Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp STAR Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |Exp DIV Exp{$$=createNode("Exp",3,$1,$2,$3);}
        |LP Exp RP{$$=createNode("Exp",3,$1,$2,$3);}
        |MINUS Exp  %prec UMINUS {$$=createNode("Exp",2,$1,$2);}
        |NOT Exp {$$=createNode("Exp",2,$1,$2);}
        |ID LP Args RP {$$=createNode("Exp",4,$1,$2,$3,$4);}
        |ID LP RP {$$=createNode("Exp",3,$1,$2,$3);}
        |Exp LB Exp RB {$$=createNode("Exp",4,$1,$2,$3,$4);}
        |Exp DOT ID {$$=createNode("Exp",3,$1,$2,$3);}
        |ID {$$=createNode("Exp",1,$1);}
        |INT {$$=createNode("Exp",1,$1);}
        |FLOAT{$$=createNode("Exp",1,$1);}
        |LP error RP{}
        |ID LP error RP{}
        |Exp LB error RB{}
        ;
Args:Exp COMMA Args {$$=createNode("Args",3,$1,$2,$3);}
        |Exp {$$=createNode("Args",1,$1);}
        ;
%%
#include "lex.yy.c"
int yyerror(char* msg) {
	errNum+=1;
 fprintf(stderr, "Error type B at Line %d: %s.\n",yylineno,msg);
}


