#include"TBA.h"

/****************************Global_Functions*****************************/
bool isExist(set<int> a, int val)
{
	set<int>::iterator i;
	for (i = a.begin(); i != a.end(); i++)
	{
		if ((*i) == val)
			return true;
	}
	return false;
}
void fullfill_info(set<string> transcondi, string &info)
{
	if (transcondi.size() == 0)
	{
		info = "true";
	}
	else
	{
		set<string>::iterator iter;
		for (iter = transcondi.begin(); iter != transcondi.end(); iter++)
		{
			info += (*iter) + "&&";
		}
		info = info.substr(0, info.length() - 2);
	}
}
/**************************************************************************/
TBA::TBA()
{
	vex_num = 0;
	arc_num = 0;
	memset(visit, 0, sizeof(int)*MAX_VERTEX_NUM);
}
bool TBA::fetchvirgin(int &adj)         //�Ӷ����б���ȡ��һ����δ��չ�Ľڵ�
{
	int i = 0;
	for (i; i < vex_num; i++)
	{
		if (vertics[i].isexplored == false)
		{
			adj = i;
			return true;
		}
	}
	return false;
}
int TBA::LocateVex(VertexType v)    //��λ������
{
	for (int i = 0; i < vex_num; i++)
	{
		if (vertics[i].data.name == v.name && vertics[i].data.num == v.num)
		{
			return i;
		}
	}
	return Nofind;
}
void TBA::CreatTBA(TGBA tgba, formula_stack Ustack)
{
	//�����ʼ���ڵ�
	vertics[vex_num].data.name = 0;
	vertics[vex_num].data.num = 0;
	vertics[vex_num].firstarc = NULL;
	vertics[vex_num].isAccept = (Ustack.size() == 0) ? true : false;
	vertics[vex_num].isexplored = false;
	vertics[vex_num].outdegree = 0;
	vex_num++;
	//����TBA
	CreatTBAsub(tgba, Ustack);
}
void TBA::CreatTBAsub(TGBA tgba, formula_stack Ustack)
{
	int virginloc;                       //��Ҫ��չ�Ľڵ��λ��
	while (fetchvirgin(virginloc))
	{
		VNode *virgin = &vertics[virginloc];
		//1.�ҵ���״̬���������б�Ǩ
		vector<TGBA_arc> virginarc;
		tgba.fetchalltransbypri(virgin->data.name, virginarc);
		//2.��ÿһ����Ǩ����ƥ������
		int base;
		if (virgin->data.num == Ustack.size())    //�����ǰ״̬��ƥ��������r
		{
			//��0��ʼ����ƥ��ֵ
			base = 0;
		}
		else                                                        //�����ǰ״̬��ƥ�����ַ�r
		{
			//�ӵ�ǰ״̬ƥ������ֿ�ʼ����ƥ��ֵ
			base = virgin->data.num;
		}
		while (!virginarc.empty())
		{
			int increasebase = base;
			TGBA_arc curarc = virginarc.back();
			virginarc.pop_back();
			//����ƥ������
			for (int i = increasebase + 1; i <= Ustack.size(); i++)
			{
				if (isExist(curarc.label, i - 1))
					increasebase = i;
				else
					break;
			}//����ƥ�����
			VertexType newvexdata;
			newvexdata.name = curarc.succeed;
			newvexdata.num = increasebase;
			int newvexloc=LocateVex(newvexdata);
			if (newvexloc == Nofind)    //�����������Ľڵ㣬����½ڵ�
			{
				//�½�һ���ڵ㣬���뵽vertics������
				vertics[vex_num].data = newvexdata;
				vertics[vex_num].firstarc = NULL;
				vertics[vex_num].isAccept = (vertics[vex_num].data.num == Ustack.size()) ? true : false;
				vertics[vex_num].isexplored = false;
				vertics[vex_num].outdegree = 0;
				//�½�һ��Ǩ��
				ArcNode *p = new ArcNode;
				p->adjvex = vex_num;
				p->nextarc = NULL;
				p->transi_condi = curarc.transcondi;
				fullfill_info(p->transi_condi, p->info);
				AddArc(virginloc, p);
				//�������+1
				vex_num++;
			}
			else              //�Ѿ����ڸýڵ�
			{
				ArcNode *p = new ArcNode;
				p->adjvex = newvexloc;
				p->nextarc = NULL;
				p->transi_condi = curarc.transcondi;
				fullfill_info(p->transi_condi, p->info);
				AddArc(virginloc, p);
			}
		}
		virgin->isexplored = true;
	}
}
void TBA::AddArc(int vexloc, ArcNode *&arcloc)     //����һ����Ǩ
{
	//1.�ж��Ƿ��Ѿ�����Ǩ��vexloc->arcloc.adjvex
	ArcNode *p = vertics[vexloc].firstarc;
	while (p != NULL)
	{
		if (p->adjvex == arcloc->adjvex)
			break;
		else
			p = p->nextarc;
	}
	if (p == NULL)    //������
	{
		//֮�����Ǩ��
		arcloc->nextarc = vertics[vexloc].firstarc;
		vertics[vexloc].firstarc = arcloc;
		vertics[vexloc].outdegree++;
		int adj = arcloc->adjvex;
		vertics[adj].incoming.insert(vexloc);
		return;
	}
	else                   //����
	{
		//��������Ա�
		if (isAincludeB(p->transi_condi, arcloc->transi_condi))  //p�������
		{
			//��pɾ������arcloc��ӽ���
			p->info = arcloc->info;
			p->transi_condi = arcloc->transi_condi;
		}
		else if (isAincludeB(arcloc->transi_condi, p->transi_condi))   //�¼ӵ��������
		{
			//֮��ɾ��arcloc������ӽ���
			delete arcloc;
		}
		else    //û�������ϵ
		{
			//��arcloc��ӽ���
			arcloc->nextarc = vertics[vexloc].firstarc;
			vertics[vexloc].firstarc = arcloc;
			vertics[vexloc].outdegree++;
			vertics[arcloc->adjvex].incoming.insert(vexloc);
		}
	}
}
void TBA::PrintBuchi(string filename)
{
	const char *file = filename.c_str();
	ofstream outfile(file, ios::out);
	for (int i = 0; i < vex_num; i++)
	{
		outfile << "S" << i << ":";
		outfile << (vertics[i].isAccept == true ? "Accepted" : "") << endl;
		ArcNode *p = vertics[i].firstarc;
		while (p != NULL)
		{
			outfile << setw(3) << " " << "if (" << p->info << ")" << endl;
			outfile << setw(3) << " " << "goto S" << p->adjvex << endl;
			outfile << setw(3) << " " << "---------------------------------" << endl;;
			p = p->nextarc;
		}
		outfile << endl;
	}
}
