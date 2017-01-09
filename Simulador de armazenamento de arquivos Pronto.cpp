/************************************
 *Nome: Amanda Barbosa Sobrinho     *
 *Nome: João Otávio Gonçalves Calis *
 *Compilador usado: Dev-C++ 5.11    *
 *Sistema operacional: Windows 10   *
 *Simulador de armazenamento de     *
 *arquivos                          *
 *************************************/

#include <stdio.h> 
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>

typedef struct item{
	int ID, posicao;
	char nome[100];
	char extensao[7];
	FILE *conteudo;
	struct item *prox;
}Arquivo;

typedef struct no{
	Arquivo *arq;
	int tamanho;
	struct no *prox;
}Lista;

char *concatenar(char *nome, char *extensao){ // concatena o nome do arquivo com sua extensão
	char *juncao = (char*) malloc(strlen(nome)+strlen(extensao));
    strcpy(juncao, nome);
    strcat(juncao, extensao);
    return juncao;
}

void mostrarDisco(int disco[]){ // mostra o estado do vetor disco
	register int i;
	printf("\nEstado atual do disco:\n");
	for(i = 0; i < 300; i++)
		printf("%3d ", disco[i]);
	printf("\n");
}

void mostrarTabela(int tabela[]){ // mostra o estado do vetor tabela
	register int i;
	printf("\nEstado atual da tabela:\n");
	for(i = 0; i < 300; i++)
		printf("%3d ", tabela[i]);
	printf("\n");
}

int espacoDisponivel(int disco[]){ // mostra o espaço disponível no disco
	register int i, cont;
	for(i = 0, cont = 0; i < 300; i++)
		if(disco[i] == 0)
			cont ++;
	return cont;
}

int proximoZero(int disco[]){ // encontra a próxima posição vaga (igual a zero) no disco
	register int i;
	for (i = 0; i < 300; i++){
		if (disco[i] == 0)
			return i;
	}
	return -1;
}

void alocaTabela(int tabela[], int tam){ // coloca o arquivo na tabela de memória; admite que há espaço no vetor >= tam
	register int i = 0, j, inicio = -1, dec = tam;// dec decrementa a cada bloco de memória alocado
	while(tabela[i] != 0)
		i++;
	inicio = j = i++;
	while(dec && i < 300){
		while(tabela[i] != 0)
			i++;
		tabela[j] = i;
		dec--;
		j = i++;
	}	
	while(tabela[j] != 0)
		j++;
	tabela[j] = -1;
}
			
void desalocaTabela(int tabela[], int inicio){ // remove um arquivo da tabela de memória
	register int i = inicio, j;
	while(tabela[i] != -1){
		j = tabela[i];
		tabela[i] = 0;
		i = j;
	}
	tabela[i] = 0;
}

void alocaVetor(int disco[], int tam, int ID){ // coloca o arquivo no disco; admite que há espaço no vetor >= tam
	register int i = 0, dec = tam;// dec decrementa a cada bloco de memória alocado
	while(dec){ // enquanto ainda há um pedaço do arquivo a ser inserido
		while(disco[i] != 0) // acha um espaço vazio
			i++;
		disco[i] = ID;
		dec--;
		i++;
	}	
}

void desalocaVetor(int disco[], int ID){ // remove um arquivo do disco
	register int i = 0;
	while(i < 300){
		if(disco[i] == ID)
			disco[i] = 0;
		i++;
	}
}

int listarArquivos(Lista *arquivos){ // lista os arquivos no disco
	if(arquivos){
		Lista *aux = arquivos;
		int i=1;
	
		puts("--------------------------\n");
		puts(" Arquivos no disco: \n");
		while(aux){
			printf("%d - Nome: %s%s   -   Tamanho: %dw\n", i, aux->arq->nome, aux->arq->extensao, aux->tamanho);
			i++;
			aux=aux->prox;
		}		
		puts("--------------------------\n");
		return i-1;
	}
	else
		return 0;
}

int IdEscolhido(Lista *arquivos){ // retorna o ID de um arquivo escolhido
	int qtd = listarArquivos(arquivos), escolha = -1, i = 0;
	if(qtd){
		do{		
			printf("Digite o número arquivo escolhido: ");
			scanf("%d", &escolha);
		}while(escolha  < 0 || escolha > qtd);
		Lista *aux = arquivos;
		while(aux){ //passa por todas as posições
			i++;
			if(i == escolha)	//e quando chegar na desejada
				break;	//para o loop
	
			aux=aux->prox; //senão continua
		}
		return aux->arq->ID;
	}
	else{
		printf("Não há arquivos em disco!\n");
		return -1;
	}
		
}

Lista *novoArquivo(Lista *arquivos, int tabela[], int disco[], int *ID){ // cria um novo arquivo e o insere no disco, tabela e lista
	Lista *novo = (Lista*) malloc(sizeof(Lista));
	novo->tamanho = rand() % 19+2;
	novo->arq = (Arquivo*) malloc(sizeof(Arquivo));
	
	if(espacoDisponivel(disco) >= novo->tamanho){ // confere se existe espaço disponível no disco para guardar o novo arquivo
		FILE *conteudo;
		char texto[500];
		int posicao;
		novo->arq->ID = (*ID)++;
		printf("Informe o nome do novo arquivo:\n");
		scanf("%[^\n]%*c", novo->arq->nome);
		printf("Informe a extensão do novo arquivo:\n");
		scanf("%[^\n]%*c", novo->arq->extensao);
		conteudo = fopen(concatenar(novo->arq->nome, novo->arq->extensao), "w");
		printf("Informe o conteúdo do arquivo:\n");
		scanf("%[^\n]%*c", texto);
		fprintf(conteudo, "%s", texto);
		novo->arq->conteudo = conteudo;
		fclose(conteudo);
		posicao = proximoZero(disco); // acha a posição do próximo espaço disponível no disco
		novo->arq->posicao = posicao; // esta é a posição em que este pedaço do arquivo fica
		novo->arq->prox = NULL;
		novo->prox = NULL;
		Lista *aux = arquivos;
		alocaTabela(tabela, novo->tamanho-1); // insere o novo elemento na tabela de memória
		alocaVetor(disco, novo->tamanho, novo->arq->ID); // insere o novo elemento no disco
		if(aux){
			while(aux->prox) // coloca o novo arquivo na lista
				aux = aux->prox;
			aux->prox = novo;
		}
		else
			arquivos = novo;
		if(novo->tamanho > 1){ // se o tamanho do arquivo for maior do que 1, precisamos alocar os outros pedaços do arquivo (já alocou o primeiro)
			register int i, j;
			Arquivo *auxArq = novo->arq;
			for(i = 1, j = 0; i < novo->tamanho; i++, j++){
				auxArq->prox = (Arquivo*) malloc(sizeof(Arquivo));
				auxArq->prox->posicao = tabela[posicao+j]; // atribui a posição desse pedaço do arquivo
				auxArq->prox->ID = auxArq->ID;
				strcpy(auxArq->prox->nome, auxArq->nome);
				strcpy(auxArq->prox->extensao, auxArq->extensao);
				auxArq->prox->conteudo = auxArq->conteudo;
				auxArq->prox->prox = NULL;
				auxArq = auxArq->prox;
			}
			auxArq->posicao = posicao+j;
		}	
		
	}else{
		printf("Não foi possível inserir o novo arquivo pois o disco está cheio!\n");
		printf("Exclua arquivos e tente novamente!\n\n");
	}
	return arquivos;
}

Lista *removerArquivo(Lista *arquivos, int tabela[], int disco[], int ID){ // remove um arquivo do disco, tabela e lista
	Lista *aux = arquivos, *ant = NULL;
	if(aux){ //pois pode acorrer que a lista esteja vazia
		while(aux && aux->arq->ID != ID) {// acha o nó pai do arquivo a ser removido na lista principal e para nele
			ant = aux;
			aux = aux->prox;
		}
		if(aux && aux->arq->ID == ID){ // para ter certeza que deve ser excluído
			// pois pode ser que o elemento não seja encontrado
			// agora andamos pelos nós filhos desse nó principal, removendo-os
			desalocaTabela(tabela, aux->arq->posicao); // remove o elemento da tabela
			desalocaVetor(disco, aux->arq->ID); // remove o elemento do disco
			Arquivo *del, *delAnt = aux->arq;
			remove(concatenar(aux->arq->nome, aux->arq->extensao)); // apaga o arquivo real criado
			while(delAnt){ // remove os nós filhos da lista secundária
				del = delAnt->prox;
				free(delAnt);
				delAnt = del;
			}
		
			
			// removendo o nó pai da lista principal
			if(ant == NULL){ // remoção do nó inicial da lista
				arquivos = arquivos->prox;
				free(aux);
			}
			else{ // remoção do nó no meio ou final da lista
				ant->prox = aux->prox;
				free(aux);
			}
		}
	}
	return arquivos;
}

void limparDisco(int disco[]){ // limpa o disco (e a tabela) e a lista
	register int i;
	for(i = 0; i < 300; i++)
		disco[i] = 0;
}

void mostrarArquivos(Lista *arquivos){
	
	register int i, tam;
	if(arquivos){
		printf("\n\nArquivos contidos no disco:\n");
		Lista *auxL = arquivos;
		Arquivo *auxA;
		while(auxL){
			printf("---------------------------- ");
			for(i = 0; i < auxL->tamanho;i++){
				printf(" ----------");
				if((i+1)%7==0)
					break;
			}
			printf("\n| Nome:%15s%4s |->", auxL->arq->nome, auxL->arq->extensao);
			for(i = 0; i < auxL->tamanho;i++){
				printf("|Bloco %2d |", i+1);
				if((i+1)%7==0)
					break;
			}
			printf("\n");
			printf("| Tamanho:%16d |  ", auxL->tamanho);
			auxA = auxL->arq;
			for(i = 0; i < auxL->tamanho;i++){
				printf("|Pos  %3d |", auxA->posicao);
				auxA = auxA->prox;
				if((i+1)%7==0)
					break;
			}
			printf("\n| ID:%21d |  ", auxL->arq->ID);
			for(i = 0; i < auxL->tamanho;i++){
				printf(" ----------");
				if((i+1)%7==0)
					break;
			}
			printf("\n| Endereco:%15d |  ", auxL);
			printf("\n---------------------------- \n");
			if(auxL->tamanho > 7){
				for(i = 7; i < auxL->tamanho;i++){
					printf(" ----------");
				}
				printf("\n");
				for(i = 7; i < auxL->tamanho;i++){
					printf("|Bloco %2d |", i+1);
				}
				printf("\n");
				for(i = 0; i < auxL->tamanho;i++){
					printf("|Pos  %3d |", auxA->posicao);
					auxA = auxA->prox;
					if(!auxA)
						break;
				}
				printf("\n");
				for(i = 7; i < auxL->tamanho;i++){
					printf(" ----------");
				}
				printf("\n");
			}
		
			auxL = auxL->prox;
		}
		printf("\n\n");
	}
}

void mostrarTudo(int tabela[], int disco[], Lista *arquivos){ // mostra a tabela, o disco e a lista
	mostrarTabela(tabela);
	mostrarDisco(disco);
	mostrarArquivos(arquivos);
}

void espacoLivre(int disco[]){ // conta o número de blocos disponíveis no disco
	register int i, cont = 0;
	for(i = 0; i < 300; i++)
		if(disco[i] == 0)
			cont++;
	printf("\nEspaço disponível: %d blocos\n", cont);
}

void abrirArquivo(Lista *arquivos, int ID){ // abre um arquivo da lista
	if(ID > 0){
	Lista *aux = arquivos;
	while(aux->arq->ID != ID)
		aux = aux->prox;
	if(aux)
		system(concatenar("notepad ", concatenar(aux->arq->nome, aux->arq->extensao)));
	}
}

int main(){
	//srand(time(NULL));
	setlocale(LC_ALL, "portuguese");
	int tabela[300], disco[300], ID = 1, op;
	Lista *arquivos = NULL;
	limparDisco(tabela);
	limparDisco(disco);
	do{
		printf("\nOpcoes do simulador:\n");
		printf("0- Finaliza o simulador\n");
		printf("1- Criar um novo arquivo\n");
		printf("2- Imprimir a tabela, disco e arquivos em disco\n");
		printf("3- Imprimir apenas a tabela\n");
		printf("4- Imprimir apenas os arquivos em disco\n");
		printf("5- Excluir um arquivo do disco\n");
		printf("6- Abrir arquivo\n");
		printf("7- Mostrar espaço total disponível em disco\n");
		scanf("%d%*c", &op);
		system("cls");
		switch(op){
			case 0:
			break;
			case 1:
				arquivos = novoArquivo(arquivos, tabela, disco, &ID);
			break;
			case 2:
				mostrarTudo(tabela, disco, arquivos);
			break;
			case 3:
				mostrarTabela(tabela);
			break;
			case 4:
				mostrarDisco(disco);
				mostrarArquivos(arquivos);
			break;
			case 5:
				arquivos = removerArquivo(arquivos, tabela, disco, IdEscolhido(arquivos));
			break;
			case 6:
				abrirArquivo(arquivos, IdEscolhido(arquivos));
			break;
			case 7:
				espacoLivre(disco);
			break;
			default:
				printf("Opção inválida!\n");				
		}
	}while(op != 0);
	return 0;
}
