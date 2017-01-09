// SJF (Shortest Job First)

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

processo* Remove (processo* inicio, processo *proc) { // processo mais curto vai para execuçao (sai da fila) - proc recebe inicio
	if (inicio != NULL) {
		processo *aux = inicio;
		processo *aux2 = NULL;
		processo *ant = NULL;
		processo *ant2 = NULL;
		int tempo = aux->tempo; //tempo recebe o tempo do primeiro elemento da lista

		while (aux != NULL) {
			ant = aux;
			aux = aux->prox;
			if (aux != NULL && aux->tempo < tempo) {
				tempo = aux->tempo; // se o tempo do proximo elemento for menor do que o tempo do anterior, tempo recebe esse novo tempo
				ant2 = ant;
				aux2 = aux;
			}
		} // o while acaba com tempo possuindo o menor tempo e aux2 apontando para o processo que vai ser retirado
		
		if (aux2 == NULL) { // o no a ser removido eh o primeiro
			proc->ID = inicio->ID;
			aux2 = inicio;
			inicio = inicio->prox;
		}
		else { // o no a ser removido esta no meio ou no final da lista
			proc->ID = aux2->ID;
			ant2->prox = aux2->prox;
		}
		proc->tempo = tempo;
		proc->estado = 3;
		proc->prox = NULL;
		free (aux2);
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

processo* Executar (processo *proc, processo *block) { // so executa processos que foram removidos
	// faz a contagem regressiva ate que seja bloqueado ou termine a contagem
	int caminho = 1 + (rand() % 2);
	Imprimir_fila (proc);
	if (caminho == 1) { // o processo executa normalmente ate o fim
		sleep (proc->tempo);
		printf("\t\t\t\t\t\tO processo %d terminou!\n", proc->ID);
		proc->estado = 5;
	}
	else { // o processo e executado por 2 segundos e depois fica bloqueado (eh inserido na fila dos bloqueados)
		if (proc->tempo > 2) {
			sleep (2);
			proc->estado = 4;
			proc->tempo -= 2;
			block = Inserir_Bloqueado (proc, block);
			Imprimir_fila (block);
		}
		else {
			sleep (proc->tempo);
			printf("\t\t\t\t\t\tO processo %d terminou!\n", proc->ID);
			proc->estado = 5;
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
	int num_proc = 0, next, i;

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
			case 2: printf("\nExecutando o proximo processo mais curto da fila (se existir)\n");
					inicio = Remove (inicio, cpu); //atualiza inicio e atualiza os dados de cpu (com os dados do que estava antes no inicio)
					if((cpu->estado == 3) && (cpu->ID != 0)) {
						bloqueados = Executar (cpu, bloqueados);
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
