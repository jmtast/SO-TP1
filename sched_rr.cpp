#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	
	vector<int>::iterator it = cores.begin();

	for (int i=0; i < argn[0]; i++){
		
		it=cores.insert(it,-1);
	}
	
	
	it=quantums.begin();
	vector<int>::iterator it2 = ticks.begin();
	
	for(int i= argn.size()-1; i > 0; i--){
	
		it= quantums.insert(it,argn[i]);
		it2=ticks.insert(it2,0);
		
	}
		
}

SchedRR::~SchedRR() {

}


void SchedRR::load(int pid) {

	unsigned int i;		
	for ( i=0; i< cores.size() ; i++){
		
		if (cores[i] == -1 ){
			 cores[i] = pid;
			 break;
		}
			
	}	
	
	if(i==cores.size()) procesos.push(pid);
	


}

void SchedRR::unblock(int pid) {

	load(pid);

}

int SchedRR::tick(int cpu, const enum Motivo m) {
	switch (m){

	case EXIT:
		
		return next(cpu);
	
	break;

	case TICK:

		ticks[cpu]++;
		
		if(ticks[cpu] == quantums[cpu]){
			
			procesos.push(cores[cpu]);
			return next(cpu);
		
		}
		else{
			return cores[cpu];
		}			

	break;

	case BLOCK:
		
		return next(cpu);
			
	break;

	}
}

int SchedRR::next(int cpu) {
	
	int pid;
	if ( procesos.empty() ) {
	
		pid = IDLE_TASK;
	}
	else {
	
		pid = procesos.front();
		procesos.pop();			
	}	
	cores[cpu] = pid;
	ticks[cpu]=0;
	
	return pid;
}
