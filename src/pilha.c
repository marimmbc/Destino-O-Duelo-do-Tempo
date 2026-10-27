#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

/* Cria uma pilha vazia. Retorna NULL se faltar memória. */
Pilha *criarPilha(void) {
    Pilha *pilha = (Pilha *) malloc(sizeof(Pilha));
    if (pilha == NULL) {
        printf("Erro: memoria insuficiente para criar a pilha.\n");
        return NULL;
    }
    pilha->topo = NULL;
    pilha->tamanho = 0;
    return pilha;
}

/* Coloca uma ação no topo da pilha. */
void empilhar(Pilha *pilha, int acao) {
    if (pilha == NULL) return;

    NoPilha *novo = (NoPilha *) malloc(sizeof(NoPilha));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente para empilhar.\n");
        return;
    }
    novo->acao = acao;
    novo->prox = pilha->topo;
    pilha->topo = novo;
    pilha->tamanho++;
}

/* Tira a ação do topo e devolve em *acao. Retorna false se estava vazia. */
bool desempilhar(Pilha *pilha, int *acao) {
    if (pilhaVazia(pilha)) return false;

    NoPilha *removido = pilha->topo;
    if (acao != NULL) *acao = removido->acao;
    pilha->topo = removido->prox;
    free(removido);
    pilha->tamanho--;
    return true;
}

bool pilhaVazia(Pilha *pilha) {
    return pilha == NULL || pilha->topo == NULL;
}

/* Libera todos os nós e a própria pilha. */
void liberarPilha(Pilha *pilha) {
    if (pilha == NULL) return;

    while (desempilhar(pilha, NULL)) { }
    free(pilha);
}
