/*************************************************
 *Nome: Amanda Barbosa Sobrinho     RA 151042251 *
 *Nome: João Otávio Gonçalves Calis RA 151044521 *
 *Compilador usado: Dev-C++ 5.11                 *
 *Simulador de banco de dados - Árvore B+        *
 *************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define tamanho 7

typedef struct aluno{
	int linha, RA, idade;
	char nome[100];
	double media;
}Aluno;

typedef struct no{
	int aluno[tamanho];//vetor com os conteúdos de cada nó
	struct no *filhos[tamanho+1];//vetor de ponteiros para os nós filhos
	short ocupados;//contador de posições ocupadas no vetor aluno
	int folha;//variável lógica para descobrir se um nó é folha ou não
}Arvore;

/*
 FUNÇÃO AUXILIAR (usada para ler os alunos do arquivo)
 converte uma tupla (string com os dados de um aluno) para uma struct do tipo Aluno
*/
Aluno *converterTuplaParaAluno(char *tupla){
	Aluno *aluno = malloc(sizeof(Aluno));
	char *aux, *inicio = malloc(sizeof(char)*strlen(tupla));
	strcpy(inicio, tupla);
	aux = strtok(inicio,",*-/;");//para segmentar a string de ao encontrar um dos caracteres indicados
	aluno->linha = atoi(aux);//converte a substring com o valor da linha para inteiro
	aux = strtok(NULL,",*-/;");
	aluno->RA = atoi(aux);
	aux = strtok(NULL, ",*-/;");
	strcpy(aluno->nome, aux);
	aux = strtok(NULL, ",*-/;");
	aluno->idade = atoi(aux);
	aux = strtok(NULL, ",*-/;");
	aluno->media = atof(aux);//atof converte uma string para float
	return aluno;
}

/*
 FUNÇÃO AUXILIAR (usada para inserir efetivamente os registros na árvore)

*/
void inserirArvore(Arvore **alunos, int endereco){
	if(*alunos == NULL){//árvore vazia, devemos criá-la
		*alunos = malloc(sizeof(Arvore));
		//inicializar ponteiros para filhos
		int i;
		for(i = 0; i <= tamanho; i++)
			(*alunos)->filhos[i] = 0;
		(*alunos)->ocupados = 1;
		(*alunos)->folha = 1;
		(*alunos)->aluno[0] = endereco;
		return;		
	}
	
}




/*
 Esta função deve ler todos os valores válidos de linhas prensentes
 no arquivo especificado. Ela também é responsável por preencher a 
 árvore inicialmente com esses dados.
*/
void lerArquivo(Arvore **alunos, FILE *arq){
	int linha;
	while(!feof(arq)){
		/* Este laço é responsável por ler todas as linhas válidas do arquivo,
		   ou seja, todas as linhas que contém registros, se uma linha não é
		   válida ela contepm um '*' no inicio ao invés do seu número. Dessa
		   forma, tentamos ler o número da linha, se nada for lido, siginifca
		   que encontramos um '*' e a linha não é válida, caso contrário o
		   valor da linha é inserido.		
		*/
		
		// o especificador "%d%*[^\n]" nos faz ler um inteiro e descartar tudo até o \n
		if(fscanf(arq, "%d%*[^\n]", &linha) == 0){
			fscanf(arq, "%*[^\n]");//se a linha é invalida, devemos descartá-la
			continue;	// também devemos pular o resto pois esta linha não deve ser inserida
		}
		inserirArvore(alunos, linha);
	}
}

void cadastrarAluno(Arvore **alunos, FILE *arq){
	
}

void atualizarAluno(Arvore **arvoreAlunos, FILE *arq){
	
}

void excluirAluno(Arvore **arvoreAlunos, FILE *arq){
	
}

void imprimirLista(Arvore **arvoreAlunos){
	
}

void imprimirArvore(Arvore **arvoreAlunos){
	
}

void consultarPorRA(Arvore **arvoreAlunos){
	
}

void destruirArvore(Arvore **arvoreAlunos){
	
}

int main(){
	/* Devido a complexidade da implementação do código, optamos por uma abordagem
	TOP-DOWN, onde a função main contém apenas as chamadas as funções principais
	de cada tipo de operação prevista. Por sua vez, cada procedimento principal
	pode chamar n procedimentos auxiliares para realizar a tarefa.
	
	Esta abordagem facilita tanto a implementação quanto o entendimento do programa;
	*/
	FILE *arquivoDados;
	Arvore *arvoreAlunos;
	int opcao;
	
	/*Inicialização das estruturas usadas ao longo da aplicação, ou seja:
	  -> Abrir o arquivo contendo os dados dos alunos
	  -> Inicializar a estrutura de árvore B+
	*/
	if ((arquivoDados = fopen("dados.txt", "r+")) == NULL){
		printf("O arquivo com os dados não pôde ser aberto !");
		return EXIT_FAILURE;// Caso haja problemas na abertura devemos finalizar o programa.
	}
	
	arvoreAlunos = NULL;
	//Fim da inicialização
	
	// Iniciadas as estruturas, devemos popular a árvore com os dados do arquivo
	lerArquivo(&arvoreAlunos, arquivoDados);
		
	//Estrutura de repetição executada para fornecer as opções disponíveis da aplicação
	//Os detalhes do funcionamento de cada função estão especificados no seu corpo e antes da definição
	do{
	  	printf("\nOpcoes   do   simulador:\n");
		printf("1- Finalizar o simulador\n");
		printf("2- Cadastrar novo aluno\n");
		printf("3- Atualizar dados de aluno\n");
		printf("4- Excluir um aluno\n");
		printf("5- Imprimir alunos no formato lista\n");
		printf("6- Imprimir alunos no formato árvore\n");
		printf("7- Buscar aluno por RA\n");
		scanf("%d%*c", &opcao);//%*c descarta um caractere no buffer, neste caso é o caractere '\n'
		system("cls");
		switch(opcao){
			case 1:
			break;
			case 2:
				cadastrarAluno(&arvoreAlunos, arquivoDados);
			break;
			case 3:
				atualizarAluno(&arvoreAlunos, arquivoDados);
			break;
			case 4:
				excluirAluno(&arvoreAlunos, arquivoDados);
			break;
			case 5:
				imprimirLista(&arvoreAlunos);
			break;
			case 6:
				imprimirArvore(&arvoreAlunos);
			break;
			case 7:
				consultarPorRA(&arvoreAlunos);
			break;
			default:
				printf("Opção inválida!\n");				
		}
	}while(opcao != 0);
	destruirArvore(&arvoreAlunos);
	fclose(arquivoDados);
	return EXIT_SUCCESS;
}


