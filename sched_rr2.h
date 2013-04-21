#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include "basesched.h"
#include<map>

using namespace std; 

class SchedRR2 : public SchedBase {
	public:
		SchedRR2(std::vector<int> argn);
        ~SchedRR2();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);
	
	private:
		queue<int> procesos;
		vector<int> cores;		
		vector<int> quantums;
		vector<int> ticks;
		int next(int cpu);
		vector<queue<int> > colas;
		map<int,int> blockedProc;	//diccionario de procesos bloqueados (y en que core estaban)
	//	int block;
	//	int ticked;
	//	int exit;
};
		
#endif
