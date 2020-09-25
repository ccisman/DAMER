//
// Created by lewis on 2020/8/24.
//
#include<string>
#include<vector>
#include<iostream>
#include"BA/tinyxml.h"
#include"product.h"
#include"cpn.h"
#include"base.h"
using namespace std;

extern char LTLFfile[] ;
extern char LTLVfile[] ;
extern char LTLCfile[] ;
void GetLTL(ofstream &out,TiXmlElement *p, vector<string> &P,vector<string> &T)
{
    string value = p->Value();
    if (value == "all-paths")
    {
        GetLTL(out,p->FirstChildElement(), P,T);
    }
    else if (value == "globally")
    {
        out << "G(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "finally")
    {
        out << "F(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "next")
    {
        out << "X(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "negation")
    {
        out << "!(";
        GetLTL(out,p->FirstChildElement(), P,T);
        out << ")";
    }
    else if(value == "conjunction" || value == "disjunction")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        GetLTL(out,m, P, T);
        m = m->NextSiblingElement();
        while(m) {
            if(value == "conjunction")
                out << "&&";
            else
                out << "||";
            GetLTL(out,m, P, T);

            m = m->NextSiblingElement();
        }
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
        out << "(";
        GetLTL(out,m->FirstChildElement(), P,T);
        out << ")U(";
        GetLTL(out,n->FirstChildElement(), P,T);
        out << ")";
    }
    else if (value == "less" || value == "equality" || value == "lesseq")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
        out << "{";
        if (mValue == "token-value")
        {
            out << "token-value(";
            TiXmlElement *left = m->FirstChildElement();
            while (left != NULL)
            {

                out << left->GetText() ;
                string temp = left->GetText();
                P.push_back(temp);
                if (temp == "")
                    break;
                left = left->NextSiblingElement();
            }
            out << ")";
        }
        else if (mValue == "int-constant"
        || mValue == "real-constant"
        || mValue == "string-constant")
        {
            out << m->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        if(value == "lesseq")
            out << "<=";
        else if(value == "less")
            out << "<";
        if(value == "equality")
            out << "==";
        if (nValue == "token-value")
        {
            out << "token-value(";
            TiXmlElement *right = n->FirstChildElement();
            while (right != NULL)
            {
                out << right->GetText() ;
                string temp = right->GetText();
                P.push_back(temp);
                right = right->NextSiblingElement();
            }
            out << ")";
        }
        else if (nValue == "int-constant"
                 || nValue == "real-constant"
                 || nValue == "string-constant")
        {
            out << n->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        out << "}";
    }
    else if (value == "is-fireable")
    {
        TiXmlElement *m;
        m = p->FirstChildElement();
        out << "is-fireable{";
        while (m != NULL)
        {
            string Value = m->Value();

            if (Value == "transition")
            {
                string temp = m->GetText();
                T.push_back(temp);
                out << m->GetText() << ",";
            }
            else
            {
                cerr << "Error in XML file! One of the element is-firability\'s child is not transition!" << endl;
                exit(-1);
            }
            m = m->NextSiblingElement();
        }
        out << "}";
    }
}

void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria) {


    TiXmlDocument *doc;
    string file;
    if (type == LTLC)
        file = LTLCfile;
    else if (type == LTLF)
        file = LTLFfile;
    else if (type == LTLV)
        file = LTLVfile;
    doc = new TiXmlDocument(file.c_str());
    if (!doc->LoadFile()) {
        cerr << doc->ErrorDesc() << endl;
    }
    file = "result.txt";
    ofstream out(file.c_str(), ios::out | ios::app);

    TiXmlElement *root = doc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        doc->Clear();
    }

    TiXmlElement *p = root->FirstChildElement();
    for (int i = 1; i < number; ++i) {
        p = p->NextSiblingElement();
    }

    TiXmlElement *id = p->FirstChildElement("id");

    TiXmlElement *formula = p->FirstChildElement("formula");
    vector<string> P, T;


    GetLTL(out,formula->FirstChildElement(), P, T);

    out<<endl;
    for (unsigned int i = 0; i < P.size(); i++) {
        auto iter = cpn->mapPlace.find(P[i]);
        CPlace *pp = &cpn->place[iter->second];
        if (!pp->control_P) {
            CTransition *tt;
            for (unsigned int j = 0; j < pp->producer.size(); j++) {
                tt = &cpn->transition[pp->producer[j].idx];
                for (unsigned int k = 0; k < tt->producer.size(); k++) {
                    if (tt->producer[k].arcType == control
                        && cpn->place[tt->producer[k].idx].expression != executed_P_name)
                        if (!exist_in(criteria, cpn->place[tt->producer[k].idx].id))
                            criteria.push_back(cpn->place[tt->producer[k].idx].id);
                }
            }
            for (unsigned int j = 0; j < pp->consumer.size(); j++) {
                tt = &cpn->transition[pp->consumer[j].idx];
                for (unsigned int k = 0; k < tt->producer.size(); k++) {
                    if (tt->producer[k].arcType == control
                        && cpn->place[tt->producer[k].idx].expression != executed_P_name)
                        if (!exist_in(criteria, cpn->place[tt->producer[k].idx].id))
                            criteria.push_back(cpn->place[tt->producer[k].idx].id);
                }
            }
        } else {
            cout << "nonsense token-value for control place" << endl;
            exit(-1);
        }
    }
    for (unsigned int i = 0; i < T.size(); i++) {
        auto iter = cpn->mapTransition.find(T[i]);
        CTransition *tt = &cpn->transition[iter->second];
        for (unsigned int j = 0; j < tt->producer.size(); j++) {
            if (tt->producer[j].arcType == control
                && cpn->place[tt->producer[j].idx].expression != executed_P_name)
                if (!exist_in(criteria, cpn->place[tt->producer[j].idx].id))
                    criteria.push_back(cpn->place[tt->producer[j].idx].id);
        }
    }
    out<<endl;
    out.close();
}
