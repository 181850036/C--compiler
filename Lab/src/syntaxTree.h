struct syntaxTree
{
    int line;
    char* name;
    struct syntaxTree *Left;
    struct syntaxTree *Right;
    struct syntaxTree *children[10];
    int childrenNum;
    union
    {
    char * typeVar;
    int typeInt;
    double typeFloat;
    };
};



#ifdef __cplusplus
extern "C"
{
#endif
    struct syntaxTree *createNode(char *name, int brotherNum,...);
    void printTREE(struct syntaxTree *a,int level);

#ifdef __cplusplus
}
#endif
