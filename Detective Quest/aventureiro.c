#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------
// Estrutura da Sala (nó da árvore binária do mapa)
// ---------------------------------------------------------
typedef struct sala {
    char nome[50];
    char pista[80];           // Pista encontrada na sala (opcional)
    struct sala *esq;         // Caminho à esquerda
    struct sala *dir;         // Caminho à direita
} Sala;

// ---------------------------------------------------------
// Estrutura do nó da BST de pistas
// ---------------------------------------------------------
typedef struct pistaNode {
    char pista[80];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;

// ---------------------------------------------------------
// criarSala()
// Cria dinamicamente uma sala com nome e pista
// ---------------------------------------------------------
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esq = NULL;
    nova->dir = NULL;
    
    return nova;
}

// ---------------------------------------------------------
// inserirPista()
// Insere uma pista coletada na BST de pistas
// ---------------------------------------------------------
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else
        raiz->dir = inserirPista(raiz->dir, pista);

    return raiz;
}

// ---------------------------------------------------------
// exibirPistas()
// Percorre a BST em ordem alfabética (in-order)
// ---------------------------------------------------------
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL)
        return;

    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// ---------------------------------------------------------
// explorarSalasComPistas()
// Permite ao jogador navegar pela mansão e coleta pistas
// ---------------------------------------------------------
void explorarSalasComPistas(Sala *atual, PistaNode **arvorePistas) {
    char opc;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Se a sala tiver pista, coletamos
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        // Exibe caminhos disponíveis
        printf("\nOpções:\n");
        if (atual->esq) printf(" (e) Ir para %s\n", atual->esq->nome);
        if (atual->dir) printf(" (d) Ir para %s\n", atual->dir->nome);
        printf(" (s) Sair da exploração\n");

        printf("Escolha: ");
        scanf(" %c", &opc);

        if (opc == 'e' && atual->esq) {
            atual = atual->esq;
        }
        else if (opc == 'd' && atual->dir) {
            atual = atual->dir;
        }
        else if (opc == 's') {
            printf("\nEncerrando exploração...\n");
            return;
        }
        else {
            printf("Opção inválida!\n");
        }
    }
}

// ---------------------------------------------------------
// main()
// Monta as duas árvores e inicia o sistema
// ---------------------------------------------------------
int main() {
    // ------------------------------
    // Construção do mapa da mansão
    // ------------------------------

    // Estrutura:
    //                 Hall
    //          /                  \
    //   Sala de Estar         Cozinha
    //    /        \                \
    // Jardim   Biblioteca        Despensa

    Sala *hall        = criarSala("Hall de Entrada", "Pegadas recentes no carpete");
    Sala *estar       = criarSala("Sala de Estar", "Livro derrubado no chão");
    Sala *cozinha     = criarSala("Cozinha", "Talher faltando no armário");
    Sala *jardim      = criarSala("Jardim", "Terra remexida perto da fonte");
    Sala *biblioteca  = criarSala("Biblioteca", "Página arrancada de um diário");
    Sala *despensa    = criarSala("Despensa", "Frasco vazio com cheiro suspeito");

    // Ligações da árvore
    hall->esq = estar;
    hall->dir = cozinha;

    estar->esq = jardim;
    estar->dir = biblioteca;

    cozinha->dir = despensa;

    // Árvore de pistas inicialmente vazia
    PistaNode *arvorePistas = NULL;

    printf("=== Detective Quest: Exploração + Coleta de Pistas ===\n");
    
    // Inicia a exploração
    explorarSalasComPistas(hall, &arvorePistas);

    // Exibe todas as pistas coletadas
    printf("\n=== Pistas Coletadas (ordem alfabética) ===\n");
    exibirPistas(arvorePistas);

    printf("\nFim da jornada!\n");

    return 0;
}
