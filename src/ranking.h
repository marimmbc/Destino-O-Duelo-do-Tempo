#ifndef RANKING_H
#define RANKING_H

#include <stdbool.h>
#include "lista.h"

/*
 * Ranking: nome do jogador, cálculo da pontuação e o arquivo com as partidas.
 * Cada linha do arquivo é "pontos;nome". Para mostrar as colocações, as
 * partidas viram uma Lista (poder = pontos) ordenada com o mergeSortLista.
 */

#define ARQUIVO_RANKING   "ranking.txt"
#define TAMANHO_MAX_NOME  15
#define QTD_COLOCACOES    10    /* quantas posições aparecem na tela */

/* Regras da pontuação */
#define PONTOS_POR_DUELO  100   /* cada duelo vencido na Fase 1 */
#define PONTOS_POR_VIDA   150   /* cada vida que o Destino ainda tinha no fim da Fase 2 */
#define PONTOS_VITORIA    500   /* derrotar o Loki */

int calcularPontuacao(int duelosVencidos, int vidasRestantes, bool derrotouLoki);
void salvarPontuacao(const char *nome, int pontos);
Lista *carregarRanking(void);

bool atualizarCampoNome(char *nome, int tamanhoMax);
bool nomeValido(const char *nome);

#endif
