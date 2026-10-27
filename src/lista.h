#ifndef LISTA_H
#define LISTA_H

/*
 * Lista duplamente encadeada usada para o exército de Sentinelas
 * e para o grupo de heróis (Fase 1).
 */

typedef struct No {
    char nome[30];      /* nome do personagem (ex.: "Sentinela Alfa") */
    int poder;          /* nível de poder exibido em cima da cabeça */
    struct No *prox;    /* próximo nó da lista */
    struct No *ant;     /* nó anterior da lista */
} No;

typedef struct {
    No *inicio;         /* primeiro nó (o mais forte, depois de ordenar) */
    No *fim;            /* último nó (o mais fraco, depois de ordenar) */
    int tamanho;        /* quantidade de nós na lista */
} Lista;

Lista *criarLista(void);
void inserirNoFim(Lista *lista, const char *nome, int poder);
void removerNo(Lista *lista, No *no);
No *buscarPorNome(Lista *lista, const char *nome);
int contarNos(Lista *lista);
void mergeSortLista(Lista *lista);
void imprimirLista(Lista *lista);
void liberarLista(Lista *lista);

#endif