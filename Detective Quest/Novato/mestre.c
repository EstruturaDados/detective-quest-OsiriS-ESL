#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101   // tamanho simples prime para tabela hash

// -----------------------------
// Estrutura da sala (árvore)
// -----------------------------
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA];   // pista associada (pode ser string vazia)
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// -----------------------------
// Nó da BST de pistas
// -----------------------------
typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// -----------------------------
// Nó para tabela hash (separate chaining)
// chave = pista, valor = suspeito
// -----------------------------
typedef struct HashNode {
    char pista[MAX_PISTA];
    char suspeito[MAX_NOME];
    struct HashNode *prox;
} HashNode;

// -----------------------------
// Protótipos
// -----------------------------
Sala* criarSala(const char *nome, const char *pista);
void explorarSalas(Sala *inicio, PistaNode **raizPistas, HashNode *tabela[]);
PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void exibirPistas(PistaNode *raiz);
void liberarPistas(PistaNode *raiz);
unsigned long hash_djb2(const char *str);
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito);
const char* encontrarSuspeito(HashNode *tabela[], const char *pista);
int verificarSuspeitoFinal(PistaNode *raizPistas, HashNode *tabela[], const char *acusado);
void liberarHash(HashNode *tabela[]);
void liberarMapa(Sala *raiz);

// -----------------------------
// criarSala()
// Cria dinamicamente uma sala com nome e pista.
// Retorna ponteiro para Sala alocada.
// -----------------------------
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: malloc criarSala\n");
        exit(1);
    }
    strncpy(s->nome, nome, MAX_NOME-1); s->nome[MAX_NOME-1] = '\0';
    if (pista && strlen(pista) > 0) {
        strncpy(s->pista, pista, MAX_PISTA-1);
        s->pista[MAX_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

// -----------------------------
// inserirPista()
// Insere uma pista na BST (ordem alfabética).
// Se pista for igual, insere à direita (permite duplicatas).
// Retorna raiz atualizada.
// -----------------------------
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || pista[0] == '\0') return raiz;
    if (raiz == NULL) {
        PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) { fprintf(stderr, "Erro: malloc inserirPista\n"); exit(1); }
        strncpy(n->pista, pista, MAX_PISTA-1); n->pista[MAX_PISTA-1] = '\0';
        n->esq = n->dir = NULL;
        return n;
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else
        raiz->dir = inserirPista(raiz->dir, pista);
    return raiz;
}

// -----------------------------
// exibirPistas()
// Percorre a BST em-order e imprime pistas (ordenadas).
// -----------------------------
void exibirPistas(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// -----------------------------
// liberarPistas()
// Libera memória da BST de pistas.
// -----------------------------
void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

// -----------------------------
// hash_djb2()
// Função hash djb2 para strings.
// -----------------------------
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

// -----------------------------
// inserirNaHash()
// Insere associação pista -> suspeito na tabela hash.
// -----------------------------
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito) {
    if (!pista || pista[0] == '\0' || !suspeito) return;
    unsigned long idx = hash_djb2(pista);
    HashNode *n = (HashNode*) malloc(sizeof(HashNode));
    if (!n) { fprintf(stderr, "Erro: malloc inserirNaHash\n"); exit(1); }
    strncpy(n->pista, pista, MAX_PISTA-1); n->pista[MAX_PISTA-1] = '\0';
    strncpy(n->suspeito, suspeito, MAX_NOME-1); n->suspeito[MAX_NOME-1] = '\0';
    n->prox = tabela[idx];
    tabela[idx] = n;
}

// -----------------------------
// encontrarSuspeito()
// Retorna ponteiro para nome do suspeito associado a uma pista.
// Se não encontrar, retorna NULL.
// -----------------------------
const char* encontrarSuspeito(HashNode *tabela[], const char *pista) {
    if (!pista || pista[0] == '\0') return NULL;
    unsigned long idx = hash_djb2(pista);
    HashNode *cur = tabela[idx];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0)
            return cur->suspeito;
        cur = cur->prox;
    }
    return NULL;
}

// -----------------------------
// explorarSalas()
// Navega pela árvore de salas a partir de 'inicio'.
// Ao entrar em sala: exibe nome, coleta pista (se houver) e
// insere a pista na BST de pistas automaticamente.
// O jogador escolhe 'e' (esq), 'd' (dir) ou 's' (sair).
// -----------------------------
void explorarSalas(Sala *inicio, PistaNode **raizPistas, HashNode *tabela[]) {
    if (!inicio) return;
    Sala *atual = inicio;
    char opc;
    while (1) {
        printf("\n--- Você entrou em: %s ---\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        // opções
        printf("\nCaminhos disponíveis:\n");
        if (atual->esq) printf(" (e) Ir para %s\n", atual->esq->nome);
        if (atual->dir) printf(" (d) Ir para %s\n", atual->dir->nome);
        printf(" (s) Sair e ir ao julgamento\n");
        printf("Escolha: ");
        scanf(" %c", &opc);
        opc = tolower(opc);

        if (opc == 'e' && atual->esq) {
            atual = atual->esq;
        } else if (opc == 'd' && atual->dir) {
            atual = atual->dir;
        } else if (opc == 's') {
            printf("\nExploração encerrada pelo jogador.\n");
            return;
        } else {
            printf("Opção inválida ou caminho inexistente. Tente novamente.\n");
        }
    }
}

// -----------------------------
// verificarSuspeitoFinal()
// Percorre a BST de pistas coletadas e conta quantas apontam
// para o suspeito acusado (usando a tabela hash).
// Retorna número de pistas que apontam para o acusado.
// -----------------------------
int verificarSuspeitoFinal(PistaNode *raizPistas, HashNode *tabela[], const char *acusado) {
    if (!raizPistas) return 0;
    int count = 0;
    // use stackless recursion to traverse; define inner function via recursion
    struct Frame { PistaNode *node; } ;
    // simple recursive traversal
    if (raizPistas->esq) count += verificarSuspeitoFinal(raizPistas->esq, tabela, acusado);
    const char *sus = encontrarSuspeito(tabela, raizPistas->pista);
    if (sus && strcmp(sus, acusado) == 0) count++;
    if (raizPistas->dir) count += verificarSuspeitoFinal(raizPistas->dir, tabela, acusado);
    return count;
}

// -----------------------------
// liberarHash()
// Libera a tabela hash.
// -----------------------------
void liberarHash(HashNode *tabela[]) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *cur = tabela[i];
        while (cur) {
            HashNode *tmp = cur;
            cur = cur->prox;
            free(tmp);
        }
        tabela[i] = NULL;
    }
}

// -----------------------------
// liberarMapa()
// Libera toda a árvore de salas (pos-order)
// -----------------------------
void liberarMapa(Sala *raiz) {
    if (!raiz) return;
    liberarMapa(raiz->esq);
    liberarMapa(raiz->dir);
    free(raiz);
}

// -----------------------------
// main()
// Monta mapa fixo, monta hash de pistas->suspeitos,
// permite exploração, exibe pistas e conduz acusação.
// -----------------------------
int main() {
    // 1) Montar mapa (árvore binária fixa)
    // Exemplo de mapa:
    //                 Hall
    //              /       \
    //         SalaEstar    Cozinha
    //         /     \         \
    //      Jardim  Biblioteca  Despensa

    Sala *hall = criarSala("Hall de Entrada", "Pegadas úmidas na passadeira");
    Sala *salaEstar = criarSala("Sala de Estar", "Livro de receitas rasgado");
    Sala *cozinha = criarSala("Cozinha", "Talher faltando no gaveteiro");
    Sala *jardim = criarSala("Jardim", "Pedaço de tecido encharcado");
    Sala *biblioteca = criarSala("Biblioteca", "Página arrancada com anotações");
    Sala *despensa = criarSala("Despensa", "Frasco com resíduo químico");

    hall->esq = salaEstar; hall->dir = cozinha;
    salaEstar->esq = jardim; salaEstar->dir = biblioteca;
    cozinha->dir = despensa;

    // 2) Inicializar tabela hash vazia
    HashNode *tabela[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; ++i) tabela[i] = NULL;

    // 3) Popular tabela hash com associações pista -> suspeito
    // (essas associações são pré-definidas no código)
    inserirNaHash(tabela, "Pegadas úmidas na passadeira", "Sr. Verde");
    inserirNaHash(tabela, "Livro de receitas rasgado", "Sra. Rosa");
    inserirNaHash(tabela, "Talher faltando no gaveteiro", "Sr. Azul");
    inserirNaHash(tabela, "Pedaço de tecido encharcado", "Sr. Verde");
    inserirNaHash(tabela, "Página arrancada com anotações", "Sra. Rosa");
    inserirNaHash(tabela, "Frasco com resíduo químico", "Sr. Amarelo");

    // 4) BST de pistas coletadas (vazia inicialmente)
    PistaNode *raizPistas = NULL;

    printf("=== Detective Quest: Investigação Final ===\n");
    printf("Explore a mansão e colete pistas. Ao sair, acuse um suspeito.\n");

    // 5) Exploração interativa
    explorarSalas(hall, &raizPistas, tabela);

    // 6) Exibir pistas coletadas
    printf("\n--- Pistas coletadas (ordem alfabética) ---\n");
    if (!raizPistas) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(raizPistas);
    }

    // 7) Fase de acusação
    char acusado[MAX_NOME];
    printf("\nDigite o nome do suspeito que você deseja acusar (ex: 'Sr. Verde'): ");
    // limpar buffer restante antes de fgets
    getchar(); // consome newline remanescente do scanf anterior
    if (fgets(acusado, MAX_NOME, stdin) != NULL) {
        // remover newline
        size_t ln = strlen(acusado);
        if (ln > 0 && acusado[ln-1] == '\n') acusado[ln-1] = '\0';
    } else {
        acusado[0] = '\0';
    }

    if (strlen(acusado) == 0) {
        printf("Nenhum acusado informado. Encerrando.\n");
    } else {
        int cont = verificarSuspeitoFinal(raizPistas, tabela, acusado);
        printf("\nO acusado: %s\n", acusado);
        printf("Número de pistas coletadas que apontam para %s: %d\n", acusado, cont);
        if (cont >= 2) {
            printf("Resultado: Há evidências suficientes. Acusação aceita!\n");
        } else {
            printf("Resultado: Evidências insuficientes. Acusação rejeitada.\n");
        }
    }

    // 8) Limpeza de memória
    liberarPistas(raizPistas);
    liberarHash(tabela);
    liberarMapa(hall);

    printf("\nInvestigação encerrada. Obrigado por jogar!\n");
    return 0;
}
