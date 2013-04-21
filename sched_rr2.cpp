#include <vector>
#include <queue>
#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	//block=0;
	//exit=0;
	//ticked=0;
	vector<int>::iterator it = cores.begin();
	vector< queue<int> >::iterator it3 = colas.begin();
	queue<int> cola;

	for (int i=0; i < argn[0]; i++){
		
		it=cores.insert(it,-1);
		it3=colas.insert(it3,cola);	//inicializo las colas por tarea
	}
	
	
	it=quantums.begin();
	vector<int>::iterator it2 = ticks.begin();
	
	for(int i= argn.size()-1; i > 0; i--){
	
		it= quantums.insert(it,argn[i]);
		it2=ticks.insert(it2,0);
		
	}
}

SchedRR2::~SchedRR2() {

}


void SchedRR2::load(int pid) {

	unsigned int i;		
	for ( i=0; i< cores.size() ; i++){
		
		if (cores[i] == -1 ){
			 cores[i] = pid;   
			 colas[i].push(pid);	//lo agrego a la cola del core
			 break;
		}
			
	}	
	
	if(i==cores.size()) procesos.push(pid); //si no va a ningun core lo agrego a la cola general
	


}

void SchedRR2::unblock(int pid) {
	
	int core;
	core= blockedProc.find(pid)->second;
	blockedProc.erase(pid);
	if(colas[core].empty()){
	 	cores[core]=pid;	
	}	
	
	colas[core].push(pid);
}	

int SchedRR2::tick(int cpu, const enum Motivo m) {
	switch (m){

	case EXIT:
		colas[cpu].pop(); // si termino, lo saco de la cola de su core
		return next(cpu);
	
	break;

	case TICK:

		ticks[cpu]++;
		
		if(ticks[cpu] == quantums[cpu]){
			
			if (colas[cpu].size() ==1){ //si esta es la ultima de este core voy a buscar a procesos. si no hay en procesos me devuelve Idle
				int pid=colas[cpu].front();
				colas[cpu].pop();
				int prox=next(cpu);
				if (prox==IDLE_TASK){  //no puedo devolver la idle porque tengo la actual para correr
					cores[cpu]=pid;
					prox=pid;
					
				}
				colas[cpu].push(pid);
				return prox;
			}
			else{
				int pid=colas[cpu].front();
				colas[cpu].pop();
				colas[cpu].push(pid); //si se le acabo el quantum la paso a la cola de su core
				return next(cpu);
			}
		}
		else{
			return cores[cpu];
		}			

	break;

	case BLOCK:
		
		blockedProc.insert(pair<int, int> (cores[cpu],cpu));
		colas[cpu].pop();		
		return next(cpu);
			
	break;

	}
}

int SchedRR2::next(int cpu) {
	
	int pid;
	int temp;
	if (colas[cpu].empty()){

		if(procesos.empty()){
			 pid=IDLE_TASK;
		}
		else{
			pid=procesos.front();		//pongo un proceso en la cola del core
			procesos.pop();	
			colas[cpu].push(pid);
			for (int i=0; i<colas[cpu].size()-1; i++){
									
				temp=colas[cpu].front();
				colas[cpu].pop();
				colas[cpu].push(temp);			
			
			}					
		}
	}
	else{


		/*if (colas[cpu].size()==1){

			if (block==1) {		//si entro aca es 
				block=0;
				pid=colas[cpu].front();
				
			}
				
			if(ticked==1){				//si entro aca es porque solo estaba corriendo esa tarea en el core, asi que voy a ver si puedo agarrar otra
				ticked=0;
				if(procesos.empty()){
					pid=colas[cpu].front(); // si no puedo, sigo con esta
				}
				else{
				
					pid=colas[cpu].front();// voy a sacar una tarea de la lista de procesos global, asignarle este core y mandar la actual al final de la cola
					colas[cpu].pop();
					colas[cpu].push(procesos.front());
					procesos.pop();
					colas[cpu].push(pid);
					pid=colas[cpu].front();
				}
		

			}
		
			if(exit=1){  //si entro aca es porque 
				exit=0;
				pid=colas[cpu].front();
			}
											
		}
		else{*/
			
			if(procesos.empty()){
			// pid=IDLE_TASK;
			pid=colas[cpu].front();
		}	
		else{
			//pid=procesos.front();
			//procesos.pop();	
			//colas[cpu].push(pid);
			pid=procesos.front();		//pongo un proceso en la cola del core
			procesos.pop();	
			colas[cpu].push(pid);
			for (int i=0; i<colas[cpu].size()-1; i++){
									
				temp=colas[cpu].front();
				colas[cpu].pop();
				colas[cpu].push(temp);			
			}
		}
		
			//pid=colas[cpu].front();
			//colas[cpu].pop();
			
	}
	cores[cpu] = pid;
	ticks[cpu]=0;
	return pid;
}
