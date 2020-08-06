#include <iostream>
#include "AST.h"
#include "base.h"
#include "cpn.h"
#include<string.h>

string rg_dirname = "../rg/";
string rg_sliceOnly_dirname = "../rg_sliceOnly/";
string origin_dirname = "../test/";
string newfile_dirname = "../newfile/";

void travel_tree(gtree *tree,ofstream &out,vector<pair<string,int>> &list)
{

    if (tree == NULL)
        return;
    string parent_place, child_place;
    int parent_no, child_no;
    if (tree->parent != NULL)
    {

        parent_place = tree->parent->place;
        parent_no = tree->parent->num;
        child_place = tree->place;
        child_no = tree->num;
        out << "node" << to_string(parent_no) << "[label=\"" << parent_place << "\"]" << endl;
        out << "node" << to_string(child_no) << "[label=\"" << child_place << "\"]" << endl;
        out << "node" << to_string(parent_no) << "->" << "node" << to_string(child_no) << ";" << endl;

    }
    travel_tree(tree->child, out, list);
    travel_tree(tree->next, out, list);


    //out.close();
}

void intofile_tree(gtree *tree)
{
    ofstream out;
    vector<pair<string,int>> list;
    out.open("tree.dot", ios::out);

    out << "digraph G{" << endl;
    travel_tree(tree, out, list);
    out << "}" << endl;
    out.close();

}

int main() {
    cout << "=================================================" << endl;
    cout << "=====This is our tool-enPAC for the MCC'2020=====" << endl;
    cout << "=================================================" << endl;

    condition_tree CT;
    CT.construct("(2,x)++(3,x)");

    CPN *cpnet = new CPN;
    cpnet->init();
    cpnet->initDecl();

    string t = "int_arr";
    map<string,MSI>::iterator iter = sorttable.mapSort.find(t);
    MultiSet ms;
    ms.tid = iter->second.tid;
    ms.sid = iter->second.sid;
    Variable *var = &cpnet->vartable[0];
    var->tid = Integer;
    var->sid = 0;
    var->id = "x";
    var->value.integer = 7;
    cpnet->mapVariable.insert(make_pair("x",0));
    cpnet->CT2MS(CT,ms);

//    gtree * tree = create_tree("1.c",true);
//    intofile_tree(tree);
//    makeGraph("tree.dot","tree.png");
//
//    CPN *cpnet = new CPN;
//    cpnet->init();
//    cpnet->initDecl();
//    cpnet->getDecl(tree);
//
//    cpnet->Add_Place("P1","int",1,false,"x");
//    Tokens *token = new Tokens;
//    token->tokencount = 1;
//    token->initiate(1,Integer,0);
//    token->color->setColor(6);
//    cpnet->init_Place("P1",token);

    return 0;
}
