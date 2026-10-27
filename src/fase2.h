#ifndef FASE2_H
#define FASE2_H

#include <stdbool.h>
#include "pilha.h"

/*
 * Fase 2: duelo por turnos entre o Destino e o Loki usando a pilha de ações.
 * (Pessoa 2 pode acrescentar campos à struct à vontade.)
 */

typedef struct {
    Pilha *acoes;           /* pilha de ações do duelo */
    int vidasDestino;       /* vem dos Sentinelas sobreviventes da Fase 1 */
    int vidaLoki;
} Fase2;

void iniciarFase2(Fase2 *fase, int vidasDestino);
void atualizarFase2(Fase2 *fase);
bool fase2Terminou(const Fase2 *fase);
bool destinoVenceu(const Fase2 *fase);
void liberarFase2(Fase2 *fase);

#endif
