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
void GetLTLV(TiXmlElement *p, vector<string> &criteria)
{
    string value = p->Value();
    if (value == "all-paths")
    {
        GetLTLV(p->FirstChildElement(), criteria);
    }
    else if (value == "globally")
    {
        //		criteria << "[](";
        GetLTLV(p->FirstChildElement(), criteria);
        //		criteria << ")";
    }
    else if (value == "finally")
    {
        //		criteria << "<>(";
        GetLTLV(p->FirstChildElement(), criteria);
        //		criteria << ")";
    }
    else if (value == "next")
    {
        //		criteria << "X(";
        GetLTLV(p->FirstChildElement(), criteria);
        //		criteria << ")";
    }
    else if (value == "negation" || value == "conjunction" || value == "disjunction")
    {
//        criteria << "[](";
        GetLTLV(p->FirstChildElement(), criteria);
//        criteria << ")";
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
        //		criteria << "(";
        GetLTLV(m->FirstChildElement(), criteria);
        //		criteria << ")U(";
        GetLTLV(n->FirstChildElement(), criteria);
        //		criteria << ")";
    }
    else if (value == "less" || value == "equality" || "lesseq")
    {
        TiXmlElement *m, *n;
        m = p->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
        //		criteria << "{";
        if (mValue == "token-value")
        {
            //			criteria << "token-count(";
            TiXmlElement *left = m->FirstChildElement();
            while (left != NULL)
            {

                //				criteria << left->GetText() << ",";
                string temp = left->GetText();
                criteria.push_back(temp);
                if (temp == "")
                    break;
                left = left->NextSiblingElement();
            }
            //			criteria << ")";
        }
        else if (mValue == "int-constant"
        || mValue == "real-constant"
        || mValue == "string-constant")
        {
            //			criteria << m->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        //		criteria << "<=";
        if (nValue == "token-value")
        {
            //			criteria << "token-count(";
            TiXmlElement *right = n->FirstChildElement();
            while (right != NULL)
            {
                //				criteria << right->GetText() << ",";
                string temp = right->GetText();
                criteria.push_back(temp);
                right = right->NextSiblingElement();
            }
            //			criteria << ")";
        }
        else if (nValue == "int-constant"
                 || nValue == "real-constant"
                 || nValue == "string-constant")
        {
            //			criteria << n->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        //		criteria << "}";
    }
}

void GetLTLf(TiXmlElement *p, vector<string> &criteria)
{
    string Value = p->Value();
    if (Value == "all-paths")
    {
        GetLTLf(p->FirstChildElement(), criteria);
    }
    else if (Value == "next")
    {
//        criteria << "X(";
        GetLTLf(p->FirstChildElement(), criteria);
//        criteria << ")";
    }
    else if (Value == "globally")
    {
//        criteria << "[](";
        GetLTLf(p->FirstChildElement(), criteria);
//        criteria << ")";
    }
    else if (Value == "negation" || Value == "conjunction" || Value == "disjunction")
    {
//        criteria << "[](";
        GetLTLf(p->FirstChildElement(), criteria);
//        criteria << ")";
    }
    else if (Value == "until")
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
//        criteria << "(";
        GetLTLf(m->FirstChildElement(), criteria);
//        criteria << ")U(";
        GetLTLf(n->FirstChildElement(), criteria);
//        criteria << ")";
    }
    else if (Value == "finally")
    {
//        criteria << "<>(";
        GetLTLf(p->FirstChildElement(), criteria);
//        criteria << ")";
    }
    else if (Value == "is-fireable")
    {
        TiXmlElement *m;
        m = p->FirstChildElement();
//        criteria << "{";
        while (m != NULL)
        {
            string Value = m->Value();

            if (Value == "transition")
            {
                string temp = m->GetText();
                criteria.push_back(temp);
//                criteria << m->GetText() << ",";
            }
            else
            {
                cerr << "Error in XML file! One of the element is-firability\'s child is not transition!" << endl;
                exit(-1);
            }
            m = m->NextSiblingElement();
        }
//        criteria << "}";
    }
}

void extract_criteria(int number,LTLCategory type,CPN *cpn,vector<string> &criteria) {

    //ofstream criteria("FormulaC.txt", ios::out);
    TiXmlDocument *doc;
    if(type == LTLC)
        doc = new TiXmlDocument(LTLCfile);
    else if(type == LTLF)
        doc = new TiXmlDocument(LTLFfile);
    else if(type == LTLV)
        doc = new TiXmlDocument(LTLVfile);
    if (!doc->LoadFile()) {
        cerr << doc->ErrorDesc() << endl;
    }
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
    if (type == LTLV) {
        GetLTLV(formula->FirstChildElement(), P);
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
                            if(!exist_in(criteria,cpn->place[tt->producer[k].idx].id))
                                criteria.push_back(cpn->place[tt->producer[k].idx].id);
                    }
                }
                for (unsigned int j = 0; j < pp->consumer.size(); j++) {
                    tt = &cpn->transition[pp->consumer[j].idx];
                    for (unsigned int k = 0; k < tt->producer.size(); k++) {
                        if (tt->producer[k].arcType == control
                            && cpn->place[tt->producer[k].idx].expression != executed_P_name)
                            if(!exist_in(criteria,cpn->place[tt->producer[k].idx].id))
                                criteria.push_back(cpn->place[tt->producer[k].idx].id);
                    }
                }
            } else {
                cout << "nonsense token-value for control place" << endl;
                exit(-1);
            }
        }
    } else if (type == LTLF) {
        GetLTLf(formula->FirstChildElement(), T);
        for(unsigned int i=0;i<T.size();i++)
        {
            auto iter = cpn->mapTransition.find(T[i]);
            CTransition *tt = &cpn->transition[iter->second];
            for(unsigned int j=0;j<tt->producer.size();j++)
            {
                if (tt->producer[j].arcType == control
                    && cpn->place[tt->producer[j].idx].expression != executed_P_name)
                    if(!exist_in(criteria,cpn->place[tt->producer[j].idx].id))
                        criteria.push_back(cpn->place[tt->producer[j].idx].id);
            }
        }
    }
    else if (type == LTLC) {
        cout << "LTLC doesn't means anything!" << endl;
        exit(-1);
    }

}

void construct_LTLF(char *filename)
{
    TiXmlDocument *doc = new TiXmlDocument(filename);
    if (!doc->LoadFile()) {
        cerr << doc->ErrorDesc() << endl;
    }
    TiXmlElement *root = doc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        doc->Clear();
    }

    TiXmlElement *p = root->FirstChildElement();

}