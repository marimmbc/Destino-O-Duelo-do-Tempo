#include <stddef.h>
#include "raylib.h"
#include "fase1.h"
#include "fase2.h"
#include "interface.h"
#include "ranking.h"

/*
 * A main é a "direção do filme": ela não faz a lógica das fases nem
 * desenha os detalhes. Ela prepara tudo, decide qual tela está ativa
 * e passa o controle para o módulo certo (fase1, fase2, ranking ou interface).
 */

#define LARGURA_TELA 1280
#define ALTURA_TELA  720
#define TITULO_JOGO  "Destino: O Duelo do Tempo"

/* Máquina de estados: em qual tela o jogo está agora. */
typedef enum {
    TELA_MENU,
    TELA_RANKING,       /* colocações de quem já jogou */
    TELA_NOME,          /* jogador digita o nome antes de começar */
    TELA_HISTORIA_1,    /* contexto antes da Fase 1 */
    TELA_FASE1,
    TELA_HISTORIA_2,    /* contexto antes da Fase 2 */
    TELA_FASE2,
    TELA_VITORIA,
    TELA_DERROTA
} Tela;

/* Usado nas telas de história: qualquer tecla de confirmação ou clique avança. */
static bool jogadorQuerAvancar(void) {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* Libera a lista e a pilha da partida anterior e monta tudo de novo do zero.
 * Sem isso, a segunda partida começaria com Sentinelas já removidos
 * ou ataques antigos na pilha. */
static void novaPartida(Fase1 *fase1, Fase2 *fase2) {
    liberarFase1(fase1);
    liberarFase2(fase2);
    iniciarFase1(fase1);
}

/* Fim de partida: calcula os pontos e grava no arquivo de ranking. */
static int registrarPartida(const char *nome, int duelosVencidos, int vidasRestantes, bool derrotouLoki) {
    int pontos = calcularPontuacao(duelosVencidos, vidasRestantes, derrotouLoki);
    salvarPontuacao(nome, pontos);
    return pontos;
}

int main(void) {
    /* ---------- 1. PREPARAÇÃO (roda uma vez) ---------- */
    InitWindow(LARGURA_TELA, ALTURA_TELA, TITULO_JOGO);
    SetTargetFPS(60);

    /* Todas as imagens são carregadas só aqui, nunca dentro do loop. */
    Recursos recursos;
    carregarRecursos(&recursos);

    Fase1 fase1 = {0};
    Fase2 fase2 = {0};
    iniciarFase1(&fase1);

    char nomeJogador[TAMANHO_MAX_NOME + 1] = "";   /* fica guardado para a próxima partida */
    int pontuacao = 0;                               /* pontos da partida que acabou */
    Lista *ranking = NULL;                           /* carregado ao abrir a tela de ranking */

    Tela telaAtual = TELA_MENU;

    /* ---------- 2. LOOP PRINCIPAL (60 vezes por segundo) ---------- */
    while (!WindowShouldClose()) {

        /* ===== ATUALIZAR: entrada e regras. Nada é desenhado aqui. ===== */
        switch (telaAtual) {
            case TELA_MENU:
                if (botaoClicado(BOTAO_JOGAR) || IsKeyPressed(KEY_ENTER)) {
                    telaAtual = TELA_NOME;
                } else if (botaoClicado(BOTAO_RANKING)) {
                    /* lê o arquivo só ao abrir a tela, nunca a cada quadro */
                    liberarLista(ranking);
                    ranking = carregarRanking();
                    telaAtual = TELA_RANKING;
                }
                break;

            case TELA_RANKING:
                if (botaoClicado(BOTAO_VOLTAR_MENU) || IsKeyPressed(KEY_ENTER)) {
                    telaAtual = TELA_MENU;
                }
                break;

            case TELA_NOME:
                if (atualizarCampoNome(nomeJogador, TAMANHO_MAX_NOME)) {
                    telaAtual = TELA_HISTORIA_1;
                }
                break;

            case TELA_HISTORIA_1:
                if (jogadorQuerAvancar()) telaAtual = TELA_FASE1;
                break;

            case TELA_FASE1:
                atualizarFase1(&fase1);
                if (fase1Terminou(&fase1)) {
                    /* 4. Passagem de dados: os Sentinelas que sobraram
                     * na lista viram as vidas do Destino na Fase 2. */
                    int sobreviventes = sentinelasSobreviventes(&fase1);
                    if (sobreviventes >= 2) {
                        iniciarFase2(&fase2, sobreviventes);
                        telaAtual = TELA_HISTORIA_2;
                    } else {
                        pontuacao = registrarPartida(nomeJogador, fase1.vitorias, 0, false);
                        telaAtual = TELA_DERROTA;
                    }
                }
                break;

            case TELA_HISTORIA_2:
                if (jogadorQuerAvancar()) telaAtual = TELA_FASE2;
                break;

            case TELA_FASE2:
                atualizarFase2(&fase2);
                if (fase2Terminou(&fase2)) {
                    bool venceu = destinoVenceu(&fase2);
                    pontuacao = registrarPartida(nomeJogador, fase1.vitorias, fase2.vidasDestino, venceu);
                    telaAtual = venceu ? TELA_VITORIA : TELA_DERROTA;
                }
                break;

            case TELA_VITORIA:
            case TELA_DERROTA:
                /* 5. Recomeçar: volta ao menu com uma partida nova. */
                if (botaoClicado(BOTAO_VOLTAR_MENU) || IsKeyPressed(KEY_ENTER)) {
                    novaPartida(&fase1, &fase2);
                    telaAtual = TELA_MENU;
                }
                break;
        }

        /* ===== DESENHAR: só mostra o estado atual. Nenhuma regra muda aqui. ===== */
        BeginDrawing();
        ClearBackground(BLACK);

        switch (telaAtual) {
            case TELA_MENU:       desenharMenu(&recursos);             break;
            case TELA_RANKING:    desenharRanking(&recursos, ranking); break;
            case TELA_NOME:       desenharEntradaNome(&recursos, nomeJogador); break;
            case TELA_HISTORIA_1: desenharHistoria(&recursos, 1);      break;
            case TELA_FASE1:      desenharFase1(&fase1, &recursos);    break;
            case TELA_HISTORIA_2: desenharHistoria(&recursos, 2);      break;
            case TELA_FASE2:      desenharFase2(&fase2, &recursos);    break;
            case TELA_VITORIA:    desenharVitoria(&recursos, nomeJogador, pontuacao); break;
            case TELA_DERROTA:    desenharDerrota(&recursos, nomeJogador, pontuacao); break;
        }

        EndDrawing();
    }

    /* ---------- 6. ENCERRAMENTO (nessa ordem) ---------- */
    liberarFase1(&fase1);           /* lista */
    liberarFase2(&fase2);           /* pilha */
    liberarLista(ranking);          /* ranking */
    descarregarRecursos(&recursos); /* imagens */
    CloseWindow();

    return 0;
}
