#include "syntaxTree.h"
#include <stdio.h>
#include<stdarg.h>
#include "stdlib.h"
#include<string.h>
#include <iostream>
extern char* yytext;
char *tokens[] =  {"ERROR", "SEMI","COMMA","ASSIGNOP","RELOP","PLUS","MINUS","STAR","DIV","AND","OR","DOT","NOT","TYPE","LP","RP","LB","RB","LC","RC","STRUCT","RETURN","IF","ELSE","WHILE","ID","INT","FLOAT"};
using namespace std;
struct syntaxTree *createNode(char *name, int brotherNum,...){    
    struct syntaxTree *daddy=(struct syntaxTree*)malloc(sizeof(struct syntaxTree));
    struct syntaxTree *temp=(struct syntaxTree*)malloc(sizeof(struct syntaxTree));
    daddy->name=name;
    daddy->childrenNum=0;
    for(int i=0;i<10;i++){
        daddy->children[i] = nullptr;
    }
//变长参数列表声明+初始化。
    va_list valist; 
    va_start(valist,brotherNum);

    if(brotherNum>0)
    {
        temp=va_arg(valist, struct syntaxTree*);
        daddy->Left=temp;
        daddy->line=temp->line;

        daddy->children[0] = temp;
        daddy->childrenNum++;
        if(brotherNum>=2) 
        {
            for(int i=0; i<brotherNum-1; ++i)
            {
                temp->Right=va_arg(valist,struct syntaxTree*);
                temp=temp->Right;
                daddy->children[i+1]=temp;
                daddy->childrenNum++;
            }
        }
    }
    else //终结符或空
    {
        int t=va_arg(valist, int); //取第1个变长参数
        char* s;
        s=(char* )malloc(sizeof(char*)*32);
        strcpy(s,yytext);
        daddy->line=t;
        //daddy->children[0] = 
        //if(t!=-1) daddy->childrenNum++;
        if((!strcmp(daddy->name,"ID"))||(!strcmp(daddy->name,"TYPE"))){
            daddy->typeVar=s;
            //daddy->childrenNum++;
        }
        else if(!strcmp(daddy->name,"INT")){ 
            if(s[0]=='0'){
                if(yytext[1]=='x' || yytext[1]=='X')
                sscanf(yytext,"%x",&daddy->typeInt);
                else 
                sscanf(yytext,"%o",&daddy->typeInt);
            }
            else sscanf(yytext,"%d",&daddy->typeInt);
            }
        else if(!strcmp(daddy->name,"FLOAT")) sscanf(yytext,"%lf",&daddy->typeFloat);
        else if(!strcmp(daddy->name,"RELOP")){
            daddy->typeVar=s;
        }
    }
    //cout<<daddy->childrenNum<<endl;
    return daddy;
}
void printTREE(struct syntaxTree *a,int level)
{
    if(a==NULL) return;
    
        int flag = 0;
        for(int j=0;j<28;j++){
            if((strcmp(a->name,tokens[j]))==0){
                flag = 1;
                break;
                 }
                        
    }
        if(a->line!=-1){ //产生空的语法单元不需要打印信息
            for(int i=0; i<level; i++)
                fprintf(stderr,"  ");
            if(flag==0)
                fprintf(stderr,"%s ",a->name);
            else 
                fprintf(stderr,"%s",a->name);
            if((!strcmp(a->name,"ID"))||(!strcmp(a->name,"TYPE")))fprintf(stderr,": %s",a->typeVar);
            else if(!strcmp(a->name,"INT"))fprintf(stderr,": %d",a->typeInt);
            else if(!strcmp(a->name,"FLOAT"))fprintf(stderr,": %lf",a->typeFloat);
            else
                {
                    if(flag==0) fprintf(stderr,"(%d)",a->line);
                }
        fprintf(stderr,"\n");
        }
        

        printTREE(a->Left,level+1);
        printTREE(a->Right,level);
    
}