//
// Created by cc on 2020/8/13.
//

#include "cpn.h"
#include "BA/xml2ltl.h"

template <class T>
bool exist_in(vector<T> vec,T element)
{
    for(unsigned int i=0;i<vec.size();i++)
        if(element == vec[i])
            return true;

    return false;
}

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
    Arc_Type exit_type,enter_type;
    if(arcType == call_enter) {
        exit_type = remain;
        enter_type = call_enter;
    }
    else if(arcType == call_exit) {
        exit_type = call_exit;
        enter_type = remain;
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

            if(place->control_P == true && searched[i]->producer[j].arcType != exit_type)
            {
                for(unsigned int k=0;k<place->producer.size();k++)
                {
                    if(place->producer[k].arcType == executed || place->producer[k].arcType == enter_type) {
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

void two_phrase_slicing(CPN *cpn, vector<string> place, vector<string> &final_P, vector<string>&final_T)
{
    vector<string> P, P_done, T;// , P_read;
    P = place;
    CPlace *p;
    //1.first phrase(not call_exit arcs)

    for(unsigned int i=0;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->place[iter->second];

        if(p->control_P)
        {
            //1.1 control dependence
            for(unsigned int j=0;j<p->consumer.size();j++)
            {
                if(p->consumer[j].arcType == control) {
                    string temp_T = cpn->transition[p->consumer[j].idx].id;
                    if(!exist_in(T,temp_T))
                        T.push_back(temp_T);
                }
            }
            for(unsigned int j=0;j<p->producer.size();j++)
            {
                if(p->producer[j].arcType == control) {
                    CTransition *trans = &cpn->transition[p->producer[j].idx];
                    if(!exist_in(T,trans->id))
                        T.push_back(trans->id);
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
            //1.2 data dependence
            for(unsigned int j=0;j<p->consumer.size();j++)
            {
                if(p->consumer[j].arcType == control)
                {
                    CTransition *trans = &cpn->transition[p->consumer[j].idx];
//                    vector<index_t> index_vec;
                    for(unsigned int k=0;k<trans->producer.size();k++)
                    {
                        if(trans->producer[k].arcType == data)
                        {
                            vector<CTransition *> trans_vec;
                            trans_vec = find_previous_define(trans,trans->producer[k].idx,cpn,call_exit);
                            for(unsigned int l=0;l<trans_vec.size();l++)
                            {
                                if (!exist_in(T, trans_vec[l]->id))
                                    T.push_back(trans_vec[l]->id);
                                for(unsigned int m=0;m<trans_vec[l]->producer.size();m++)
                                {
                                    if(trans_vec[l]->producer[m].arcType == control) {
                                        string temp_P = cpn->place[trans_vec[l]->producer[m].idx].id;
                                        if (!exist_in(P,temp_P))
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

    //2.second phrase(no call_enter arcs)
    for(unsigned int i=0;i<P.size();i++)
    {
        auto iter = cpn->mapPlace.find(P[i]);
        p = &cpn->place[iter->second];

        if(p->control_P)
        {
            //1.1 control dependence
            for(unsigned int j=0;j<p->consumer.size();j++)
            {
                if(p->consumer[j].arcType == control) {
                    string temp_T = cpn->transition[p->consumer[j].idx].id;
                    if(!exist_in(T,temp_T))
                        T.push_back(temp_T);
                }
            }
            for(unsigned int j=0;j<p->producer.size();j++)
            {
                if(p->producer[j].arcType == control) {
                    CTransition *trans = &cpn->transition[p->producer[j].idx];
                    if(!exist_in(T,trans->id))
                        T.push_back(trans->id);
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
            //1.2 data dependence
            for(unsigned int j=0;j<p->consumer.size();j++)
            {
                if(p->consumer[j].arcType == control)
                {
                    CTransition *trans = &cpn->transition[p->consumer[j].idx];
//                    vector<index_t> index_vec;
                    for(unsigned int k=0;k<trans->producer.size();k++)
                    {
                        if(trans->producer[k].arcType == data)
                        {
                            vector<CTransition *> trans_vec;
                            trans_vec = find_previous_define(trans,trans->producer[k].idx,cpn,call_enter);
                            for(unsigned int l=0;l<trans_vec.size();l++)
                            {
                                if (!exist_in(T, trans_vec[l]->id))
                                    T.push_back(trans_vec[l]->id);
                                for(unsigned int m=0;m<trans_vec[l]->producer.size();m++)
                                {
                                    if(trans_vec[l]->producer[m].arcType == control) {
                                        string temp_P = cpn->place[trans_vec[l]->producer[m].idx].id;
                                        if (!exist_in(P,temp_P))
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

    final_P = P;
    final_T = T;
}

//void post_process(C_Petri &petri, vector<string> change_P, vector<string> change_T, vector<Arc> &change_Arc)
//{
//    string main_begin = find_P_name(petri, "main begin");
//    for (unsigned i = 0; i < change_P.size(); i++)
//    {
//        bool flag = false;
//        if (change_P[i] != main_begin)
//        {
//            for (unsigned j = 0; j < change_Arc.size(); j++)
//            {
//                if (change_Arc[j].sourceP == false && change_Arc[j].target == change_P[i] && change_Arc[j].type != 6)
//                {
//                    flag = true;
//                    break;
//                }
//            }
//            if (flag == false)
//            {
//                vector<string> temp_v = forward_exist_T(petri, change_T, change_P[i]);
//                bool flag1 = false;
//                for (unsigned int j = 0; j < temp_v.size(); j++)
//                {
//                    Arc arc(temp_v[j], change_P[i], "executed", false);
//
//                    //in case of repeat add
//                    for (int k = int(change_Arc.size() - 1); k >= 0; k--)
//                        if (change_Arc[k].source == arc.source && change_Arc[k].target == arc.target)
//                        {
//                            flag1 = true;
//                            break;
//                        }
//                    if (flag1 == false)
//                    {
//
//                        change_Arc.push_back(arc);
//                    }
//                }
//            }
//        }
//    }
//}
