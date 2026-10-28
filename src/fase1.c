#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "fase1.h"

/* ------------------------------------------------------------------
 * Posições na tela (3 linhas, uma por duelo)
 * ------------------------------------------------------------------ */

#define TOPO_PRIMEIRA_LINHA 140
#define ALTURA_LINHA        160
#define TAMANHO_SPRITE      110

static float topoLinha(int posicao) {
    return (float)(TOPO_PRIMEIRA_LINHA + posicao * ALTURA_LINHA);
}

Rectangle spriteSentinela(int posicao) {
    return (Rectangle){ 230, topoLinha(posicao), TAMANHO_SPRITE, TAMANHO_SPRITE };
}

Rectangle spriteHeroi(int posicao) {
    return (Rectangle){ 940, topoLinha(posicao), TAMANHO_SPRITE, TAMANHO_SPRITE };
}

/* Área clicável do Sentinela: nome, número, barra e sprite. */
Rectangle areaSentinela(int posicao) {
    return (Rectangle){ 30, topoLinha(posicao) - 52, 330, 165 };
}

Rectangle areaNucleo(int indice) {
    return (Rectangle){ 50.0f + indice * 120, 620, 80, 80 };
}

/* ------------------------------------------------------------------
 * Sorteio dos poderes
 * ------------------------------------------------------------------ */

static void trocarInteiros(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* true se algum valor que um Sentinela pode alcançar (sem núcleo, com um
 * ou com os dois) for igual ao poder de algum herói. Empate não pode. */
static bool temEmpate(const int sentinela[], const int heroi[], const int nucleo[]) {
    for (int i = 0; i < QTD_DUELOS; i++) {
        int possiveis[4] = {
            sentinela[i],
            sentinela[i] + nucleo[0],
            sentinela[i] + nucleo[1],
            sentinela[i] + nucleo[0] + nucleo[1]
        };
        for (int p = 0; p < 4; p++) {
            for (int j = 0; j < QTD_DUELOS; j++) {
                if (possiveis[p] == heroi[j]) return true;
            }
        }
    }
    return false;
}

static bool temRepetido(const int valores[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        for (int j = i + 1; j < quantidade; j++) {
            if (valores[i] == valores[j]) return true;
        }
    }
    return false;
}

/* Cria os 3 heróis com poderes aleatórios e ordena com Merge Sort:
 * o herói 1 é sempre o mais forte. */
static void criarHerois(Fase1 *fase) {
    int poder[QTD_DUELOS];
    poder[0] = GetRandomValue(72, 88);
    poder[1] = poder[0] - GetRandomValue(5, 12);
    poder[2] = poder[1] - GetRandomValue(5, 12);

    /* embaralha antes de inserir, para a ordenação ter trabalho de verdade */
    for (int i = QTD_DUELOS - 1; i > 0; i--) {
        trocarInteiros(&poder[i], &poder[GetRandomValue(0, i)]);
    }

    fase->herois = criarLista();
    for (int i = 0; i < QTD_DUELOS; i++) {
        inserirNoFim(fase->herois, "Heroi", poder[i]);
    }
    mergeSortLista(fase->herois);

    /* depois de ordenar, o nome segue a posição: Heroi 1, 2 e 3 */
    No *atual = fase->herois->inicio;
    for (int i = 1; atual != NULL; i++, atual = atual->prox) {
        snprintf(atual->nome, sizeof(atual->nome), "Heroi %d", i);
    }
}

/* As 6 ordens possíveis de 3 Sentinelas. A primeira é a ordem certa. */
static const int ORDENS[6][QTD_DUELOS] = {
    { 0, 1, 2 }, { 0, 2, 1 }, { 1, 0, 2 }, { 1, 2, 0 }, { 2, 0, 1 }, { 2, 1, 0 }
};

/* Maior número de duelos que dá para vencer com os Sentinelas nessa ordem,
 * testando todos os jeitos de entregar os 2 núcleos. */
static int melhorResultado(const int sentinela[], const int heroi[], const int nucleo[]) {
    int melhor = 0;
    for (int a = 0; a < QTD_DUELOS; a++) {
        for (int b = 0; b < QTD_DUELOS; b++) {
            int vitorias = 0;
            for (int i = 0; i < QTD_DUELOS; i++) {
                int poder = sentinela[i];
                if (i == a) poder += nucleo[0];
                if (i == b) poder += nucleo[1];
                if (poder > heroi[i]) vitorias++;
            }
            if (vitorias > melhor) melhor = vitorias;
        }
    }
    return melhor;
}

/* Cria os Sentinelas e os núcleos de forma que exista uma solução:
 *  - um Sentinela passa de um herói por pouco, mas perde para os mais fortes;
 *  - os outros dois ficam abaixo de todos os heróis e só vencem com o
 *    núcleo certo: o mais fraco deles precisa do núcleo maior, e o núcleo
 *    menor não é suficiente para ele.
 * A ordem inicial é escolhida para que, sem trocar, no máximo 1 duelo
 * possa ser vencido: o jogador precisa usar as trocas. */
static void criarSentinelas(Fase1 *fase) {
    int heroi[QTD_DUELOS];
    No *atual = fase->herois->inicio;
    for (int i = 0; i < QTD_DUELOS; i++, atual = atual->prox) {
        heroi[i] = atual->poder;
    }

    int certo[QTD_DUELOS];  /* certo[i] = Sentinela que deveria enfrentar o herói i */
    int nucleo[QTD_NUCLEOS];
    int candidatas[6];      /* ordens iniciais que obrigam o jogador a trocar */
    int qtdCandidatas;
    do {
        int sozinho = GetRandomValue(0, 2);
        /* dos outros dois heróis, "forte" é o de mais poder (menor posição) */
        int forte = (sozinho == 0) ? 1 : 0;
        int fraco = (sozinho == 2) ? 1 : 2;

        int maisFraco = heroi[QTD_DUELOS - 1];
        certo[sozinho] = heroi[sozinho] + GetRandomValue(2, 4);
        certo[fraco]   = maisFraco - GetRandomValue(1, 4);
        certo[forte]   = certo[fraco] - GetRandomValue(4, 7);

        nucleo[0] = heroi[fraco] - certo[fraco] + GetRandomValue(1, 3);
        nucleo[1] = heroi[forte] - certo[forte] + GetRandomValue(1, 3);

        qtdCandidatas = 0;
        for (int o = 1; o < 6; o++) {
            int ordenado[QTD_DUELOS];
            for (int i = 0; i < QTD_DUELOS; i++) ordenado[i] = certo[ORDENS[o][i]];
            if (melhorResultado(ordenado, heroi, nucleo) <= 1) candidatas[qtdCandidatas++] = o;
        }
    } while (temEmpate(certo, heroi, nucleo) || temRepetido(certo, QTD_DUELOS) ||
             nucleo[0] == nucleo[1] || qtdCandidatas == 0);

    const int *ordem = ORDENS[candidatas[GetRandomValue(0, qtdCandidatas - 1)]];

    const char *nomes[QTD_DUELOS] = { "Sentinela Alfa", "Sentinela Beta", "Sentinela Gama" };
    fase->sentinelas = criarLista();
    for (int i = 0; i < QTD_DUELOS; i++) {
        inserirNoFim(fase->sentinelas, nomes[i], certo[ordem[i]]);
    }

    /* os núcleos também aparecem em ordem aleatória */
    if (GetRandomValue(0, 1)) trocarInteiros(&nucleo[0], &nucleo[1]);
    for (int i = 0; i < QTD_NUCLEOS; i++) {
        fase->nucleos[i] = nucleo[i];
        fase->nucleoUsado[i] = false;
    }
}

/* ------------------------------------------------------------------
 * Ciclo de vida da fase
 * ------------------------------------------------------------------ */

void iniciarFase1(Fase1 *fase) {
    criarHerois(fase);
    criarSentinelas(fase);

    fase->nucleoSelecionado = -1;
    fase->sentinelaSelecionado = -1;
    fase->trocasRestantes = MAX_TROCAS;

    fase->etapa = ETAPA_PREPARACAO;
    fase->dueloAtual = 0;
    fase->tempoDuelo = 0.0f;
    fase->vitorias = 0;
    fase->terminou = false;
}

bool podeIniciarDuelos(const Fase1 *fase) {
    for (int i = 0; i < QTD_NUCLEOS; i++) {
        if (!fase->nucleoUsado[i]) return false;
    }
    return true;
}

/* Tira a fotografia dos 3 pares e começa os duelos. */
static void comecarDuelos(Fase1 *fase) {
    No *sentinela = fase->sentinelas->inicio;
    No *heroi = fase->herois->inicio;

    for (int i = 0; i < QTD_DUELOS; i++) {
        Duelo *duelo = &fase->duelos[i];
        duelo->sentinela = sentinela;
        strcpy(duelo->nomeSentinela, sentinela->nome);
        duelo->poderSentinela = sentinela->poder;
        duelo->poderHeroi = heroi->poder;
        duelo->resolvido = false;
        duelo->sentinelaVenceu = false;

        sentinela = sentinela->prox;
        heroi = heroi->prox;
    }

    fase->nucleoSelecionado = -1;
    fase->sentinelaSelecionado = -1;
    fase->dueloAtual = 0;
    fase->tempoDuelo = 0.0f;
    fase->etapa = ETAPA_DUELOS;
}

/* Clique num Sentinela: recebe o núcleo selecionado ou participa de uma troca. */
static void clicarSentinela(Fase1 *fase, int posicao) {
    No *clicado = buscarPorPosicao(fase->sentinelas, posicao);

    if (fase->nucleoSelecionado != -1) {
        clicado->poder += fase->nucleos[fase->nucleoSelecionado];
        fase->nucleoUsado[fase->nucleoSelecionado] = true;
        fase->nucleoSelecionado = -1;
    } else if (fase->sentinelaSelecionado == -1) {
        if (fase->trocasRestantes > 0) fase->sentinelaSelecionado = posicao;
    } else if (fase->sentinelaSelecionado == posicao) {
        fase->sentinelaSelecionado = -1;     /* clicou de novo: cancela */
    } else {
        No *primeiro = buscarPorPosicao(fase->sentinelas, fase->sentinelaSelecionado);
        trocarConteudo(primeiro, clicado);
        fase->trocasRestantes--;
        fase->sentinelaSelecionado = -1;
    }
}

static void atualizarPreparacao(Fase1 *fase) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < QTD_NUCLEOS; i++) {
        if (!fase->nucleoUsado[i] && CheckCollisionPointRec(mouse, areaNucleo(i))) {
            fase->nucleoSelecionado = (fase->nucleoSelecionado == i) ? -1 : i;
            fase->sentinelaSelecionado = -1;
            return;
        }
    }

    for (int posicao = 0; posicao < QTD_DUELOS; posicao++) {
        if (CheckCollisionPointRec(mouse, areaSentinela(posicao))) {
            clicarSentinela(fase, posicao);
            return;
        }
    }

    if (CheckCollisionPointRec(mouse, BOTAO_INICIAR_DUELOS) && podeIniciarDuelos(fase)) {
        comecarDuelos(fase);
        return;
    }

    /* clique no vazio cancela qualquer seleção */
    fase->nucleoSelecionado = -1;
    fase->sentinelaSelecionado = -1;
}

static void atualizarDuelos(Fase1 *fase) {
    fase->tempoDuelo += GetFrameTime();
    Duelo *duelo = &fase->duelos[fase->dueloAtual];

    if (!duelo->resolvido && fase->tempoDuelo >= DURACAO_ATAQUE) {
        duelo->resolvido = true;
        duelo->sentinelaVenceu = duelo->poderSentinela > duelo->poderHeroi;
        if (duelo->sentinelaVenceu) {
            fase->vitorias++;
        } else {
            /* o Sentinela derrotado sai da lista */
            removerNo(fase->sentinelas, duelo->sentinela);
            duelo->sentinela = NULL;
        }
    }

    if (fase->tempoDuelo >= DURACAO_ATAQUE + DURACAO_RESULTADO) {
        fase->dueloAtual++;
        fase->tempoDuelo = 0.0f;
        if (fase->dueloAtual == QTD_DUELOS) fase->etapa = ETAPA_RESULTADO;
    }
}

void atualizarFase1(Fase1 *fase) {
    switch (fase->etapa) {
        case ETAPA_PREPARACAO:
            atualizarPreparacao(fase);
            break;
        case ETAPA_DUELOS:
            atualizarDuelos(fase);
            break;
        case ETAPA_RESULTADO:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER)) {
                fase->terminou = true;
            }
            break;
    }
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
