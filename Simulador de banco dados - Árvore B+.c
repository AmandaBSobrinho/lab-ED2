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

/*

C�DIGO COPIADO

*/

#define bool char
#define falso 0
#define verdadeiro 1
#define tamanhoPadrao 4
#define tamanhoMinimo 3 // o tamanhoMinimo � necessariamente 3
#define tamanhoMaximo 20 // esse tamanhoMaximo pode mudar

// Tipos

// registro em rela��o ao qual uma chave se refere
typedef struct registro { 
	int valor; // o registro pode conter dados ou um arquivo
} registro;

// tipo que representa um n� na �rvore B+; serve tanto para n�s folha como para n�s internos
typedef struct no {
	void **ponteiros; // arranjo de ponteiros correspondentes �s chaves
	int *chaves; // arranjo de chaves
	struct no *pai;
	bool ehFolha; // para dizer se um n� � folha ou n�o
	int numChaves; // conta o n�mero de chaves v�lidas; em um n� interno, o n�mero de ponteiros v�lidos � numChaves+1; no n� folha, esse n�mero � numChaves 
	struct no *proximo; // aponta para o pr�ximo n� folha
} no;
// a rela��o entre chaves e ponteiros � diferente para n�s folha e n�s internos.
// No n� folha, o �ndice de cada chave � igual ao �ndice do seu ponteiro correspondente, com um n�mero m�ximo de pares chave-ponteiro igual a ordem-1,
// e o �ltimo ponteiro aponta para o n� folha � direita.
// Em um n� interno, o primeiro ponteiro refere-se aos n�s mais baixos com chaves menores do que a menor chave no arranjo de chaves.
// Ent�o, com �ndices i come�ando em 0, o ponteiro em i+1 aponta para a sub�rvore com chaves maiores ou iguais � chave neste n� com �ndice i.

// Vari�veis Globais

int ordem = tamanhoPadrao; // a ordem determina os n�meros m�ximo e m�nimo de entradas (chaves e ponteiros) em um n�
// cada n� tem no m�ximo ordem-1 chaves e no m�nimo metade desse n�mero
// cada n� folha tem tantos ponteiros para dados quanto chaves, e cada n� interno tem um ponteiro a mais para uma sub�rvore do que o n�mero de chaves

no *fila = NULL; // usada para imprimir a �rvore em ordem de n�veis, come�ando da raiz e indo at� as folhas

// Fun��es da �rvore B+

// OUTPUT AND UTILITIES

/* Routine to print portion of GPL license to stdout.
 */
void print_license( int license_part ) {
	int start, end, line;
	FILE * fp;
	char buffer[0x100];

	switch(license_part) {
	case LICENSE_WARRANTEE:
		start = LICENSE_WARRANTEE_START;
		end = LICENSE_WARRANTEE_END;
		break;
	case LICENSE_CONDITIONS:
		start = LICENSE_CONDITIONS_START;
		end = LICENSE_CONDITIONS_END;
		break;
	default:
		return;
	}

	fp = fopen(LICENSE_FILE, "r");
	if (fp == NULL) {
		perror("print_license: fopen");
		exit(EXIT_FAILURE);
	}
	for (line = 0; line < start; line++)
		fgets(buffer, sizeof(buffer), fp);
	for ( ; line < end; line++) {
		fgets(buffer, sizeof(buffer), fp);
		printf("%s", buffer);
	}
	fclose(fp);
}

/* Second message to the user.
 */
void usage_2( void ) {
	printf("Enter any of the following commands after the prompt > :\n"
	"\ti <k>  -- Insert <k> (an integer) as both chave and valor).\n"
	"\tf <k>  -- Find the valor under chave <k>.\n"
	"\tp <k> -- Print the path from the raiz to chave k and its associated "
           "valor.\n"
	"\tr <k1> <k2> -- Print the chaves and values found in the range "
			"[<k1>, <k2>\n"
	"\td <k>  -- deletar chave <k> and its associated valor.\n"
	"\tx -- Destroy the whole tree.  Start again with an empty tree of the "
           "same ordem.\n"
	"\tt -- Print the B+ tree.\n"
	"\tl -- Print the chaves of the leaves (bottom row of the tree).\n"
	"\tv -- Toggle output of ponteiro addresses (\"verbose\") in tree and "
           "leaves.\n"
	"\tq -- Quit. (Or use Ctl-D.)\n"
	"\t? -- Print this help message.\n");
}

// Fun��o que auxilia na impress�o da �rvore B+; � usada na fun��o imprimirArvore
void enfileirar (no *novoNo) {
	no *c;
	if (fila == NULL) {
		fila = novoNo;
		fila->proximo = NULL;
	}
	else {
		c = fila;
		while(c->proximo != NULL) {
			c = c->proximo;
		}
		c->proximo = novoNo;
		novoNo->proximo = NULL;
	}
}

// Fun��o que auxilia na impress�o da �rvore B+; tamb�m � usada na fun��o imprimirArvore
no *desenfileirar (void) {
	no *n = fila;
	fila = fila->proximo;
	n->proximo = NULL;
	return n;
}

// Imprime a fileira mais inferior de chaves da �rvore
void imprimirFolhas (no *raiz) {
	int i;
	no *c = raiz;
	if (raiz == NULL) {
		printf("�rvore vazia!\n");
		return;
	}
	while (!c->ehFolha)
		c = (no*) c->ponteiros[0];
	while (verdadeiro) {
		for (i = 0; i < c->numChaves; i++)
			printf("%d ", c->chaves[i]);
		if (c->ponteiros[ordem - 1] != NULL) {
			printf(" | ");
			c = (no*) c->ponteiros[ordem - 1];
		}
		else
			break;
	}
	printf("\n");
}

// Informa a altura da �rvore
int altura (no *raiz) {
	int h = 0;
	no *c = raiz;
	while (!c->ehFolha) {
		c = (no*) c->ponteiros[0];
		h++;
	}
	return h;
}

// Informa o n�mero de arestas no caminho de qualquer n� at� a raiz; � usada na fun��o imprimirArvore
int distanciaRaizNo (no *raiz, no *filho) {
	int distancia = 0;
	no *c = filho;
	while (c != raiz) {
		c = c->pai;
		distancia++;
	}
	return distancia;
}

// Imprime a �rvore B+ em ordem de n�veis, com as chaves em cada n� e o s�mbolo '|' para separar os n�s
void imprimirArvore (no *raiz) {
	no *n = NULL;
	int i = 0;
	int caminho = 0;
	int novoCaminho = 0;

	if (raiz == NULL) {
		printf("�rvore vazia!\n");
		return;
	}
	fila = NULL;
	enfileirar (raiz);
	while (fila != NULL) {
		n = desenfileirar();
		if (n->pai != NULL && n == n->pai->ponteiros[0]) {
			novoCaminho = distanciaRaizNo (raiz, n);
			if (novoCaminho != caminho) {
				caminho = novoCaminho;
				printf("\n");
			}
		}
		for (i = 0; i < n->numChaves; i++)
			printf("%d ", n->chaves[i]);
		if (!n->ehFolha)
			for (i = 0; i <= n->numChaves; i++)
				enfileirar((no*)n->ponteiros[i]);
		printf("| ");
	}
	printf("\n");
}

// Encontra um registro correspondente a uma dada chave e o imprime
void buscarImprimir (no *raiz, int chave) {
	registro *r = buscar (raiz, chave);
	if (r == NULL)
		printf("Registro n�o encontrado com a chave %d.\n", chave);
	else 
		printf("Registro em %lx -- chave %d, valor %d.\n", (unsigned long*)r, chave, r->valor);
}

// Encontra e imprime as chaves, ponteiros e valores em um range de chaves entre chaveInicial e chaveFinal, incluindo os limites
void buscarImprimirRange (no *raiz, int chaveInicial, int chaveFinal) {
	int i;
	int tamanhoArranjo = chaveFinal - chaveInicial + 1;
	int chavesEncontradas[tamanhoArranjo];
	void *ponteirosEncontrados[tamanhoArranjo];
	int numeroEncontrado = buscarRange (raiz, chaveInicial, chaveFinal, chavesEncontradas, ponteirosEncontrados);
	if (!numeroEncontrado)
		printf("Nenhum encontrado!\n");
	else {
		for (i = 0; i < numeroEncontrado; i++)
			printf("Chave: %d   Localiza��o: %lx  Valor: %d\n", chavesEncontradas[i], (unsigned long*)ponteirosEncontrados[i], ((registro *)ponteirosEncontrados[i])->valor);
	}
}

// Encontra chaves e seus ponteiros, se presentes, no range especificado por chaveInicial e chaveFinal, e os coloca nos arranjos chavesEncontradas e ponteirosEncontrados
// � usada na fun��o buscarImprimirRange
int buscarRange (no *raiz, int chaveInicial, int chaveFinal, int chavesEncontradas[], void *ponteirosEncontrados[]) {
	int i, numeroEncontrado;
	numeroEncontrado = 0;
	no *n = buscarFolha (raiz, chaveInicial);
	if (n == NULL) 
		return 0;
	for (i = 0; i < n->numChaves && n->chaves[i] < chaveInicial; i++);
	if (i == n->numChaves) 
		return 0;
	while (n != NULL) {
		for ( ; i < n->numChaves && n->chaves[i] <= chaveFinal; i++) {
			chavesEncontradas[numeroEncontrado] = n->chaves[i];
			ponteirosEncontrados[numeroEncontrado] = n->ponteiros[i];
			numeroEncontrado++;
		}
		n = (no*) n->ponteiros[ordem - 1];
		i = 0;
	}
	return numeroEncontrado; // retorna o n�mero de entradas encontradas
}

// Faz o caminho da raiz at� uma folha, procurando por chave
// � usada nas fun��es inserir, deletar, buscar e buscarRange
no *buscarFolha (no *raiz, int chave) {
	int i = 0;
	no *c = raiz;
	if (c == NULL) {
		printf("�rvore vazia!\n");
		return c;
	}
	while (!c->ehFolha) {
		printf("[");
		for (i = 0; i < c->numChaves - 1; i++)
			printf("%d ", c->chaves[i]);
		printf("%d] ", c->chaves[i]);
		i = 0;
		while (i < c->numChaves) {
			if (chave >= c->chaves[i]) 
				i++;
			else 
				break;
		}
		printf("%d ->\n", i);
		c = (no*)c->ponteiros[i];
	}
	printf("Leaf [");
	for (i = 0; i < c->numChaves - 1; i++)
		printf("%d ", c->chaves[i]);
	printf("%d] ->\n", c->chaves[i]);
	return c; // retorna a folha contendo a chave buscada
}

// Encontra e retorna o registro ao qual uma chave se refere
// � usada nas fun��es inserir, deletar e buscarImprimir
registro *buscar (no *raiz, int chave) {
	int i = 0;
	no *c = buscarFolha( raiz, chave);
	if (c == NULL) 
		return NULL;
	for (i = 0; i < c->numChaves; i++)
		if (c->chaves[i] == chave) 
			break;
	if (i == c->numChaves) 
		return NULL;
	else
		return (registro*)c->ponteiros[i];
}

// Encontra o local apropriado para dividir um n� muito grande ao meio
int dividir (int distancia) {
	if (distancia % 2 == 0)
		return distancia/2;
	else
		return distancia/2 + 1;
}

// INSER��O

// Cria um novo registro para guardar o valor ao qual uma chave se refere
registro *criarRegistro (int valor) {
	registro *novoRegistro = (registro*)malloc(sizeof(registro));
	if (novoRegistro == NULL)
		printf ("\nErro na cria��o do novo registro!\n");
	else
		novoRegistro->valor = valor;
	return novoRegistro;
}

// Cria um novo n� geral, que pode servir tanto como n� folha ou interno
no *criarNo (void) {
	no *novoNo;
	novoNo = (no*) malloc(sizeof(no));
	if (novoNo == NULL)
		printf ("\nErro na cria��o do n�!\n");
	novoNo->chaves = (int*) malloc( (ordem - 1) * sizeof(int) );
	if (novoNo->chaves == NULL)
		printf ("\nErro na aloca��o das chaves no n�!\n");
	novoNo->ponteiros = (void**) malloc( ordem * sizeof(void *) );
	if (novoNo->ponteiros == NULL)
		printf ("\nErro na aloca��o dos ponteiros no n�!\n");
	novoNo->ehFolha = falso;
	novoNo->numChaves = 0;
	novoNo->pai = NULL;
	novoNo->proximo = NULL;
	return novoNo;
}

// Cria uma nova folha criando um n� e adaptando-o apropriadamente
no *criarFolha (void) {
	no *folha = criarNo();
	folha->ehFolha = verdadeiro;
	return folha;
}

// Fun��o auxiliar usada na fun��o inserirNoPai para buscar o �ndice do ponteiro do n� pai para o n� � esquerda da chave a ser inserida
int obterIndiceEsquerdo (no *pai, no *esquerdo) {
	int indiceEsquerdo = 0;
	while (indiceEsquerdo <= pai->numChaves && pai->ponteiros[indiceEsquerdo] != esquerdo)
		indiceEsquerdo++;
	return indiceEsquerdo;
}

// Insere um novo ponteiro em um registro e sua chave correspondente em um n� folha
no *inserirNaFolha (no *folha, int chave, registro *ponteiro) {
	int i, pontoDeInsercao;
	pontoDeInsercao = 0;
	while (pontoDeInsercao < folha->numChaves && folha->chaves[pontoDeInsercao] < chave)
		pontoDeInsercao++;

	for (i = folha->numChaves; i > pontoDeInsercao; i--) {
		folha->chaves[i] = folha->chaves[i - 1];
		folha->ponteiros[i] = folha->ponteiros[i - 1];
	}
	folha->chaves[pontoDeInsercao] = chave;
	folha->ponteiros[pontoDeInsercao] = ponteiro;
	folha->numChaves++;
	return folha; // retorna o n� folha alterado
}

// Insere uma nova chave e ponteiro em um novo registro em um n� folha para exceder a ordem da �rvore, levando a folha a ser dividida ao meio
no *inserirNaFolhaAposDividir (no *raiz, no *folha, int chave, registro *ponteiro) {
	no *novaFolha;
	int *chavesTemp;
	void **ponteirosTemp;
	int indiceInsercao, dividir, novaChave, i, j;

	novaFolha = criarFolha();

	chavesTemp = (int*) malloc( ordem * sizeof(int) );
	if (chavesTemp == NULL)
		printf ("\nErro na aloca��o do vetor chavesTemp!\n");

	ponteirosTemp =(void**) malloc( ordem * sizeof(void *) );
	if (ponteirosTemp == NULL)
		printf ("\nErro na aloca��o do vetor ponteirosTemp!\n");

	indiceInsercao = 0;
	while (indiceInsercao < ordem - 1 && folha->chaves[indiceInsercao] < chave)
		indiceInsercao++;

	for (i = 0, j = 0; i < folha->numChaves; i++, j++) {
		if (j == indiceInsercao) j++;
		chavesTemp[j] = folha->chaves[i];
		ponteirosTemp[j] = folha->ponteiros[i];
	}

	chavesTemp[indiceInsercao] = chave;
	ponteirosTemp[indiceInsercao] = ponteiro;

	folha->numChaves = 0;

	dividir = dividir(ordem - 1);

	for (i = 0; i < dividir; i++) {
		folha->ponteiros[i] = ponteirosTemp[i];
		folha->chaves[i] = chavesTemp[i];
		folha->numChaves++;
	}

	for (i = dividir, j = 0; i < ordem; i++, j++) {
		novaFolha->ponteiros[j] = ponteirosTemp[i];
		novaFolha->chaves[j] = chavesTemp[i];
		novaFolha->numChaves++;
	}

	free(ponteirosTemp);
	free(chavesTemp);

	novaFolha->ponteiros[ordem - 1] = folha->ponteiros[ordem - 1];
	folha->ponteiros[ordem - 1] = novaFolha;

	for (i = folha->numChaves; i < ordem - 1; i++)
		folha->ponteiros[i] = NULL;
	for (i = novaFolha->numChaves; i < ordem - 1; i++)
		novaFolha->ponteiros[i] = NULL;

	novaFolha->pai = folha->pai;
	novaChave = novaFolha->chaves[0];

	return inserirNoPai (raiz, folha, novaChave, novaFolha);
}

// Insere uma nova chave e ponteiro em um n� em que eles podem caber sem violar as propriedades da �rvore
no *inserirNoNo (no *raiz, no *n, int indiceEsquerdo, int chave, no *direito) {
	int i;
	for (i = n->numChaves; i > indiceEsquerdo; i--) {
		n->ponteiros[i + 1] = n->ponteiros[i];
		n->chaves[i] = n->chaves[i - 1];
	}
	n->ponteiros[indiceEsquerdo + 1] = direito;
	n->chaves[indiceEsquerdo] = chave;
	n->numChaves++;
	return raiz;
}

// Insere uma nova chave e ponteiro em um n�, fazendo com que o tamanho do n� exceda a ordem e fazendo com que ele seja dividido ao meio
no *inserirNoNoAposDividir (no *raiz, no *noAntigo, int indiceEsquerdo, int chave, no *direito) {
	int i, j, dividir, k_inicial;
	no *novoNo, *filho;
	int *chavesTemp;
	no **ponteirosTemp;

	// Primeiro criamos um grupo de chaves e ponteiros tempor�rios para guardar tudo em ordem, incluindo a nova chave e ponteiro, inseridos em seus locais corretos.
	ponteirosTemp = (no**) malloc( (ordem + 1) * sizeof(no *) );
	if (ponteirosTemp == NULL)
		printf ("\nErro na aloca��o do vetor ponteirosTemp!\n");

	chavesTemp = (int*) malloc( ordem * sizeof(int) );
	if (chavesTemp == NULL)
		printf ("\nErro na aloca��o do vetor chavesTemp!\n");

	for (i = 0, j = 0; i < noAntigo->numChaves + 1; i++, j++) {
		if (j == indiceEsquerdo + 1) j++;
		ponteirosTemp[j] = (no*) noAntigo->ponteiros[i];
	}

	for (i = 0, j = 0; i < noAntigo->numChaves; i++, j++) {
		if (j == indiceEsquerdo) j++;
		chavesTemp[j] = noAntigo->chaves[i];
	}

	ponteirosTemp[indiceEsquerdo + 1] = direito;
	chavesTemp[indiceEsquerdo] = chave;

	// Em seguida, criamos um novo n� e copiamos metade das chaves e ponteiros para o n� antigo e a outra metade para o novo
	dividir = dividir(ordem);
	novoNo = criarNo();
	noAntigo->numChaves = 0;
	for (i = 0; i < dividir - 1; i++) {
		noAntigo->ponteiros[i] = ponteirosTemp[i];
		noAntigo->chaves[i] = chavesTemp[i];
		noAntigo->numChaves++;
	}
	noAntigo->ponteiros[i] = ponteirosTemp[i];
	k_inicial = chavesTemp[dividir - 1];
	for (++i, j = 0; i < ordem; i++, j++) {
		novoNo->ponteiros[j] = ponteirosTemp[i];
		novoNo->chaves[j] = chavesTemp[i];
		novoNo->numChaves++;
	}
	novoNo->ponteiros[j] = ponteirosTemp[i];
	free(ponteirosTemp);
	free(chavesTemp);
	novoNo->pai = noAntigo->pai;
	for (i = 0; i <= novoNo->numChaves; i++) {
		filho = (no*) novoNo->ponteiros[i];
		filho->pai = novoNo;
	}

	// Inserimos uma nova chave no n� pai dos dois n�s resultantes da divis�o, com o n� antigo � esquerda e o novo � direita
	return inserirNoPai (raiz, noAntigo, k_inicial, novoNo);
}

// Insere um novo n� (folha ou interno) na �rvore B+
// retorna a raiz da �rvore ap�s a inser��o
no *inserirNoPai(no *raiz, no *esquerdo, int chave, no *direito) {
	int indiceEsquerdo;
	no *pai;
	pai = esquerdo->pai;

	// quando a raiz � nova
	if (pai == NULL)
		return inserirNaRaizNova (esquerdo, chave, direito);

	// quando temos um n� folha ou um n� interno
	// encontramos o ponteiro do n� pai do n� esquerdo
	indiceEsquerdo = obterIndiceEsquerdo (pai, esquerdo);

	// quando a nova chave cabe no n�
	if (pai->numChaves < ordem - 1)
		return inserirNoNo (raiz, pai, indiceEsquerdo, chave, direito);

	// quando � preciso dividir um n� para preservar as propriedades da �rvore B+
	return inserirNoNoAposDividir (raiz, pai, indiceEsquerdo, chave, direito); 
}

// Cria uma nova raiz para duas sub�rvores e insere a chave apropriada na nova raiz
no *inserirNaRaizNova (no *esquerdo, int chave, no *direito) {
	no *raiz = criarNo();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = esquerdo;
	raiz->ponteiros[1] = direito;
	raiz->numChaves++;
	raiz->pai = NULL;
	esquerdo->pai = raiz;
	direito->pai = raiz;
	return raiz;
}

// Primeira inser��o: come�a uma nova �rvore
no *comecarNovaArvore (int chave, registro *ponteiro) {
	no * raiz = criarFolha();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = ponteiro;
	raiz->ponteiros[ordem - 1] = NULL;
	raiz->pai = NULL;
	raiz->numChaves++;
	return raiz;
}

// Insere uma chave e seu valor associado na �rvore, fazendo com que esta se ajuste conforme necess�rio para manter suas propriedades
// Ignora valores duplicados
no *inserir (no *raiz, int chave, int valor) {
	registro *ponteiro;
	no *folha;

	if (buscar (raiz, chave) != NULL)
		return raiz;

	// Criando um novo registro para o valor
	ponteiro = criarRegistro(valor);

	// quando a �rvore ainda n�o existe, temos que cri�-la
	if (raiz == NULL) 
		return comecarNovaArvore (chave, ponteiro);

	// quando a �rvore j� existe
	folha = buscarFolha (raiz, chave);

	// quando o n� folha tem espa�o para a chave e o ponteiro
	if (folha->numChaves < ordem - 1) {
		folha = inserirNaFolha (folha, chave, ponteiro);
		return raiz;
	}

	// quando � preciso dividir o n� folha
	return inserirNaFolhaAposDividir (raiz, folha, chave, ponteiro);
}

// DELE��O

// Fun��o auxiliar para a dele��o. Busca o �ndice do vizinho mais pr�ximo � esquerda de um n� (se existir); se n�o (o n� � o filho mais � esquerda), retorna -1
int obterIndiceVizinho (no *n) {
	int i;

	// retorna o �ndice da chave � esquerda do ponteiro do n� pai apontando para n
	// se for o filho mais � esquerda, retorna -1
	for (i = 0; i <= n->pai->numChaves; i++)
		if (n->pai->ponteiros[i] == n)
			return i - 1;

	// se n�o encontrar 
	printf("Busca por ponteiro n�o existente para o n� no n� pai!\n");
	printf("N�:  %#lx\n", (unsigned long*)n);
	exit(EXIT_FAILURE);
}

// Remove a chave e o ponteiro de um n�
no *removerEntradaDoNo (no *n, int chave, no *ponteiro) {
	int i, numPonteiros;

	// Remove a chave e muda as outras corretamente
	i = 0;
	while (n->chaves[i] != chave)
		i++;
	for (++i; i < n->numChaves; i++)
		n->chaves[i - 1] = n->chaves[i];

	// Remove o ponteiro e muda os outros corretamente
	// Primeiro, deteminamos o n�mero de ponteiros
	numPonteiros = n->ehFolha ? n->numChaves : n->numChaves + 1;
	i = 0;
	while (n->ponteiros[i] != ponteiro)
		i++;
	for (++i; i < numPonteiros; i++)
		n->ponteiros[i - 1] = n->ponteiros[i];

	// Uma chave a menos
	n->numChaves--;

	// Coloca os outros ponteiros em NULL
	// Os n�s folha usam o �ltimo ponteiro para apontar para o pr�ximo n� folha
	if (n->ehFolha)
		for (i = n->numChaves; i < ordem - 1; i++)
			n->ponteiros[i] = NULL;
	else
		for (i = n->numChaves + 1; i < ordem; i++)
			n->ponteiros[i] = NULL;

	return n;
}

// Ajusta a raiz da �rvore
no *ajustarRaiz (no *raiz) {
	no *novaRaiz;

	// quando a raiz n�o � vazia, a chave e o ponteiro j� foram deletados, ent�o nada precisa ser feito
	if (raiz->numChaves > 0)
		return raiz;

	// quando a raiz � vazia
	// se ela tem um n� filho, promover o �nico filho a nova raiz
	if (!raiz->ehFolha) {
		novaRaiz =(no*) raiz->ponteiros[0];
		novaRaiz->pai = NULL;
	}

	// se a raiz � folha (n�o tem filhos), ent�o a �rvore fica vazia
	else
		novaRaiz = NULL;

	free(raiz->chaves);
	free(raiz->ponteiros);
	free(raiz);

	return novaRaiz;
}

// Junta um n� que ficou muito pequeno ap�s a dele��o com um n� vizinho que pode aceitar entradas adicionais sem exceder o m�ximo
no *juntarNos (no *raiz, no *n, no *vizinho, int indiceVizinho, int k_inicial) {
	int i, j, indiceDeInsercaoVizinho, n_final;
	no *tmp;

	// Trocar vizinho com o n� se o n� estiver no extremo esquerdo e o vizinho estiver � sua direita
	if (indiceVizinho == -1) {
		tmp = n;
		n = vizinho;
		vizinho = tmp;
	}

	// O ponto inicial no vizinho para copiar chaves e ponteiros de n
	// n e vizinho trocaram de lugar no caso especial de n ser o filho mais � esquerda
	indiceDeInsercaoVizinho = vizinho->numChaves;

	// quando o n� n�o � folha
	if (!n->ehFolha) {
		vizinho->chaves[indiceDeInsercaoVizinho] = k_inicial; // acresentar k_inicial
		vizinho->numChaves++;
		n_final = n->numChaves;

		// acrescentar todos os ponteiros e chaves do vizinho
		for (i = indiceDeInsercaoVizinho + 1, j = 0; j < n_final; i++, j++) {
			vizinho->chaves[i] = n->chaves[j];
			vizinho->ponteiros[i] = n->ponteiros[j];
			vizinho->numChaves++;
			n->numChaves--;
		}

		// o n�mero de ponteiros � sempre um a mais do que o n�mero de chaves
		vizinho->ponteiros[i] = n->ponteiros[j];

		// todos os filhos devem apontar para o mesmo pai
		for (i = 0; i < vizinho->numChaves + 1; i++) {
			tmp = (no*)vizinho->ponteiros[i];
			tmp->pai = vizinho;
		}
	}

	// em um n� folha, acrescentar as chaves e ponteiros de n ao vizinho
	// fazer o �ltimo ponteiro do vizinho apontar para o que era o vizinho direito de n
	else {
		for (i = indiceDeInsercaoVizinho, j = 0; j < n->numChaves; i++, j++) {
			vizinho->chaves[i] = n->chaves[j];
			vizinho->ponteiros[i] = n->ponteiros[j];
			vizinho->numChaves++;
		}
		vizinho->ponteiros[ordem - 1] = n->ponteiros[ordem - 1];
	}

	raiz = deletarEntrada (raiz, n->pai, k_inicial, n);
	free(n->chaves);
	free(n->ponteiros);
	free(n); 
	return raiz;
}

// Redistribui entradas entre dois n�s quando um ficou muito pequeno ap�s a dele��o 
// mas seu vizinho � muito grande para acrescentar as entradas do n� pequeno sem exceder o m�ximo 
no *redistribuirNos (no *raiz, no *n, no *vizinho, int indiceVizinho, int k_primeiro_indice, int k_inicial) {  
	int i;
	no *tmp;

	// quando n tem um vizinho � esquerda
	// pegar o �ltimo par chave-ponteiro do vizinho do fim direito do vizinho para o fim esquerdo de n
	if (indiceVizinho != -1) {
		if (!n->ehFolha)
			n->ponteiros[n->numChaves + 1] = n->ponteiros[n->numChaves];
		for (i = n->numChaves; i > 0; i--) {
			n->chaves[i] = n->chaves[i - 1];
			n->ponteiros[i] = n->ponteiros[i - 1];
		}
		if (!n->ehFolha) {
			n->ponteiros[0] = vizinho->ponteiros[vizinho->numChaves];
			tmp = (no*)n->ponteiros[0];
			tmp->pai = n;
			vizinho->ponteiros[vizinho->numChaves] = NULL;
			n->chaves[0] = k_inicial;
			n->pai->chaves[k_primeiro_indice] = vizinho->chaves[vizinho->numChaves - 1];
		}
		else {
			n->ponteiros[0] = vizinho->ponteiros[vizinho->numChaves - 1];
			vizinho->ponteiros[vizinho->numChaves - 1] = NULL;
			n->chaves[0] = vizinho->chaves[vizinho->numChaves - 1];
			n->pai->chaves[k_primeiro_indice] = n->chaves[0];
		}
	}

	// quando n � o filho mais � esquerda
	// pegar um par chave-ponteiro do vizinho � direita e mover o par chave-ponteiro mais � esquerda do vizinho para a posi��o mais � direita de n
	else {  
		if (n->ehFolha) {
			n->chaves[n->numChaves] = vizinho->chaves[0];
			n->ponteiros[n->numChaves] = vizinho->ponteiros[0];
			n->pai->chaves[k_primeiro_indice] = vizinho->chaves[1];
		}
		else {
			n->chaves[n->numChaves] = k_inicial;
			n->ponteiros[n->numChaves + 1] = vizinho->ponteiros[0];
			tmp = (no *)n->ponteiros[n->numChaves + 1];
			tmp->pai = n;
			n->pai->chaves[k_primeiro_indice] = vizinho->chaves[0];
		}
		for (i = 0; i < vizinho->numChaves - 1; i++) {
			vizinho->chaves[i] = vizinho->chaves[i + 1];
			vizinho->ponteiros[i] = vizinho->ponteiros[i + 1];
		}
		if (!n->ehFolha)
			vizinho->ponteiros[i] = vizinho->ponteiros[i + 1];
	}

	// n agora tem uma chave a mais e um ponteiro a mais, e o vizinho tem um a menos de cada
	n->numChaves++;
	vizinho->numChaves--;

	return raiz;
}

// Deleta uma entrada da �rvore
// remove o registro e sua chave e ponteiro do n� folha, e depois faz todas as mudan�as necess�rias para preservar as propriedades da �rvore
no *deletarEntrada (no *raiz, no *n, int chave, void *ponteiro) {
	int chaves_min;
	no *vizinho;
	int indiceVizinho;
	int k_primeiro_indice, k_inicial;
	int capacidade;

	// Remove chave e ponteiro do n�
	n = removerEntradaDoNo (n, chave,(no*) ponteiro);

	// quando a dele��o � feita na raiz
	if (n == raiz) 
		return ajustarRaiz (raiz);

	// quando a dele��o � feita de um n� abaixo da raiz
	// determinar o tamanho m�nimo permitido de n�, para ser preservado ap�s a dele��o
	chaves_min = n->ehFolha ? dividir(ordem - 1) : dividir(ordem) - 1;

	// quanoo o n� fica no m�nimo ou abaixo dele 
	if (n->numChaves >= chaves_min)
		return raiz;

	// quando o n� fica abaixo do m�nimo, � preciso fazer a jun��o ou redistribui��o
	// encontramos o vizinho apropriado para fazer a uni�o
	// tamb�m buscamos a chave (k_inicial) no n� pai entre o ponteiro para o n� n e o ponteiro para o vizinho
	indiceVizinho = obterIndiceVizinho( n );
	k_primeiro_indice = indiceVizinho == -1 ? 0 : indiceVizinho;
	k_inicial = n->pai->chaves[k_primeiro_indice];
	vizinho = (no*) indiceVizinho == -1 ? (no*) n->pai->ponteiros[1] : 
		n->pai->ponteiros[indiceVizinho];

	capacidade = n->ehFolha ? ordem : ordem - 1;

	// fazendo a jun��o de n�s
	if (vizinho->numChaves + n->numChaves < capacidade)
		return juntarNos (raiz, n, vizinho, indiceVizinho, k_inicial);

	// fazendo a redistribui��o
	else
		return redistribuirNos (raiz, n, vizinho, indiceVizinho, k_primeiro_indice, k_inicial);
}

// Fun��o principal de dele��o
no *deletar (no *raiz, int chave) {
	no *folhaChave;
	registro *registroChave;

	registroChave = buscar (raiz, chave);
	folhaChave = buscarFolha(raiz, chave);
	if (registroChave != NULL && folhaChave != NULL) {
		raiz = deletarEntrada(raiz, folhaChave, chave, registroChave);
		free(registroChave);
	}
	return raiz;
}

// Destr�i os n�s da �rvore
void destruirNosArvore (no *raiz) {
	int i;
	if (raiz->ehFolha)
		for (i = 0; i < raiz->numChaves; i++)
			free(raiz->ponteiros[i]);
	else
		for (i = 0; i < raiz->numChaves + 1; i++)
			destruirNosArvore((no*)raiz->ponteiros[i]);
	free(raiz->ponteiros);
	free(raiz->chaves);
	free(raiz);
}

// Destruir �rvore
no *destruirArvore (no *raiz) {
	destruirNosArvore(raiz);
	return NULL;
}

/*

FIM DO C�DIDO COPIADO

*/

typedef struct aluno{
	int linha, RA, idade;
	char nome[100];
	double media;
}Aluno;

typedef struct no{
	
	/*
	int linhas[tamanho];//vetor com as linhas de cada aluno no arquivo
	struct no *filhos[tamanho+1];//vetor de ponteiros para os n�s filhos
	int ocupados;//contador de posi��es ocupadas no vetor aluno
	int folha;//vari�vel l�gica para descobrir se um n� � folha ou n�o
	struct no *pai;*/
	
	void ** ponteiros;
	int * chaves;
	struct no * pai;
	bool ehFolha;
	int numChaves;
	struct no * proximo; // Used for fila.
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
	/*Arvore *aux = *alunos;
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
	return aux;//se chegou at� aqui encontrou uma folha*/
	Arvore *aux = *alunos;
	if(!aux)
		return NULL;
	int i;
	while(!aux->ehFolha){// procurando um n� ate que ele sea uma folha
		i = 0;//se n�o � o contador reseta
		while(i < aux->numChaves){//para um n�, procurar o filho para "descer" de n�vel
			if(endereco >= aux->chaves[i])
				i++;
			else
				break;
		}
		aux = aux->ponteiros[i];//"descendo de n�vel"
	}
	return aux;//se chegou at� aqui encontrou uma folha
}

/*
 FUN��O AUXILIAR (usada para inserir um valor em um n� folha)

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
*/
/*
 FUN��O AUXILIAR (usada para inserir um valor internamente na �rvore)
 Esta fun��o trata os demoais casos de inser��o

void inserirInternamente(Arvore **alunos, Arvore **folha, int endereco){
	Arvore *novaFolha = malloc(sizeof(Arvore));
	Arvore **filhosTemp = malloc(sizeof(Arvore*) * tamanho);
	
	int *linhasTemp = malloc(sizeof(int) * tamanho);
	int indiceInsercao = 0, divisao, novalinha, i, j;
	
	for(i = 0; i <= tamanho; i++)
		(*alunos)->filhos[i] = 0;
	(*alunos)->ocupados = 0;
	
	while(indiceInsercao < tamanho - 1 && (*folha)->linhas[indiceInsercao] < endereco)
		indiceInsercao++;
	for(i = 0, j = 0; i < (*folha)->ocupados; i++, j++){
		if(j == indiceInsercao)
			j++;
		linhasTemp[j] = (*folha)->linhas[i];
		filhosTemp[j] = (*folha)->filhos[i];
	}
	linhasTemp[indiceInsercao] = endereco;
	
	(*folha)->ocupados = 0;
	divisao = (tamanho-1) % 2 == 0 ? (tamanho-1)/2 : (tamanho-1)/2 + 1; 
	for(i = 0; i < divisao; i++){
		
	} 
}
*/
/*
 FUN��O AUXILIAR (usada para inserir efetivamente os registros na �rvore)
*/
void inserirArvore(Arvore **alunos, int endereco){
	/*if(*alunos == NULL){//�rvore vazia, devemos cri�-la
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
	//Se nenhum dos casos f�ceis foi atingido, devemos inserir internamente
	inserirInternamente(alunos, &folha, endereco);*/
	
	//REMOVER DEPOIS******************************************
	*alunos = inserir(*alunos, endereco, endereco);
	
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
	//OBTER CADASTRO ALUNO DEVE RETORNAR UM ALUNO AO INV�S DE UMA TUPLA
}

void atualizarAluno(Arvore **arvoreAlunos, FILE *arq){
	
}

void excluirAluno(Arvore **arvoreAlunos, FILE *arq){
	int numero;
	printf("Digite o RA do aluno a ser excluido:");
	scanf("%d", &numero);
	*arvoreAlunos = deletar(*arvoreAlunos, numero);
}

void imprimirLista(Arvore **arvoreAlunos){
	imprimirFolhas(*arvoreAlunos);
}

void imprimirArvore(Arvore **arvoreAlunos){
	imprimirArvore(*arvoreAlunos);
}

void consultarPorRA(Arvore **arvoreAlunos){
	int numero;
	printf("Digite o RA a ser encontrado:");
	scanf("%d", &numero);
	registro *aux = buscar(*arvoreAlunos, numero, falso);
	aux == NULL ? printf("Encontrado") : printf("N�o Encontrado");
}

void destruirArvore(Arvore **arvoreAlunos){
	destruirArvore(*arvoreAlunos);
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
	}while(opcao != 1);
	destruirArvore(&arvoreAlunos);
	fclose(arquivoDados);
	printf("Estruturas finalizadas!\n");
	return EXIT_SUCCESS;
}


