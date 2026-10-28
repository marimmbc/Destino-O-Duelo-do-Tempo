#ifndef FASE1_H
#define FASE1_H

#include <stdbool.h>
#include "raylib.h"
#include "lista.h"

/*
 * Fase 1: Destino tem 3 Sentinelas contra 3 heróis.
 * O jogador reforça os Sentinelas com 2 núcleos de energia, pode trocar
 * a posição deles até 2 vezes e depois assiste aos 3 duelos:
 * herói 1 x sentinela 1, herói 2 x sentinela 2, herói 3 x sentinela 3.
 * O Sentinela derrotado sai da lista.
 */

#define QTD_DUELOS   3
#define QTD_NUCLEOS  2
#define MAX_TROCAS   2

typedef enum {
    ETAPA_PREPARACAO,   /* jogador usa os núcleos e troca posições */
    ETAPA_DUELOS,       /* os 3 duelos acontecem um de cada vez */
    ETAPA_RESULTADO     /* mostra quantos duelos foram vencidos */
} EtapaFase1;

/* Fotografia de um duelo, tirada quando os duelos começam.
 * Serve para continuar desenhando o Sentinela mesmo depois de removido da lista. */
typedef struct {
    No *sentinela;          /* nó na lista (NULL depois de removido) */
    char nomeSentinela[30];
    int poderSentinela;
    int poderHeroi;
    bool resolvido;         /* o resultado já foi decidido */
    bool sentinelaVenceu;
} Duelo;

typedef struct {
    Lista *sentinelas;      /* exército do Destino, na ordem dos duelos */
    Lista *herois;          /* heróis, do mais forte para o mais fraco */

    int nucleos[QTD_NUCLEOS];
    bool nucleoUsado[QTD_NUCLEOS];
    int nucleoSelecionado;      /* -1 = nenhum */
    int sentinelaSelecionado;   /* -1 = nenhum (primeiro clique de uma troca) */
    int trocasRestantes;

    EtapaFase1 etapa;
    Duelo duelos[QTD_DUELOS];
    int dueloAtual;
    float tempoDuelo;           /* segundos desde o início do duelo atual */
    int vitorias;

    bool terminou;          /* true quando o jogador sai da tela de resultado */
} Fase1;

void iniciarFase1(Fase1 *fase);
void atualizarFase1(Fase1 *fase);
bool fase1Terminou(const Fase1 *fase);
int sentinelasSobreviventes(const Fase1 *fase);
void liberarFase1(Fase1 *fase);

bool podeIniciarDuelos(const Fase1 *fase);

/* Posições na tela: usadas para os cliques aqui e para o desenho na interface */
#define DURACAO_ATAQUE     1.2f     /* segundos de animação antes do resultado */
#define DURACAO_RESULTADO  1.0f     /* segundos mostrando quem venceu */
#define BOTAO_INICIAR_DUELOS ((Rectangle){ 520, 640, 240, 55 })

Rectangle spriteSentinela(int posicao);
Rectangle spriteHeroi(int posicao);
Rectangle areaSentinela(int posicao);
Rectangle areaNucleo(int indice);

#endif
