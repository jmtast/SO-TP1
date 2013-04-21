#include <vector>
#include <queue>
#include <map>
#include "sched_rsd.h"
#include "basesched.h"
#include <iostream>

/*
Revisar el tema de la quota de nivel y la quota de los procesos. La idea es que la quota por nivel no se pase mas (arranca en 0), y se va aumentando a medida que llega un proceso (con lo que le corresponde).
*/

	/**
		Preparado para multicore pero en realidad se utiliza con un solo core
	**/

using namespace std;

SchedRSD::SchedRSD(vector<int> argn) {

	//Asumimos que arg[0] = cant cores, despues n quantums, despues l niveles, despues l quotas globales para cada nivel
	//despues l quotas de proceso para cada nivel

	//Inicializamos cada core con tarea IDLE
	vector<int>::iterator it = cores.begin();
	int cantCores = argn[0];
	for (int i=0; i < cantCores; i++){
		
		it=cores.insert(it,-1);
	}

	//Inicializamos vector de quantums	
	it=quantums.begin();
	vector<int>::iterator it2 = ticks.begin();
	
	for(int i= cantCores; i > 0; i--){
	
		it= quantums.insert(it,argn[i]);
		it2=ticks.insert(it2,0);
	}

	//Inicializamos las escaleras con colas vacias
	int cantLevels = argn[cantCores+1];
	cantNiveles= cantLevels;

	vector< queue<int> >::iterator it3 = staircase_a.begin();
	vector< queue<int> >::iterator it4 = staircase_b.begin();
	queue<int> cola;	
	for(int i = 0; i < cantLevels; i++){
		it3 = staircase_a.insert(it3, cola);
		it4 = staircase_b.insert(it4, cola);
	}

	//Inicializamos las cuotas globales y de procesos para cada nivel
	cantCores = cantCores + 2; //ya no representa cores sino un indice
	//it = levelQuota.begin();
	it2 = procQuota.begin();
	vector<int>::iterator it5 = levelQuotaLeft.begin();
	for(int i = cantCores + cantLevels - 1; i > cantCores-1; i--){
	//	it = levelQuota.insert(it, argn[i]);
		it5 = levelQuotaLeft.insert(it5, 0);	
		it2 = procQuota.insert(it2, argn[i+cantLevels]);	
	}

	activeStaircase = &staircase_a; 

	activeLevel = -1; //-1 es que no hay tareas cargadas, A lo intel, 0 es el de mayor prioridad... cantLevels el de menos.	
	
}

SchedRSD::~SchedRSD() {

}


void SchedRSD::load(int pid) {
	//Por defecto los procesos se intentan cargar en el nivel de mayor prioridad
	int priority = 0;

	if(activeLevel == -1){	//No hay ningun proceso cargado ni corriendo (puede haber bloiqueados si se cargara in media res)
		cores[0] = pid; //seguro el core está libre
		activeLevel = priority;
		quotaLeft.insert(pair<int,quota>(pid, procQuota[priority]));
		levelQuotaLeft[activeLevel] += procQuota[activeLevel];
	}else{
		if(priority <= activeLevel){ //Lo siguiente contempla que se puedan cargar tareas en medio de la simulacion
			int i;		
			for ( i=0; i< cores.size() ; i++){	//busco si hay un core libre
				if (cores[i] == -1){
					 cores[i] = pid;
					 quotaLeft.insert(pair<int,quota>(pid, procQuota[activeLevel]));
					 levelQuotaLeft[activeLevel] += procQuota[activeLevel];
					 break;
				}
			}
			if(i==cores.size()){	//si no hay ningun core libre, lo mando a la cola de tareas de ese nivel
				(activeStaircase->at(activeLevel)).push(pid);
				quotaLeft.insert(pair<int,quota>(pid, procQuota[activeLevel]));
				levelQuotaLeft[activeLevel] += procQuota[activeLevel];
			}
		}else{		//si es de un nivel de menos prioridad la mando a su correspondiente cola de tareas
			(activeStaircase->at(priority)).push(pid);
			quotaLeft.insert(pair<int,quota>(pid, procQuota[priority]));
			levelQuotaLeft[priority] += procQuota[priority];
		}
	}
}

void SchedRSD::unblock(int pid) {

	int priority = blockedProc.find(pid)->second; //obtengo el nivel de prioridad de la tarea bloqueada
	int quota = quotaLeft.find(pid)->second;

	if(activeLevel == -1){	//No hay ningun proceso cargado ni corriendo (puede haber bloiqueados)
		cores[0] = pid;
		activeLevel = priority;
		levelQuotaLeft[activeLevel] += quota; //reestablecemos la quota de ese proceso en nivel		
	}else{
		if(priority <= activeLevel){ //Si se desbloqueo un proc + importante, se actualiza a su nivel de ejec.
			activeLevel = priority;	
			levelQuotaLeft[activeLevel] += quota;		
			(activeStaircase->at(activeLevel)).push(pid);	
		}else{		//si es de un nivel de menos prioridad la mando a su correspondiente cola de tareas
			(activeStaircase->at(priority)).push(pid);
			levelQuotaLeft[priority] += quota;
		}
	}
	blockedProc.erase(pid);
	
}



int SchedRSD::tick(int cpu, const enum Motivo m) {

	int pidR = cores[cpu];
	int level = activeLevel + 1;

	bool hayQuotaGlobal = levelQuotaLeft[activeLevel] > 0;
	bool hayQuotaInd = quotaLeft.find(pidR)->second > 0;
	bool hayQuantum = !(ticks[cpu]==quantums[cpu]);
	
	vector< queue<pid> >* newStaircase = activeStaircase;

	if(level == cantNiveles){ //si no hay mas niveles, apunto a la otra escalera
		
		level = 0; //cada vez que se comienza de nuevo arrancan en prioridad max
		if (activeStaircase==&staircase_a){
			newStaircase=&staircase_b;
		}else{
			newStaircase=&staircase_a;
		}
	}
	
	switch (m){

	case EXIT:

		levelQuotaLeft[activeLevel] -= quotaLeft.find(pidR)->second; 	
		if(!hayQuotaGlobal){
			bajarProcesos();
		}
		quotaLeft.erase(pidR);
		pidR = next(cpu);
		ticks[cpu]++;
		levelQuotaLeft[activeLevel]--; //disminuimos la cuota del nivel
		quotaLeft.find(pidR)->second--; //disminuimos la cuota del proceso para su nivel actual
		return pidR;

	break;
		
	case TICK:

		if(!hayQuantum){
			if(!hayQuotaInd){ 
				
				newStaircase->at(level).push(pidR); //la bajo de nivel
				quotaLeft.find(pidR)->second = procQuota[level]; //ahora tiene la quota del nivel de abajo
				if(!hayQuotaGlobal || activeStaircase->at(activeLevel).empty() ){ //NO HAY NI QUANTUM, NI QUOTA IND, NI QUOTA GLOBAL
					bajarProcesos();		
				}
				pidR=next(cpu);
			
			}else{ 
				activeStaircase->at(activeLevel).push(pidR); //la pongo en la cola de este nivel
				if(!hayQuotaGlobal){	//NO HAY QUANTUM, HAY QUOTA IND, NO HAY QUOTA GLOBAL
					bajarProcesos();		
				}
				pidR=next(cpu);
			}
		}else{
			if(!hayQuotaInd){
				newStaircase->at(level).push(pidR); //la bajo de nivel
				quotaLeft.find(pidR)->second = procQuota[level]; //le asigno la quota individual del nuevo nivel
				if(!hayQuotaGlobal || activeStaircase->at(activeLevel).empty()){		// HAY QUANTUM, NO HAY QUOTA IND NI QUOTA GLOBAL
					bajarProcesos();				
				}
				pidR=next(cpu);	
			}else{	
				if(!hayQuotaGlobal){	//tengo que bajar todas    HAY QUANTUM , HAY QUOTA IND, PERO NO HAY QUOTA GLOBAL
					activeStaircase->at(activeLevel).push(pidR); //la dejo en la cola de este nivel				
					bajarProcesos();
					pidR = next(cpu);		
				}else{
					pidR=cores[cpu];	
				}
			}
		}
		levelQuotaLeft[activeLevel]--; //disminuimos la cuota del nivel
		quotaLeft.find(pidR)->second--; //disminuimos la cuota del proceso para su nivel actual
		ticks[cpu]++; //aumentamos el tick consumido por esa tarea
		return pidR;			

	break;

	case BLOCK:
		if(!hayQuotaInd){
			blockedProc.insert(pair<pid,int>(pidR,level));
			quotaLeft.find(pidR)->second = procQuota[level];
			if (!hayQuotaGlobal){
				bajarProcesos();
			}
		}else{
			blockedProc.insert(pair<pid,int>(pidR,activeLevel));
			levelQuotaLeft[activeLevel] -= quotaLeft.find(pidR)->second; //al bloquear saco su quota del nivel
		}	

		pidR = next(cpu);
		levelQuotaLeft[activeLevel]--; //disminuimos la cuota del nivel
		quotaLeft.find(pidR)->second--; //disminuimos la cuota del proceso para su nivel actual
		ticks[cpu]++; //aumentamos el tick consumido por esa tarea
		return pidR;
	break;

	}
}

int SchedRSD::next(int cpu) {
	
	int pid;
	int j = activeLevel;

	j = nextLevel(j);
	
	if (j==cantNiveles){ 	//si ninguno de los niveles tiene procesos, cambio de staircase 
		
		swapStaircase();
		j = nextLevel(0);
		if(j == cantNiveles) {
			pid = IDLE_TASK;
			//activeLevel = -1;
		}else{
			activeLevel = j;
			pid = (activeStaircase->at(j)).front();
			(activeStaircase->at(j)).pop();
		}
	}
	else{
		activeLevel=j;			//puede que me quede o cambie de nivel
		pid = (activeStaircase->at(j)).front();  //cambio a la tarea del proximo nivel con procesos a ejecutar
		(activeStaircase->at(j)).pop();	//saco la tarea que voy a ejecutar

	}

	cores[cpu] = pid;
	ticks[cpu] = 0;

	return pid;		
			
}


void SchedRSD::swapStaircase(){			//cambiar de staircase  

	if (activeStaircase== &staircase_a) {
		 activeStaircase= &staircase_b;
	}
	else {
		activeStaircase= &staircase_a;
	}

	/*for (int i=0; i< cantNiveles;i++){		//reseteo las quotas globales de los niveles
		
		levelQuotaLeft[i]=levelQuota[i];	

	}*/
}

int SchedRSD::nextLevel(int desdeLevel){
	int j;
	bool libre;
	for (j=desdeLevel; j < cantNiveles; j ++){	//voy a buscar cual es el primer nivel con procesos para ejecutar
		libre=(activeStaircase->at(j)).empty(); //me fijo si no hay mas procesos en el nivel
		if(!libre) break;
	}
	return j;
}


void SchedRSD::bajarProcesos(){		//baja los procesos al inmediatamente mas bajo. si era el ultimo nivel lo pasa a la otra staircase en nivel 0.

	int pidR;
	int level=activeLevel+1;
	vector< queue<pid> >* newStaircase;	
	if (level==cantNiveles) {
		level=0;
	//	vector< queue<pid> >* newStaircase;
		if (activeStaircase==&staircase_a){
			newStaircase=&staircase_b;
		}
		else{
			newStaircase=&staircase_a;
		}
	}else{
		newStaircase=activeStaircase;
	}
	
	if(newStaircase!=activeStaircase) {


	}
	while(!((activeStaircase->at(activeLevel)).empty())){
	
		pidR=(activeStaircase->at(activeLevel)).front();
		(activeStaircase->at(activeLevel)).pop();
		(newStaircase->at(level)).push(pidR);	//la bajo de nivel
		quotaLeft.find(pidR)->second = procQuota[level]; //le actualizo la quota individual de este nivel
		levelQuotaLeft[level] =+ procQuota[level];
		
	}
	
	if(newStaircase!=activeStaircase) swapStaircase();
	activeLevel = level;

}

	
