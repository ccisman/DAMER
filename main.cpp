#include <iostream>
#include "AST.h"
#include "base.h"
#include "cpn_rg.h"
#include "v_table.h"
#include "product.h"
#include<string.h>


string rg_dirname = "../rg/";
string rg_sliceOnly_dirname = "../rg_sliceOnly/";
string origin_dirname = "../test/";
string newfile_dirname = "../newfile/";
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait"};

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
void CHECKLTL(CPN *cpnet, bool cardinality,int num) {
    RG *graph = new RG;
    graph->init(cpnet);

    string propertyid;
    char ff[]="LTLFireability.xml";
    char cc[]="LTLCardinality.xml";
    Syntax_Tree syntaxTree;
    if(cardinality)
        syntaxTree.ParseXML(cc,propertyid,num);
    else
        syntaxTree.ParseXML(ff,propertyid,num);
        cout<<"original tree:"<<endl;
        syntaxTree.PrintTree();
        cout<<"-----------------------------------"<<endl;
    syntaxTree.Push_Negation(syntaxTree.root);
//        cout<<"after negation:"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;
    syntaxTree.SimplifyLTL();
//        cout<<"after simplification:"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;
    syntaxTree.Universe(syntaxTree.root);
//        cout<<"after universe"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;

    syntaxTree.Get_DNF(syntaxTree.root);
    syntaxTree.Build_VWAA();
    syntaxTree.VWAA_Simplify();

    General GBA;
    GBA.Build_GBA(syntaxTree);
    GBA.Simplify();
    GBA.self_check();

    Buchi BA;
    BA.Build_BA(GBA);
    BA.Simplify();
    BA.self_check();
    BA.Backward_chaining();
        BA.PrintBuchi("BA.dot");

    StateBuchi SBA;
    SBA.Build_SBA(BA);
    SBA.Simplify();
    SBA.Tarjan();
    SBA.Complete1();
    SBA.Add_heuristic();
    SBA.Complete2();
    SBA.self_check();
        SBA.PrintStateBuchi();
/*******************************************************/
    //cout << "begin:ON-THE-FLY" << endl;

    CPN_Product_Automata *product;
    product = new CPN_Product_Automata(cpnet, &SBA, graph);
    product->GetProduct();
    product->printresult(propertyid);
    //cout<<" "<<graph->nodecount<<endl;

    delete product;
    delete graph;
}


int main() {

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

//    vector<string> final_P,final_T,criteria;
//    criteria.push_back("P15");
//    two_phrase_slicing(cpnet,criteria,final_P,final_T);
//    Bubble_sort(final_T);
//    Bubble_sort(final_P);
//
//    CPN *cpnet_slice = new CPN;
//    cpnet_slice->copy_childtree(cpnet,final_P,final_T);
//    post_process(cpnet,cpnet_slice,final_T);
//    filename_prefix = "2";
//    cpnet_slice->print_CPN(filename_prefix + ".txt");
//    readGraph(filename_prefix + ".txt",filename_prefix + ".dot");
//    makeGraph(filename_prefix + ".dot",filename_prefix + ".png");

    CHECKLTL(cpnet,0,2);
//    RG rg;
//    rg.init(cpnet);
//    rg.GENERATE(cpnet);
//    rg.print_RG("rg.txt",cpnet);
//    RG rg1;
//    rg1.init(cpnet_slice);
//    rg1.GENERATE(cpnet_slice);
//    rg1.print_RG("rg1.txt",cpnet_slice);
    return 0;
}
