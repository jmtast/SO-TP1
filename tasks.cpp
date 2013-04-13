#include "tasks.h"

using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}



void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
}

// Ejercicio 1 Programar un tipo de tarea TaskConsola, que simular ́a una tarea interactiva. La tarea debe realizar n llamadas bloqueantes, cada una de una duraci ́on al
// azar1 entre bmin y bmax (inclusive). La tarea debe recibir tres par ́ametros: n, bmin y bmax (en ese orden) que ser ́an interpretados como los tres elementos del vector 
// de enteros que recibe la funci ́on.

void TaskConsola(int pid, vector<int> params) { // params: n, bmin y bmax (n = cant de llamadas bloqueantes a IO, bmin y bmax boundaries inferior y superior)
	int cantidad = params[0];
	int bmin = params[1];
	int bmax = params[2];
	while (cantidad > 0) {
		int duracion = (rand() % (bmax - bmin)) + min;	// rand da un número random entre 0 y una constante RAND_MAX (ver man 3 rand)
		uso_IO(pid, duracion);
		cantidad--;
	}
}