#include <stdlib.h>
#include "raylib.h"
#include "fase1.h"

/*
 * ESBOÇO PROVISÓRIO: só existe para o jogo compilar e a main ser testada.
 * Pessoa 1 substitui atualizarFase1 pela lógica real (núcleos,
 * "Ordenar exército" com mergeSortLista e os duelos).
 */

void iniciarFase1(Fase1 *fase) {
    fase->sentinelas = criarLista();
    inserirNoFim(fase->sentinelas, "Sentinela Alfa", 0);
    inserirNoFim(fase->sentinelas, "Sentinela Beta", 0);
    inserirNoFim(fase->sentinelas, "Sentinela Gama", 0);

    fase->herois = criarLista();
    inserirNoFim(fase->herois, "Heroi 1", 30);
    inserirNoFim(fase->herois, "Heroi 2", 20);
    inserirNoFim(fase->herois, "Heroi 3", 10);

    fase->terminou = false;
}

void atualizarFase1(Fase1 *fase) {
    /* provisório: R remove um Sentinela (simula derrota), ENTER encerra a fase */
    if (IsKeyPressed(KEY_R) && fase->sentinelas->fim != NULL) {
        removerNo(fase->sentinelas, fase->sentinelas->fim);
    }
    if (IsKeyPressed(KEY_ENTER)) fase->terminou = true;
}

bool fase1Terminou(const Fase1 *fase) {
    return fase->terminou;
}

/* Quantos Sentinelas sobraram na lista: vira o número de vidas da Fase 2. */
int sentinelasSobreviventes(const Fase1 *fase) {
    return contarNos(fase->sentinelas);
}

void liberarFase1(Fase1 *fase) {
    liberarLista(fase->sentinelas);
    liberarLista(fase->herois);
    fase->sentinelas = NULL;
    fase->herois = NULL;
    fase->terminou = false;
}
