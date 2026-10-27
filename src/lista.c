#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

/* ------------------------------------------------------------------
 * Operações básicas da lista duplamente encadeada
 * ------------------------------------------------------------------ */

/* Cria uma lista vazia. Retorna NULL se faltar memória. */
Lista *criarLista(void) {
    Lista *lista = (Lista *) malloc(sizeof(Lista));
    if (lista == NULL) {
        printf("Erro: memoria insuficiente para criar a lista.\n");
        return NULL;
    }
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

/* Adiciona um novo personagem no final da lista. */
void inserirNoFim(Lista *lista, const char *nome, int poder) {
    if (lista == NULL) return;

    No *novo = (No *) malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente para inserir %s.\n", nome);
        return;
    }

    /* copia o nome com segurança, sem passar do tamanho do vetor */
    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';
    novo->poder = poder;
    novo->prox = NULL;
    novo->ant = lista->fim;

    if (lista->fim == NULL) {
        /* lista estava vazia: o novo nó é o início e o fim */
        lista->inicio = novo;
    } else {
        lista->fim->prox = novo;
    }
    lista->fim = novo;
    lista->tamanho++;
}

/* Remove um nó da lista (usado quando um Sentinela é derrotado). */
void removerNo(Lista *lista, No *no) {
    if (lista == NULL || no == NULL) return;

    /* religa o vizinho de trás */
    if (no->ant != NULL) {
        no->ant->prox = no->prox;
    } else {
        lista->inicio = no->prox;   /* era o primeiro */
    }

    /* religa o vizinho da frente */
    if (no->prox != NULL) {
        no->prox->ant = no->ant;
    } else {
        lista->fim = no->ant;       /* era o último */
    }

    free(no);
    lista->tamanho--;
}

/* Procura um personagem pelo nome. Retorna NULL se não encontrar. */
No *buscarPorNome(Lista *lista, const char *nome) {
    if (lista == NULL) return NULL;

    No *atual = lista->inicio;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

/* Conta quantos nós existem percorrendo a lista.
 * Ao fim da Fase 1, indica quantos Sentinelas sobraram (vidas da Fase 2). */
int contarNos(Lista *lista) {
    if (lista == NULL) return 0;

    int quantidade = 0;
    No *atual = lista->inicio;
    while (atual != NULL) {
        quantidade++;
        atual = atual->prox;
    }
    return quantidade;
}

/* ------------------------------------------------------------------
 * Merge Sort (ordem decrescente de poder)
 *
 * Ideia: dividir a lista ao meio, ordenar cada metade recursivamente
 * e depois intercalar as duas metades já ordenadas.
 * Durante a ordenação usamos apenas o ponteiro "prox"; no final
 * os ponteiros "ant" e o "fim" da lista são reconstruídos.
 * ------------------------------------------------------------------ */

/* Divide a lista em duas metades usando dois ponteiros:
 * "lento" anda 1 nó por vez e "rapido" anda 2.
 * Quando o rápido chega ao fim, o lento está no meio.
 * Retorna o início da segunda metade. */
static No *dividirLista(No *inicio) {
    No *lento = inicio;
    No *rapido = inicio->prox;

    while (rapido != NULL && rapido->prox != NULL) {
        lento = lento->prox;
        rapido = rapido->prox->prox;
    }

    No *segundaMetade = lento->prox;
    lento->prox = NULL;     /* corta a lista em duas */
    return segundaMetade;
}

/* Junta duas listas já ordenadas (do maior para o menor) em uma só.
 * Usa um nó auxiliar "cabeca" para simplificar o encadeamento.
 * Em caso de empate, o nó da primeira metade vem antes (ordenação estável). */
static No *intercalarListas(No *primeira, No *segunda) {
    No cabeca;
    No *ultimo = &cabeca;
    cabeca.prox = NULL;

    while (primeira != NULL && segunda != NULL) {
        if (primeira->poder >= segunda->poder) {
            ultimo->prox = primeira;
            primeira = primeira->prox;
        } else {
            ultimo->prox = segunda;
            segunda = segunda->prox;
        }
        ultimo = ultimo->prox;
    }

    /* o que sobrou de uma das metades já está ordenado: só encaixar */
    ultimo->prox = (primeira != NULL) ? primeira : segunda;

    return cabeca.prox;
}

/* Parte recursiva do Merge Sort. */
static No *mergeSortRecursivo(No *inicio) {
    /* caso base: lista vazia ou com um só nó já está ordenada */
    if (inicio == NULL || inicio->prox == NULL) {
        return inicio;
    }

    No *segundaMetade = dividirLista(inicio);

    No *primeiraOrdenada = mergeSortRecursivo(inicio);
    No *segundaOrdenada = mergeSortRecursivo(segundaMetade);

    return intercalarListas(primeiraOrdenada, segundaOrdenada);
}

/* Ordena a lista do mais forte para o mais fraco.
 * A ordem resultante define os duelos da Fase 1. */
void mergeSortLista(Lista *lista) {
    if (lista == NULL || lista->inicio == NULL) return;

    lista->inicio = mergeSortRecursivo(lista->inicio);

    /* reconstrói os ponteiros "ant" e encontra o novo fim */
    No *anterior = NULL;
    No *atual = lista->inicio;
    while (atual != NULL) {
        atual->ant = anterior;
        anterior = atual;
        atual = atual->prox;
    }
    lista->fim = anterior;
}

/* ------------------------------------------------------------------
 * Utilitários
 * ------------------------------------------------------------------ */

/* Mostra a lista no terminal (útil para testar sem interface). */
void imprimirLista(Lista *lista) {
    if (lista == NULL) return;

    No *atual = lista->inicio;
    while (atual != NULL) {
        printf("%s (%d)", atual->nome, atual->poder);
        if (atual->prox != NULL) printf(" -> ");
        atual = atual->prox;
    }
    printf("\n");
}

/* Libera todos os nós e a própria lista. */
void liberarLista(Lista *lista) {
    if (lista == NULL) return;

    No *atual = lista->inicio;
    while (atual != NULL) {
        No *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    free(lista);
}