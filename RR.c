// RR (Round-Robin)

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

typedef struct PROCESSO {
	int ID, estado, tempo;
	struct PROCESSO *prox;
} processo;


processo* Inserir (processo *inicio, int num_proc) { // processo novo eh criado e inserido na fila
	processo *aux = inicio;
	processo *novo = (processo*)malloc(sizeof(processo));
	novo->ID = ++num_proc;
	novo->tempo = 3 + (rand() % 5);
	novo->prox = NULL;
	novo->estado = 1;
	if (inicio == NULL) {
		inicio = novo;
	}
	else {
		while (aux->prox != NULL)
				aux = aux->prox;
		aux->prox = novo;
	}
	return inicio;
}

processo* Remove (processo* inicio, processo *proc) { // processo foi para execuçao (sai da fila) - proc recebe inicio
	if (inicio != NULL) {
		processo *aux = inicio;
		proc->ID = inicio->ID;
		proc->tempo = inicio->tempo;
		proc->estado = 3;
		proc->prox = NULL;
		inicio = inicio->prox;
		free (aux);
		return inicio;
	}
	else {
		proc->ID = 0;
		return NULL;
	}
}

void Imprimir_fila (processo *fila) {
	char Estado [10];
	processo *aux = fila;
	printf("\t-----------------------------\t\t-------------------------------------\n");
	while (aux != NULL) {
		switch (aux->estado) {
			case 1: strcpy (Estado, "Novo");
					break;
			case 2: strcpy (Estado, "Pronto");
					break;
			case 3: strcpy (Estado, "Executando");
					printf ("\t\t\t\t\t");
					break;
			case 4: strcpy (Estado, "Bloqueado");
					printf ("\t\t\t\t\t");
					break;
		}
		printf ("\t%d\t%s\t\t%d\n", aux->ID, Estado, aux->tempo);
		aux = aux->prox;
	}
	return;
}

processo* Inserir_Bloqueado (processo *proc, processo *inicio) { // insere um processo na fila de bloqueados
	processo *aux = inicio;
	processo *aux2 = (processo*)malloc(sizeof(processo));
	aux2->ID = proc->ID;
	aux2->tempo = proc->tempo;
	aux2->estado = proc->estado;
	aux2->prox = NULL;
	if (inicio == NULL) {
		inicio = aux2;
	}
	else {
		while (aux->prox != NULL)
			aux = aux->prox;
		aux->prox = aux2;
	}

	return inicio;
}

processo* Executar (processo *proc, processo *block, int quantum, int *flag) { // so executa processos que foram removidos
	// faz a contagem regressiva ate que seja bloqueado ou termine a contagem
	Imprimir_fila (proc);
	if (proc->tempo <= quantum) { // o processo executa normalmente ate o fim
		sleep (proc->tempo);
		printf("\t\t\t\t\t\tO processo %d terminou!\n", proc->ID);
		proc->estado = 5;
	}
	else { // se o tempo do processo for maior que o quantum, o processo eh executado pelo quantum e depois pode ser bloquado ou vai pro fim da fila
		int caminho = 1 + (rand() % 2);
		
		if (caminho == 1) { // processo vai pro fim da fila
			sleep (quantum);
			proc->tempo -= quantum;
			*flag = 1;
			proc->estado = 2;
		}
		else { // processo eh bloqueado
			sleep (2);
			proc->tempo -= 2;
			proc->estado = 4;
			block = Inserir_Bloqueado (proc, block);
			Imprimir_fila (block);
		}
	}
	return block;
}

processo* Reinserir (processo *proc, processo *inicio) { // pega um processo que esta bloqueado e o recoloca no fim da fila
	processo *aux = inicio;
	processo *novo = (processo*)malloc(sizeof(processo));
	novo->ID = proc->ID;
	novo->tempo = proc->tempo;
	novo->prox = NULL;
	novo->estado = 2;
	if (inicio == NULL) {
		inicio = novo;
	}
	else {
		while (aux->prox != NULL)
			aux = aux->prox;
		aux->prox = novo;
	}

	return inicio;
}


int main () {
	processo *inicio = NULL;
	processo *bloqueados = NULL;
	processo *cpu = (processo*)malloc(sizeof(processo)), *aux1, *aux2 = (processo*)malloc(sizeof(processo));
	int num_proc = 0, next, i, quantum = 4;
	int flag;

	printf("\n");
	printf ("\t\t\tFila\t\t\t\t\tCPU\n");
	printf("\t-----------------------------\t\t-------------------------------------\n");
	printf ("\tID\tEstado\t\tTempo\t\tID\tEstado\t\t\tTempo\t\n");
	inicio = Inserir (inicio, num_proc); // inserimos o primeiro processo na fila
	num_proc++;
	Imprimir_fila (inicio);
	
	for (i = 0; i <= 40; i++ ) {
		next = 1 + (rand() % 3); // sorteia um numero entre 1 e 3 para saber o que fazer em seguida
		switch (next) {
			case 1: printf("\nInserindo um novo processo\n");
					inicio = Inserir (inicio, num_proc);
					num_proc++;
					break;
			case 2: printf("\nExecutando o proximo processo da fila (se existir)\n");
					inicio = Remove (inicio, cpu); //atualiza inicio e atualiza os dados de cpu (com os dados do que estava antes no inicio)
					if((cpu->estado == 3) && (cpu->ID != 0)) {
						flag = 0;
						bloqueados = Executar (cpu, bloqueados, quantum, &flag);
						if (flag)
							inicio = Reinserir (cpu, inicio);
					}
					break;
			case 3: printf("\nReinserindo um processo bloqueado (se existir)\n");
					if (bloqueados) {
						bloqueados = Remove (bloqueados, aux2);
						inicio = Reinserir (aux2, inicio);
					}
		}
		if (inicio)
			Imprimir_fila (inicio);
		if (bloqueados)
			Imprimir_fila (bloqueados);
	}

	free (cpu);
	free (aux2);
	return 0;
}