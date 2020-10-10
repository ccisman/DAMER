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
#include"pdnet_parseoption.h"
#include<cmath>
#include<sys/time.h>

extern void Bubble_sort(vector<string> &change_P);

char LTLFfile[50],LTLVfile[50],LTLCfile[50];

string origin_dirname = "../test/";
string newfile_dirname = "../newfile/";

extern void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria);
void CHECKLTL(CPN *cpnet, LTLCategory type,int num,int &rgnum,string &res) {
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
//    clock_t start=clock();
    CPN_Product_Automata *product;
    product = new CPN_Product_Automata(cpnet, &SBA, graph);
    product->GetProduct();
    product->printresult(propertyid);
//    clock_t end = clock();
//
//    cout<<"product time:"<<(start-end)/1000.0<<endl;
    cout<<"graph node num: "<<graph->node_num<<endl;
    rgnum = graph->node_num;
    res = product->GetResult();

    delete product;
    delete graph;

}

int fileNameFilter(const struct dirent *cur) {
    std::string str(cur->d_name);
    if (str.find(".c") != std::string::npos) {
        return 1;
    }
    return 0;
}

void GetFileNames(string path,vector<string>& ret)
{
    struct dirent **namelist;
    int n = scandir(path.c_str(), &namelist, fileNameFilter, alphasort);
    if (n < 0) {
        cout<<"There is no file!"<<endl;
        exit(-1);
    }
    for (int i = 0; i < n; ++i) {
        std::string filePath(namelist[i]->d_name);
        ret.push_back(filePath);
        free(namelist[i]);
    };
    free(namelist);
}

//****************************************************//
//*****There is two check step in this function*******//
//*****1.construct program's CPN and checking LTL*****//
//*****2.slice CPN and checking LTL again*************//
//****************************************************//
//*****check_file：program filename*******************//
//*****ltltype：check LTLF or LTLV（including LTLF）***//
//*****num：checked property num in the LTLFile*******//
void construct_and_slice(string check_file,LTLCategory ltltype,int num,bool gen_picture)
{
    string filename;
    int pre_P_num,pre_T_num,pre_rgnode_num,slice_P_num,slice_T_num,slice_rgnode_num;
    clock_t pre_time,slice_time,pre_model,pre_check,slice,slice_check;
    string pre_res,slice_res;
    double base_clock = 1000.0;

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
    out<<endl;
    cout<<endl;
    out<<"current file: "<<check_file<<endl;
    cout<<"current file: "<<check_file<<endl;
    out<<"formula:"<<num<<endl;
    cout<<"formula:"<<num<<endl;
    out<<endl;
    cout<<endl;

    clock_t start,finish;
    start = clock();

    //1.preprocess and build program's AST
    gtree * tree = create_tree(check_file,true);
    cut_tree(tree);
//    intofile_tree(tree);
//    makeGraph("tree.dot","tree.png");

    CPN *cpnet = new CPN;

    v_tables.clear();
    init_v_table();

    //2.construct program's CPN
    cpnet->init();
    cpnet->initDecl();
    cpnet->getDecl(tree);
    cpnet->create_PDNet(tree);
    cpnet->delete_compound(tree);
    cpnet->set_producer_consumer();

//    RG rg;
//    rg.init(cpnet);
//    rg.GENERATE(cpnet);
//    rg.print_RG("rg.txt",cpnet);

    out<<"placenum: "<<cpnet->placecount<<endl;
    cout<<"placenum: "<<cpnet->placecount<<endl;
    out<<"transnum: "<<cpnet->transitioncount<<endl;
    cout<<"transnum: "<<cpnet->transitioncount<<endl;
    pre_P_num = cpnet->placecount;
    pre_T_num = cpnet->transitioncount;


    clock_t direct_build_end = clock();

    string filename_prefix;
    if(gen_picture) {
        filename_prefix = "1";
        cpnet->print_CPN(filename_prefix + ".txt");
        readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
        makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
    }
    //3.verify CPN's properties
    CHECKLTL(cpnet,ltltype,num,pre_rgnode_num,pre_res);
    finish = clock();

    pre_model = (direct_build_end - start);
    pre_check = (finish - direct_build_end);
    pre_time = pre_model + pre_check;

    out<<"direct build time: "<<setiosflags(ios::fixed)<<setprecision(3)<<pre_model/base_clock<<endl;
    out<<"model_check: "<<setiosflags(ios::fixed)<<setprecision(3)<<pre_check/base_clock<<endl;
    out<<"total time: "<<setiosflags(ios::fixed)<<setprecision(3)<<pre_time/base_clock<<endl;
    out<<endl;

    vector<string> final_P,final_T,criteria;

    //4.extract criteria from LTL file and generate “.txt” to describe formulas
    extract_criteria(num,ltltype,cpnet,criteria);

    start = clock();

    //5.slicing CPN
    two_phrase_slicing(cpnet,criteria,final_P,final_T);
    Bubble_sort(final_T);
    Bubble_sort(final_P);

    CPN *cpnet_slice = new CPN;
    cpnet_slice->copy_childNet(cpnet,final_P,final_T);

    //6.post_process
    post_process(cpnet,cpnet_slice,final_T);

    if(gen_picture) {
        filename_prefix = "2";
        cpnet_slice->print_CPN(filename_prefix + ".txt");
        readGraph(filename_prefix + ".txt", filename_prefix + ".dot");
        makeGraph(filename_prefix + ".dot", filename_prefix + ".png");
    }

    out<<"placenum: "<<cpnet_slice->placecount<<endl;
    cout<<"placenum: "<<cpnet_slice->placecount<<endl;
    out<<"transnum: "<<cpnet_slice->transitioncount<<endl;
    cout<<"transnum: "<<cpnet_slice->transitioncount<<endl;

    slice_P_num = cpnet_slice->placecount;
    slice_T_num = cpnet_slice->transitioncount;

    clock_t slice_end = clock();

    //7.verify sliced CPN's property
    CHECKLTL(cpnet_slice,ltltype,num,slice_rgnode_num,slice_res);
    finish = clock();

    slice = (slice_end - start);
    slice_check = (finish - slice_end);
    slice_time = slice + slice_check + pre_model;

    out<<"slice time: "<<setiosflags(ios::fixed)<<setprecision(3)<<slice/base_clock<<endl;
    out<<"model_check: "<<setiosflags(ios::fixed)<<setprecision(3)<<slice_check/base_clock<<endl;
    out<<"total time: "<<setiosflags(ios::fixed)<<setprecision(3)<<slice_time/base_clock<<endl;
    out<<endl;

    out<<setiosflags(ios::fixed)<<setprecision(3)<<"& \\emph{"<<pre_res<<"}\n& "<<pre_P_num<<"\n& "<<pre_T_num<<"\n& "<<pre_rgnode_num<<"\n& "<<pre_model/base_clock<<"\n& "<<pre_check/base_clock<<"\n& "<<pre_time/base_clock<<"\n& \\emph{"<<slice_res<<"}\n& "<<slice_P_num<<"\n& "<<slice_T_num<<"\n& "<<slice_rgnode_num<<"\n& "<<slice/base_clock<<"\n& "<<slice_check/base_clock<<"\n& "<<slice_time/base_clock<<"\\\\ \\cline{2-12}";
    out<<endl;
    out<<endl;
    out<<"criteria P : ";
    for(unsigned int i=0;i<criteria.size();i++)
        out<<criteria[i]<<",";
    out<<endl;
    out<<endl;
    out<<"deleted P : ";
    for(unsigned int i=0;i<cpnet->placecount;i++)
        if(!exist_in(final_P,cpnet->place[i].id)){
            out<<"$P_{";
            out<<cpnet->place[i].id.substr(1);
            out<<"}$,";
        }
    out<<endl;
    out<<endl;
    out<<"deleted T : ";
    for(unsigned int i=0;i<cpnet->transitioncount;i++)
        if(!exist_in(final_T,cpnet->transition[i].id))
            {
                out<<"$T_{";
                out<<cpnet->transition[i].id.substr(1)<<",";
                out<<"}$";
            }
    out<<endl;
    out.close();
    release_v_table();
    delete cpnet;
}

void docmd(cmdlinet &cmd){

}

int main(int argc,char **argv) {

    cmdlinet cmd;
    cmd.parse(argc,argv);
    init_pthread_type();
    docmd(cmd);

//    LTLCategory ltltype = LTLV;
//    string check_file_name;//it stands for checked file's name
//    int num;//it stands for checked property number
//
//    bool gen_picture = false;
//
//    construct_and_slice(check_file_name,ltltype,num,gen_picture);

/********    Testing.....    ***********/
//    vector<string> files;
//    get filename in origin_dirname
//    so we can check all the .c file in origin_dirname
//    GetFileNames(origin_dirname,files);
//    for(unsigned int i=0;i<files.size();i++)
//        for(unsigned int num=1;num<=3;num++)
//            construct_and_slice(files[i],ltltype,num,gen_picture);
    return 0;
}
