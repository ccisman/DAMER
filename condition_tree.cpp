//
// Created by cc on 2020/8/5.
//

#include "condition_tree.h"
#include<stack>
#include<iostream>


bool judge_logic_operator(string s)
{
    if (s == "&&" || s == "||")
        return true;
    return false;
}

bool judge_relation_operator(string s)
{
    if (s == ">" || s == "<" || s == ">=" || s == "<=" || s == "==" || s == "!=")
        return true;
    return false;
}

int priority(string s)
{
    if (s == "#")
        return -2;
    else
    {
        if (s == "(")
            return -1;
        else if(s == "++")
            return 0;
        else if (judge_logic_operator(s))
            return 1;
        else if (judge_relation_operator(s))
            return 2;
        else if (s == "+" || s == "-")
            return 3;
        else if (s == "*" || s == "/" || s == "%")
            return 4;
        else if (s == "&" || s == "|")
            return 5;
        else if (s == ",")
            return 6;
        else if (s == ")" || s == "!" || s == "@" || s == "^")
            return 7;

    }
    cout << "priority error!" << endl;
    return -2;
}

bool is_operator(string s)
{
    if (s == "+" || s == "-" || s == "*"
        || s == "/" || s == "(" || s == ")"
        || s == "#" || s == "&" || s == "|"
        || s == "^" || s == "%" || s == "!"
        || s == "," || s == "++"
        ||judge_logic_operator(s)||judge_relation_operator(s))
    {
        return true;
    }
    return false;
}

int is_operator(string s, int pos)
{
    if (s[pos] == '#')
        return 1;
    string temp_s;
    temp_s = s[pos];
    temp_s.append(1,s[pos + 1]);
    if (is_operator(temp_s))
        return 2;
    temp_s = s[pos];
    if (is_operator(temp_s))
        return 1;
    return 0;
}

bool is_unary(string s)
{
    if (s == "!" || s == "@" )
        return true;
    else
        return false;
}

void process_stack(stack<condition_tree_node *> &Stack_n,stack<string> &Stack_o)
{
    string top = Stack_o.top();
    Stack_o.pop();

    condition_tree_node *node = new condition_tree_node;
    condition_tree_node *top1,*top2;
    if(top == "," || top == "++")
        node->node_type = TokenOperator;
    else if(judge_logic_operator(top) || judge_relation_operator(top) )
        node->node_type = RelationOperator;
    else
       node->node_type = Operator;
    node->node_name = top;
    if(!is_unary(top))
    {
        top1 = Stack_n.top();
        Stack_n.pop();
        top2 = Stack_n.top();
        Stack_n.pop();

        node->right = top1;
        node->left = top2;
        Stack_n.push(node);
    }
    else
    {
        top1 = Stack_n.top();
        Stack_n.pop();
        node->left = top1;
        Stack_n.push(node);
    }
}

void condition_tree::construct(string s) {
    stack<string> Stack_o;
    stack<condition_tree_node*> Stack_n;

    exp = s;
    Stack_o.push("#");
    s = s +"#";


    string temp_s;
    string translator;
    condition_tree_node *node;

    for(unsigned int i=0;i<s.length();i++)
    {
        if (s[i] == ' ')
            continue;
        int count = is_operator(s, i);
        if (count != 0)
        {
            if (temp_s != "")
            {
                node = new condition_tree_node;
                if(temp_s[0]=='_'||isalpha(temp_s[0]))
                    node->node_type = variable;
                else
                    node->node_type = color;
                node->node_name = temp_s;
                node->value = temp_s;//just for color node now
                temp_s.clear();
                Stack_n.push(node);
            }
            else if (s[i] == '-')
            {
                Stack_o.push("@");
                continue;
            }
            if (s[i] == '(')
            {
                translator = s[i];
                Stack_o.push(translator);
            }
            else if (s[i] == ')')
            {
                while (Stack_o.top() != "(")
                {
                    process_stack(Stack_n, Stack_o);
                }
                Stack_o.pop();
            }
            else
            {
                if (count == 1)
                    translator = s[i];
                else
                {
                    translator = s[i];
                    translator.append(1, s[i + 1]);
                }
                while (priority(translator) <= priority(Stack_o.top()))
                {
                    if (s[i] == '#' && Stack_o.top() == "#")
                    {
                        root = Stack_n.top();
                        Stack_n.pop();
                        return ;
                    }
                    process_stack(Stack_n, Stack_o);

                }
                Stack_o.push(translator);
            }
        }
        else
            temp_s.append(1, s[i]);
        if (count == 2)
            i++;
    }
}

