//Nome: João Otávio Gonçalves Calis
//RA: 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct processo{
	int ID, estado, tempo;
	struct processo *prox;
}Fila;

Fila *inserir(Fila *f, int *ID){
	Fila *novo = (Fila*) malloc(sizeof(Fila));
	novo->ID = ++*ID;
	novo->estado = 0;
	novo->tempo = 3 + (rand() % 5);
	novo->prox = NULL;
	if(f == NULL)		
		f = novo;
	else{
		Fila *aux = f;
		while(aux->prox)
			aux = aux->prox;
		aux->prox = novo;
	}
	return f;
}

Fila *remover(Fila *f, Fila *elemento){
	if(f != NULL){
		if(f == elemento){
			f = f->prox;
		} else{
			Fila *aux = f;
			while(aux->prox != elemento)
				aux = aux->prox;
			aux->prox = elemento->prox;
		}
		free(elemento);
	}
	return f;
}

void imprimir(Fila *f){
	Fila *aux = f;
	printf("\n=================================================");
	printf("\nElementos da fila:\n");
	while(aux != NULL){
		printf("\nID: %d\nESTADO: %s\nTEMPO: %d\n", aux->ID, aux->estado ? "Bloqueado": "Pronto", aux->tempo);
		aux = aux->prox;
	}
	printf("\n=================================================\n\n");
}

int processosProntos(Fila *f){
	int sentinela = 0;
	Fila *aux = f;
	while(aux && sentinela == 0){
		if(aux->estado == 0)
			sentinela = 1;
		aux = aux->prox;
	}
	return sentinela;
}

int processosBloqueados(Fila *f){
	int sentinela = 0;
	Fila *aux = f;
	while(aux && sentinela == 0){
		if(aux->estado == 1)
			sentinela = 1;
		aux = aux->prox;
	}
	return sentinela;
}

Fila *bloquear(Fila *f, Fila *elemento){
	Fila *temp = (Fila*) malloc(sizeof(Fila));
	if(elemento->estado == 0){
		printf("\nProcesso de ID %d bloqueado!\n\n", elemento->ID);
		temp->ID = elemento->ID;
		temp->estado = 1;
		temp->tempo = elemento->tempo;
		temp->prox = NULL;
		f = remover(f, elemento);
		// Reinserindo o elemento bloqueado no fim da fila
		if(f == NULL)		
			f = temp;
		else{
			Fila *aux = f;
			while(aux->prox)
				aux = aux->prox;
			aux->prox = temp;
		}
	}
	return f;
}

Fila *desbloquear(Fila *f){
	Fila *aux = f;
	if(processosBloqueados(f)){
		while(aux != NULL && aux->estado == 0)
			aux = aux->prox;

		if(aux != NULL){
			Fila *temp = (Fila*) malloc(sizeof(Fila)); 
			printf("\nProcesso de ID %d pronto!\n\n", aux->ID);
			temp->ID = aux->ID;
			temp->estado = 0;
			temp->tempo = aux->tempo;
			temp->prox = NULL;
			f = remover(f, aux);
			// Reinserindo o elemento desbloqueado no fim da fila
			if(f == NULL)		
				f = temp;
			else{
				Fila *aux2 = f;
				while(aux2->prox)
					aux2 = aux2->prox;
				aux2->prox = temp;
			}
		}
	}
	else
		printf("\nNenhum processo esta bloqueado!\n");
	return f;
}

Fila *FCFS(Fila *f){
	Fila *aux = f;
	int caminho = 1 + (rand() % 2);
	while(aux != NULL){
		if(aux->estado == 0){
			printf("\nExecutando processo\n");
			printf("ID: %d\nTEMPO NECESSARIO: %ds\n", aux->ID, aux->tempo);
			if (caminho == 1) { // o processo executa normalmente ate o fim
				sleep(aux->tempo);
				f = remover(f, aux);
				Fila *aux = f;
				printf("\nProcesso finalizado!\n");
			}
			else { // o processo e executado por 2 segundos e depois fica bloqueado
				if (aux->tempo > 2) {
					sleep (2);
					aux->tempo -= 2;
					f = bloquear(f, aux);
					Fila *aux = f;
				}
				else {
					sleep(aux->tempo);
					f = remover(f, aux);
					Fila *aux = f;
					printf("\nProcesso finalizado!\n");
				}
			}
		}
		if (aux)
			aux = aux->prox;
		caminho = 1 + (rand() % 2);
	}
	return f;
}

Fila *SJF(Fila *f){
	int caminho;
	Fila *aux = f, *temp;
	int menor;
	if(f != NULL){
		while(processosProntos(f)){
			aux = f, temp = NULL;
			menor = 1000;
			while(aux != NULL){ // acha o processo que tem menor tempo dentre os que estao prontos
				if(aux->estado == 0 && aux->tempo < menor){
					temp = aux;
					menor = aux->tempo;
				}
				aux = aux->prox;
			}
			caminho = 1 + (rand() % 2);	
			if(temp != NULL && temp->estado == 0){
				printf("\nExecutando processo\n");
				printf("ID: %d\nTEMPO NECESSARIO: %ds\n", temp->ID, temp->tempo);
				if (caminho == 1) { // o processo executa normalmente ate o fim
					sleep(temp->tempo);
					f = remover(f, temp);
					Fila *aux = f;
					printf("\nProcesso finalizado!\n");
				}
				else { // o processo e executado por 2 segundos e depois fica bloqueado
					if (temp->tempo > 2) {
						sleep (2);
						temp->tempo -= 2;
						f = bloquear(f, temp);
						Fila *aux = f;
					}
					else {
						sleep(temp->tempo);
						f = remover(f, temp);
						Fila *aux = f;
						printf("\nProcesso finalizado!\n");
					}
				}
			}
		}
	}
	return f;	
}

Fila *RR(Fila *f, int quantum){// quantum em segundos
	int caminho;
	if(f != NULL){
		Fila *auxp = f;
		
		imprimir(f);
		
		while(auxp != NULL){
			if(auxp->estado == 0){
				caminho = 1 + (rand() % 2);
				printf("\nExecutando processo:");
				printf("\nID: %d\nTEMPO NECESSARIO: %ds\n", auxp->ID, auxp->tempo);
				if(caminho == 1){
					if(auxp->tempo <= quantum){
						sleep(auxp->tempo);
						f = remover(f, auxp);
						Fila *auxp = f;
						printf("\nProcesso finalizado!\n");
					}else{
						sleep(quantum);
						auxp->tempo -= quantum;
						printf("\nQuantum excedido, o processo nao terminara agora!\n");
					}
				}
				else{ //bloqueia o processo
					if (auxp->tempo > 1) {
						sleep (1);
						auxp->tempo -= 1;
						f = bloquear(f, auxp);
						Fila *auxp = f;
					}
					else {
						sleep(auxp->tempo);
						f = remover(f, auxp);
						Fila *auxp = f;
						printf("\nProcesso finalizado!\n");
					}
				}
			}
			if (auxp)
				auxp = auxp->prox;
		}
		
	}
	return f;
}

int main(){
	Fila *f = NULL;
	int op = 0, quantum, ID = 0;
	do{
		printf("\nOpcoes do simulador:\n0-Finaliza o simulador\n");
		printf("1-Insere um processo na fila\n");
		printf("2-Imprime os processos da fila\n");
		printf("3-Executa o algoritmo FCFS\n");
		printf("4-Executa o algoritmo SJF\n");
		printf("5-Executa o algoritmo Round Robin\n");
		printf("6-Desbloquear processo\n");
		scanf("%d", &op);
		switch(op){
			case 0:
			break;
			case 1:
				f = inserir(f, &ID);
			break;
			case 2:
				imprimir(f);
			break;
			case 3:
				f = FCFS(f);
			break;
			case 4:
				f = SJF(f);
			break;
			case 5:
				printf("Digite um valor para o quantum em segundos: ");
				scanf("%d", &quantum);
				f = RR(f, quantum);
			break;
			case 6:
				f = desbloquear(f);
			break;
			default: 
				printf("Opcao invalida!");
		}
	}while(op != 0);
	return 0;
}
