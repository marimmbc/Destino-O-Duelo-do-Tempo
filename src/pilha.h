#ifndef PILHA_H
#define PILHA_H

#include <stdbool.h>

/*
 * Pilha encadeada usada para as ações do duelo contra o Loki (Fase 2).
 */

typedef struct NoPilha {
    int acao;               /* código da ação (ataque, escudo, feitiço...) */
    struct NoPilha *prox;   /* nó logo abaixo na pilha */
} NoPilha;

typedef struct {
    NoPilha *topo;          /* última ação empilhada */
    int tamanho;            /* quantidade de ações na pilha */
} Pilha;

Pilha *criarPilha(void);
void empilhar(Pilha *pilha, int acao);
bool desempilhar(Pilha *pilha, int *acao);
bool pilhaVazia(Pilha *pilha);
void liberarPilha(Pilha *pilha);

#endif
