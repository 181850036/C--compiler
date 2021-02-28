#include "Semantic.h"
#include "string.h"
#include "iostream"
#include "map"
#include <string>
#include <vector>
using namespace std;
std::map<string,Type> symbolMap;
std::map<string,Type> structMap;
std::map<string,Function> funcMap;
//查找Struct里的变量
void typeCopy(Type &t1, Type &t2){
    t1->kind = t2->kind;
    t1->u = t2->u;
}
bool isMatched(Type t1,  Type t2);
void Program(struct syntaxTree *node);
void ExtDefList(struct syntaxTree *node);
void ExtDef(struct syntaxTree* node);
void ExtDecList(struct syntaxTree* node, Type t);
Type Specifiers(struct syntaxTree* node);
Type StructSpecifier(struct syntaxTree* node);
string VarDec(struct syntaxTree* node, Type t, bool isInStruct, Type &ifStruct);
void FunDec(struct syntaxTree* node , Type t);
void VarList(struct syntaxTree* node, Function &Func);
void ParamDec(struct syntaxTree* node,Function &Func);
void CompSt(struct syntaxTree* node, Type t);
void StmtList(struct syntaxTree* node,Type t);
void Stmt(struct syntaxTree* node,Type t);
void DefList(struct syntaxTree* node,bool isInStruct, Type &ifStruct);
void Def(struct syntaxTree* node,bool isInStruct, Type &ifStruct);
void DecList(struct syntaxTree*node, Type t, bool isInStruct,Type &ifStruct);
void Dec(struct syntaxTree*node, Type t, bool isInStruct, Type &ifStruct);
Type Exp(struct syntaxTree* node);
void Args(struct syntaxTree* node,vector<Type> &T);
bool isWrong = false;
bool argsF=false;
int num=0;
void addFunc(){
    Function read = new Function_();
    read->returnType = new Type_();
    read->returnType->kind = Type_::BASIC;
    read->returnType->u.basic = INT;
    funcMap.insert({"read", read});


    Function write = new Function_();
    write->returnType = new Type_();
    write->returnType->kind = Type_::BASIC;
    write->returnType->u.basic = INT;
    field out;
    out.name = "output";
    out.type = new Type_();
    out.type->kind = Type_::BASIC;
    out.type->u.basic =INT;
    write->varLi.push_back(out);
    funcMap.insert({"write", write});
}
bool isMatched(Type t1,  Type t2){
    //return false;
    if(t1==nullptr && t2==nullptr) return false;
    if(t1==nullptr || t2==nullptr) return false;
    if(t1->kind!=t2->kind) return false;
    if(t1->kind==Type_::BASIC){
        if(t1->u.basic==t2->u.basic) return true;
        else return false;
    }
    else if(t1->kind==Type_::ARRAY){
        return isMatched(t1->u.array.elem,t2->u.array.elem);
    }
    else{
        vector<field> a= t1->u.structure->fieldList;
        vector<field> b= t2->u.structure->fieldList;
        //cout<<a.size()<<endl<<b.size()<<endl;
        //cout<<endl<<a.size()<<b.size()<<endl;
        if(a.size()!=b.size()) return false;
        for(int i=0;i<a.size();i++){
            if(!isMatched(a.at(i).type,b.at(i).type)) return false;
        }
        return true;
    }
}
void Program(struct syntaxTree *node){
    addFunc();
    ExtDefList(node->children[0]);
}

//ExtDefList: ExtDef ExtDefList
void ExtDefList(struct syntaxTree *node){
    if(node->childrenNum==0) return;
    if(node==nullptr) return ;
    ExtDef(node->children[0]);
    ExtDefList(node->children[1]);
}
/*
ExtDef : Specifier ExtDecList SEMI
        |Specifier SEMI
        |Specifier FunDec CompSt
*/
void ExtDef(struct syntaxTree* node){
    if(node==nullptr) return;
    Type t  =new Type_();
    t= Specifiers(node->children[0]);
    if(t==nullptr) return;
    if(node->childrenNum==2){
        ;                       //结构体  struct{...};
    }
    else if(node->childrenNum==3){
        if(string(node->children[1]->name)=="ExtDecList"){
            ExtDecList(node->children[1],t);//int a,b,c;
        }
        else if(string(node->children[1]->name) == "FunDec"){
            FunDec(node->children[1],t);//函数定义
            if(!isWrong){
            CompSt(node->children[2],t);
            }
            else{
                isWrong = false;
            }
        }
    }
}
/*
ExtDecList : VarDec
            |VarDec COMMA ExtDecList
*/
void ExtDecList(struct syntaxTree* node, Type t){
    if(node==nullptr) return ;
    Type temp = new Type_();
    if(node->childrenNum==1){
        VarDec(node->children[0], t, false,temp);
    }
    else{
        VarDec(node->children[0], t, false,temp);
        ExtDecList(node->children[2],t);
    }
}
/*
Specifier   :TYPE
            |StructSpecifier
*/
Type Specifiers(struct syntaxTree* node){
    if(node==nullptr) return nullptr;
    Type res=new Type_();
    if(string(node->children[0]->name)=="TYPE"){
    res->kind=Type_::BASIC;
        if(string(node->children[0]->typeVar) == "int"){
            res->u.basic=INT;
        }
        else if(string(node->children[0]->typeVar)=="float"){
            res->u.basic=FLOAT;
        }
        return res;
    }
    else {
        return StructSpecifier(node->children[0]);
    }
}

/*
StructSpecifier : STRUCT OptTag LC DefList RC       eg:struct A{int a; int b;};
                | STRUCT Tag                           struct A...
*/
Type StructSpecifier(struct syntaxTree* node){
    if(node==nullptr) return nullptr;
    Type res=new Type_() ;
    if(node->childrenNum==2){
        string temp = node->children[1]->children[0]->typeVar;
        int have = structMap.count(temp);
        auto it = structMap.find(temp);
        if(it==structMap.end()){
            cerr<<"Error type 17 at Line "<< node->children[1]->line<<": Undefined structure"<<endl;
            return nullptr;
        }
        else{
            if(it->second->kind!=Type_::STRUCTURE){  
            cerr<<"Error type 17 at Line "<< node->children[1]->line<<": Undefined structure"<<endl;
            return nullptr;
            }
            else{
                res = it->second;
            }
        }
    }
    else{
        string structName="";
        bool isEmpty = true;
        if(node->children[1]->line!=-1){
            isEmpty=false;
            structName = node->children[1]->children[0]->typeVar;
        }
        //else return nullptr;
        //cout<<structName<<endl;
        if(isEmpty==false){
        auto it = structMap.find(structName);
        auto it1= symbolMap.find(structName);
        if(it!=structMap.end() || it1!=symbolMap.end()){  //二次定义
            cerr<<"Error type 16 at Line "<< node->children[1]->line<<": Duplicated name"<<endl;
            return nullptr;
        }
        else{
            res->kind = Type_::STRUCTURE;
            res->u.structure = new structure_();
            res->u.structure->name=structName;
            DefList(node->children[3],true,res);
            structMap.insert({structName,res});
        }
        }
        else {
            res->kind = Type_::STRUCTURE;
            res->u.structure=new structure_();
            DefList(node->children[3],true,res);
        }
    }
    return res;
}
/*
VarDec : ID
        |VarDec LB INT RB

*/
string VarDec(struct syntaxTree* node, Type t, bool isInStruct, Type &ifStruct){
    if(node==nullptr) return "";
    if(node->childrenNum==1){
    string IDname = node->children[0]->typeVar;
            if((symbolMap.find(IDname)!=symbolMap.end()) || structMap.find(IDname)!=structMap.end()){
                if(isInStruct)
                    cerr<<"Error type 15 at Line "<< node->children[0]->line<<": Redefinde filed"<<endl;
                else
                {
                    cerr<<"Error type 3 at Line "<< node->children[0]->line<<": Redefinde var"<<endl;
                }
                
                return "";
            }
            else{
                struct field f;
                if(isInStruct){
                    f.name = IDname;
                    f.type = t;
                    ifStruct->u.structure->fieldList.push_back(f);
                }
                symbolMap.insert({IDname,t});
                return IDname;
            }
        }
    //}
    else{//数组
        Type li = new Type_();
        //Type arrayType = new Type_();
        li->kind=Type_::ARRAY;
        li->u.array.elem = t;
        li->u.array.size = node->children[2]->typeInt;
        return VarDec(node->children[0],li,isInStruct,ifStruct);
    }
}
/*
FunDec : ID LP VarList RP
| ID LP RP
*/
void FunDec(struct syntaxTree* node , Type t){
    if(node==nullptr) return ;
    Function func=new Function_();
    func->returnType = new Type_();
    if(node->childrenNum==3){
        if(funcMap.find(string(node->children[0]->typeVar))!=funcMap.end()){
            isWrong = true;
            cerr<<"Error type 4 at Line "<< node->children[0]->line<<": Redefinde function"<<endl;
            return;
        }
        else{
            func->returnType = t;
            funcMap.insert({string(node->children[0]->typeVar),func});
            return;
        }
    }
    else{
        if(funcMap.find(string(node->children[0]->typeVar))!=funcMap.end()){
            isWrong = true;
            cerr<<"Error type 4 at Line "<< node->children[0]->line<<": Redefinde function"<<endl;
            return;
        }
        func->returnType=t;
        string name = node->children[0]->typeVar;
        //vector<field> funArgs;
        if(isWrong){
            return;
        }
        VarList(node->children[2],func);
        funcMap.insert({name, func});
    }
}
/*
VarList : ParamDec COMMA VarList
| ParamDec

*/
void VarList(struct syntaxTree* node, Function &Func){
    if(node==nullptr) return ;
    string temp="aa";
    if(node->childrenNum==1){
        ParamDec(node->children[0],Func);
        //if(isWrong) return ;
    }
    else{
        ParamDec(node->children[0],Func);
        //if(isWrong) return ;
        VarList(node->children[2],Func);
        
    }
}

//ParamDec  Specifier VarDec
void ParamDec(struct syntaxTree* node,Function &Func){
    if(node==nullptr) return ;
    Type t=new Type_();
    t = Specifiers(node->children[0]);
     if(t==nullptr) return  ;
    string temp=VarDec(node->children[1],t,false,t);
    if(temp.empty()) {
        isWrong=true;
        return ;
    }
    struct  field f;
    f.name = temp;
    f.type=symbolMap.find(temp)->second;
    Func->varLi.push_back(f);
}

//CompSt : LC DefList StmtList RC
void CompSt(struct syntaxTree* node, Type t){
    if(node==nullptr) return ;
    Type temp=new Type_();
    DefList(node->children[1],false,temp);
    StmtList(node->children[2],t);
}

/*
StmtList : Stmt StmtList
        | 
*/
void StmtList(struct syntaxTree* node,Type t){
    if(node==nullptr) return ;
    if(node->children[0]==nullptr) return;
    else{
        Stmt(node->children[0],t);
        StmtList(node->children[1],t);
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
void Stmt(struct syntaxTree* node,Type t){
    if(node==nullptr) return ;
    string name = node->children[0]->name;
    if(name=="Exp"){
        Exp(node->children[0]);
    }
    else if(name=="CompSt"){
        CompSt(node->children[0],t);
    }
    else if(name=="RETURN"){
            Type temp = Exp(node->children[1]);
            if(temp==nullptr) return;
            if(!isMatched(t,temp)){
                cerr<<"Error type 8 at Line "<< node->children[1]->line<<": Return type mismatched for return."<<endl;
            }
        }
    else if(name=="IF"){
        if(node->childrenNum==5){
            Type temp=Exp(node->children[2]);
            if(temp!=nullptr){
            if(!(temp->kind==Type_::BASIC)||!(temp->u.basic==BASIC::INT)){
                cerr<<"Error type 7 at Line "<< node->children[2]->line<<": Type mismatched."<<endl;
            }
            }
            Stmt(node->children[4],t);
        }
        else{
            Type temp=Exp(node->children[2]);
            if(temp!=nullptr){
            if(!(temp->kind==Type_::BASIC)||!(temp->u.basic==BASIC::INT)){
            cerr<<"Error type 7 at Line "<< node->children[2]->line<<": Type mismatched."<<endl;
            }
            }
            Stmt(node->children[4],t);
            Stmt(node->children[6],t);

        }
    }
    else if(name=="WHILE"){
            Type temp=Exp(node->children[2]);
            if(temp!=nullptr){
            if(!(temp->kind==Type_::BASIC)||!(temp->u.basic==BASIC::INT)){
            cerr<<"Error type 7 at Line "<< node->children[2]->line<<": Type mismatched."<<endl;
            }
            }
            Stmt(node->children[4],t);  
    }
}

/*
DefList : Def DefList
        | 

*/

void DefList(struct syntaxTree* node,bool isInStruct, Type &ifStruct){
    if(node==nullptr) return ;
    if(node->line==-1 || node->childrenNum==0|| node->children[0]==nullptr) return ;
    if(node->line!=-1){
        Def(node->children[0],isInStruct,ifStruct);
        DefList(node->children[1],isInStruct,ifStruct);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
}
//Def: Specifier DecList SEMI
void Def(struct syntaxTree* node,bool isInStruct, Type &ifStruct){
    if(node==nullptr) return ;
    Type t = Specifiers(node->children[0]);
    if(t!=nullptr)
        DecList(node->children[1],t,isInStruct,ifStruct);
    else{
        return;
    }
}
/*
DecList : Dec
| Dec COMMA DecList
*/
void DecList(struct syntaxTree*node, Type t, bool isInStruct,  Type &ifStruct){
    if(node==nullptr) return ;
    if(node->childrenNum==1){
        Dec(node->children[0],t,isInStruct,ifStruct);
    }
    else{
        Dec(node->children[0],t,isInStruct,ifStruct);
        DecList(node->children[2],t,isInStruct,ifStruct);
    }
}
/*
Dec : VarDec
| VarDec ASSIGNOP EXP
*/
void Dec(struct syntaxTree*node, Type t, bool isInStruct, Type &ifStruct){
    if(node==nullptr) return ;
    if(isInStruct && node->childrenNum==3){
        cerr<<"Error type 15 at Line "<< node->children[0]->line<<": Type mismatched for assignment."<<endl;
        //return;
    }
    if(node->childrenNum==1){
        VarDec(node->children[0],t,isInStruct,ifStruct);
    }
    else{
        VarDec(node->children[0],t,isInStruct, ifStruct);
        Type temp = Exp(node->children[2]);
        if(!isMatched(t,temp)){
        cerr<<"Error type 5 at Line "<< node->children[0]->line<<": Type mismatched for assignment."<<endl;
        }
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
Type Exp(struct syntaxTree* node){
    if(node==nullptr) return nullptr;
    int sonNum = node->childrenNum;
    string first,second;
    Type res=new Type_();
    first = node->children[0]->name;
    if(sonNum>1){
        second = node->children[1]->name;
    }
    if(sonNum==1){
        if(first=="INT"){
            res->kind=Type_::BASIC;
            res->u.basic=BASIC::INT;
            res->side=Type_::right;
            return res;
        }
        else if(first == "FLOAT"){
            res->kind=Type_::BASIC;
            res->u.basic=BASIC::FLOAT;
            res->side=Type_::right;
            return res;
        }
        else{
            auto it = symbolMap.find(string(node->children[0]->typeVar));
            if(it==symbolMap.end()){
                cerr<<"Error type 1 at Line "<< node->children[0]->line<<": Undefined Var."<<endl;
                return nullptr;
            }
            typeCopy(res,it->second);
            res->side = Type_::left;
            return res;
        }
    }
    else if(sonNum==2){
        if(first=="MINUS"){
            Type op = Exp(node->children[1]);
            if(op==nullptr) return op;
            if(op->kind!=Type_::BASIC){
                cerr<<"Error type 7 at Line "<< node->children[1]->line<<": Type mismatched for operands."<<endl;
                return nullptr;
            }
            typeCopy(res,op);
            res->side=Type_::right;
            return res;
        }
        else{
            Type op = Exp(node->children[1]);
            if(op==nullptr) return op;
            if(op->kind!=Type_::BASIC || op->u.basic!=INT){
                cerr<<"Error type 7 at Line "<< node->children[1]->line<<": Type mismatched for operands."<<endl;
                return nullptr;
            }
            typeCopy(res,op);
            res->side=Type_::right;
            return res;
        }
    }
    else if((sonNum==3)){
        if(first=="Exp" && string(node->children[2]->name)=="Exp"){
            if(second=="ASSIGNOP"){
                //cout<<1;
                Type op1 = Exp(node->children[0]);
                Type op2 = Exp(node->children[2]);
                if(op1==nullptr || op2==nullptr){
                    return nullptr;
                }
                if(!isMatched(op1,op2)){
                    cerr<<"Error type 5 at Line "<< node->children[0]->line<<": Type mismatched for assignment."<<endl;
                    return nullptr;
                }
                if(op1->side==Type_::right){
                    cerr<<"Error type 6 at Line "<< node->children[0]->line<<": Type mismatched for assignment."<<endl;
                    return nullptr;
                }
                typeCopy(res,op1);
                res->side = Type_::left;
            }
            else if(second=="AND"||second=="OR"||second=="RELOP"||second=="STAR"||second=="DIV"||second=="PLUS"||second=="MINUS"){
                Type op1 = Exp(node->children[0]);
                Type op2 = Exp(node->children[2]);
                if(op1==nullptr || op2==nullptr){
                    return nullptr;
                }
                if(!isMatched(op1,op2)||!(op1->kind==Type_::BASIC&&op2->kind==Type_::BASIC)){
                    cerr<<"Error type 7 at Line "<< node->children[0]->line<<": Type mismatched for assignment."<<endl;
                    return nullptr;
                }
                if(second=="RELOP"){
                    res->kind=Type_::BASIC;
                    res->u.basic=INT;
                    res->side = Type_::right;
                    return res;
                }
                else if(second=="AND" ||second=="OR"){
                    if(!(op1->u.basic==INT && op2->u.basic==INT)){
                        cerr<<"Error type 7 at Line "<< node->children[0]->line<<": Type mismatched for assignment."<<endl;
                    return nullptr;
                    }
                    res->kind=Type_::BASIC;
                    res->u.basic=INT;
                    res->side = Type_::right;
                    return res;
                }
                else{
                    typeCopy(res,op1);
                    res->side =Type_::right;
                    return res;
                }
            }
        }
        else if(first=="LP" && second=="Exp"){
            Type op =  Exp(node->children[1]);
            if(op==nullptr) return op;
            typeCopy(res,op);
            res->side=Type_::right;
            return res;
        }
        else if(first=="ID" && second=="LP"){
            auto it = funcMap.find(string(node->children[0]->typeVar));
            if(it==funcMap.end()){
                if(symbolMap.find(string(node->children[0]->typeVar))==symbolMap.end()){
                    cerr<<"Error type 2 at Line "<< node->children[0]->line<<": Undefined function."<<endl;
                    return nullptr;
                }
                
                else{
                    cerr<<"Error type 11 at Line "<< node->children[0]->line<<": Not A function."<<endl;
                    return nullptr;        
                }
            }
            else if(it->second->varLi.size()!=0){
                cerr<<"Error type 9 at Line "<< node->children[0]->line<<": Function is not applicable for arguments"<<endl;
                return nullptr;
            }
            else {
               typeCopy( res ,it->second->returnType);
                res->side=Type_::right;
                return res;
            }
        }
        else if(first=="Exp" && second == "DOT"){
            Type op1 = Exp(node->children[0]);
            if(op1==nullptr) return nullptr;
            if(op1->kind!=Type_::STRUCTURE){
                cerr<<"Error type 13 at Line "<< node->children[0]->line<<": Not A structure."<<endl;
                return nullptr;
            }
            bool flag = false;
            for(int i=0;i<op1->u.structure->fieldList.size();i++){
                if(string(node->children[2]->typeVar)==op1->u.structure->fieldList.at(i).name){
                    flag = true;
                    break;
                }
            }
            //if(flag==false) return nullptr;
            if(flag==false){
                cerr<<"Error type 14 at Line "<< node->children[0]->line<<": Undefined In structure."<<endl;
                return nullptr;
                
            }
            typeCopy(res , symbolMap.find(string(node->children[2]->typeVar))->second);
            res->side=Type_::left;
            return res;
        }
    }
    else if(sonNum==4){
        if(first=="ID"){ //有参函数
            auto it = funcMap.find(string(node->children[0]->typeVar));
            if(it==funcMap.end()){
                if(symbolMap.find(string(node->children[0]->typeVar))==symbolMap.end()){
                    cerr<<"Error type 2 at Line "<< node->children[0]->line<<": Undefined function."<<endl;
                    return nullptr;
                }
                else{
                    cerr<<"Error type 11 at Line "<< node->children[0]->line<<": Not A function."<<endl;
                    return nullptr;        
                }
            }
            else {
                vector <Type> T ;
                Args(node->children[2],T);
                if(argsF==true){
                    argsF=false;
                    return nullptr;
                }
                if(T.size()!=it->second->varLi.size()){
                    //if(num==0) return nullptr;
                     cerr<<"Error type 9 at Line "<< node->children[0]->line<<": Function is not applicable for arguments"<<endl;
                     num++;
                     return nullptr;
                }
                for(int i=0;i<T.size();i++){
                    if(!isMatched(T.at(i),it->second->varLi.at(i).type)){
                        //if(num==0) return nullptr;
                        cerr<<"Error type 9 at Line "<< node->children[0]->line<<": Function is not applicable for arguments"<<endl;
                        num++;
                        return nullptr;
                    }
                }
            }
            typeCopy(res,it->second->returnType);
            res->side=Type_::right;
            return res;
        }
        else if(first=="Exp"){ //数组
            Type op1 = Exp(node->children[0]);
            Type op2 = Exp(node->children[2]);
            if(op1==nullptr||op2==nullptr) return nullptr;
            if(op1->kind!=Type_::ARRAY){
                cerr<<"Error type 10 at Line "<< node->children[0]->line<<": Not An array"<<endl;
                return nullptr;
            }
            if(op2->u.basic!=INT || op2->kind!=Type_::BASIC){
                cerr<<"Error type 12 at Line "<< node->children[0]->line<<": Not An Int"<<endl;
                return nullptr;
            }
            typeCopy(res , op1->u.array.elem);
            res->side = Type_::left;
            return res;
        }
    }
    return res;
}

/*
Args -> Exp COMMA Args
| Exp
*/
void Args(struct syntaxTree* node,vector<Type> &T){
    if(node==nullptr) return ;
    Type temp = Exp(node->children[0]);
    if(temp==nullptr) 
    {
        argsF = true;
        return;
    }
    T.push_back(temp);
    if(node->childrenNum>1){
        Args(node->children[2],T);
    }
}