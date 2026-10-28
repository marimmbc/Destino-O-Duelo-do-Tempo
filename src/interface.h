#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include "raylib.h"
#include "fase1.h"
#include "fase2.h"

/*
 * Interface: carrega as imagens e desenha cada tela.
 * Só desenha; nenhuma regra do jogo muda aqui.
 */

/* Todas as imagens da pasta assets, carregadas uma única vez. */
typedef struct {
    Texture2D cenarioBatalha;
    Texture2D cenarioLoki;
    Texture2D telaVitoria;
    Texture2D telaDerrota;

    Texture2D destino;
    Texture2D destinoEscudo;
    Texture2D destinoFeitico;
    Texture2D destinoRaio;

    Texture2D sentinela;
    Texture2D sentinelaAtaque;

    Texture2D heroi[3];
    Texture2D heroiAtaque[3];

    Texture2D loki;
    Texture2D lokiAtaque;
    Texture2D lokiReverter;
} Recursos;

/* Botões das telas simples (menu e finais) */
#define BOTAO_JOGAR       ((Rectangle){ 540, 460, 200, 60 })
#define BOTAO_RANKING     ((Rectangle){ 540, 540, 200, 60 })
#define BOTAO_VOLTAR_MENU ((Rectangle){ 540, 600, 200, 60 })

void carregarRecursos(Recursos *r);
void descarregarRecursos(Recursos *r);

bool botaoClicado(Rectangle area);

void desenharMenu(const Recursos *r);
void desenharRanking(const Recursos *r, const Lista *ranking);
void desenharEntradaNome(const Recursos *r, const char *nome);
void desenharHistoria(const Recursos *r, int capitulo);
void desenharFase1(const Fase1 *fase, const Recursos *r);
void desenharFase2(const Fase2 *fase, const Recursos *r);
void desenharVitoria(const Recursos *r, const char *nome, int pontos);
void desenharDerrota(const Recursos *r, const char *nome, int pontos);

#endif
