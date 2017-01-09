/************************************
 *Nome: Amanda Barbosa Sobrinho     *
 *Nome: João Otávio Gonçalves Calis *
 *Compilador usado: Dev-C++ 5.11    *
 *Gerenciador e escalonador de procs*
 *************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define tMemoria 256 // tamanho da memória

typedef struct processo{ // esta estrutura controlará a fila com todos os processos na memória
	int ID, estado, tempo;
	struct processo *prox;
}Fila;

typedef struct item{ // esta estrutura controlará a lista dos processos e espaços livres em memória 
	int ID;			 //identificador do bloco
	int tamanho;	 //tamanho do bloco
	char status;	 //valores possíveis: l = livre, o = ocupado
	struct item *prox;
}Lista;

/*

Funções do gerenciador de memória

*/
void alocaVetor(int memoria[], int inicio, int tam, int ID){ // preenche o vetor memória com a ID do processo inserido
	register int i;
	for(i = inicio; i < tam+inicio; i++)
		memoria[i] = ID;
}
			
void desalocaVetor(int memoria[], int ID){ // tira do vetor memória a ID do processo retirado
	register int i;
	for(i = 0; i < 256; i++)
		if(memoria[i] == ID)
			memoria[i] = 0;
}

int alocaMem(int num, int *ID, Lista *lProcessos, int memoria[]){ //insere um novo processo no vetor memória e na lista que gerencia a memória 
	Lista *aux = lProcessos;
	int achou = 0;
	int tamAchado = 0;
	int posicao = 0;
	
	while(aux){ // encontra o primeiro espaço livre que tenha tamanho suficiente para inserir o novo processo
		if(aux->tamanho >= num && aux->status == 'l'){	
			achou = 1;
			tamAchado = aux->tamanho;
			break;
		}
		aux = aux->prox;
	}

	if(!achou)
		return 1;
	else{
		aux = lProcessos;
		while(aux->tamanho != tamAchado || aux->status != 'l' ) //vai para a posição encontrada em que o processo pode ser inserido
			aux = aux->prox;
		 
		if(aux->tamanho == num) { //se a quantidade solicitada for exatamente do tamanho do bloco
			aux->status = 'o'; //marca como ocupado
			aux->ID = *ID;     //coloca o ID do processo inserido naquele bloco
		}
		else { // se o tamanho do bloco for maior que o necessário, ocupa só o que precisa e tira o espaço que sobrar
			//tira "num" do bloco apontado por aux
			//bloco apontado por aux agora aponta para o novo bloco de tamanho "num"
			aux->tamanho = aux->tamanho - num;
			Lista *novo = (Lista*) malloc(sizeof(Lista)); //cria um novo nó
			novo->ID = *ID;			// com a ID do processo
			novo->tamanho = num;	// de tamanho num
			novo->status = 'o';		// ocupado
			novo->prox = aux->prox;	// que fica após o bloco escolhido		
			aux->prox = novo;
			aux = aux->prox;		//aux agora aponta para o bloco novo
		}

		// aux aponta para o novo processo inserido
		// contando os tamanhos dos blocos para descobrir a posição de inserção do novo processo no vetor memória
		Lista *aux2 = lProcessos;
		while(aux2 != aux){
			posicao += aux2->tamanho;
			aux2 = aux2->prox;
		}
		alocaVetor(memoria,posicao,aux->tamanho,aux->ID);
		
	}	
	return 0;
}
			
void getMemLivre(Lista *lProcessos){ // contabiliza a memória livre
	Lista *aux = lProcessos;
	int tamanho = 0;
	int maiorBloco = 0;
	while(aux){
		if(aux->status == 'l') { //se o bloco for livre
			tamanho += aux->tamanho;  //contabiliza
			if(aux->tamanho > maiorBloco)
				maiorBloco = aux->tamanho; //achar o maior bloco
		}
		aux = aux->prox;
	}

	puts("-------------------------------");
	printf("Quantidade de memoria livre: %d\n",tamanho);
	printf("Tamanho do maior bloco disponivel: %d\n",maiorBloco);
	puts("-------------------------------");
}		

void imprimeMem(Lista *lProcessos){ // impressão da memória
	Lista *aux = lProcessos;
	int i, tam;

	puts("--------------------------");
	while(aux){
		tam = aux->tamanho;
		for(i = 0; i < tam/100; i++) puts("|\t\t\t |");
			aux->status == 'l' ? printf("|  SEM ID \t\t |\n") : printf("|  ID: %d\t\t |\n",aux->ID);
		for(i = 0; i < tam/100; i++) puts("|\t\t\t |");
			printf("|  Endereco: %p\t |\n",aux);
		for(i = 0; i < tam/150; i++) puts("|\t\t\t |");
			printf("|  Tamanho: %d\t\t |\n",aux->tamanho);
		for(i = 0; i < tam/150; i++) puts("|\t\t\t |");
			aux->status == 'l' ? printf("|  Estado: Livre\t |\n") : printf("|  Estado: Ocupado\t |\n");
		for(i = 0; i < tam/100; i++) puts("|\t\t\t |");
			puts("--------------------------");
		aux = aux->prox;
	}
}

int contaOcupada(Lista *lProcessos){ // conta o número de blocos ocupados na memória
	Lista *aux = lProcessos;
	int i = 1;
	while(aux){
		if(aux->status == 'o')
			i++;
		aux = aux->prox;
	}
	return i-1;
}

void merge(Lista *lProcessos){ // une espaços livres vizinhos na lista que gerencia a memória
	Lista *aux = lProcessos;
	Lista *del;
	while(aux->prox){ //enquanto o próximo for válido também
		if(aux->status == 'l' && aux->prox->status == 'l'){ //e ambos forem livres
			aux->tamanho = aux->tamanho + aux->prox->tamanho; //soma os tamanhos dos dois blocos livres e atribui esse valor ao apontado por aux
			del = aux->prox;
			aux->prox = aux->prox->prox;
			free(del); // apaga o bloco seguinte ao apontado por aux
		}
		else
			aux = aux->prox;
	}
}		

	
/*

Funções do escalonador de processos

*/

Fila *inserir(Fila *f, int *ID){ // insere um processo novo na fila de processos
	Fila *novo = (Fila*) malloc(sizeof(Fila));
	novo->ID = *ID;
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

Fila *remover(Fila *f, Fila *elemento){ // remove um processo da lista de processos
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

void imprimir(Fila *f){ // imprime a fila de processos
	Fila *aux = f;
	printf("\n=================================================");
	printf("\nElementos da fila:\n");
	while(aux != NULL){
		printf("\nID: %d\nESTADO: %s\nTEMPO: %d\n", aux->ID, aux->estado ? "Bloqueado": "Pronto", aux->tempo);
		aux = aux->prox;
	}
	printf("\n=================================================\n\n");
}

int processosProntos(Fila *f){ // verifica se há processos prontos na fila
	int sentinela = 0;
	Fila *aux = f;
	while(aux && sentinela == 0){
		if(aux->estado == 0)
			sentinela = 1;
		aux = aux->prox;
	}
	return sentinela;
}

int processosBloqueados(Fila *f){ // verifica se há processos bloqueados na fila
	int sentinela = 0;
	Fila *aux = f;
	while(aux && sentinela == 0){
		if(aux->estado == 1)
			sentinela = 1;
		aux = aux->prox;
	}
	return sentinela;
}

Fila *bloquear(Fila *f, Fila *elemento){ // bloqueia um processo da fila
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

Fila *desbloquear(Fila *f){ // desbloqueia um processo que está bloqueado
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
			if (caminho == 1) { // o processo executa normalmente até o fim
				sleep(aux->tempo);
				f = remover(f, aux);
				Fila *aux = f;
				printf("\nProcesso finalizado!\n");
			}
			else { // o processo é executado por 2 segundos e depois fica bloqueado
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
		if(aux)
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
			while(aux != NULL){ // acha o processo que tem menor tempo dentre os que estão prontos
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
				if (caminho == 1) { // o processo executa normalmente até o fim
					sleep(temp->tempo);
					f = remover(f, temp);
					Fila *aux = f;
					printf("\nProcesso finalizado!\n");
				}
				else { // o processo é executado por 2 segundos e depois fica bloqueado
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

Fila *RR(Fila *f, int quantum){ // quantum em segundos
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
				else{ //o processo é executado por 1 segundo e depois fica bloqueado
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


/*

Funções para integração

*/

int buscaFila(Fila *f, int ID){ // diz se um ID está presente na fila de processos
	Fila *aux = f;
	while(aux){
		if(aux->ID == ID)
			return 1;
		aux = aux->prox;
	}
	return 0;
}

int buscaLista(Lista *lProcessos, int ID){ // diz se um ID está presente na lista que gerencia a memória
	Lista *aux = lProcessos;
	while(aux){
		if(aux->ID == ID)
			return 1;
		aux = aux->prox;
	}
	return 0;
}

Lista *removeLista(Lista *lProcessos, int ID){ // remove um processo da lista que gerencia a memória
	Lista *aux = lProcessos, *aux2;
	while(aux->ID != ID) // acha na lista a localização do processo buscado
		aux = aux->prox;
	aux2 = lProcessos;
	if(aux == aux2){ // se o processo buscado for o primeiro da lista
		aux =  aux->prox;
		free(aux2);
		return aux;
	}	
	while(aux->prox != aux2) // senão, procura pelo processo na lista
		aux2 = aux2->prox;
	aux2->prox = aux->prox;
	free(aux);
	return lProcessos;
}

void igualarEstruturas(Fila *f, Lista *lProcessos, int memoria[]){// remove os elementos que foram retirados da fila do vetor memória e da lista que a gerencia
	Lista *aux = lProcessos;
	int max = contaOcupada(lProcessos); // conta o número de processos na lista
	if(max > 0){ //tem processos na memória 
		if(f){ // se a fila tiver processos
			while(aux){ //percorre toda a lista em busca de elementos que precisam ser removidos (estão na lista mas não estão mais na fila)
				if(aux->status == 'o'){
					if(buscaFila(f, aux->ID) == 0){ // se o processo não está mais na fila
						aux->status = 'l'; // retira o processo da lista
						desalocaVetor(memoria,aux->ID); // retira o processo do vetor memória
						merge(lProcessos); // junta os espaços vazios vizinhos na lista
					}
				} 
				if(aux)
					aux = aux->prox;
			}
		}
		else { // se a fila estiver vazia, então a lista e o vetor memória devem ficar vazios também
			aux = lProcessos;
			while(aux){
				if(aux->status == 'o'){
					aux->status = 'l';
					desalocaVetor(memoria,aux->ID);
					merge(lProcessos);
				}
				if(aux)
					aux = aux->prox;
			}
		}
	}
}

int main(){
	Fila *f = NULL;
	int op = 0, quantum, ID = 1;
	int qMemoria, memoria[tMemoria], i;

	Lista *bl = (Lista*) malloc(sizeof(Lista));	//lista encadeada que representa a memória
	
	//limpar vetor memória
	for(op = 0; op < tMemoria; op++)
		memoria[op] = 0;
	
	//cria o primeiro bloco livre que contempla toda a memória
	bl->tamanho = tMemoria;
	bl->status = 'l';
	bl->prox = NULL;

	puts("========= Utilizando Metodo First Fit  =======");

	do{
		printf("\nOpcoes do simulador:\n0- Finaliza o simulador\n");
		printf("1-Insere um processo a fila\n");
		printf("2-Imprime os processos da fila\n");
		printf("3-Executa o algoritmo FCFS\n");
		printf("4-Executa o algoritmo SJF\n");
		printf("5-Executa o algoritmo Round Robin\n");
		printf("6-Desbloquear processo\n");
		printf("7-Mostrar maior espaco de memoria disponivel\n");
		scanf("%d", &op);
		//system("cls");
		switch(op){
			case 0:
			break;
			case 1:
				qMemoria =  rand() % 61 + 4;// gera um tamanho de processo
				if(alocaMem(qMemoria, &ID, bl, memoria) == 0){// tenta alocar na memória processo desse tamanho
					f = inserir(f, &ID);//se conseguiu, insere também na fila de processos
					ID++;
					puts("\nProcesso alocado com sucesso!\n");
				}
				else
					puts("\nErro - sem espaco disponivel!\n");
			break;
			case 2:
				printf("\nEstado da lista de memoria:\n");
				imprimeMem(bl);
				printf("\nVetor memoria:\n");
				for(i = 255; i > 0; i--)
					printf("%3d ", memoria[i]);
				printf("\n");
				printf("\nEstado da fila de processos:\n");
				imprimir(f);
			break;
			case 3:
				f = FCFS(f);
				igualarEstruturas(f, bl, memoria);
			break;
			case 4:
				f = SJF(f);
				igualarEstruturas(f, bl, memoria);
			break;
			case 5:
				printf("Digite um valor para o quantum em segundos: ");
				scanf("%d", &quantum);
				f = RR(f, quantum);
				igualarEstruturas(f, bl, memoria);
			break;
			case 6:
				f = desbloquear(f);
			break;
			case 7:
				getMemLivre(bl);
			break;
			default: 
				printf("Opcao invalida!");
		}
	}while(op != 0);
	return 0;
}
