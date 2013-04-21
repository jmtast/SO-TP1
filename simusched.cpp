#include <vector>
#include "basetask.h"
#include "basesched.h"
#include "tasks.h"

using namespace std;


int main(int argc, char* argv[]) {
	tasks_init();
	srand(0);
	vector<ptskvi> ts = tasks_load("lote.tsk");
	
	return 0;
}
