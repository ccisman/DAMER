//
// Created by cc on 2020/8/4.
//

#ifndef PDNET_CHECKER_CPN_H
#define PDNET_CHECKER_CPN_H

#endif //PDNET_CHECKER_CPN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string.h>
#include "AST.h"
#include "y.tab.h"
#include "condition_tree.h"
#include <gperftools/tcmalloc.h>
#include <gperftools/malloc_extension.h>
#define H1FACTOR 13

using namespace std;


typedef unsigned short VARID;
typedef unsigned short SORTID;
//typedef unsigned short COLORID;
typedef unsigned short NUM_t;
typedef int Integer_t;
typedef double Real_t;
typedef string String_t;
typedef unsigned short MS_size_t;
typedef unsigned short token_count_t;
typedef unsigned short index_t;


class CPN;
class SortTable;
struct mapsort_info;
extern SortTable sorttable;
//extern NUM_t placecount;
//extern NUM_t transitioncount;
//extern NUM_t varcount;
extern CPN *cpnet;

enum type{dot,finiteintrange,productsort,usersort,Integer,Real,String};
enum Arc_Type{executed,control,call_enter,call_exit,data};


/*========================Sort==========================*/
class Sort
{
public:
    type mytype;
    virtual ~Sort(){};
};

class ProductSort:public Sort
{
public:
    string id;
    int sortnum;
    vector<string> sortname;
    vector<string> membername;//used for 'struct,union' in program
    vector<mapsort_info> sortid;
};

//UserSort is a user declared enumerable sort
//class UserSort:public Sort
//{
//public:
//    string id;
//    SHORTNUM feconstnum;
//    vector<string> cyclicenumeration;
//    map<string,COLORID> mapValue;
//
//    COLORID getColorIdx(string color) {
//        map<string,COLORID>::iterator iter;
//        iter = mapValue.find(color);
//        if(iter!=mapValue.end()) {
//            return iter->second;
//        }
//        else {
//            cerr<<"Can not find color"<<color<<"in colorset"<<this->id<<endl;
//            exit(-1);
//        }
//    }
//};

//DotSort
//class DotSort:public Sort
//{
//public:
//    //empty
//};
//
////IntegerSORT
//class IntegerSort:public Sort
//{
//public:
//    //empty
//};
//
////StringSORT
//class StringSort:public Sort
//{
//public:
//    //empty
//};

//class FiniteIntRange:public Sort
//{
//public:
//    string id;
//    int start;
//    int end;
//};

/*==========================SortValue=========================*/
/*==================Abstract Base Class==================*/
class SortValue
{
public:
    virtual ~SortValue(){};
//    virtual void setColor(COLORID cid)=0;
//    virtual void getColor(COLORID &cid)=0;
    virtual void setColor(SortValue **cid,int size)=0;
    virtual void setColor(Integer_t cid)=0;
    virtual void setColor(Real_t cid)=0;
    virtual void setColor(String_t cid)=0;
    virtual void getColor(SortValue **cid,int size)=0;
    virtual void getColor(Integer_t &cid)=0;
    virtual void getColor(Real_t &cid)=0;
    virtual void getColor(String_t &cid)=0;
};

/*====================ProductSortValue================*/
class ProductSortValue:public SortValue
{
private:
    //this is a index;
    SortValue **valueindex;
public:
    ProductSortValue(int sortnum) {
        valueindex = new SortValue*[sortnum];
    }
    ~ProductSortValue() {
        delete [] valueindex;
    }

//    void setColor(COLORID cid){};
//    void getColor(COLORID &cid){};
    void setColor(SortValue **cid,int size) {
        memcpy(valueindex,cid,sizeof(SortValue*)*size);
    }
    void getColor(SortValue **cid,int size) {
        memcpy(cid,valueindex,sizeof(SortValue*)*size);
    }
    virtual void setColor(Integer_t cid){};
    virtual void setColor(Real_t cid){};
    virtual void setColor(String_t cid){};
    virtual void getColor(Integer_t &cid){};
    virtual void getColor(Real_t &cid){};
    virtual void getColor(String_t &cid){};
};

/*===================UserSortValue=================*/
//class UserSortValue:public SortValue
//{
//private:
//    //this is a index;
//    COLORID colorindex;
//public:
//    virtual void setColor(COLORID cid) {colorindex = cid;}
//    virtual void getColor(COLORID &cid) {cid = colorindex;}
//    virtual void setColor(type_union *cid,int size){};
//    virtual void getColor(type_union *cid,int size){};
//    virtual void setColor(int cid){};
//    virtual void setColor(double cid){};
//    virtual void setColor(char *cid){};
//    virtual void getColor(int &cid){};
//    virtual void getColor(double &cid){};
//    virtual void getColor(char *cid){};
//    ~UserSortValue(){};
//};

/*==================FIRSortValue==================*/
//class FIRSortValue:public SortValue
//{
//public:
//    int value;
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
//    virtual void setColor(type_union *cid,int size){};
//    virtual void getColor(type_union *cid,int size){};
//    virtual void setColor(int cid){};
//    virtual void setColor(double cid){};
//    virtual void setColor(char *cid){};
//    virtual void getColor(int &cid){};
//    virtual void getColor(double &cid){};
//    virtual void getColor(char *cid){};
//    ~FIRSortValue(){};
//};

/*==================DotSortValue==================*/
//class DotSortValue:public SortValue
//{
//private:
//    COLORID value;
//public:
//    void setColor(COLORID cid){value = cid;};
//    void getColor(COLORID &cid){cid = value;};
//    virtual void setColor(COLORID *cid,int size){};
//    virtual void getColor(COLORID *cid,int size){};
//};

/*==================Integer SortValue==================*/
class IntegerSortValue:public SortValue
{
private:
    Integer_t value;
public:
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
    virtual void setColor(SortValue **cid,int size){};
    virtual void getColor(SortValue **cid,int size){};
    virtual void setColor(Integer_t cid){value = cid;};
    virtual void setColor(Real_t cid){};
    virtual void setColor(String_t cid){};
    virtual void getColor(Integer_t &cid){cid = value;};
    virtual void getColor(Real_t &cid){};
    virtual void getColor(String_t &cid){};
};

/*==================Real SortValue==================*/
class RealSortValue:public SortValue
{
private:
    Real_t value;
public:
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
    virtual void setColor(SortValue **cid,int size){};
    virtual void getColor(SortValue **cid,int size){};
    virtual void setColor(Integer_t cid){};
    virtual void setColor(Real_t cid){value = cid;};
    virtual void setColor(String_t cid){};
    virtual void getColor(Integer_t &cid){};
    virtual void getColor(Real_t &cid){cid = value;};
    virtual void getColor(String_t &cid){};
};

/*==================String SortValue==================*/
class StringSortValue:public SortValue
{
private:
    String_t value;
public:
//    virtual void setColor(COLORID cid){};
//    virtual void getColor(COLORID &cid){};
    virtual void setColor(SortValue **cid,int size){};
    virtual void getColor(SortValue **cid,int size){};
    virtual void setColor(Integer_t cid){};
    virtual void setColor(Real_t cid){};
    virtual void setColor(String_t cid){value = cid;};
    virtual void getColor(Integer_t &cid){};
    virtual void getColor(Real_t &cid){};
    virtual void getColor(String_t &cid){cid = value;};
    ~StringSortValue(){};
};


class Tokens
{
public:
    SortValue *color;
    token_count_t tokencount;
    Tokens *next;

    Tokens() {color = NULL;tokencount=0;next=NULL;}
    ~Tokens() {
        if(color!=NULL)
            delete color;
    }
    void initiate(token_count_t tc,type sort,int PSnum=0);
    void init_a_token(type tid,Integer_t value);
    void init_a_token(type tid,Real_t value);
    void init_a_token(type tid,String_t value);
};

//**Multiset contains tokens with the same type**//
class MultiSet
{
public:
    type tid;
    SORTID sid;
    Tokens *tokenQ;//queue contains head node
    MS_size_t color_count;
    index_t hash_value;

    MultiSet(){tokenQ = new Tokens;tokenQ->next=NULL;color_count = 0;}
    ~MultiSet(){
        Tokens *temp;
        while(tokenQ)
        {
            temp = tokenQ;
            tokenQ = tokenQ->next;
            delete temp;
        }
    }
    index_t Hash();
    void clear();
    void insert(Tokens *token);
    bool operator>=(const MultiSet &ms1);
    bool operator==(const MultiSet &ms);
    void operator=(const MultiSet &ms);
    void MINUS(MultiSet &ms);
    void PLUS(MultiSet &ms);

};

/*===========================================================*/
typedef struct mapsort_info
{
    //its index in corresponding vector
    SORTID sid;
    type tid;
} MSI;

//typedef struct mapcolor_info  //only used for cyclicenumeration
//{
//    SORTID sid;
//    COLORID cid;
//} MCI;

class SortTable
{
public:
    vector<ProductSort> productsort;
//    vector<UserSort> usersort;
//    vector<FiniteIntRange> finitintrange;
    bool hasdot;

    static SORTID psptr;

public:
    SortTable(){hasdot = false;}
    //first string is the sortname,the second MSI is this sort's information;
    map<string,MSI> mapSort;
//    map<string,MCI> mapColor;
//    SORTID getPSid(string PSname);
//    SORTID getUSid(string USname);
//    SORTID getFIRid(string FIRname);
    friend class CPN;
};


struct Variable
{
    string id;
    SORTID sid;
    SortValue *value;//assume variable is not a productsort
    type tid;

    bool operator == (const Variable &var) const {

    }
    bool operator < (const Variable &var) const {
//        if(this->vid < var.vid)
//            return true;
//        else
//            return false;
    }
};

/*========================Net Element========================*/
typedef struct CPN_Small_Arc
{
    index_t idx;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool onlydot;
} CSArc;

typedef struct CPN_Place
{
    string id;
//    type tid;
//    SORTID sid;
    MultiSet initMarking;
    vector<CSArc>producer;
    vector<CSArc>consumer;

    //***PDNet added start***/
    bool control_P;
    string expression;
    vector<string> enter;
    vector<string> exit;
    vector<string> false_exit;
    vector<string> enter_P;
    vector<pair<string,string>> para_list;
    //***PDNet added end***/

    CPN_Place(){expression="";}

    void printTokens(string &str);
//    ~CPN_Place(){
//        if(initMarking!=NULL)
//            delete [] initMarking;
//
//    }
} CPlace;

typedef struct CPN_Transition
{
    string id;
    condition_tree guard;
    bool hasguard;
    vector<CSArc> producer;
    vector<CSArc> consumer;
    set<string> relvars;
    //vector<Variable> relvararray;

    //***PDNet added start***/

    //***PDNet added end***/
} CTransition;

typedef struct CPN_Arc
{
    string id;
    bool isp2t;
    string source_id;
    string target_id;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool onlydot;
//    MultiSet arc_MS;
//    ~CPN_Arc() {
//        arc_exp.destructor(arc_exp.root);
//    }
} CArc;

/*========================Color_Petri_Net=======================*/
class CPN
{
public:
    CPlace *place;
    CTransition *transition;
    CArc *arc;

    Variable *vartable;
    NUM_t placecount;
    NUM_t transitioncount;
    NUM_t arccount;
    NUM_t varcount;
    map<string,index_t> mapPlace;
    map<string,index_t> mapTransition;
    map<string,VARID> mapVariable;
    map<string,string> mapFunction;

    //***PDNet added start***/
    void CTN_cal(condition_tree_node *CTN);
    void CT2MS(condition_tree ct,MultiSet &ms);
    void CTN2MS(condition_tree_node *ctn,MultiSet &ms);
    void CTN2COLOR(condition_tree_node *ctn,MultiSet &ms);
    void get_tuplecolor(condition_tree_node *ctn,vector<SortValue *> &color,vector<mapsort_info> sortid);
    //***PDNet added end***/

    CPN();


    //***PDNet added start***//
    void getDecl(gtree *tree);
    void initDecl();
    void init();
    void Add_Place(string id,string Type_name,int size,bool control_P,string exp);
    void Add_Transition(string id,string guard,string exp);
    void Add_Arc(string source,string target,string exp,bool sourceP,Arc_Type arcType);
    void Add_Arc_override(string source,string target,string exp,bool sourceP,Arc_Type arcType);
    void Add_Variable(string id,string related_P);
    void init_Place(string id,Tokens *token);
    void process_declaration(gtree *declaration,string base);
    void process_declarator(gtree *declarator, string tag, string base, bool para);
    void process_para_type_list(gtree *para_type_list, string base_Vname);
    void set_producer_consumer();
    void set_enter_P(string p_name,vector<string> enter_P);
    vector<string> get_enter_P(string p_name);
    void set_exit_T(string p_name,vector<string> exit_T);
    vector<string> get_exit_T(string p_name);
    void set_falseexit_T(string p_name,vector<string> exit_T);
    vector<string> get_falseexit_T(string p_name);
    void set_enter_T(string p_name,vector<string> enter_T);
    vector<string> get_enter_T(string p_name);

    //***PDNet added end***//
    ~CPN();
    void print_CPN(string filename);

    void create_PDNet(gtree *tree);
private:
};


//void Tokenscopy(Tokens &t1,const Tokens &t2,type tid,int PSnum=0);
extern string arr_suffix;
extern map<string,type> map_build_in_type;