//
// Created by lewis on 2020/8/24.
//
#include<string>
#include<vector>
#include<iostream>
#include"BA/tinyxml.h"
using namespace std;

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
        if (mValue == "tokens-value")
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
        if (nValue == "tokens-count")
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
            TiXmlElement *left = m->FirstChildElement();
            if (Value == "transition")
            {
                string temp = left->GetText();
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

void xml_trans_C(char *filename, char *to_file, vector<string> &criteria)
{

    //ofstream criteria("FormulaC.txt", ios::out);
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
        GetLTLV(formula->FirstChildElement(), criteria);

        p = p->NextSiblingElement();
    }

    doc->SaveFile(to_file);
}
vector<string> get_criteria(TiXmlNode *root)
{

}