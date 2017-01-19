/*************************************************
 *Nome: Amanda Barbosa Sobrinho     RA 151042251 *
 *Nome: Jo�o Ot�vio Gon�alves Calis RA 151044521 *
 *Compilador usado: Dev-C++ 5.11                 *
 *Simulador de banco de dados                    *
 *************************************************/

#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<locale.h>

int profundidade = 2; // profundidade global dos buckets

typedef struct aluno{ // struct que define cada registro de aluno que fica guardado nos buckets
	int linha, RA, idade;
	char nome[100];
	double media;
}Aluno;

typedef struct lista{ // struct da lista que representa o diret�rio de ponteiros para os buckets
	Aluno *bucket;
	struct lista *prox;
}Lista;

// Fun��o que imprime os hashes com a formata��o adequada (com os zeros � esquerda)
char *converterInteiro(int numero, int casas){
	// s � a string final de sa�da formatada, aux cont�m o n�mero bin�rio correspondente
	char *s =(char*) malloc(sizeof(char)*4), aux[4]; 
	// o n�mero recebido � convertido para bin�rio e colocado na string
	itoa(numero, aux, 2); // o terceiro par�metro diz que o n�mero � ser� convertido para a base bin�ria
	int t = strlen(aux), i;
	//colocamos zeros a esquerda na string final para padronizar os n�meros na hora da impress�o
	for(i = 0; i < casas - t; i++)
		s[i] = '0';
	s[i] = '\0';
	strcat(s,aux); //concatena o numero com a quantidade de zeros necess�ria
	return s;
}

// Fun��o que converte uma tupla (string com os dados de um aluno) para uma struct do tipo Aluno
Aluno *tuplaParaAluno(char *tupla){
	Aluno *aluno = malloc(sizeof(Aluno)); // alocando o aluno
	char *aux, *inicio = malloc(sizeof(char)*strlen(tupla));
	strcpy(inicio, tupla);
	aux = strtok(inicio,",*-/;"); //para segmentar a string ao encontrar um dos caracteres indicados
	aluno->linha = atoi(aux); //converte a substring com o valor da linha para inteiro
	aux = strtok(NULL,",*-/;"); //continua segmentando,
	aluno->RA = atoi(aux);
	aux = strtok(NULL, ",*-/;");
	strcpy(aluno->nome, aux);
	aux = strtok(NULL, ",*-/;");
	aluno->idade = atoi(aux);
	aux = strtok(NULL, ",*-/;");
	aluno->media = atof(aux); //atof converte uma string para float
	return aluno;
}

// Fun��o que obt�m os dados do aluno a ser inserido e os transforma em tupla (para posteriormente fazer a inser��o no arquivo de texto)
char *obterCadastroAluno(){
	char *tupla = malloc(sizeof(char)*200), aux[120];
	int x;
	float y;
	tupla[0] = '0';
	tupla[1] = ',';
	tupla[2] = '\0';
	printf("Digite o RA do aluno: ");
	scanf("%d", &x);
	sprintf(aux, "%d", x);
	strcat(tupla,aux);
	strcat(tupla,",");
	
	printf("Digite o nome do aluno: ");
	scanf("%*c%[^\n]%*c", aux); //ler tudo at� '\n' e descarta o pr�ximo caracte (com o %*c)
	strcat(tupla,aux);
	strcat(tupla,",");
	
	printf("Digite a idade do aluno: ");
	scanf("%d", &x);
	sprintf(aux, "%d", x);
	strcat(tupla,aux);
	strcat(tupla,",");
	
	printf("Digite a media anual do aluno: ");
	scanf("%f", &y);
	sprintf(aux, "%.3f", y);
	strcat(tupla,aux);
	
	return tupla;
}

// Fun��o que inicializa a lista que representa o diret�rio de ponteiros que aponta para os buckets
Lista *inicializarDiretorio(Lista *diretorio){
	int i,j;
	Lista *aux, *aux2;
	diretorio = (Lista*) malloc(sizeof(Lista)); // faz a aloca��o din�mica da lista (diret�rio)
	diretorio->bucket = (Aluno*) malloc(sizeof(Aluno)*5); //pois o primeiro elemento da lista guarda dados do bucket
	diretorio->bucket[0].RA = 2; //a primeira posi��o cont�m apenas a profundidade local (n�o � uma posi��o aloc�vel)
	for(i = 1; i < 5; i++)
		diretorio->bucket[i].RA = 0; //inicializa��o do bucket
	aux = diretorio;
	
	for(j = 0; j < 3; j++){ //faz o processo anterior para os pr�ximos 3 elementos da lista (e os 3 buckets para os quais eles apontam)
		aux2 = (Lista*) malloc(sizeof(Lista));
		aux2->bucket = (Aluno*) malloc(sizeof(Aluno)*5);
		aux2->bucket[0].RA = 2;
		for(i = 1; i < 5; i++)
			aux2->bucket[i].RA = 0; //inicializa��o do bucket
		aux->prox = aux2;
		aux = aux->prox;
	}
	aux->prox = NULL;
	printf("Registros carregados no diretorio com sucesso!\n");
	return diretorio;
}

// Procura onde deve ser inserido um novo aluno nos buckets
// Retorna 1 caso consiga inserir e 0 caso n�o seja poss�vel inserir (um bucket n�o possa mais ser duplicado)
int inserirNoDiretorio(Lista *diretorio, Aluno *aluno){
	Lista *aux = diretorio, *aux2, *aux3; // a vari�vel aux aponta para o diret�rio (lista de ponteiros para os buckets)
	int local, i = 0, j, cb; // a vari�vel local recebe o hash do RA do aluno que ser� inserido
	if(profundidade == 2) 
		local = aluno->RA % 4; // se a profundidade for 2, a fun��o hash � calculada com o m�dulo de 4
	else
		local = aluno->RA % 8; // se a profundidade for 3, a fun��o hash � calculada com o m�dulo de 8 
	while(i < local){ //move aux at� o elemento da lista que aponta para o bucket correto
		i++;
		aux = aux->prox;
	}
	i = 1;
	if (profundidade == 2) { // se a profundidade global for 2
		while(i < 5 && aux->bucket[i].RA != 0) //procura uma posi��o vazia no bucket encontrado anteriormente
			i++;
		if (i < 5) // se couber no bucket, faz a inser��o
			aux->bucket[i] = *aluno;
		else { // sen�o, aumenta a profundidade local e global
			aux3 = aux; // aux3 marca o bucket que ser� dividido
			while(aux->prox)
				aux = aux->prox; // chega at� o final da lista atual que corresponde ao diret�rio que aponta para os buckets
			for(j = 0; j < 4; j++){ // duplicando o diret�rio
				aux2 = (Lista*) malloc(sizeof(Lista));
				aux->prox = aux2;
				aux = aux->prox;
			}
			aux->prox = NULL;
			profundidade = 3; //alterando a profundidade global
			
			// fazendo os novos elementos do diret�rio apontarem para os mesmos buckets que os antigos
			aux = diretorio; // aux aponta para o diret�rio
			aux2 = aux->prox;
			i = 0;
			while (i < 3){ // faz aux2 apontar para o primeiro dos novos buckets
				aux2 = aux2->prox;
				i++;
			}
			while(aux2){ // fazendo os novos elementos do diret�rio apontarem para os mesmos buckets que os antigos
				aux2->bucket = aux->bucket;
				aux = aux->prox;
				aux2 = aux2->prox;
			}
			
			// dividindo o bucket no qual o novo aluno ser� inserido (a profundidade local aumentou)
			aux = diretorio;
			i = 0;
			while(i < local+4){// move aux at� o bucket que acabou de ser criado pela divis�o
				i++;
				aux = aux->prox;
			}
			aux->bucket = (Aluno*) malloc(sizeof(Aluno)*5); // cria o bucket
			aux->bucket[0].RA = 3; // com profundidade local 3
			aux3->bucket[0].RA = 3; // a profundidade do bucket que acabou de ser dividido tamb�m muda
			for(i = 1; i < 5; i++)
				aux->bucket[i].RA = 0; //inicializa��o do bucket rec�m-dividido
			
			// temos que realocar as chaves que estavam no bucket que acabou de ser dividido
			i = 1, j = 1;
			while(i < 5 && aux3->bucket[i].RA != 0) { // olha cada elemento dentro do bucket
				if(aux3->bucket[i].RA % 8 > 4) { // se o hash do aluno � maior que 4, o elemento vai para o novo bucket que acabou de ser criado (e sai do antigo)
					aux->bucket[j] = aux3->bucket[i];
					aux3->bucket[i].RA = 0;
					j++;
				}
				i++;
			}
			// inserindo a nova chave (novo aluno)
			i = 0;
			aux = diretorio;
			local = aluno->RA % 8;
			while(i < local){ //move aux at� o elemento do diret�rio que aponta para o bucket correto
				i++;
				aux = aux->prox;
			}
			i = 1;
			while(i < 5 && aux->bucket[i].RA != 0) //procura uma posi��o vazia no bucket
				i++;
			if (i < 5) // se couber no bucket, faz a inser��o
				aux->bucket[i] = *aluno;
			else
				return 0; //como fizemos a inser��o em um bucket de profundidade 3, se n�o couber, n�o ser� feita a inser��o
		}
	}
	else { // se a profundidade global for 3
		while(i < 5 && aux->bucket[i].RA != 0) // procura uma posi��o vazia no bucket
			i++;
		if (i < 5) // se couber no bucket, faz a inser��o
			aux->bucket[i] = *aluno;
		else {
			if (aux->bucket[0].RA == 3) // se n�o couber no bucket e se a profundidade local daquele bucket for 3, n�o d� para fazer a inser��o
				return 0;
			else { // se a profundidade local daquele bucket for 2, podemos aument�-la
				aux3 = aux; // aux3 marca o bucket que ser� dividido
				
				// dividindo o bucket
				aux = diretorio;
				i = 0;
				if (local < 4) {
					while(i < local+4){ // move aux at� o bucket mais inferior
						i++;
						aux = aux->prox;
					}
					// cb marca qual bucket ser� dividido, se � o inferior ou o superior
					cb = 0; // o bucket a ser dividido � o superior
				}
				else {
					while(i < local-4){ // move aux at� o bucket mais superior
						i++;
						aux = aux->prox;
					}
					cb = 1; // o bucket a ser dividido � o inferior
				}
				aux->bucket = (Aluno*) malloc(sizeof(Aluno)*5); // cria o bucket
				aux->bucket[0].RA = 3; // com profundidade local 3
				aux3->bucket[0].RA = 3; // a profundidade do bucket que acabou de ser dividido tamb�m muda
				for(i = 1; i < 5; i++)
					aux->bucket[i].RA = 0; // inicializa��o do bucket rec�m-dividido
				
				// temos que realocar as chaves que estavam no bucket que acabou de ser dividido
				i = 1, j = 1;
				while(i < 5 && aux3->bucket[i].RA != 0) {
					if (cb == 0) // se o bucket a ser dividido � o superior
						if(aux3->bucket[i].RA % 8 > 4) { // o elemento vai para o novo bucket (e sai do antigo)
							aux->bucket[j] = aux3->bucket[i];
							aux3->bucket[i].RA = 0;
							j++;
						}
					else // se o bucket a ser dividido � o inferior
						if(aux3->bucket[i].RA % 8 < 4) { // o elemento vai para o novo bucket (e sai do antigo)
							aux->bucket[j] = aux3->bucket[i];
							aux3->bucket[i].RA = 0;
							j++;
						}
					i++;
				}
				
				// inserindo a nova chave (novo aluno)
				i = 0;
				aux = diretorio;
				while(i < local){ // move aux at� o bucket correto
					i++;
					aux = aux->prox;
				}
				i = 1;
				while(i < 5 && aux->bucket[i].RA != 0) // procura uma posi��o vazia no bucket
					i++;
				if (i < 5) // se couber no bucket, faz a inser��o
					aux->bucket[i] = *aluno;
				else
					return 0; //como fizemos a inser��o em um bucket de profundidade 3, se n�o couber, n�o ser� feita a inser��o
			}	
		}
	}
	return 1;
}

// L� todos os arquivos presentes no arquivo de texto (disco) inicialmente e insere no diret�rio
Lista *lerArquivo(Lista *diretorio, FILE *dados){
	char tupla[150];
	Aluno *aux;
	while(!feof(dados)){
		// l� uma linha inteira do arquivo (onde cada linha � um registro de um aluno)
		fscanf(dados, "%[^\n]%*c", tupla); //l� tudo at� '\n' e descarta o pr�ximo caracte (com o %*c)
		if(!strstr(tupla, "*")){ // se � uma linha v�lida (strstr busca o * na tupla, que corresponde a uma linha exclu�da)
			aux = tuplaParaAluno(tupla); //converte a tupla (linha) lida para o tipo Aluno
			if(!inserirNoDiretorio(diretorio, aux)) //pode ocorrer de n�o ter mais espa�o para novas inser��es
				printf("Erro ao trazer registro para a mem�ria!\n");	
		}
	}
}

// Faz a impress�o do diret�rio e dos buckets para os quais ele aponta
void imprimeDiretorio(Lista *diretorio){
	Lista *aux = diretorio;
	int i,j;
	printf("\nListagem dos elementos do diretorio (profundidade global %d)\n", profundidade);
	printf("+-----------------------------------------------------------------------------------------------+\n");
	printf("|HSH| RA | NOME                                                                 | IDADE | MEDIA |\n");
	printf("+-----------------------------------------------------------------------------------------------+\n");
	for(i = 0; aux; aux = aux->prox, i++){ //percorre todos os elementos do diret�rio
		if(profundidade == 2) // para formatar a impress�o corretamente
			printf("| %s| Profundidade local %-71d|\n", converterInteiro(i, profundidade), aux->bucket[0].RA); 
		else
			printf("|%s| Profundidade local %-71d|\n", converterInteiro(i, profundidade), aux->bucket[0].RA);
		printf("|   |-------------------------------------------------------------------------------------------|\n");
		for(j = 1; j < 5; j++){ //percorre cada todos os elementos do bucket
			if(aux->bucket[j].RA)
				printf("|   | %-3d|%-70s| %-6d| %.3f |\n", aux->bucket[j].RA,aux->bucket[j].nome,aux->bucket[j].idade,aux->bucket[j].media);
			else
				printf("|   |    |                                                                      |       |       |\n");
		}
		printf("+-----------------------------------------------------------------------------------------------+\n");
	}
}

// Faz a impress�o apenas dos alunos contidos nos buckets
void imprimirAlunos(Lista *diretorio){
	Lista *aux = diretorio;
	int j;
	printf("+-------------------------------------------------------------------------------------------+\n");
	printf("| RA | NOME                                                                 | IDADE | MEDIA |\n");
	printf("+-------------------------------------------------------------------------------------------+\n");
	while(aux){
		for(j = 1; j < 5; j++){ //percorre cada todos os elementos do bucket
			if(aux->bucket[j].RA)
				printf("| %-3d|%-70s| %-6d| %.3f |\n", aux->bucket[j].RA,aux->bucket[j].nome,aux->bucket[j].idade,aux->bucket[j].media);
		}
		aux = aux->prox;
	}
	printf("+-------------------------------------------------------------------------------------------+\n");
}

// Para fazer a atualiza��o de algum dado de algum aluno
void atualizarAluno(Lista *diretorio, FILE *dados){
	imprimirAlunos(diretorio); // faz a impress�o dos alunos para auxiliar na escolha
	Lista *aux = diretorio, *aux2;
	int RA, i, x, op, encontrou = 0;
	float y;
	char nome[40];
	
	do{		
		printf("Digite o numero do RA desejado: ");
		scanf("%d", &RA);
	}while(RA <= 0); // se o RA for um n�mero menor ou igual a zero, � um valor inv�lido
	
	while(aux && !encontrou){ //percorre todos os elementos do diret�rio
		for(i = 1; i < 5; i++){ //percorre todos os elementos do bucket
			if (aux->bucket[i].RA == RA) { // se encontrou o RA procurado
				encontrou = 1;
				aux2 = aux;
				break;
			}
		}
		aux = aux->prox;
	}
	if (!encontrou) {
		printf ("\nO RA fornecido nao foi encontrado!\n");
		return;
	}
	do{
	  	printf("\nOpcoes   de   atualizacao:\n");
		printf("0- Sair\n");
		printf("1- Atualizar nome do aluno\n");
		printf("2- Atualizar idade do aluno\n");
		printf("3- Atualizar media do aluno\n");
		scanf("%d%*c", &op);
		switch(op){
			case 0:
			break;
			case 1:
				printf("Digite o novo nome do aluno: ");
				scanf("%[^\n]%*c", nome);
				strcpy(aux2->bucket[i].nome,nome);
				imprimirAlunos(diretorio);
			break;
			case 2:
				printf("Digite a nova idade do aluno: ");
				scanf("%d", &x);
				aux2->bucket[i].idade = x;
				imprimirAlunos(diretorio);
			break;
			case 3:
				printf("Digite a nova media do aluno: ");
				scanf("%f", &y);
				aux2->bucket[i].media = y;
				imprimirAlunos(diretorio);
			break;
			default:
				printf("Op��o inv�lida!\n");				
		}
	}while(op != 0);
	char *tupla = malloc (sizeof(char)*92); 
	// cria uma nova tupla com os dados modificados
	sprintf(tupla, "%-3d,%-3d,%-70s,%-6d,%.3f\n", aux2->bucket[i].linha,aux2->bucket[i].RA,aux2->bucket[i].nome,aux2->bucket[i].idade,aux2->bucket[i].media);
	// sobrescreve a tupla antiga com a nova no arquivo de texto
	fseek(dados,93*(aux2->bucket[i].linha),SEEK_SET); 
	fprintf(dados, "%s", tupla);
}

// Para fazer a exclus�o de um aluno
void excluirAluno(Lista *diretorio, FILE *dados){
	imprimirAlunos(diretorio);
	Lista *aux = diretorio, *aux2;
	int RA, i, x, op, encontrou = 0;
	float y;
	char nome[40];
	
	do{		
		printf("Digite o numero do RA desejado: ");
		scanf("%d", &RA);
	}while(RA <= 0); // se o RA for um n�mero menor ou igual a zero, � um valor inv�lido
	
	while(aux && !encontrou){ //percorre todos os elementos do diret�rio
		for(i = 1; i < 5; i++){ //percorre todos os elementos do bucket
			if (aux->bucket[i].RA == RA) { // se encontrou o RA procurado
				encontrou = 1;
				aux2 = aux;
				break;
			}
		}
		if(!encontrou) //pois pode ocorrer do for ter sido interrompido, ent�o n�o podemos incrementar a vari�vel auxiliar
			aux = aux->prox;
	}
	if (!encontrou) {
		printf ("\nO RA fornecido nao foi encontrado!\n");
		return;
	}
	aux->bucket[i].RA = 0;
	char *tupla = malloc(sizeof(char)*92);
	// faz a aloca��o de uma nova tupla que cont�m uma * no lugar do RA do aluno
	sprintf(tupla, "%-3d,*  ,%-70s,%-6d,%.3f\n", aux2->bucket[i].linha,aux2->bucket[i].nome,aux2->bucket[i].idade,aux2->bucket[i].media);
	// sobrescreve a tupla antiga com a nova no arquivo de texto
	fseek(dados,93*(aux2->bucket[i].linha),SEEK_SET);
	fprintf(dados, "%s", tupla);
	printf("Aluno excluido com sucesso!");
}

// Para fazer uma consulta de um aluno pelo RA
void consultarPorRA(Lista *diretorio){
	Lista *aux = diretorio;
	int RA, i;
	do{		
		printf("Digite o numero do RA a ser buscado: ");
		scanf("%d", &RA);
	}while(RA <= 0); // se o RA for um n�mero menor ou igual a zero, � um valor inv�lido
	while(aux){ //percorre todos os elementos do diret�rio
		for(i = 1; i < 5; i++){ //percorre todos os elementos do bucket
			if (aux->bucket[i].RA == RA) { // se encontrou o RA procurado, faz a impress�o
				printf("+-------------------------------------------------------------------------------------------+\n");
				printf("| RA | NOME                                                                 | IDADE | MEDIA |\n");
				printf("+-------------------------------------------------------------------------------------------+\n");
				printf("| %-3d|%-70s| %-6d| %.3f |\n", aux->bucket[i].RA,aux->bucket[i].nome,aux->bucket[i].idade,aux->bucket[i].media);
				printf("+-------------------------------------------------------------------------------------------+\n");
				return;
			}
		}
		aux = aux->prox;
	}
	printf ("\nO RA fornecido nao foi encontrado!\n"); // se n�o encontrou o RA buscado
}

int main(){
	FILE *dados;
	int op;
	if ((dados = fopen("dados.txt", "r+")) == NULL){
		printf("O arquivo com os dados nao pode ser aberto !");
		return 1;
	}
	Lista *diretorio;
	Aluno *aluno;
	diretorio = inicializarDiretorio(diretorio); // faz a inicializa��o da lista que � o diret�rio
	char *tupla = malloc(sizeof(char)*92), aux[100];
	lerArquivo(diretorio, dados); // faz a leitura do arquivo de dados (txt) e insere os registros no diret�rio
	
	do{
	  	printf("\nOpcoes   do   simulador:\n");
		printf("0- Finalizar o simulador\n");
		printf("1- Cadastrar novo aluno\n");
		printf("2- Atualizar dados de aluno\n");
		printf("3- Excluir um aluno\n");
		printf("4- Imprimir diretorio e buckets\n");
		printf("5- Imprimir alunos cadastrados\n");
		printf("6- Buscar por RA\n");
		scanf("%d%*c", &op);
		system("cls");
		switch(op){
			case 0:
			break;
			case 1:
				tupla = obterCadastroAluno(); // obt�m um aluno na forma de string
				if(inserirNoDiretorio(diretorio,tuplaParaAluno(tupla))){ // se inseriu na lista (que � o diret�rio)
					//faz a inser��o no arquivo de texto
					rewind(dados);
					char c = 0;
					int i=0, cont=0;
					// este loop procura um poss�vel registro que tenha sido exclu�do
					// estes registros s� marcados por um '*', caso um deles seja encontrado,
					// o novo registro ser� inserido nesse ponto, sobrescrevendo o aluno antigo
					while(c != '*' && !feof(dados)){ // enquanto n�o estiver no fim do arquivo e n�o acharmos uma linha que pode ser sobrescrita
						c = getc(dados);
						if(c == '\n')
							cont ++; // contamos a quantidade de linhas
						i++;
					}
					// precisamos reconverter a tupla para aluno pois ela ser� formatada no padr�o do arquivo
					aluno = tuplaParaAluno(tupla);
					aluno->linha = cont;
					sprintf(tupla, "%-3d,%-3d,%-70s,%-6d,%.3f\n", aluno->linha,aluno->RA,aluno->nome,aluno->idade,aluno->media);
					if(c == '*'){ //encontrou linha marcada como exclu�da, faz a inser��o nela
						fseek(dados,93*cont,SEEK_SET); // levando em conta cont
						fprintf(dados, "%s", tupla);
					} else{ //insere no final do arquivo, j� que n�o foi encontrada nenhuma linha marcada
						sprintf(tupla, "%-3d,%-3d,%-70s,%-6d,%.3f\n", aluno->linha+1,aluno->RA,aluno->nome,aluno->idade,aluno->media);
						tupla[strlen(tupla)-1] = '\0'; //insere um '\0' no fim da tupla
						fseek(dados,0,SEEK_END); //move para o fim do arquivo
						fprintf(dados, "\n%s", tupla);
					}
					printf("Inserido com sucesso\n");
				}
					
				else
					printf("Nao ha mais espaco para inserir alunos!\n");
			break;
			case 2:
				atualizarAluno(diretorio, dados);
			break;
			case 3:
				excluirAluno(diretorio, dados);
			break;
			case 4:
				imprimeDiretorio(diretorio);
			break;
			case 5:
				imprimirAlunos(diretorio);
			break;
			case 6:
				consultarPorRA(diretorio);
			break;
			default:
				printf("Op��o inv�lida!\n");				
		}
	}while(op != 0);
	
	fclose(dados);
	return 0;
}
