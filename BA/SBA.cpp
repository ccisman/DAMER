#include"SBA.h"

SBA::SBA()
{
	svex_num = 0;
}
void SBA::CreatSBA(TBA &tba)
{
	int num = tba.vex_num;
	for (int statesi = 0; statesi < num; statesi++)    //statesi��ʾtba�ж����ĵ�i���ڵ�
	{
		if (statesi == 9)
			int a = 0;
		set<int> sincoming;       //statesi��SBA���ڽӱ��е�ǰ���ڵ�
		set<int> soutset;            //statesi��SBA���ڽӱ��е�ǰ���ڵ�

		//1.�ҵ�statesi�ڵ���������
		set<int> incoming = tba.vertics[statesi].incoming;
		set<int>::iterator arcj_statesi;
		for (arcj_statesi=incoming.begin();arcj_statesi!=incoming.end();arcj_statesi++)
		{
			//arcj_statesi��ʾ����statesi�ĵ�j����
			ArcNode *p = tba.vertics[*arcj_statesi].firstarc;
			while (p != NULL)
			{
				if (p->adjvex == statesi)
				{
					if (p->sstateloc == -1)
					{
						//��Ǩ�ƻ�û�ж�Ӧ��״̬
						sincoming.insert(svex_num);
						p->sstateloc = svex_num;
						//������Ӧ��״̬
						svertics[svex_num].data = svex_num;
						svertics[svex_num].firstarc = NULL;
						svertics[svex_num].isAccept = tba.vertics[statesi].isAccept;
						svertics[svex_num].isInitial = false;
						svertics[svex_num].label = p->info;
						svertics[svex_num].fullfilledAP = p->transi_condi;
						svex_num++;
					}
					else
					{
						//��Ǩ�����ж�Ӧ��״̬
						sincoming.insert(p->sstateloc);
						svertics[p->sstateloc].isAccept = tba.vertics[statesi].isAccept;
					}
					break;
				}
				else
				{
					p = p->nextarc;
				}
			}
			if (p == NULL)
			{
				cerr << "Error! Can't find incoming!" << endl;
			}
		}

		//2.�ҵ�statesi�ڵ�����г���
		ArcNode *q = tba.vertics[statesi].firstarc;
		while (q != NULL)
		{
			if (q->sstateloc == -1)
			{
				//��Ǩ�����ڽӱ��л�δ�����Ӧ��״̬
				soutset.insert(svex_num);
				q->sstateloc = svex_num;
				svertics[svex_num].data = svex_num;
				svertics[svex_num].firstarc = NULL;
				svertics[svex_num].isAccept = tba.vertics[q->adjvex].isAccept;
				svertics[svex_num].isInitial = (statesi == 0) ? true : false;
				svertics[svex_num].label = q->info;
				svertics[svex_num].fullfilledAP = q->transi_condi;
				svex_num++;
			}
			else
			{
				//��Ǩ�����ڽӱ���������Ӧ��״̬
				soutset.insert(q->sstateloc);
				svertics[q->sstateloc].isInitial= (statesi == 0) ? true : false;
			}
			q = q->nextarc;
		}

		//3.ÿ����ߵ�ÿ������Ǩһ��Ǩ��
		set<int>::iterator in;
		set<int>::iterator out;
		for (in = sincoming.begin(); in != sincoming.end(); in++)
		{
			for (out = soutset.begin(); out != soutset.end(); out++)
			{
				AddArc(*in, *out);
			}
		}
	}
}
void SBA::AddArc(int pri, int suc)
{
	SArcNode *p = new SArcNode;
	p->adjvex = suc;
	p->nextarc = svertics[pri].firstarc;
	svertics[pri].firstarc = p;
	svertics[pri].outset.insert(suc);
	svertics[suc].incoming.insert(pri);
}
void SBA::Delallarc(int vexloc)
{
	SArcNode *p, *q;
	p = svertics[vexloc].firstarc;
	while (p!=NULL)
	{
		q = p->nextarc;
		svertics[p->adjvex].incoming.erase(vexloc);
		delete p;
		p = q;
	}
	svertics[vexloc].firstarc = NULL;
}
void SBA::Delarc(int vexloc, int adj)
{
	if (svertics[vexloc].firstarc->adjvex == adj)
	{
		SArcNode *temp = svertics[vexloc].firstarc;
		svertics[vexloc].firstarc = temp->nextarc;
		svertics[vexloc].outset.erase(temp->adjvex);
		delete temp;
		return;
	}
	SArcNode *p = svertics[vexloc].firstarc;
	SArcNode *q=p;
	while (p != NULL)
	{
		if (p->adjvex == adj)
		{
			q->nextarc = p->nextarc;
			svertics[vexloc].outset.erase(p->adjvex);
			delete p;
			return;
		}
		q = p;
		p = p->nextarc;
	}
}
void SBA::Simplify()
{
	for (int i = 0; i < svex_num-1; i++)
	{
		if (svertics[i].data == -1)
			continue;
		for (int j = i + 1; j < svex_num; j++)
		{
			if (svertics[j].data == -1)
				continue;
			if ((svertics[i].fullfilledAP == svertics[j].fullfilledAP) 
				&& (svertics[i].outset == svertics[j].outset)
				&& (svertics[i].isAccept==svertics[j].isAccept))    //����״̬�ȼ�
			{
				//ɾ��״̬j��j����������Ǩ�ƣ���ָ��j��Ǩ��ָ��״̬i
				//1.ɾ��״̬j��j����������Ǩ��
				svertics[j].data = -1;
				Delallarc(j);
				//2.��ָ��j��Ǩ��ָ��״̬i
				for (set<int>::iterator iter = svertics[j].incoming.begin(); iter != svertics[j].incoming.end(); iter++)
				{
					Delarc(*iter, j);
					if (!isExist(svertics[*iter].outset, i))
					{
						SArcNode *p = new SArcNode;
						p->adjvex = i;
						p->nextarc = svertics[*iter].firstarc;
						svertics[*iter].firstarc = p;
						svertics[i].incoming.insert(*iter);
						svertics[*iter].outset.insert(i);
					}
				}
			}
		}
	}
}
void SBA::Compress()
{
	int vexn = 0;
	int empty_ptr, occupied_ptr;
	findnextempty(0, empty_ptr);
	findnextoccupied(0, occupied_ptr, vexn);
	while (occupied_ptr < svex_num)
	{
		if (empty_ptr > occupied_ptr)
		{
			findnextoccupied(occupied_ptr + 1, occupied_ptr, vexn);
			continue;
		}
		else
		{
			//1.ά��incoming������
			set<int>::iterator in;
			for (in = svertics[occupied_ptr].incoming.begin(); in != svertics[occupied_ptr].incoming.end(); in++)
			{
				SArcNode *q = svertics[*in].firstarc;
				while (q != NULL)
				{
					if (q->adjvex == occupied_ptr)
					{
						q->adjvex = empty_ptr;
						svertics[*in].outset.erase(occupied_ptr);
						svertics[*in].outset.insert(empty_ptr);
						break;
					}
					else
						q = q->nextarc;
				}
				if (q == NULL)
				{
					cerr << "Error! Can't find " << *in << "->" << occupied_ptr << endl;
					exit(-1);
				}
			}
			
			//2.ά��outset������
			set<int>::iterator out;
			for (out = svertics[occupied_ptr].outset.begin(); out != svertics[occupied_ptr].outset.end(); out++)
			{
				svertics[*out].incoming.erase(occupied_ptr);
				svertics[*out].incoming.insert(empty_ptr);
			}

			//3.��occupied_ptr����Ų��empty_ptr������
			//memcpy(&svertics[empty_ptr], &svertics[occupied_ptr], sizeof(SVNode));
			svertics[empty_ptr].data = svertics[occupied_ptr].data;
			svertics[empty_ptr].firstarc = svertics[occupied_ptr].firstarc;
			svertics[empty_ptr].isAccept = svertics[occupied_ptr].isAccept;
			svertics[empty_ptr].isInitial = svertics[occupied_ptr].isInitial;
			//svertics[empty_ptr].incoming = svertics[occupied_ptr].incoming;
			//svertics[empty_ptr].outset = svertics[occupied_ptr].outset;
			//svertics[empty_ptr].fullfilledAP = svertics[occupied_ptr].fullfilledAP;
			svertics[empty_ptr].label = svertics[occupied_ptr].label;

			svertics[occupied_ptr].data = -1;
			findnextoccupied(occupied_ptr + 1, occupied_ptr, vexn);
			findnextempty(empty_ptr + 1, empty_ptr);
		}
	}
	svex_num = vexn;
}
void SBA::findnextempty(int start, int &next)
{
	int s = start;
	while (s<svex_num)
	{
		if (svertics[s].data==-1)
		{
			next = s;
			return;
		}
		s++;
	}
	next = svex_num;
}
void SBA::findnextoccupied(int start, int &next, int &vexn)
{
	int s = start;
	while (s < svex_num)
	{
		if (svertics[s].data != -1)
		{
			next = s;
			vexn++;
			return;
		}
		s++;
	}
	next = svex_num;
}
void SBA::PrintSBA() {
    ofstream outsba("SBA.dot",ios::out);
    if(!outsba) {
        cerr<<"CANNOT OPEN SBA.txt!"<<endl;
        exit(-1);
    }
    outsba<<"digraph SBA{"<<endl;
    outsba<<'\t'<<"rankdir=LR"<<endl;
    for(int i=0;i<svex_num;i++)
    {
        outsba<<"\t"<<i<<" ";
        if(svertics[i].isAccept)
        {
            outsba<<"[shape=doublecircle,";
        } else{
            outsba<<"[shape=circle,";
        }
        if(svertics[i].isInitial)
        {
            outsba<<"style=filled,";
        }
        outsba<<"]";
//        outsba<<"label=\""<<svertics[i].label<<"\"]";
        outsba<<endl;
    }
    outsba<<endl;
    for(int i=0;i<svex_num;i++) {
        SArcNode *p=svertics[i].firstarc;
        while(p!=NULL)
        {
            outsba<<"\t"<<i<<"->"<<p->adjvex<<endl;
            p=p->nextarc;
        }
    }
    outsba<<"}"<<endl;
}

void SBA::ChangeOrder() {
    for(int i=0;i<svex_num;++i)
    {
        if(svertics[i].firstarc==NULL)
            continue;
        SArcNode *pre,*p;
        p = svertics[i].firstarc->nextarc;
        pre = svertics[i].firstarc;
        while(p) {
            if(svertics[p->adjvex].isAccept == true) {
                //提前
                pre->nextarc = p->nextarc;
                p->nextarc = svertics[i].firstarc;
                svertics[i].firstarc = p;
                p=pre->nextarc;
            }
            else {
                pre = p;
                p=p->nextarc;
            }
        }
    }
}