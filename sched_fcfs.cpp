#include <vector>
#include <queue>
#include "sched_fcfs.h"

using namespace std;

SchedFCFS::SchedFCFS(vector<int> argn) {
        
        vector<int>::iterator it = cores.begin();

        for (int i=0; i < argn[0]; i++){
                
                it=cores.insert(it,-1);
                }
        
}

SchedFCFS::~SchedFCFS() {
}

void SchedFCFS::load(int pid) {
        
        //procesos.push(pid);
        
        int i;          
        for ( i=0; i< cores.size() ; i++){
                
                if (cores[i] == -1 ){
                         cores[i] = pid;
                         break;
                }
                        
        }       
        
        if(i==cores.size()) procesos.push(pid);
        

}

void SchedFCFS::unblock(int pid) {
                
/*      for (int i=0; i< cores.size() ; i++){
                
                if (cores[i] == -1 ){
                         cores[i] = pid;
                         break;
                }
                        
        }       
        
        if(i==cores.size()) procesos.push(pid);*/
        
}

int SchedFCFS::tick(int cpu, const enum Motivo m) {
        switch (m){

        case EXIT:
                
                //procesos.pop();
                int pid;
                if ( procesos.empty() ) {
                
                        pid = IDLE_TASK;                        
                }
                else {
                
                        pid = procesos.front();
                        procesos.pop();                 
                }       
                cores[cpu] = pid;
                return pid;
                
        
        break;

        case TICK:

        /*      if (procesos.empty()){
                         return IDLE_TASK;
                }       
                else{
                        int pid= procesos.front();
                        procesos.pop();                         
                        procesos.push(pid);
                        return procesos.front();                
                        
                

                return current_pid(cpu); */

                return cores[cpu];

        break;

        case BLOCK:

                return cores[cpu];
        
        break;

}
                

}
 
