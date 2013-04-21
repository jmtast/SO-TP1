#ifndef __SCHED_RSD__
#define __SCHED_RSD__

#include <vector>
#include <queue>
#include <map>
#include "basesched.h"

using namespace std;

typedef int pid;
typedef int level;
typedef int quota;
typedef int cpu;

class SchedRSD : public SchedBase {
	public:
		SchedRSD(std::vector<int> argn);
        	~SchedRSD();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int cantNiveles;
		//vector<int> procesosPorNivel;
		vector< queue<pid> > staircase_a; //estructura de escalera A
		vector< queue<pid> > staircase_b; //estructura de escalera B
		vector<quota> levelQuota;	//arreglo de quota de tiempo global para i-esimo nivel
		vector<quota> levelQuotaLeft;
		vector<quota> procQuota;	//arreglo de quota de tiempo particular para cada proceso (por nivel)
		map<pid,level> blockedProc;	//diccionario de procesos bloqueados (y en que nivel estaban)
		map<pid,quota> quotaLeft;	//diccionario de quota restante para cada proceso
		vector< queue<pid> >* activeStaircase; //puntero a escalera activa
		int activeLevel;
		vector<cpu> cores;		
		vector<cpu> quantums;
		vector<cpu> ticks;
		int next(int cpu);
		void swapStaircase();
		void bajarProcesos();
		int nextLevel(int desdeLevel);	
};

#endif
