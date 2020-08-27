#include <iostream>
#include "AST.h"
#include "base.h"
#include "cpn_rg.h"
#include "v_table.h"
#include "product.h"
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<dirent.h>

extern void Bubble_sort(vector<string> &change_P);

char LTLFfile[50] = "LTLFireability.xml";
char LTLVfile[50] = "LTLVariablity.xml";
char LTLCfile[50] = "LTLCardinality.xml";

string origin_dirname = "../test/";
string newfile_dirname = "../newfile/";
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait"};


extern void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria);
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
void CHECKLTL(CPN *cpnet, LTLCategory type,int num) {
    RG *graph = new RG;
    graph->init(cpnet);

    string propertyid;
//    char ff[]=LTLFfile;
//    char cc[]=LTLCfile;
//    char vv[]=LTLVfile;
    Syntax_Tree syntaxTree;
    if(type == LTLC)
        syntaxTree.ParseXML(LTLCfile,propertyid,num);
    else if(type == LTLF)
        syntaxTree.ParseXML(LTLFfile,propertyid,num);
    else if(type == LTLV)
        syntaxTree.ParseXML(LTLVfile,propertyid,num);
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

    ofstream out;
    out.open("result.txt",ios::out|ios::app);
    if (out.fail())
    {
        cout << "open result failed!" << endl;
        exit(1);
    }
    cout<<"graph node num: "<<graph->node_num<<endl;
    out<<"graph node num: "<<graph->node_num<<endl;
    out.close();
    delete product;
    delete graph;

}

void GetFileNames(string path,vector<string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0
        && strcmp(ptr->d_name, "..") != 0
        && strstr(ptr->d_name,".c")){
            filenames.push_back(ptr->d_name);
        }
    }
    closedir(pDir);
}

void construct_and_slice(string check_file,LTLCategory ltltype,int num)
{
    string filename;
    switch(ltltype)
    {
        case LTLF:
            filename = origin_dirname + check_file.substr(0,check_file.length()-2) + "-F.xml";
            strcpy(LTLFfile,filename.c_str());
            break;
        case LTLV:
            filename = origin_dirname + check_file.substr(0,check_file.length()-2) + "-V.xml";
            strcpy(LTLVfile,filename.c_str());
            break;
        case LTLC:
            //We can support LTLC ,but it is not meaningful;
            filename = origin_dirname + check_file.substr(0,check_file.length()-2) + "-C.xml";
            strcpy(LTLCfile,filename.c_str());
            break;
    }
    ofstream out;
    out.open("result.txt",ios::out|ios::app);
    if (out.fail())
    {
        cout << "open result failed!" << endl;
        exit(1);
    }
    out<<"current file: "<<check_file<<endl;
    cout<<"current file: "<<check_file<<endl;
    out<<endl;
    cout<<endl;

    clock_t start,finish;
    start = clock();
    gtree * tree = create_tree(check_file,true);
    cut_tree(tree);
//    intofile_tree(tree);
//    makeGraph("tree.dot","tree.png");

    CPN *cpnet = new CPN;

    v_tables.clear();
    init_v_table();

    cpnet->init();
    cpnet->initDecl();
    cpnet->getDecl(tree);
    cpnet->create_PDNet(tree);
    cpnet->set_producer_consumer();

    out<<"placenum: "<<cpnet->placecount<<endl;
    cout<<"placenum: "<<cpnet->placecount<<endl;
    out<<"transnum: "<<cpnet->transitioncount<<endl;
    cout<<"transnum: "<<cpnet->transitioncount<<endl;
//    string filename_prefix = "1";
//    cpnet->print_CPN(filename_prefix + ".txt");
//    readGraph(filename_prefix + ".txt",filename_prefix + ".dot");
//    makeGraph(filename_prefix + ".dot",filename_prefix + ".png");
    CHECKLTL(cpnet,ltltype,num);
    finish = clock();
    out<<"time: "<<(finish-start)/1000000.0<<endl;
    out<<endl;

    vector<string> final_P,final_T,criteria;
    extract_criteria(num,ltltype,cpnet,criteria);

    start = clock();
    two_phrase_slicing(cpnet,criteria,final_P,final_T);
    Bubble_sort(final_T);
    Bubble_sort(final_P);

    CPN *cpnet_slice = new CPN;
    cpnet_slice->copy_childNet(cpnet,final_P,final_T);
    post_process(cpnet,cpnet_slice,final_T);
//    filename_prefix = "2";
//    cpnet_slice->print_CPN(filename_prefix + ".txt");
//    readGraph(filename_prefix + ".txt",filename_prefix + ".dot");
//    makeGraph(filename_prefix + ".dot",filename_prefix + ".png");


    out<<"placenum: "<<cpnet_slice->placecount<<endl;
    cout<<"placenum: "<<cpnet_slice->placecount<<endl;
    out<<"transnum: "<<cpnet_slice->transitioncount<<endl;
    cout<<"transnum: "<<cpnet_slice->transitioncount<<endl;
    CHECKLTL(cpnet_slice,ltltype,num);
    finish = clock();

    out<<"time: "<<(finish-start)/1000000.0<<endl;
    out<<endl;
    out.close();
    release_v_table();
    delete cpnet;
}

int main() {
    int num = 1;
    LTLCategory ltltype = LTLF;
    init_pthread_type();

//    gtree * tree = create_tree("triangular-2.c",true);
//    cut_tree(tree);
//    intofile_tree(tree);
//    makeGraph("tree.dot","tree.png");
//
//    CPN *cpnet = new CPN;
//
//    V_Table *table = new V_Table("global");
//    table->fa == NULL;
//    v_tables.push_back(table);
//
//    cpnet->init();
//    cpnet->initDecl();
//    cpnet->getDecl(tree);
//    cpnet->create_PDNet(tree);
//    cpnet->set_producer_consumer();
//    string filename_prefix = "1";
//
//    cpnet->print_CPN(filename_prefix + ".txt");
//    readGraph(filename_prefix + ".txt",filename_prefix + ".dot");
//    makeGraph(filename_prefix + ".dot",filename_prefix + ".png");
//
//    RG rg;
//    rg.init(cpnet);
//    rg.GENERATE(cpnet);
//    rg.print_RG("rg.txt",cpnet);

    vector<string> files;
    GetFileNames(origin_dirname,files);

//    for(unsigned int i=0;i<files.size();i++)
//        construct_and_slice(files[i],ltltype,num);
    construct_and_slice("triangular-2.c",ltltype,num);
    return 0;
}
