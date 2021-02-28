#include "string.h"
#include "iostream"
#include "map"
#include <fstream>
#include <vector>
#include "Semantic.h"
//#include "ir.h"
using namespace std;
std::string out1 = "";
map <string,int>symbols;
extern map<string,Function> funcMap;
extern map<string,Type> symbolMap;
int labelNum = 0;
int varNum = 0;
int tempNum = 0;
string nullPlace  = "t1000000";
Structure getS = new structure_();
Type arrayElem = new Type_();
int arrayElemSize = 0;
void addFunc();
string newLabel();
string newTemp();
string newVar(string name);
int getSize(Type t);
void translate_Program(struct syntaxTree *node);
void translate_ExtDefList(struct syntaxTree *node);
void translate_ExtDef(struct syntaxTree *node);
void translate_FunDec(struct syntaxTree *node);
void translate_VarList(struct syntaxTree *node);
string translate_VarDec(struct syntaxTree *node);
void translate_CompSt(struct syntaxTree *node);
void translate_StmtList(struct syntaxTree* node);
void translate_Stmt(struct syntaxTree* node);
void translate_DefList(struct syntaxTree* node);
void translate_Def(struct syntaxTree* node);
void translate_DecList(struct syntaxTree*node);
void translate_Dec(struct syntaxTree*node);
void translate_Cond(struct syntaxTree*node,string trueLabel,string falseLabel);
void translate_Exp(struct syntaxTree*node,string place);
void translate_Args(struct syntaxTree *node, vector<string> &T);
Type ARRorSTCT(struct syntaxTree *node, string place, bool isArray);
string leftSideValue(struct syntaxTree *node);
void translate_Exp(struct syntaxTree *node);
void getFile(char* path){
        ofstream fout;
        fout.open(path,ios::out);
        fout<<out1<<endl;
        cout<<out1;
        fout.close();
}


string newLabel(){
    string res = "LABEL"+to_string(labelNum);
    labelNum++;
    return res;
}
string newTemp(){
    string res="t"+to_string(tempNum);
    tempNum++;
    return res;
}
string newVar(string name){
    auto it = symbols.find(name);
    if(it==symbols.end()){
        symbols.insert({name,varNum});
        //varNum++;
        return "v"+to_string(varNum++);
    }
    else{
        return "v"+to_string(it->second);
    }
}
void write_A_Line(string line){

}

int getSize(Type t){
    int res = 0;
    if(t->kind==Type_::BASIC){
        if(t->u.basic==INT) return 4;
        else return 8;
    }
    else if(t->kind==Type_::ARRAY){
        return t->u.array.size*getSize(t->u.array.elem);
    }
    else{
        for(int i=0;i<t->u.structure->fieldList.size();i++){
            res += getSize(t->u.structure->fieldList.at(i).type);
        }
        return res;
    }
}
void translate_Program(struct syntaxTree *node){
    out1 = "";
    //addFunc();
    translate_ExtDefList(node->children[0]);
}

//ExtDefList: ExtDef ExtDefList
void translate_ExtDefList(struct syntaxTree *node){
    if(node->childrenNum==0) return;
    if(node==nullptr) return ;
    translate_ExtDef(node->children[0]);
    translate_ExtDefList(node->children[1]);
}

/*
ExtDef : Specifier ExtDecList SEMI .//全局变量不能赋值 没用
        |Specifier SEMI
        |Specifier FunDec CompSt
*/
void translate_ExtDef(struct syntaxTree *node){
    if(string(node->children[1]->name)=="FunDec"){
        translate_FunDec(node->children[1]);
        translate_CompSt(node->children[2]);
    }
}

/*
FunDec : ID LP VarList RP
| ID LP RP
*/
void translate_FunDec(struct syntaxTree *node){
    string line = "FUNCTION "+string(node->children[0]->typeVar)+" :\n";
    out1+=line;
    if(node->childrenNum==4){
        auto it = funcMap.find(string(node->children[0]->typeVar));
        vector<field> temp = it->second->varLi;
        for(int i=0;i<temp.size();i++){
            out1=out1+"PARAM "+newVar(temp.at(i).name)+"\n";
        }
    }

}


string translate_VarDec(struct syntaxTree *node)
{
    string first = node->children[0]->name;
    if (first == "ID"){
        return string(node->children[0]->typeVar);
    }
    else
    {
        return translate_VarDec(node->children[0]);
    }
}

//CompSt : LC DefList StmtList RC
void translate_CompSt(struct syntaxTree *node){
    if(node==nullptr) return;
    translate_DefList(node->children[1]);
    translate_StmtList(node->children[2]);
}

/*
StmtList : Stmt StmtList
        | 
*/
void translate_StmtList(struct syntaxTree* node){
    if(node==nullptr) return ;
    if(node->children[0]==nullptr) return;
    else{
        translate_Stmt(node->children[0]);
        translate_StmtList(node->children[1]);
    }

}

/*
Stmt : Exp SEMI
| CompSt
| RETURN Exp SEMI
| IF LP Exp RP Stmt
| IF LP Exp RP Stmt ELSE Stmt
| WHILE LP Exp RP Stmt
*/

void translate_Stmt(struct syntaxTree* node){
    if(node==nullptr) return;
    string first = string(node->children[0]->name);
    if(first=="CompSt"){
        translate_CompSt(node->children[0]);
    }
    else if(first=="RETURN"){
        string t1 = newTemp();
        translate_Exp(node->children[1],t1);
        out1=out1+"RETURN "+t1 +"\n";
    }
    else if(first=="Exp"){
        translate_Exp(node->children[0],nullPlace);
    }
    else if(first=="IF"){
        if(node->childrenNum==5){ //if
            string label1 = newLabel();
            string label2 = newLabel();
            translate_Cond(node->children[2],label1,label2);
            out1=out1+"LABEL "+label1+" :"+"\n";
            translate_Stmt(node->children[4]);
            out1=out1+"LABEL "+label2+" :"+"\n";

        }
        else{  //if else
            string label1 = newLabel();
            string label2 = newLabel();
            translate_Cond(node->children[2],label1,label2);
            out1=out1+"LABEL "+label1+" :"+"\n";
            translate_Stmt(node->children[4]);
            string label3 = newLabel();
            out1=out1 + "GOTO " +label3+"\n";
            out1=out1+"LABEL "+label2+" :"+"\n";
            translate_Stmt(node->children[6]);
            out1=out1+"LABEL "+label3+" :"+"\n";
        }
    }
    else{ //while
        string label1 = newLabel();
        string label2 = newLabel();
        string label3 = newLabel();
        out1=out1+"LABEL "+label1+" :"+"\n";
        translate_Cond(node->children[2],label2,label3);
        out1=out1+"LABEL "+label2+" :"+"\n";
        translate_Stmt(node->children[4]);
        out1=out1+"GOTO "+label1+"\n";
        out1=out1+"LABEL "+label3+" :"+"\n";
    }
}

/*
DefList : Def DefList
        | 

*/
void translate_DefList(struct syntaxTree* node){
    if(node==nullptr) return ;
    if(node->line==-1 || node->childrenNum==0|| node->children[0]==nullptr) return ;
    if(node->line!=-1){
        translate_Def(node->children[0]);
        translate_DefList(node->children[1]);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
}

//Def: Specifier DecList SEMI
void translate_Def(struct syntaxTree* node){
        translate_DecList(node->children[1]);
}
/*
DecList : Dec
| Dec COMMA DecList
*/
void translate_DecList(struct syntaxTree*node){
    if(node==nullptr) return ;
    if(node->childrenNum==1){
        translate_Dec(node->children[0]);
    }
    else{
        translate_Dec(node->children[0]);
        translate_DecList(node->children[2]);
    }
}
/*
Dec : VarDec
| VarDec ASSIGNOP EXP
*/
void translate_Dec(struct syntaxTree*node){
    if(node==nullptr) return ;
    string name = translate_VarDec(node->children[0]);
    if(symbolMap.find(name)->second->kind!=Type_::BASIC){
        out1=out1+"DEC "+ newVar(name) + " " + to_string(getSize(symbolMap.find(name)->second))+"\n";
    }
    if(node->childrenNum==3){
        string id = string(node->children[0]->children[0]->typeVar);
        string var = newVar(id);
        translate_Exp(node->children[2],var);
    }
}
/*
    Exp:  NOT Exp
        | Exp AND Exp
        | Exp OR Exp
        | Exp RELOP Exp
*/
void translate_Cond(struct syntaxTree*node,string trueLabel,string falseLabel){
    string first = string(node->children[0]->name);
    if(first=="NOT"){
        translate_Cond(node->children[1],falseLabel,trueLabel);
    }
    else if(node->children[1]!=nullptr){
        string second = string(node->children[1]->name);
        if(second=="RELOP"){
            string t1 = newTemp();
            string t2 = newTemp();
            translate_Exp(node->children[0],t1);
            translate_Exp(node->children[2],t2);
            string op = string(node->children[1]->typeVar);
            out1=out1+"IF "+t1+" "+op+" "+t2+" GOTO "+trueLabel+"\n";
            out1=out1+"GOTO "+falseLabel+"\n";
        }
        else if(second=="AND"){
            string label1 = newLabel();
            translate_Cond(node->children[0],label1,falseLabel);
            out1=out1+"LABEL "+label1+" :"+"\n";
            translate_Cond(node->children[2],trueLabel,falseLabel);
            
        }
        else if(second=="OR"){
            string label1 = newLabel();
            translate_Cond(node->children[0],trueLabel,label1);
            out1=out1+"LABEL "+label1+" :"+"\n";
            translate_Cond(node->children[2],trueLabel,falseLabel);
            
        }
        else{
            string t1 = newTemp();
            translate_Exp(node,t1);
            out1=out1+"IF "+t1+" != #0 GOTO "+trueLabel+"\n";
            out1=out1+"GOTO "+falseLabel+"\n";
        }
    }
    else{
        string t1 = newTemp();
        translate_Exp(node,t1);
        out1=out1+"IF "+t1+" != #0 GOTO "+trueLabel+"\n";
        out1=out1+"GOTO "+falseLabel+"\n";
    }
}
/*
    Exp: Exp ASSIGNOP Exp
        | Exp AND Exp
        | Exp OR Exp
        | Exp RELOP Exp
        | Exp PLUS Exp
        | Exp MINUS Exp
        | Exp STAR Exp
        | Exp DIV Exp
        | LP Exp RP
        | MINUS Exp
        | NOT Exp
        | ID LP Args RP
        | ID LP RP
        | Exp LB Exp RB
        | Exp DOT ID
        | ID
        | INT
        | FLOAT
*/

void translate_Exp(struct syntaxTree*node,string place){
    if(node==nullptr) return ;
    int sonNum = node->childrenNum;
    string first,second;
    Type res=new Type_();
    first = string(node->children[0]->name);
    if(sonNum>1){
        second = string(node->children[1]->name);
    }
    if(sonNum==1){
        if(first=="INT"){
            string value = to_string(node->children[0]->typeInt);
            out1 = out1+place+" := "+"#"+value+"\n";
        }
        else if(first == "FLOAT"){
            string value = to_string(node->children[0]->typeFloat);
            out1 = out1+place+" := "+"#"+value+"\n";
        }
        else{  //ID
            string name = leftSideValue(node);
            string id = newVar(name);
            out1=out1+place+" := "+name+"\n";
        }
    }
    else if(sonNum==2){
        if(first=="MINUS"){
            string t1 = newTemp();
            translate_Exp(node->children[1],t1);
            out1 = out1 + place + " := #0 - " + t1+"\n";
        }
        else{ //NOT
            string label1 = newLabel();
            string label2 = newLabel();
            out1 = out1 + place + " := #0" + "\n";
            translate_Cond(node, label1,label2);
            out1 = out1 + "LABEL " + label1 + " :"+"\n"+place + " := #1"+"\n"+"LABEL " + label2 + " :"+"\n";

        }
    }
    else if((sonNum==3)){
        if(first=="Exp" && string(node->children[2]->name)=="Exp"){
            if(second=="ASSIGNOP"){
                string left = leftSideValue(node->children[0]);
                string t1 = newTemp();
                translate_Exp(node->children[2],t1);
                out1 = out1 + left + " := " + t1 + "\n";
                out1 = out1 + place + " := " + left + "\n";
            }
            else if(second=="AND"||second=="OR"||second=="RELOP"){
                string label1 = newLabel();
                string label2 = newLabel();
                out1 = out1 + place + " := #0" + "\n";
                translate_Cond(node, label1,label2);
                out1 = out1 + "LABEL " + label1 + " :"+"\n"+place + " := #1"+"\n"+"LABEL " + label2 + " :"+"\n";
            }
            else if(second=="STAR"||second=="DIV"||second=="PLUS"||second=="MINUS"){
                string t1 = newTemp();
                string t2 = newTemp();
                translate_Exp(node->children[0],t1);
                translate_Exp(node->children[2],t2);
                string op = "";
                if(second=="STAR") op = "*";
                else if(second=="DIV") op = "/";
                else if(second=="PLUS") op = "+";
                else if(second=="MINUS") op = "-";
                out1 = out1 + place + " := " + t1 + " " + op + " " + t2 + "\n";
            }
        }
        else if(first=="LP" && second=="Exp"){
            translate_Exp(node->children[1],place);
        }
        else if(first=="ID" && second=="LP"){
            if(string(node->children[0]->typeVar)=="read"){
                out1 = out1 + "READ "+place+ "\n";
                return;
            }
           // if(node->childrenNum==3){
               // if(place!=nullPlace)
                    out1=out1+ place + " := CALL " + string(node->children[0]->typeVar)+"\n";
               // else{
                  //  out1=out1+ newTemp() + " := CALL " + string(node->children[0]->typeVar)+"\n";
               // }
           // }
        }
        else if(first=="Exp" && second == "DOT"){
            string left = leftSideValue(node);
            out1 = out1 + place + " := " + left + "\n";
        }
    }
    else if(sonNum==4){
        if(first=="ID"){ //有参函数
            vector <string> args;
            if(node->childrenNum==4) translate_Args(node->children[2],args);
            if(string(node->children[0]->typeVar)=="write"){
                out1 = out1 + "WRITE " + args.at(0)+"\n";//zzz
                return;
            }
            for(int i=0;i<args.size();i++){
                out1=out1+"ARG "+ args.at(i)+"\n";
            }
           // if(place!=nullPlace)
                out1=out1+ place + " := CALL " + string(node->children[0]->typeVar)+"\n";
           // else{
              //  out1=out1+ newTemp() + " := CALL " + string(node->children[0]->typeVar)+"\n";
            //}
        }
        else if(first=="Exp"){ //数组
            string left = leftSideValue(node);
            out1 = out1 + place + " := " + left +"\n";
        }
    }
}


void translate_Args(struct syntaxTree *node, vector<string> &T)
{
    if(node==nullptr) return ;
    string t1 =newTemp();
    translate_Exp(node->children[0],t1);
    if(node->childrenNum>1){
        translate_Args(node->children[2],T);
    }
    T.push_back(t1);
}
 Type ARRorSTCT(struct syntaxTree *node, string place, bool isArray)
{
    if(string(node->children[0]->name)=="ID"){
        out1 = out1+place + " := &" + newVar(string(node->children[0]->typeVar))+"\n";
        return symbolMap.find(string(node->children[0]->typeVar))->second;
    }
    else{
        if(isArray){
            string t1 = newTemp();
            Type t = new Type_();
            t= ARRorSTCT(node->children[0],t1,true);
            string t2 = newTemp();
            translate_Exp(node->children[2],t2);
            string t3 = newTemp();
            out1=out1+t3 + " := " + t2 + " * #" + to_string(getSize(t->u.array.elem))+"\n";
            out1=out1+place + " := " + t1 + " + " + t3+"\n";
            return t;
        }
        else{
            string t1 = newTemp();
            Type t = new Type_();
            t->u.structure = new structure_();
            t= ARRorSTCT(node->children[0],t1,false);
            string id = string(node->children[2]->typeVar);
            int off = 0;
            Type res= new Type_();
            for(int i=0;i<t->u.structure->fieldList.size();i++){
                if(t->u.structure->fieldList.at(i).name==id){
                    res->u.structure = t->u.structure->fieldList.at(i).type->u.structure;
                    break;
                }
                off+=getSize(t->u.structure->fieldList.at(i).type);
            }
            out1=out1+place + " := " + t1 + " + #" + to_string(off)+"\n";
            return res;
        }
    }
}
string leftSideValue(struct syntaxTree *node)
{
    string first = string(node->children[0]->name);
    if(first == "ID"){
        return newVar(string(node->children[0]->typeVar));
    }
    string second = string(node->children[1]->name);
    if (second == "LB"){
        string t1 = newTemp();
        Type t = new Type_();
        t= ARRorSTCT(node->children[0],t1,true);
        string t2 = newTemp();
        translate_Exp(node->children[2],t2);
        std::string t3 = newTemp();
        out1=out1+t3 + " := " + t2 + " * #" + to_string(getSize(t->u.array.elem))+"\n";
        std::string t4 = newTemp();
        out1=out1+t4 + " := " + t1 + " + " + t3+"\n";
        return "*" + t4;
    }
    else{
        std::string t1 = newTemp();
        Type t = new Type_();
        t->u.structure = new structure_();
        t= ARRorSTCT(node->children[0],t1,false);
        string id = string(node->children[2]->typeVar);
        int offset = 0;
        Type res = new Type_();
        for (int i = 0; i < t->u.structure->fieldList.size(); i++){
            if (t->u.structure->fieldList.at(i).name == id){
                res = t->u.structure->fieldList.at(i).type;
                break;
            }
                offset += getSize(t->u.structure->fieldList.at(i).type);
        }
        string t2 = newTemp();
        out1 = out1+ t2 + " := " + t1 + " + #" + to_string(offset)+"\n";
        return "*" + t2;
    }

}