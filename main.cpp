#include <iostream>
#include "AST.h"
#include "base.h"
#include "cpn_rg.h"
#include "v_table.h"
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


    gtree * tree = create_tree("1.c",true);
    intofile_tree(tree);
    makeGraph("tree.dot","tree.png");

    CPN *cpnet = new CPN;

    V_Table *table = new V_Table("global");
    v_tables.push_back(table);

    cpnet->init();
    cpnet->initDecl();
    cpnet->getDecl(tree);
    cpnet->create_PDNet(tree);
    cpnet->set_producer_consumer();
    string filename_prefix = "1";

    cpnet->print_CPN(filename_prefix + ".txt");
    readGraph(filename_prefix + ".txt",filename_prefix + ".dot");
    makeGraph(filename_prefix + ".dot",filename_prefix + ".png");

    vector<string> final_P,final_T,criteria;
    criteria.push_back("P20");
    two_phrase_slicing(cpnet,criteria,final_P,final_T);

    CPN *cpnet_slice = new CPN;
    cpnet_slice->copy_childtree(cpnet,final_P,final_T);
    post_process(cpnet,cpnet_slice,final_T);
    filename_prefix = "2";
    cpnet_slice->print_CPN(filename_prefix + ".txt");
    readGraph(filename_prefix + ".txt",filename_prefix + ".dot");
    makeGraph(filename_prefix + ".dot",filename_prefix + ".png");

    RG rg;
    rg.init(cpnet);
    rg.GENERATE(cpnet);
    rg.print_RG("rg.txt",cpnet);

    RG rg1;
    rg1.init(cpnet_slice);
    rg1.GENERATE(cpnet_slice);
    rg1.print_RG("rg1.txt",cpnet_slice);
    return 0;
}
