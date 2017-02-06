/*************************************************
 *Nome: Amanda Barbosa Sobrinho     RA 151042251 *
 *Nome: Jo�o Ot�vio Gon�alves Calis RA 151044521 *
 *Compilador usado: Dev-C++ 5.11                 *
 *Simulador de banco de dados - �rvore B+        *
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
	int linhas[tamanho];//vetor com as linhas de cada aluno no arquivo
	struct no *filhos[tamanho+1];//vetor de ponteiros para os n�s filhos
	int ocupados;//contador de posi��es ocupadas no vetor aluno
	int folha;//vari�vel l�gica para descobrir se um n� � folha ou n�o
	struct no *pai;
}Arvore;

/*
 FUN��O AUXILIAR (usada para ler os alunos do arquivo)
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
 FUN��O AUXILIAR (usada para encontrar um caminho da raiz at� uma folha)
 Deve retornar um n� folha, que � o mais apropriado para a inser��o
*/
Arvore *encontrarFolha(Arvore **alunos, int endereco){
	Arvore *aux = *alunos;
	if(!aux)
		return NULL;
	int i;
	while(!aux->folha){// procurando um n� ate que ele sea uma folha
		i = 0;//se n�o � o contador reseta
		while(i < aux->ocupados){//para um n�, procurar o filho para "descer" de n�vel
			if(endereco >= aux->linhas[i])
				i++;
			else
				break;
		}
		aux = aux->filhos[i];//"descendo de n�vel"
	}
	return aux;//se chegou at� aqui encontrou uma folha
}

/*
 FUN��O AUXILIAR (usada para inserir um valor em um n� folha)
*/
Arvore *inserirEmFolha(Arvore **folha, int endereco){
	int i, localInsercao = 0;
	while(localInsercao < (*folha)->ocupados && (*folha)->linhas[localInsercao] < endereco)
		localInsercao++;
	for(i = (*folha)->ocupados; i > localInsercao; i--){
		(*folha)->linhas[i] = (*folha)->linhas[i-1];
		(*folha)->filhos[i] = (*folha)->filhos[i-1];
	}
	(*folha)->linhas[localInsercao] = endereco;
	(*folha)->ocupados++;
	return (*folha);
}

/*
 FUN��O AUXILIAR (usada para inserir efetivamente os registros na �rvore)
*/
void inserirArvore(Arvore **alunos, int endereco){
	if(*alunos == NULL){//�rvore vazia, devemos cri�-la
		*alunos = malloc(sizeof(Arvore));
		//inicializar ponteiros para filhos
		int i;
		for(i = 0; i <= tamanho; i++)
			(*alunos)->filhos[i] = 0;
		(*alunos)->ocupados = 1;
		(*alunos)->folha = 1;
		(*alunos)->linhas[0] = endereco;
		return;		
	}
	Arvore *folha = encontrarFolha(alunos, endereco);
	if(folha->ocupados < tamanho-1){//temos espa�o para inserir nessa folha
		folha = inserirEmFolha(alunos, endereco);
		return;
	}
}




/*
 Esta fun��o deve ler todos os valores v�lidos de linhas prensentes
 no arquivo especificado. Ela tamb�m � respons�vel por preencher a 
 �rvore inicialmente com esses dados.
*/
void lerArquivo(Arvore **alunos, FILE *arq){
	int linha;
	while(!feof(arq)){
		/* Este la�o � respons�vel por ler todas as linhas v�lidas do arquivo,
		   ou seja, todas as linhas que cont�m registros, se uma linha n�o �
		   v�lida ela contepm um '*' no inicio ao inv�s do seu n�mero. Dessa
		   forma, tentamos ler o n�mero da linha, se nada for lido, siginifca
		   que encontramos um '*' e a linha n�o � v�lida, caso contr�rio o
		   valor da linha � inserido.		
		*/
		
		// o especificador "%d%*[^\n]" nos faz ler um inteiro e descartar tudo at� o \n
		if(fscanf(arq, "%d%*[^\n]", &linha) == 0){
			fscanf(arq, "%*[^\n]");//se a linha � invalida, devemos descart�-la
			continue;	// tamb�m devemos pular o resto pois esta linha n�o deve ser inserida
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
	/* Devido a complexidade da implementa��o do c�digo, optamos por uma abordagem
	TOP-DOWN, onde a fun��o main cont�m apenas as chamadas as fun��es principais
	de cada tipo de opera��o prevista. Por sua vez, cada procedimento principal
	pode chamar n procedimentos auxiliares para realizar a tarefa.
	
	Esta abordagem facilita tanto a implementa��o quanto o entendimento do programa;
	*/
	FILE *arquivoDados;
	Arvore *arvoreAlunos;
	int opcao;
	
	/*Inicializa��o das estruturas usadas ao longo da aplica��o, ou seja:
	  -> Abrir o arquivo contendo os dados dos alunos
	  -> Inicializar a estrutura de �rvore B+
	*/
	if ((arquivoDados = fopen("dados.txt", "r+")) == NULL){
		printf("O arquivo com os dados n�o p�de ser aberto !");
		return EXIT_FAILURE;// Caso haja problemas na abertura devemos finalizar o programa.
	}
	
	arvoreAlunos = NULL;
	//Fim da inicializa��o
	
	// Iniciadas as estruturas, devemos popular a �rvore com os dados do arquivo
	lerArquivo(&arvoreAlunos, arquivoDados);
		
	//Estrutura de repeti��o executada para fornecer as op��es dispon�veis da aplica��o
	//Os detalhes do funcionamento de cada fun��o est�o especificados no seu corpo e antes da defini��o
	do{
	  	printf("\nOpcoes   do   simulador:\n");
		printf("1- Finalizar o simulador\n");
		printf("2- Cadastrar novo aluno\n");
		printf("3- Atualizar dados de aluno\n");
		printf("4- Excluir um aluno\n");
		printf("5- Imprimir alunos no formato lista\n");
		printf("6- Imprimir alunos no formato �rvore\n");
		printf("7- Buscar aluno por RA\n");
		scanf("%d%*c", &opcao);//%*c descarta um caractere no buffer, neste caso � o caractere '\n'
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
				printf("Op��o inv�lida!\n");				
		}
	}while(opcao != 0);
	destruirArvore(&arvoreAlunos);
	fclose(arquivoDados);
	return EXIT_SUCCESS;
}


