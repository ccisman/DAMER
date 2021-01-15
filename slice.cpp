//
// Created by cc on 2020/10/10.
//

#include "cpn.h"
#include "BA/tinyxml.h"
#include "v_table.h"

extern vector<V_Table *> v_tables;
extern string end_suffix,return_suffix,begin_suffix,join_suffix;


void GetLTLC(CPN *cpn, TiXmlElement *p, vector<string> &criteria)
{
    string value = p->Value();
    if (value == "all-paths")
    {
        GetLTLC(cpn, p->FirstChildElement(), criteria);
    }
    else if (value == "globally")
    {
//		outltl << "[](";
        GetLTLC(cpn, p->FirstChildElement(), criteria);
//		outltl << ")";
    }
    else if (value == "finally")
    {
//		outltl << "<>(";
        GetLTLC(cpn, p->FirstChildElement(), criteria);
//		outltl << ")";
    }
    else if (value == "next")
    {
//		outltl << "X(";
        GetLTLC(cpn, p->FirstChildElement(), criteria);
//		outltl << ")";
    }
    else if (value == "until")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
        if (mValue != "before")
        {
            cerr << "Error in XML file! The element until\'s first child is not before!" << endl;
            exit(-1);
        }
        if (nValue != "reach")
        {
            cerr << "Error in XML file! The element until\'s second child is not reach!" << endl;
            exit(-1);
        }
//		outltl << "(";
        GetLTLC(cpn, m->FirstChildElement(), criteria);
//		outltl << ")U(";
        GetLTLC(cpn, n->FirstChildElement(), criteria);
//		outltl << ")";
    }
    else if (value == "integer-le")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
//		outltl << "{";
        if (mValue == "tokens-count")
        {
//			outltl << "token-count(";
            TiXmlElement *left = m->FirstChildElement();
            while (left != NULL)
            {

//				outltl << left->GetText() << ",";
                string temp = left->GetText();
                temp = cpn->mapVariable.find(temp)->second;
                criteria.push_back(temp);
                if (temp == "")
                    break;
                char variable[50];
                strcpy(variable, temp.c_str());
                left->FirstChild()->SetValue(variable);
                left = left->NextSiblingElement();
            }
//			outltl << ")";
        }
        else if (mValue == "variable")
        {
//			outltl << "token-count(";

            //				outltl << left->GetText() << ",";
            string temp = m->GetText();
            temp = cpn->mapVariable.find(temp)->second;
            criteria.push_back(temp);
            if (temp == "")
            {
                cout << "error in MC!" << endl;
                exit(-1);
            }
            char variable[50];
            strcpy(variable, temp.c_str());
            m->SetValue("tokens-count");
            m->Clear();
            TiXmlElement *pNewNode = new TiXmlElement("place");
            pNewNode->SetValue("place");
            m->LinkEndChild(pNewNode);

            TiXmlText *pValue = new TiXmlText(variable);
            pNewNode->LinkEndChild(pValue);


//			outltl << ")";
        }
        else if (mValue == "integer-constant")
        {
//			outltl << m->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
//		outltl << "<=";
        if (nValue == "tokens-count")
        {
//			outltl << "token-count(";
            TiXmlElement *right = n->FirstChildElement();
            while (right != NULL)
            {
//				outltl << right->GetText() << ",";
                string temp = right->GetText();
                temp = cpn->mapVariable.find(temp)->second;
                criteria.push_back(temp);
                if (temp == "")
                    break;
                char variable[50];
                strcpy(variable, temp.c_str());
                right->FirstChild()->SetValue(variable);
                right = right->NextSiblingElement();
            }
//			outltl << ")";
        }
        else if (nValue == "variable")
        {
            string temp = n->GetText();
            temp = cpn->mapVariable.find(temp)->second;
            criteria.push_back(temp);
            if (temp == "")
            {
                cout << "error in MC!" << endl;
                exit(-1);
            }
            char variable[50];
            strcpy(variable, temp.c_str());
            n->SetValue("tokens-count");
            TiXmlElement *pNewNode = new TiXmlElement("place");
            pNewNode->SetValue(variable);
            n->LinkEndChild(pNewNode);

            TiXmlText *pValue = new TiXmlText(variable);
            pNewNode->LinkEndChild(pValue);
        }
        else if (nValue == "integer-constant")
        {
//			outltl << n->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
//		outltl << "}";
    }
}

void xml_trans_C(CPN *cpn, char *filename, char *to_file, vector<string> &criteria)
{

    //ofstream outltl("FormulaC.txt", ios::out);
    TiXmlDocument *doc = new TiXmlDocument(filename);
    if (!doc->LoadFile())
    {
        cerr << doc->ErrorDesc() << endl;
    }
    TiXmlElement *root = doc->RootElement();
    if (root == NULL)
    {
        cerr << "Failed to load file: no root element!" << endl;
        doc->Clear();
    }
    TiXmlElement *p = root->FirstChildElement();
    while (p != NULL)
    {
        TiXmlElement *id = p->FirstChildElement("id");

        TiXmlElement *formula = p->FirstChildElement("formula");
        GetLTLC(cpn, formula->FirstChildElement(), criteria);

        p = p->NextSiblingElement();
    }

    doc->SaveFile(to_file);
}

vector<string> get_criteria(CPN *cpn,string filename)
{
    vector<string> result;
    string xml = filename;
    xml.replace(xml.find("new.c"), 5, "LTL.xml");

    char xml1[100], xml2[100];
    strcpy(xml1, xml.c_str());
    xml.replace(xml.find("LTL.xml"), 7, "LTL1.xml");
    strcpy(xml2, xml.c_str());

    xml_trans_C(cpn, xml1, xml2, result);
//    for (int i = 0; i < cpn->placecount; i++)
//    {
//        string temp = petri.place[i].v_name;
//        if (temp == "main begin")
//        {
//            result.push_back(petri.place[i].name);
//            break;
//        }
//    }
    return result;
}

vector<CTransition *> find_previous_define(CTransition  *trans,index_t var_index,CPN *cpn,Arc_Type arcType)
{
    vector<CTransition *> searched,res;
    searched.push_back(trans);
    Arc_Type allowed_type;
    if(arcType == call_enter) {
        allowed_type = call_exit;
    }
    else if(arcType == call_exit) {
        allowed_type = call_enter;
    }
    else
    {
        cout<<"two phrase algorithm is always lack a type of arc!"<<endl;
        exit(-1);
    }
    for(unsigned int i=0;i<searched.size();i++)
    {
        bool find_flag = false;
        for(unsigned int j=0;j<searched[i]->producer.size();j++)
        {
            if(searched[i]->producer[j].idx == var_index && searched[i]->producer[j].arcType == write)
            {
                res.push_back(searched[i]);
                find_flag = true;
                break;
            }
        }
        if(find_flag == true)
            continue;
        for(unsigned int j=0;j<searched[i]->producer.size();j++)
        {
            CPlace *place = &cpn->place[searched[i]->producer[j].idx];

            if(place->control_P == true)
            {
                for(unsigned int k=0;k<place->producer.size();k++)
                {
                    if(place->producer[k].arcType == executed
                    || place->producer[k].arcType == call_connect
                    || place->producer[k].arcType == allowed_type) {
                        CTransition *t = &cpn->transition[place->producer[k].idx];
                        if(!exist_in(searched,t))
                            searched.push_back(t);
                    }
                }
            }
        }
    }
    return res;

}

bool contain_path(CTransition *src,CTransition *tgt,CPN *cpn,string global_P)
{
    vector<CTransition *> searched,res;
    searched.push_back(src);

    auto iter = cpn->mapPlace.find(global_P);
    index_t idx = iter->second;

    for(unsigned int i=0;i<searched.size();i++)
    {
        for(unsigned int j=0;j<searched[i]->consumer.size();j++)
        {
            if(searched[i]->consumer[j].arcType == write
                    && searched[i]->consumer[j].idx == idx)
                return false;
        }
        if(searched[i] == tgt)
            return true;
        CTransition *tran = searched[i];
        for(unsigned int j=0;j>tran->consumer.size();j++)
        {
            if(tran->consumer[j].arcType == executed
            || tran->consumer[j].arcType == call_enter
            || tran->consumer[j].arcType == call_exit)
            {
                CPlace *place = &cpn->place[tran->consumer[j].idx];
                for(unsigned int k=0;k<place->consumer.size();k++)
                {
                    if(!exist_in(searched,&cpn->transition[place->consumer[k].idx]))
                        searched.push_back(&cpn->transition[place->consumer[k].idx]);
                }
            }
        }
    }
    return false;
}

bool is_global(string var)
{
    if(var.find(return_suffix)!=string::npos)
        return false;
    if (v_tables[0]->find_P(var))
        return true;
    else
        return false;
}

bool in_a_thread(CTransition *t1,CTransition *t2)
{
    return false;
}

void two_phrase_slicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string>&final_T)
{
    vector<string> P, P_done, T;// , P_read;
    P = place;
    CPlace *p;

    for(unsigned int i=0;i<place.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->place[iter->second];

        //T'=T' U {p.}
        for(unsigned int j=0;j<p->consumer.size();j++)
        {
            CTransition *tran = &cpn->transition[p->consumer[j].idx];
            T.push_back(tran->id);
            for(unsigned int k=0;k<tran->producer.size();k++)
            {
                if(tran->producer[k].arcType == control)
                    if(!exist_in(P,cpn->place[tran->producer[k].idx].id))
                        P.push_back(cpn->place[tran->producer[k].idx].id);
            }
        }
    }

    //first phrase
    for(unsigned int i=0;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->place[iter->second];
        if(p->control_P && p->is_executed == false)
            ;
        else
            continue;
        //cor_P
        for(unsigned int j=0;j<p->consumer.size();j++)
        {
            CTransition *tran = &cpn->transition[p->consumer[j].idx];
            if(!exist_in(T,tran->id))
                T.push_back(tran->id);
            for(unsigned int k=0;k<tran->producer.size();k++)
            {
                if(tran->producer[k].arcType == control)
                    if(!exist_in(P,cpn->place[tran->producer[k].idx].id))
                        P.push_back(cpn->place[tran->producer[k].idx].id);
            }
        }
        vector<string> cor_P = cpn->get_correspond_P(P[i]);
        for(unsigned int j=0;j<cor_P.size();j++)
            if (!exist_in(P, cor_P[j]))
                P.push_back(cor_P[j]);

        //control dependence

        for(unsigned int j=0;j<p->producer.size();j++)
        {
            if(p->producer[j].arcType == control
            || p->producer[j].arcType == call_connect
            || p->producer[j].arcType == call_enter) {
                CTransition *trans = &cpn->transition[p->producer[j].idx];
//                if(!exist_in(T,trans->id))
//                    T.push_back(trans->id);
                for(unsigned int k=0;k<trans->producer.size();k++)
                {
                    if(trans->producer[k].arcType == control) {
                        string temp_P = cpn->place[trans->producer[k].idx].id;
                        if (!exist_in(P, temp_P))
                            P.push_back(temp_P);
                    }
                }
            }
        }

//        if(p->is_mutex)
//        {
//            for(unsigned int j=0;j<p->consumer.size();j++)
//            {
//                if(p->consumer[j].arcType == control) {
//                    CTransition *trans = &cpn->transition[p->consumer[j].idx];
//                    if(!exist_in(T,trans->id))
//                        T.push_back(trans->id);
//                    for(unsigned int k=0;k<trans->producer.size();k++)
//                    {
//                        if(trans->producer[k].arcType == control) {
//                            string temp_P = cpn->place[trans->producer[k].idx].id;
//                            if (!exist_in(P, temp_P))
//                                P.push_back(temp_P);
//                        }
//                    }
//                }
//            }
//        }

        //data dependence
        for(unsigned int j=0;j<p->consumer.size();j++) {
            if (p->consumer[j].arcType == control) {
                CTransition *trans = &cpn->transition[p->consumer[j].idx];
//                    vector<index_t> index_vec;
//                if(!exist_in(T,trans->id))
//                    T.push_back(trans->id);
                for (unsigned int k = 0; k < trans->producer.size(); k++) {
                    if (trans->producer[k].arcType == data) {
                        vector<CTransition *> trans_vec;
                        string var = cpn->place[trans->producer[k].idx].id;
                        if (is_global(var))
                        {
                            CPlace *global_v_P = &cpn->place[trans->producer[k].idx];
                            if (!exist_in(P, global_v_P->id))
                                P.push_back(global_v_P->id);
                            for (unsigned int l = 0; l < global_v_P->producer.size(); l++) {
                                if (global_v_P->producer[l].arcType == write) {
                                    CTransition *t = &cpn->transition[global_v_P->producer[l].idx];
                                    if(in_a_thread(t,trans)) {
                                        if (contain_path(t, trans, cpn, global_v_P->id)) {
//                                            if (!exist_in(T, t->id))
//                                                T.push_back(t->id);
                                            for (unsigned int m = 0; m < t->producer.size(); m++) {
                                                if (t->producer[m].arcType == control
                                                    || t->producer[m].arcType == call_connect) {
                                                    if (!exist_in(P, cpn->place[t->producer[m].idx].id))
                                                        P.push_back(cpn->place[t->producer[m].idx].id);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
//                                        if (!exist_in(T, t->id))
//                                            T.push_back(t->id);
                                        for (unsigned int m = 0; m < t->producer.size(); m++) {
                                            if (t->producer[m].arcType == control
                                                || t->producer[m].arcType == call_connect) {
                                                if (!exist_in(P, cpn->place[t->producer[m].idx].id))
                                                    P.push_back(cpn->place[t->producer[m].idx].id);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            trans_vec = find_previous_define(trans, trans->producer[k].idx, cpn, call_exit);
                            if (trans_vec.size() != 0 && !exist_in(P, var))
                                P.push_back(var);
                            for (unsigned int l = 0; l < trans_vec.size(); l++) {
//                                if (!exist_in(T, trans_vec[l]->id))
//                                    T.push_back(trans_vec[l]->id);
                                for (unsigned int m = 0; m < trans_vec[l]->producer.size(); m++) {
                                    if (trans_vec[l]->producer[m].arcType == control) {
                                        string temp_P = cpn->place[trans_vec[l]->producer[m].idx].id;
                                        if (!exist_in(P, temp_P))
                                            P.push_back(temp_P);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //second phrase
    for(unsigned int i=0;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->place[iter->second];
        if(p->control_P && p->is_executed == false)
            ;
        else
            continue;
        //cor_P
        for(unsigned int j=0;j<p->consumer.size();j++)
        {
            CTransition *tran = &cpn->transition[p->consumer[j].idx];
            if(!exist_in(T,tran->id))
                T.push_back(tran->id);
            for(unsigned int k=0;k<tran->producer.size();k++)
            {
                if(tran->producer[k].arcType == control)
                    if(!exist_in(P,cpn->place[tran->producer[k].idx].id))
                        P.push_back(cpn->place[tran->producer[k].idx].id);
            }
        }
        vector<string> cor_P = cpn->get_correspond_P(P[i]);
        for(unsigned int j=0;j<cor_P.size();j++)
            if (!exist_in(P, cor_P[j]))
                P.push_back(cor_P[j]);

        //control dependence

        for(unsigned int j=0;j<p->producer.size();j++)
        {
            if(p->producer[j].arcType == control
               || p->producer[j].arcType == call_connect
               || p->producer[j].arcType == call_enter) {
                CTransition *trans = &cpn->transition[p->producer[j].idx];
//                if(!exist_in(T,trans->id))
//                    T.push_back(trans->id);
                for(unsigned int k=0;k<trans->producer.size();k++)
                {
                    if(trans->producer[k].arcType == control) {
                        string temp_P = cpn->place[trans->producer[k].idx].id;
                        if (!exist_in(P, temp_P))
                            P.push_back(temp_P);
                    }
                }
            }
        }

//        if(p->is_mutex)
//        {
//            for(unsigned int j=0;j<p->consumer.size();j++)
//            {
//                if(p->consumer[j].arcType == control) {
//                    CTransition *trans = &cpn->transition[p->consumer[j].idx];
//                    if(!exist_in(T,trans->id))
//                        T.push_back(trans->id);
//                    for(unsigned int k=0;k<trans->producer.size();k++)
//                    {
//                        if(trans->producer[k].arcType == control) {
//                            string temp_P = cpn->place[trans->producer[k].idx].id;
//                            if (!exist_in(P, temp_P))
//                                P.push_back(temp_P);
//                        }
//                    }
//                }
//            }
//        }

        //data dependence
        for(unsigned int j=0;j<p->consumer.size();j++) {
            if (p->consumer[j].arcType == control) {
                CTransition *trans = &cpn->transition[p->consumer[j].idx];
//                    vector<index_t> index_vec;
//                if(!exist_in(T,trans->id))
//                    T.push_back(trans->id);
                for (unsigned int k = 0; k < trans->producer.size(); k++) {
                    if (trans->producer[k].arcType == data) {
                        vector<CTransition *> trans_vec;
                        string var = cpn->place[trans->producer[k].idx].id;
                        if (is_global(var))
                        {
                            CPlace *global_v_P = &cpn->place[trans->producer[k].idx];
                            if (!exist_in(P, global_v_P->id))
                                P.push_back(global_v_P->id);
                            for (unsigned int l = 0; l < global_v_P->producer.size(); l++) {
                                if (global_v_P->producer[l].arcType == write) {
                                    CTransition *t = &cpn->transition[global_v_P->producer[l].idx];
                                    if(in_a_thread(t,trans)) {
                                        if (contain_path(t, trans, cpn, global_v_P->id)) {
//                                            if (!exist_in(T, t->id))
//                                                T.push_back(t->id);
                                            for (unsigned int m = 0; m < t->producer.size(); m++) {
                                                if (t->producer[m].arcType == control
                                                    || t->producer[m].arcType == call_connect) {
                                                    if (!exist_in(P, cpn->place[t->producer[m].idx].id))
                                                        P.push_back(cpn->place[t->producer[m].idx].id);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
//                                        if (!exist_in(T, t->id))
//                                            T.push_back(t->id);
                                        for (unsigned int m = 0; m < t->producer.size(); m++) {
                                            if (t->producer[m].arcType == control
                                                || t->producer[m].arcType == call_connect) {
                                                if (!exist_in(P, cpn->place[t->producer[m].idx].id))
                                                    P.push_back(cpn->place[t->producer[m].idx].id);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            trans_vec = find_previous_define(trans, trans->producer[k].idx, cpn, call_enter);
                            if (trans_vec.size() != 0 && !exist_in(P, var))
                                P.push_back(var);
                            for (unsigned int l = 0; l < trans_vec.size(); l++) {
//                                if (!exist_in(T, trans_vec[l]->id))
//                                    T.push_back(trans_vec[l]->id);
                                for (unsigned int m = 0; m < trans_vec[l]->producer.size(); m++) {
                                    if (trans_vec[l]->producer[m].arcType == control) {
                                        string temp_P = cpn->place[trans_vec[l]->producer[m].idx].id;
                                        if (!exist_in(P, temp_P))
                                            P.push_back(temp_P);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

//    //correspond pthread_create and join
//    for(unsigned int i=0;i<P.size();i++) {
//        auto iter = cpn->mapJoin.find(P[i]);
//        if (iter != cpn->mapJoin.end()) {
//            auto pier = cpn->mapPlace.find(iter->second);
//            CPlace *pp = &cpn->place[pier->second];
//            for (unsigned int j = 0; j < pp->consumer.size(); j++) {
//                if (pp->consumer[j].arcType == control) {
//                    CTransition *tt = &cpn->transition[pp->consumer[j].idx];
//                    if (!exist_in(T, tt->id))
//                        T.push_back(tt->id);
//                    for (unsigned int k = 0; k < tt->producer.size(); k++)
//                        if (tt->producer[k].arcType == control) {
//                            string temp_p = cpn->place[tt->producer[k].idx].id;
//
//                            if (!exist_in(P, temp_p))
//                                P.push_back(temp_p);
//                        }
//                }
//            }
//        }
//    }
    final_P = P;
    final_T = T;
}

vector<CTransition *> find_previous_T(string executed_place,CPN *cpn,vector<string> transitions,bool underprocedure)
{
    vector<CTransition *> searched,res;
    auto iter = cpn->mapPlace.find(executed_place);
    CPlace *executed_P = &cpn->place[iter->second];
    for(unsigned int i=0;i<executed_P->producer.size();i++)
        searched.push_back(&cpn->transition[executed_P->producer[i].idx]);

    for(unsigned int i=0;i<searched.size();i++)
    {
        if(exist_in(transitions,searched[i]->id)) {
            res.push_back(searched[i]);
            continue;
        }
        for(unsigned int j=0;j<searched[i]->producer.size();j++)
        {
            CPlace *place = &cpn->place[searched[i]->producer[j].idx];

            if(place->control_P == true)
            {
                for(unsigned int k=0;k<place->producer.size();k++) {
                    if (underprocedure) {
                        if (place->producer[k].arcType == executed
                            || place->producer[k].arcType == call_connect) {
                            CTransition *t = &cpn->transition[place->producer[k].idx];
                            if (!exist_in(searched, t))
                                searched.push_back(t);
                        }
                    } else if (place->producer[k].arcType == executed
                               || place->producer[k].arcType == call_exit) {
                        CTransition *t = &cpn->transition[place->producer[k].idx];
                        if (!exist_in(searched, t))
                            searched.push_back(t);
                    }
                }
            }
        }
    }
    return res;
}

void post_process(CPN *cpn,CPN *cpn_slice,vector<string> transitions)
{

    for(int i=0;i<cpn_slice->placecount;i++)
    {
        if(cpn_slice->place[i].producer.size()==0)
//            if(cpn_slice->place[i].expression == executed_P_name && cpn_slice->place[i].producer.size()==0)
        {
            vector<CTransition*> trans_vec;
            string suffix = cpn_slice->place[i].expression;

            if(suffix.length()>end_suffix.length()
            && suffix.substr(suffix.length()-end_suffix.length()) == end_suffix)
                trans_vec = find_previous_T(cpn_slice->place[i].id,cpn,transitions,false);
            else
                trans_vec = find_previous_T(cpn_slice->place[i].id,cpn,transitions,true);
            for(unsigned int j=0;j<trans_vec.size();j++) {
                cpn_slice->Add_Arc(trans_vec[j]->id, cpn_slice->place[i].id, "1`tid", false, executed);
                CSArc csArc1,csArc2;
                csArc1.arcType = cpn_slice->arc[cpn_slice->arccount-1].arcType;
                csArc1.arc_exp = cpn_slice->arc[cpn_slice->arccount-1].arc_exp;
                csArc1.onlydot = cpn_slice->arc[cpn_slice->arccount-1].onlydot;
                csArc2.arcType = cpn_slice->arc[cpn_slice->arccount-1].arcType;
                csArc2.arc_exp = cpn_slice->arc[cpn_slice->arccount-1].arc_exp;
                csArc2.onlydot = cpn_slice->arc[cpn_slice->arccount-1].onlydot;
                csArc1.idx = cpn_slice->mapTransition.find(trans_vec[j]->id)->second;
                csArc2.idx = i;
                cpn_slice->place[i].producer.push_back(csArc1);
                cpn_slice->transition[csArc1.idx].consumer.push_back(csArc2);

            }
        }
    }
}
