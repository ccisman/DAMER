//
// Created by cc on 2020/8/5.
//


#ifndef PDNET_CHECKER_CONDITION_TREE_H
#define PDNET_CHECKER_CONDITION_TREE_H

#endif //PDNET_CHECKER_CONDITION_TREE_H

#include<string>
#include<map>
using namespace std;


enum CTN_type{TokenOperator,RelationOperator,Operator,color,variable};
// Operator{"Addition","Subtraction","Multiplication","Division","Module"};

//**  'value' stands for the color value                           **//
//**  operator_name stands for different thing for different node  **//
//**  1.For a variable node,it stands for variable name            **//
//**  2.For a color node,it stands for color name                  **//
//**  3.For TokenOperator and Operator,it stands for operation     **//
class condition_tree_node
{
public:
    CTN_type node_type;
    string node_name;
    string value;
    condition_tree_node *left;
    condition_tree_node *right;

};

class condition_tree
{
public:
    condition_tree_node *root;
    string exp;
    void construct(string s);
};