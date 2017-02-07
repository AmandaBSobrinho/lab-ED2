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

/*

CÓDIGO COPIADO

*/

#define bool char
#define falso 0
#define verdadeiro 1
#define tamanhoPadrao 4
#define tamanhoMinimo 3 // o tamanhoMinimo é necessariamente 3
#define tamanhoMaximo 20 // esse tamanhoMaximo pode mudar

// Tipos

// registro em relação ao qual uma chave se refere
typedef struct registro { 
	int valor; // o registro pode conter dados ou um arquivo
} registro;

// tipo que representa um nó na árvore B+; serve tanto para nós folha como para nós internos
typedef struct no {
	void **ponteiros; // arranjo de ponteiros correspondentes às chaves
	int *chaves; // arranjo de chaves
	struct no *pai;
	bool ehFolha; // para dizer se um nó é folha ou não
	int numChaves; // conta o número de chaves válidas; em um nó interno, o número de ponteiros válidos é numChaves+1; no nó folha, esse número é numChaves 
	struct no *proximo; // aponta para o próximo nó folha
} no;
// a relação entre chaves e ponteiros é diferente para nós folha e nós internos.
// No nó folha, o índice de cada chave é igual ao índice do seu ponteiro correspondente, com um número máximo de pares chave-ponteiro igual a ordem-1,
// e o último ponteiro aponta para o nó folha à direita.
// Em um nó interno, o primeiro ponteiro refere-se aos nós mais baixos com chaves menores do que a menor chave no arranjo de chaves.
// Então, com índices i começando em 0, o ponteiro em i+1 aponta para a subárvore com chaves maiores ou iguais à chave neste nó com índice i.

// Variáveis Globais

int ordem = tamanhoPadrao; // a ordem determina os números máximo e mínimo de entradas (chaves e ponteiros) em um nó
// cada nó tem no máximo ordem-1 chaves e no mínimo metade desse número
// cada nó folha tem tantos ponteiros para dados quanto chaves, e cada nó interno tem um ponteiro a mais para uma subárvore do que o número de chaves

no *fila = NULL; // usada para imprimir a árvore em ordem de níveis, começando da raiz e indo até as folhas

// Funções da Árvore B+

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

// Função que auxilia na impressão da árvore B+; é usada na função imprimirArvore
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

// Função que auxilia na impressão da árvore B+; também é usada na função imprimirArvore
no *desenfileirar (void) {
	no *n = fila;
	fila = fila->proximo;
	n->proximo = NULL;
	return n;
}

// Imprime a fileira mais inferior de chaves da árvore
void imprimirFolhas (no *raiz) {
	int i;
	no *c = raiz;
	if (raiz == NULL) {
		printf("Árvore vazia!\n");
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

// Informa a altura da árvore
int altura (no *raiz) {
	int h = 0;
	no *c = raiz;
	while (!c->ehFolha) {
		c = (no*) c->ponteiros[0];
		h++;
	}
	return h;
}

// Informa o número de arestas no caminho de qualquer nó até a raiz; é usada na função imprimirArvore
int distanciaRaizNo (no *raiz, no *filho) {
	int distancia = 0;
	no *c = filho;
	while (c != raiz) {
		c = c->pai;
		distancia++;
	}
	return distancia;
}

// Imprime a árvore B+ em ordem de níveis, com as chaves em cada nó e o símbolo '|' para separar os nós
void imprimirArvore (no *raiz) {
	no *n = NULL;
	int i = 0;
	int caminho = 0;
	int novoCaminho = 0;

	if (raiz == NULL) {
		printf("Árvore vazia!\n");
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
		printf("Registro não encontrado com a chave %d.\n", chave);
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
			printf("Chave: %d   Localização: %lx  Valor: %d\n", chavesEncontradas[i], (unsigned long*)ponteirosEncontrados[i], ((registro *)ponteirosEncontrados[i])->valor);
	}
}

// Encontra chaves e seus ponteiros, se presentes, no range especificado por chaveInicial e chaveFinal, e os coloca nos arranjos chavesEncontradas e ponteirosEncontrados
// é usada na função buscarImprimirRange
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
	return numeroEncontrado; // retorna o número de entradas encontradas
}

// Faz o caminho da raiz até uma folha, procurando por chave
// é usada nas funções inserir, deletar, buscar e buscarRange
no *buscarFolha (no *raiz, int chave) {
	int i = 0;
	no *c = raiz;
	if (c == NULL) {
		printf("Árvore vazia!\n");
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
// é usada nas funções inserir, deletar e buscarImprimir
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

// Encontra o local apropriado para dividir um nó muito grande ao meio
int dividir (int distancia) {
	if (distancia % 2 == 0)
		return distancia/2;
	else
		return distancia/2 + 1;
}

// INSERÇÃO

// Cria um novo registro para guardar o valor ao qual uma chave se refere
registro *criarRegistro (int valor) {
	registro *novoRegistro = (registro*)malloc(sizeof(registro));
	if (novoRegistro == NULL)
		printf ("\nErro na criação do novo registro!\n");
	else
		novoRegistro->valor = valor;
	return novoRegistro;
}

// Cria um novo nó geral, que pode servir tanto como nó folha ou interno
no *criarNo (void) {
	no *novoNo;
	novoNo = (no*) malloc(sizeof(no));
	if (novoNo == NULL)
		printf ("\nErro na criação do nó!\n");
	novoNo->chaves = (int*) malloc( (ordem - 1) * sizeof(int) );
	if (novoNo->chaves == NULL)
		printf ("\nErro na alocação das chaves no nó!\n");
	novoNo->ponteiros = (void**) malloc( ordem * sizeof(void *) );
	if (novoNo->ponteiros == NULL)
		printf ("\nErro na alocação dos ponteiros no nó!\n");
	novoNo->ehFolha = falso;
	novoNo->numChaves = 0;
	novoNo->pai = NULL;
	novoNo->proximo = NULL;
	return novoNo;
}

// Cria uma nova folha criando um nó e adaptando-o apropriadamente
no *criarFolha (void) {
	no *folha = criarNo();
	folha->ehFolha = verdadeiro;
	return folha;
}

// Função auxiliar usada na função inserirNoPai para buscar o índice do ponteiro do nó pai para o nó à esquerda da chave a ser inserida
int obterIndiceEsquerdo (no *pai, no *esquerdo) {
	int indiceEsquerdo = 0;
	while (indiceEsquerdo <= pai->numChaves && pai->ponteiros[indiceEsquerdo] != esquerdo)
		indiceEsquerdo++;
	return indiceEsquerdo;
}

// Insere um novo ponteiro em um registro e sua chave correspondente em um nó folha
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
	return folha; // retorna o nó folha alterado
}

// Insere uma nova chave e ponteiro em um novo registro em um nó folha para exceder a ordem da árvore, levando a folha a ser dividida ao meio
no *inserirNaFolhaAposDividir (no *raiz, no *folha, int chave, registro *ponteiro) {
	no *novaFolha;
	int *chavesTemp;
	void **ponteirosTemp;
	int indiceInsercao, dividir, novaChave, i, j;

	novaFolha = criarFolha();

	chavesTemp = (int*) malloc( ordem * sizeof(int) );
	if (chavesTemp == NULL)
		printf ("\nErro na alocação do vetor chavesTemp!\n");

	ponteirosTemp =(void**) malloc( ordem * sizeof(void *) );
	if (ponteirosTemp == NULL)
		printf ("\nErro na alocação do vetor ponteirosTemp!\n");

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

// Insere uma nova chave e ponteiro em um nó em que eles podem caber sem violar as propriedades da árvore
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

// Insere uma nova chave e ponteiro em um nó, fazendo com que o tamanho do nó exceda a ordem e fazendo com que ele seja dividido ao meio
no *inserirNoNoAposDividir (no *raiz, no *noAntigo, int indiceEsquerdo, int chave, no *direito) {
	int i, j, dividir, k_inicial;
	no *novoNo, *filho;
	int *chavesTemp;
	no **ponteirosTemp;

	// Primeiro criamos um grupo de chaves e ponteiros temporários para guardar tudo em ordem, incluindo a nova chave e ponteiro, inseridos em seus locais corretos.
	ponteirosTemp = (no**) malloc( (ordem + 1) * sizeof(no *) );
	if (ponteirosTemp == NULL)
		printf ("\nErro na alocação do vetor ponteirosTemp!\n");

	chavesTemp = (int*) malloc( ordem * sizeof(int) );
	if (chavesTemp == NULL)
		printf ("\nErro na alocação do vetor chavesTemp!\n");

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

	// Em seguida, criamos um novo nó e copiamos metade das chaves e ponteiros para o nó antigo e a outra metade para o novo
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

	// Inserimos uma nova chave no nó pai dos dois nós resultantes da divisão, com o nó antigo à esquerda e o novo à direita
	return inserirNoPai (raiz, noAntigo, k_inicial, novoNo);
}

// Insere um novo nó (folha ou interno) na árvore B+
// retorna a raiz da árvore após a inserção
no *inserirNoPai(no *raiz, no *esquerdo, int chave, no *direito) {
	int indiceEsquerdo;
	no *pai;
	pai = esquerdo->pai;

	// quando a raiz é nova
	if (pai == NULL)
		return inserirNaRaizNova (esquerdo, chave, direito);

	// quando temos um nó folha ou um nó interno
	// encontramos o ponteiro do nó pai do nó esquerdo
	indiceEsquerdo = obterIndiceEsquerdo (pai, esquerdo);

	// quando a nova chave cabe no nó
	if (pai->numChaves < ordem - 1)
		return inserirNoNo (raiz, pai, indiceEsquerdo, chave, direito);

	// quando é preciso dividir um nó para preservar as propriedades da árvore B+
	return inserirNoNoAposDividir (raiz, pai, indiceEsquerdo, chave, direito); 
}

// Cria uma nova raiz para duas subárvores e insere a chave apropriada na nova raiz
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

// Primeira inserção: começa uma nova árvore
no *comecarNovaArvore (int chave, registro *ponteiro) {
	no * raiz = criarFolha();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = ponteiro;
	raiz->ponteiros[ordem - 1] = NULL;
	raiz->pai = NULL;
	raiz->numChaves++;
	return raiz;
}

// Insere uma chave e seu valor associado na árvore, fazendo com que esta se ajuste conforme necessário para manter suas propriedades
// Ignora valores duplicados
no *inserir (no *raiz, int chave, int valor) {
	registro *ponteiro;
	no *folha;

	if (buscar (raiz, chave) != NULL)
		return raiz;

	// Criando um novo registro para o valor
	ponteiro = criarRegistro(valor);

	// quando a árvore ainda não existe, temos que criá-la
	if (raiz == NULL) 
		return comecarNovaArvore (chave, ponteiro);

	// quando a árvore já existe
	folha = buscarFolha (raiz, chave);

	// quando o nó folha tem espaço para a chave e o ponteiro
	if (folha->numChaves < ordem - 1) {
		folha = inserirNaFolha (folha, chave, ponteiro);
		return raiz;
	}

	// quando é preciso dividir o nó folha
	return inserirNaFolhaAposDividir (raiz, folha, chave, ponteiro);
}

// DELEÇÃO

// Função auxiliar para a deleção. Busca o índice do vizinho mais próximo à esquerda de um nó (se existir); se não (o nó é o filho mais à esquerda), retorna -1
int obterIndiceVizinho (no *n) {
	int i;

	// retorna o índice da chave à esquerda do ponteiro do nó pai apontando para n
	// se for o filho mais à esquerda, retorna -1
	for (i = 0; i <= n->pai->numChaves; i++)
		if (n->pai->ponteiros[i] == n)
			return i - 1;

	// se não encontrar 
	printf("Busca por ponteiro não existente para o nó no nó pai!\n");
	printf("Nó:  %#lx\n", (unsigned long*)n);
	exit(EXIT_FAILURE);
}

// Remove a chave e o ponteiro de um nó
no *removerEntradaDoNo (no *n, int chave, no *ponteiro) {
	int i, numPonteiros;

	// Remove a chave e muda as outras corretamente
	i = 0;
	while (n->chaves[i] != chave)
		i++;
	for (++i; i < n->numChaves; i++)
		n->chaves[i - 1] = n->chaves[i];

	// Remove o ponteiro e muda os outros corretamente
	// Primeiro, deteminamos o número de ponteiros
	numPonteiros = n->ehFolha ? n->numChaves : n->numChaves + 1;
	i = 0;
	while (n->ponteiros[i] != ponteiro)
		i++;
	for (++i; i < numPonteiros; i++)
		n->ponteiros[i - 1] = n->ponteiros[i];

	// Uma chave a menos
	n->numChaves--;

	// Coloca os outros ponteiros em NULL
	// Os nós folha usam o último ponteiro para apontar para o próximo nó folha
	if (n->ehFolha)
		for (i = n->numChaves; i < ordem - 1; i++)
			n->ponteiros[i] = NULL;
	else
		for (i = n->numChaves + 1; i < ordem; i++)
			n->ponteiros[i] = NULL;

	return n;
}

// Ajusta a raiz da árvore
no *ajustarRaiz (no *raiz) {
	no *novaRaiz;

	// quando a raiz não é vazia, a chave e o ponteiro já foram deletados, então nada precisa ser feito
	if (raiz->numChaves > 0)
		return raiz;

	// quando a raiz é vazia
	// se ela tem um nó filho, promover o único filho a nova raiz
	if (!raiz->ehFolha) {
		novaRaiz =(no*) raiz->ponteiros[0];
		novaRaiz->pai = NULL;
	}

	// se a raiz é folha (não tem filhos), então a árvore fica vazia
	else
		novaRaiz = NULL;

	free(raiz->chaves);
	free(raiz->ponteiros);
	free(raiz);

	return novaRaiz;
}

// Junta um nó que ficou muito pequeno após a deleção com um nó vizinho que pode aceitar entradas adicionais sem exceder o máximo
no *juntarNos (no *raiz, no *n, no *vizinho, int indiceVizinho, int k_inicial) {
	int i, j, indiceDeInsercaoVizinho, n_final;
	no *tmp;

	// Trocar vizinho com o nó se o nó estiver no extremo esquerdo e o vizinho estiver à sua direita
	if (indiceVizinho == -1) {
		tmp = n;
		n = vizinho;
		vizinho = tmp;
	}

	// O ponto inicial no vizinho para copiar chaves e ponteiros de n
	// n e vizinho trocaram de lugar no caso especial de n ser o filho mais à esquerda
	indiceDeInsercaoVizinho = vizinho->numChaves;

	// quando o nó não é folha
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

		// o número de ponteiros é sempre um a mais do que o número de chaves
		vizinho->ponteiros[i] = n->ponteiros[j];

		// todos os filhos devem apontar para o mesmo pai
		for (i = 0; i < vizinho->numChaves + 1; i++) {
			tmp = (no*)vizinho->ponteiros[i];
			tmp->pai = vizinho;
		}
	}

	// em um nó folha, acrescentar as chaves e ponteiros de n ao vizinho
	// fazer o último ponteiro do vizinho apontar para o que era o vizinho direito de n
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

// Redistribui entradas entre dois nós quando um ficou muito pequeno após a deleção 
// mas seu vizinho é muito grande para acrescentar as entradas do nó pequeno sem exceder o máximo 
no *redistribuirNos (no *raiz, no *n, no *vizinho, int indiceVizinho, int k_primeiro_indice, int k_inicial) {  
	int i;
	no *tmp;

	// quando n tem um vizinho à esquerda
	// pegar o último par chave-ponteiro do vizinho do fim direito do vizinho para o fim esquerdo de n
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

	// quando n é o filho mais à esquerda
	// pegar um par chave-ponteiro do vizinho à direita e mover o par chave-ponteiro mais à esquerda do vizinho para a posição mais à direita de n
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

// Deleta uma entrada da árvore
// remove o registro e sua chave e ponteiro do nó folha, e depois faz todas as mudanças necessárias para preservar as propriedades da árvore
no *deletarEntrada (no *raiz, no *n, int chave, void *ponteiro) {
	int chaves_min;
	no *vizinho;
	int indiceVizinho;
	int k_primeiro_indice, k_inicial;
	int capacidade;

	// Remove chave e ponteiro do nó
	n = removerEntradaDoNo (n, chave,(no*) ponteiro);

	// quando a deleção é feita na raiz
	if (n == raiz) 
		return ajustarRaiz (raiz);

	// quando a deleção é feita de um nó abaixo da raiz
	// determinar o tamanho mínimo permitido de nó, para ser preservado após a deleção
	chaves_min = n->ehFolha ? dividir(ordem - 1) : dividir(ordem) - 1;

	// quanoo o nó fica no mínimo ou abaixo dele 
	if (n->numChaves >= chaves_min)
		return raiz;

	// quando o nó fica abaixo do mínimo, é preciso fazer a junção ou redistribuição
	// encontramos o vizinho apropriado para fazer a união
	// também buscamos a chave (k_inicial) no nó pai entre o ponteiro para o nó n e o ponteiro para o vizinho
	indiceVizinho = obterIndiceVizinho( n );
	k_primeiro_indice = indiceVizinho == -1 ? 0 : indiceVizinho;
	k_inicial = n->pai->chaves[k_primeiro_indice];
	vizinho = (no*) indiceVizinho == -1 ? (no*) n->pai->ponteiros[1] : 
		n->pai->ponteiros[indiceVizinho];

	capacidade = n->ehFolha ? ordem : ordem - 1;

	// fazendo a junção de nós
	if (vizinho->numChaves + n->numChaves < capacidade)
		return juntarNos (raiz, n, vizinho, indiceVizinho, k_inicial);

	// fazendo a redistribuição
	else
		return redistribuirNos (raiz, n, vizinho, indiceVizinho, k_primeiro_indice, k_inicial);
}

// Função principal de deleção
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

// Destrói os nós da árvore
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

// Destruir árvore
no *destruirArvore (no *raiz) {
	destruirNosArvore(raiz);
	return NULL;
}

/*

FIM DO CÓDIDO COPIADO

*/

typedef struct aluno{
	int linha, RA, idade;
	char nome[100];
	double media;
}Aluno;

typedef struct no{
	
	/*
	int linhas[tamanho];//vetor com as linhas de cada aluno no arquivo
	struct no *filhos[tamanho+1];//vetor de ponteiros para os nós filhos
	int ocupados;//contador de posições ocupadas no vetor aluno
	int folha;//variável lógica para descobrir se um nó é folha ou não
	struct no *pai;*/
	
	void ** ponteiros;
	int * chaves;
	struct no * pai;
	bool ehFolha;
	int numChaves;
	struct no * proximo; // Used for fila.
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
 FUNÇÃO AUXILIAR (usada para encontrar um caminho da raiz até uma folha)
 Deve retornar um nó folha, que é o mais apropriado para a inserção
*/
Arvore *encontrarFolha(Arvore **alunos, int endereco){
	/*Arvore *aux = *alunos;
	if(!aux)
		return NULL;
	int i;
	while(!aux->folha){// procurando um nó ate que ele sea uma folha
		i = 0;//se não é o contador reseta
		while(i < aux->ocupados){//para um nó, procurar o filho para "descer" de nível
			if(endereco >= aux->linhas[i])
				i++;
			else
				break;
		}
		aux = aux->filhos[i];//"descendo de nível"
	}
	return aux;//se chegou até aqui encontrou uma folha*/
	Arvore *aux = *alunos;
	if(!aux)
		return NULL;
	int i;
	while(!aux->ehFolha){// procurando um nó ate que ele sea uma folha
		i = 0;//se não é o contador reseta
		while(i < aux->numChaves){//para um nó, procurar o filho para "descer" de nível
			if(endereco >= aux->chaves[i])
				i++;
			else
				break;
		}
		aux = aux->ponteiros[i];//"descendo de nível"
	}
	return aux;//se chegou até aqui encontrou uma folha
}

/*
 FUNÇÃO AUXILIAR (usada para inserir um valor em um nó folha)

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
 FUNÇÃO AUXILIAR (usada para inserir um valor internamente na árvore)
 Esta função trata os demoais casos de inserção

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
 FUNÇÃO AUXILIAR (usada para inserir efetivamente os registros na árvore)
*/
void inserirArvore(Arvore **alunos, int endereco){
	/*if(*alunos == NULL){//árvore vazia, devemos criá-la
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
	if(folha->ocupados < tamanho-1){//temos espaço para inserir nessa folha
		folha = inserirEmFolha(alunos, endereco);
		return;
	}
	//Se nenhum dos casos fáceis foi atingido, devemos inserir internamente
	inserirInternamente(alunos, &folha, endereco);*/
	
	//REMOVER DEPOIS******************************************
	*alunos = inserir(*alunos, endereco, endereco);
	
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
	//OBTER CADASTRO ALUNO DEVE RETORNAR UM ALUNO AO INVÉS DE UMA TUPLA
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
	aux == NULL ? printf("Encontrado") : printf("Não Encontrado");
}

void destruirArvore(Arvore **arvoreAlunos){
	destruirArvore(*arvoreAlunos);
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
	}while(opcao != 1);
	destruirArvore(&arvoreAlunos);
	fclose(arquivoDados);
	printf("Estruturas finalizadas!\n");
	return EXIT_SUCCESS;
}


