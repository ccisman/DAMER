#pragma once
#include"Syntax_Tree.h"

typedef struct vex
{
	int num;                //������
	bool explored = false;  //�ýڵ��Ƿ�����չ��
	formula_stack form;     //�ýڵ���������ӹ�ʽ����
	set<int> nextvexset ;   //his successors
}TGBA_vex;

typedef struct arc
{
	int num;
	int pioneer;
	int succeed;
	set<string> transcondi;
	set<int> label;
}TGBA_arc;

/*********************Global_Function***********************/
bool isSame(TGBA_vex s1, TGBA_vex s2);
void shrink(TGBA_vex &ss);
bool isAincludeB(set<string> A, set<string> B);
bool isAincludeB(set<int> A, set<int> B);
bool isAequB(set<int> A, set<int> B);
bool isAequB(set<string> A, set<string> B);
bool isSatisfyUright(STNode p, set<string> transcond);
/************************************************************/

class TGBA
{
private:
	vector<TGBA_vex> states;
	vector<TGBA_arc> transition;
	int statecounter;         //״̬����
	int transcounter;         //��Ǩ����
public:
	TGBA();
	void AssignID(TGBA_vex &ss);
	//creat-serial functions:
	void CreatTGBA(formula_stack Ustack, ST_Node *root);
	void CreatState(CFTreeLeaf ctl, TGBA_vex &ss, set<string> &transcond);
	void CreatTransition(vector<TGBA_vex>::iterator &front, vector<TGBA_vex>::iterator rear, set<string> transcond, formula_stack Ustack);
	void AddState(TGBA_vex ss, vector<TGBA_vex>::iterator &addposi);
	bool isStateEqu(TGBA_vex A, TGBA_vex B);
	//find-serial functions:
	bool isFindUnexpState(vector<TGBA_vex>::iterator &iter);
	void findstatesbynum(int num, vector<TGBA_vex>::iterator &frontposi);
	void findtransitionbyps(int pioneer, int succeed, vector<TGBA_arc>::iterator &transloc);
	bool findtransitionbysuc(int succeed, vector<TGBA_arc>::iterator &transloc);
	//fetch-serial functions:
	void fetchalltransbypri(int pri, vector<TGBA_arc> &transet);
	//simplify-related functions:
	void SimplifyStates();
	void deltransbypri(int pri);                //ɾ��pri���������б�Ǩ
	void modifytransbysuc(int oldsuc, int newsuc);     //��ָ��oldsuc��Ǩ������ָ��newsuc�����޸ķ�����Ǩ�Ƶ�״̬��nextvexset
};