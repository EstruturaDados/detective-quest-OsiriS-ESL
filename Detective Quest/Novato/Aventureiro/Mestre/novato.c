#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------
// Estrutura da sala (nó da árvore)
// ------------------------------
typedef struct sala {
    char nome[50];           // Nome do cômodo da mansão
    struct sala *esq;        // Caminho à esquerda
    struct sala *dir;        // Caminho à direita
} Sala;

// --------------------------------------------------------
// criarSala()
// Cria dinamicamente uma sala da mansão com nome informado
// --------------------------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro: não foi possível alocar memória.\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}

// ----------------------------------------------------------------------
// explorarSalas()
// Realiza a navegação interativa pela árvore a partir do Hall de Entrada
// O jogador escolhe 'e' (esquerda), 'd' (direita) ou 's' (sair)
// A exploração termina ao alcançar um nó-folha (sem caminhos)
// ----------------------------------------------------------------------
void explorarSalas(Sala *atual) {
    char escolha;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Se for nó-folha, acabou a exploração
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Você chegou a um cômodo sem saídas. Fim da exploração!\n");
            return;
        }

        // Exibe opções disponíveis
        printf("Escolha um caminho:\n");
        if (atual->esq) printf("  (e) Ir para %s\n", atual->esq->nome);
        if (atual->dir) printf("  (d) Ir para %s\n", atual->dir->nome);
        printf("  (s) Sair do jogo\n");

        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esq) {
            atual = atual->esq;
        }
        else if (escolha == 'd' && atual->dir) {
            atual = atual->dir;
        }
        else if (escolha == 's') {
            printf("Encerrando exploração...\n");
            return;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// --------------------------------------------
// main()
// Monta a árvore da mansão e inicia o jogo
// --------------------------------------------
int main() {

    // Árvore (modelo fixo da mansão)
    //              Hall
    //           /         \
    //     Sala de Estar    Cozinha
    //      /      \          \
    //   Jardim   Biblioteca   Despensa

    Sala *hall         = criarSala("Hall de Entrada");
    Sala *salaEstar    = criarSala("Sala de Estar");
    Sala *cozinha      = criarSala("Cozinha");
    Sala *jardim       = criarSala("Jardim");
    Sala *biblioteca   = criarSala("Biblioteca");
    Sala *despensa     = criarSala("Despensa");

    // Ligações da árvore
    hall->esq = salaEstar;
    hall->dir = cozinha;

    salaEstar->esq = jardim;
    salaEstar->dir = biblioteca;

    cozinha->dir = despensa;

    // Início da exploração
    printf("=== Detective Quest: Exploração da Mansão ===\n");
    explorarSalas(hall);

    // Liberação da memória (boa prática)
    free(jardim);
    free(biblioteca);
    free(salaEstar);
    free(despensa);
    free(cozinha);
    free(hall);

    return 0;
}
