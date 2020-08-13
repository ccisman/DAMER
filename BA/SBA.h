#pragma once
#include"TBA.h"

typedef int SVertexType ;
typedef struct SArcNode
{
	int adjvex;
	SArcNode *nextarc = NULL;                 //��һ��״̬
}SArcNode;
typedef struct SVNode
{
	SVertexType data=-1;		 //���㣨״̬��
	bool isAccept;           //��״̬�Ƿ�Ϊ�ɽ���״̬
	bool isInitial;              //��״̬�Ƿ�Ϊ
	set<int> incoming;      //��״̬��ǰ��
	set<int> outset;           //��״̬�ĺ�
	//��״̬��label���ϣ������·�ʽ������{} && {} && {}������ÿһ��{}����һ��ԭ������
	//�����LTLFirability��ʽ������������������ʽ��{t1,t2,...}������t1,t2��ʾ��Ǩ������
	//�����LTLCardinality��ʽ��������������������ʽ{token-count(p1,p3,...)<=token-count(p,p,...)}������pn��ʾ����������
	//ע�⣡��label�����е�ԭ��������ܻ���������ʽ����!{}����ʽ������ !{}&&!{}&&{}
	//ע�⣡��LTLCardinality��ʽ��token-count()���ܻ���һ����������ʽ������ {2<=token-count(p,p,...)}��{token-count(p1,p3,...)<=2}
	//����Ĵ���ʽ�����ȶ�label���string���ԡ�&&������Ϊ�и�����ֳ�һ��һ�� {} �� !{} ,Ȼ���ٶ�{}����
	//�ж�{}���Ƿ���֡�<=�����ţ���������ʾ��LTLCardinality��ʽ���������ʾ��LTLFirability��ʽ
	//�����LTLCardinality��ʽ�����ԡ�<=��Ϊ�и���ţ��ֳ�ǰ�������֣�����ǳ��������账�������token-count()��ʽ�����ԡ�,��Ϊ�и���ŵõ�һ��һ��p
	//�����LTLFirability��ʽ�����ԡ�,��Ϊ�и���ţ��ֳ�һ��һ��t
	string label;
	set<string> fullfilledAP;
	SArcNode *firstarc = NULL;     //��һ��״̬
}SVNode;

class SBA
{
public:
	int svex_num;
	SVNode svertics[MAX_VERTEX_NUM];
public:
	SBA();
	void CreatSBA(TBA &tba);
	void Simplify();
	void Compress();
    void PrintSBA();
    void ChangeOrder();
private:
	void AddArc(int pri, int suc);
	void Delallarc(int vexloc);
	void Delarc(int vexloc, int adj);
	void findnextempty(int start, int &next);
	void findnextoccupied(int start, int &next, int &vexn);
};
