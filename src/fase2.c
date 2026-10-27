#include <stdlib.h>
#include "raylib.h"
#include "fase2.h"

/*
 * ESBOÇO PROVISÓRIO: só existe para o jogo compilar e a main ser testada.
 * Pessoa 2 substitui atualizarFase2 pela lógica real dos turnos.
 */

#define VIDA_INICIAL_LOKI 100

void iniciarFase2(Fase2 *fase, int vidasDestino) {
    liberarFase2(fase);     /* garante que não sobra pilha antiga */
    fase->acoes = criarPilha();
    fase->vidasDestino = vidasDestino;
    fase->vidaLoki = VIDA_INICIAL_LOKI;
}

void atualizarFase2(Fase2 *fase) {
    /* provisório: A empilha um ataque, ESPAÇO resolve o topo, D tira uma vida do Destino */
    if (IsKeyPressed(KEY_A)) empilhar(fase->acoes, 1);
    if (IsKeyPressed(KEY_SPACE) && desempilhar(fase->acoes, NULL)) fase->vidaLoki -= 25;
    if (IsKeyPressed(KEY_D)) fase->vidasDestino--;
}

bool fase2Terminou(const Fase2 *fase) {
    return fase->vidaLoki <= 0 || fase->vidasDestino <= 0;
}

bool destinoVenceu(const Fase2 *fase) {
    return fase->vidaLoki <= 0;
}

void liberarFase2(Fase2 *fase) {
    liberarPilha(fase->acoes);
    fase->acoes = NULL;
    fase->vidasDestino = 0;
    fase->vidaLoki = 0;
}
