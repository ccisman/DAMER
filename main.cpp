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
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait"};

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

void init_pthread_type()
{

    aka temp;
    for(unsigned int i=0;i<pthread_type.size();i++) {
        temp.origin_name = "";
        temp.aka_name = pthread_type[i];
        temp.level = 0;
        aka_type_array.push_back(temp);
    }
}

void sort_change(vector<string> &change_P)
{
    for (unsigned int i = 0; i < change_P.size(); i++)
    {
        for (unsigned int j = 0; j < change_P.size() - 1 - i; j++)
        {
            int num1 = atoi(change_P[j].substr(1).c_str());
            int num2 = atoi(change_P[j + 1].substr(1).c_str());
            if (num1 > num2)
            {
                string temp = change_P[j];
                change_P[j] = change_P[j + 1];
                change_P[j + 1] = temp;
            }
        }
    }
}

int main() {
    cout << "=================================================" << endl;
    cout << "=====This is our tool-enPAC for the MCC'2020=====" << endl;
    cout << "=================================================" << endl;


    cout<<"size="<<sizeof(MultiSet)<<endl;

    init_pthread_type();
    gtree * tree = create_tree("1.c",true);
    cut_tree(tree);
    intofile_tree(tree);
    makeGraph("tree.dot","tree.png");

    CPN *cpnet = new CPN;

    V_Table *table = new V_Table("global");
    table->fa == NULL;
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
    criteria.push_back("P15");
    two_phrase_slicing(cpnet,criteria,final_P,final_T);
    sort_change(final_T);
    sort_change(final_P);

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
