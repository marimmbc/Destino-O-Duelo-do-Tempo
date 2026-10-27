#ifndef FASE1_H
#define FASE1_H

#include <stdbool.h>
#include "lista.h"

/*
 * Fase 1: o jogador distribui os núcleos, ordena o exército
 * e assiste aos duelos Sentinela x Herói.
 * (Pessoa 1 pode acrescentar campos à struct à vontade.)
 */

typedef struct {
    Lista *sentinelas;      /* exército do Destino */
    Lista *herois;          /* grupo de heróis adversário */
    bool terminou;          /* true quando todos os duelos acabaram */
} Fase1;

void iniciarFase1(Fase1 *fase);
void atualizarFase1(Fase1 *fase);
bool fase1Terminou(const Fase1 *fase);
int sentinelasSobreviventes(const Fase1 *fase);
void liberarFase1(Fase1 *fase);

#endif
