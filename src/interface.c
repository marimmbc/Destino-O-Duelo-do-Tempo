#include <stddef.h>
#include "interface.h"
#include "ranking.h"

/*
 * Carregamento das imagens + desenhos PROVISÓRIOS das telas.
 * Pessoa 3 substitui os desenhar* pelo visual final.
 */

/* Carrega uma imagem já com filtro de pixel art (não borra ao ampliar). */
static Texture2D carregarPixelArt(const char *caminho) {
    Texture2D textura = LoadTexture(caminho);
    SetTextureFilter(textura, TEXTURE_FILTER_POINT);
    return textura;
}

void carregarRecursos(Recursos *r) {
    r->cenarioBatalha  = carregarPixelArt("assets/cenario_batalha.png");
    r->cenarioLoki     = carregarPixelArt("assets/cenario_loki.png");
    r->telaVitoria     = carregarPixelArt("assets/tela_vitoria.png");
    r->telaDerrota     = carregarPixelArt("assets/tela_derrota.png");

    r->destino         = carregarPixelArt("assets/destino.png");
    r->destinoEscudo   = carregarPixelArt("assets/destino_escudo.png");
    r->destinoFeitico  = carregarPixelArt("assets/destino_feitico.png");
    r->destinoRaio     = carregarPixelArt("assets/destino_raio.png");

    r->sentinela       = carregarPixelArt("assets/sentinela.png");
    r->sentinelaAtaque = carregarPixelArt("assets/sentinela_ataque.png");

    r->heroi[0]        = carregarPixelArt("assets/heroi_1.png");
    r->heroi[1]        = carregarPixelArt("assets/heroi_2.png");
    r->heroi[2]        = carregarPixelArt("assets/heroi_3.png");
    r->heroiAtaque[0]  = carregarPixelArt("assets/heroi_1_ataque.png");
    r->heroiAtaque[1]  = carregarPixelArt("assets/heroi_2_ataque.png");
    r->heroiAtaque[2]  = carregarPixelArt("assets/heroi_3_ataque.png");

    r->loki            = carregarPixelArt("assets/loki.png");
    r->lokiAtaque      = carregarPixelArt("assets/loki_ataque.png");
    r->lokiReverter    = carregarPixelArt("assets/loki_reverter.png");
}

void descarregarRecursos(Recursos *r) {
    UnloadTexture(r->cenarioBatalha);
    UnloadTexture(r->cenarioLoki);
    UnloadTexture(r->telaVitoria);
    UnloadTexture(r->telaDerrota);

    UnloadTexture(r->destino);
    UnloadTexture(r->destinoEscudo);
    UnloadTexture(r->destinoFeitico);
    UnloadTexture(r->destinoRaio);

    UnloadTexture(r->sentinela);
    UnloadTexture(r->sentinelaAtaque);

    for (int i = 0; i < 3; i++) {
        UnloadTexture(r->heroi[i]);
        UnloadTexture(r->heroiAtaque[i]);
    }

    UnloadTexture(r->loki);
    UnloadTexture(r->lokiAtaque);
    UnloadTexture(r->lokiReverter);
}

bool botaoClicado(Rectangle area) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), area);
}

/* ------------------------------------------------------------------
 * Auxiliares de desenho
 * ------------------------------------------------------------------ */

/* Estica uma imagem para ocupar a janela inteira. */
static void desenharFundo(Texture2D textura) {
    Rectangle origem = { 0, 0, (float) textura.width, (float) textura.height };
    Rectangle destino = { 0, 0, (float) GetScreenWidth(), (float) GetScreenHeight() };
    DrawTexturePro(textura, origem, destino, (Vector2){ 0, 0 }, 0.0f, WHITE);
}

static void desenharTextoCentralizado(const char *texto, int y, int tamanho, Color cor) {
    int largura = MeasureText(texto, tamanho);
    DrawText(texto, (GetScreenWidth() - largura) / 2, y, tamanho, cor);
}

/* Botão desativado fica apagado e não reage ao mouse. */
static void desenharBotao(Rectangle area, const char *texto, bool ativo) {
    bool mouseEmCima = ativo && CheckCollisionPointRec(GetMousePosition(), area);
    Color fundo = !ativo ? Fade(DARKGRAY, 0.5f) : (mouseEmCima ? DARKGREEN : DARKGRAY);
    Color letra = ativo ? WHITE : GRAY;
    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, 3, letra);
    int largura = MeasureText(texto, 28);
    DrawText(texto, (int)(area.x + (area.width - largura) / 2),
             (int)(area.y + (area.height - 28) / 2), 28, letra);
}

/* ------------------------------------------------------------------
 * Telas
 * ------------------------------------------------------------------ */

void desenharMenu(const Recursos *r) {
    desenharFundo(r->cenarioBatalha);
    desenharTextoCentralizado("DESTINO", 150, 90, GREEN);
    desenharTextoCentralizado("O Duelo do Tempo", 260, 40, RAYWHITE);
    desenharBotao(BOTAO_JOGAR, "Jogar", true);
    desenharBotao(BOTAO_RANKING, "Ranking", true);
}

void desenharEntradaNome(const Recursos *r, const char *nome) {
    desenharFundo(r->cenarioBatalha);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    desenharTextoCentralizado("Qual e o seu nome?", 200, 44, RAYWHITE);

    Rectangle caixa = { 390, 290, 500, 70 };
    DrawRectangleRec(caixa, Fade(BLACK, 0.7f));
    DrawRectangleLinesEx(caixa, 3, GOLD);
    DrawText(nome, (int)caixa.x + 20, (int)caixa.y + 17, 36, WHITE);

    /* cursor piscando depois da última letra */
    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawText("_", (int)caixa.x + 24 + MeasureText(nome, 36), (int)caixa.y + 17, 36, GOLD);
    }

    desenharTextoCentralizado("Digite seu nome e aperte ENTER para comecar", 390, 22, LIGHTGRAY);
}

/* Cores das 3 primeiras colocações: ouro, prata e bronze. */
static Color corDaColocacao(int colocacao) {
    switch (colocacao) {
        case 1:  return GOLD;
        case 2:  return (Color){ 200, 200, 210, 255 };
        case 3:  return (Color){ 205, 127, 50, 255 };
        default: return WHITE;
    }
}

void desenharRanking(const Recursos *r, const Lista *ranking) {
    desenharFundo(r->cenarioBatalha);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
    desenharTextoCentralizado("RANKING", 35, 60, GOLD);

    if (ranking == NULL || ranking->inicio == NULL) {
        desenharTextoCentralizado("Ninguem jogou ainda. Seja o primeiro!", 300, 30, RAYWHITE);
    } else {
        desenharTextoCentralizado(TextFormat("Partidas jogadas: %d", ranking->tamanho), 105, 22, LIGHTGRAY);

        Rectangle tabela = { 330, 140, 620, 440 };
        DrawRectangleRec(tabela, Fade(BLACK, 0.5f));
        DrawRectangleLinesEx(tabela, 2, Fade(GOLD, 0.6f));

        DrawText("POS", 360, 155, 22, GRAY);
        DrawText("JOGADOR", 460, 155, 22, GRAY);
        DrawText("PONTOS", 920 - MeasureText("PONTOS", 22), 155, 22, GRAY);

        No *atual = ranking->inicio;
        for (int colocacao = 1; colocacao <= QTD_COLOCACOES && atual != NULL; colocacao++) {
            int y = 150 + colocacao * 40;
            Color cor = corDaColocacao(colocacao);
            const char *pontos = TextFormat("%d", atual->poder);

            DrawText(TextFormat("%d.", colocacao), 360, y, 26, cor);
            DrawText(atual->nome, 460, y, 26, cor);
            DrawText(pontos, 920 - MeasureText(pontos, 26), y, 26, cor);
            atual = atual->prox;
        }
    }

    desenharBotao(BOTAO_VOLTAR_MENU, "Voltar", true);
}

void desenharHistoria(const Recursos *r, int capitulo) {
    (void) r;
    if (capitulo == 1) {
        desenharTextoCentralizado("Os herois avancam contra o castelo do Destino.", 260, 30, RAYWHITE);
        desenharTextoCentralizado("Distribua os nucleos e organize os Sentinelas.", 310, 30, RAYWHITE);
    } else {
        desenharTextoCentralizado("Os Sentinelas que sobreviveram protegem o Destino.", 260, 30, RAYWHITE);
        desenharTextoCentralizado("Agora ele enfrenta o Loki em um duelo pelo tempo.", 310, 30, RAYWHITE);
    }
    desenharTextoCentralizado("Clique ou aperte ENTER para continuar", 600, 22, GRAY);
}

/* ------------------------------------------------------------------
 * Fase 1
 * ------------------------------------------------------------------ */

#define COR_SENTINELA       LIME
#define COR_HEROI           (Color){ 60, 140, 255, 255 }
#define COR_NUCLEO          (Color){ 255, 200, 40, 255 }
#define PODER_MAXIMO_BARRA  125
#define QUADROS_ATAQUE      4       /* as imagens de ataque têm 4 quadros lado a lado */

/* Barra horizontal de poder acima do sprite, com o número em cima da barra. */
static void desenharBarraPoder(Rectangle sprite, int poder, Color cor) {
    int largura = 150, altura = 14;
    int x = (int)(sprite.x + (sprite.width - largura) / 2);
    int y = (int)sprite.y - 22;
    float proporcao = (float) poder / PODER_MAXIMO_BARRA;
    if (proporcao > 1.0f) proporcao = 1.0f;

    DrawRectangle(x, y, largura, altura, Fade(BLACK, 0.6f));
    DrawRectangle(x, y, (int)(largura * proporcao), altura, cor);
    DrawRectangleLines(x, y, largura, altura, WHITE);

    const char *numero = TextFormat("%d", poder);
    DrawText(numero, (int)(sprite.x + (sprite.width - MeasureText(numero, 22)) / 2), y - 26, 22, WHITE);
}

/* Personagem parado, ou atacando: a tira de ataque toca uma vez durante o golpe. */
static void desenharPersonagem(Texture2D parado, Texture2D ataque, bool atacando,
                               float tempo, Rectangle destino, Color tinta) {
    Rectangle origem = { 0, 0, (float) parado.width, (float) parado.height };
    Texture2D textura = parado;

    if (atacando) {
        float larguraQuadro = (float) ataque.width / QUADROS_ATAQUE;
        int quadro = (int)(tempo / DURACAO_ATAQUE * QUADROS_ATAQUE);
        if (quadro >= QUADROS_ATAQUE) quadro = QUADROS_ATAQUE - 1;
        origem = (Rectangle){ quadro * larguraQuadro, 0, larguraQuadro, (float) ataque.height };
        textura = ataque;
    }
    DrawTexturePro(textura, origem, destino, (Vector2){ 0, 0 }, 0.0f, tinta);
}

/* Uma linha da batalha: Sentinela à esquerda e herói à direita. */
static void desenharLinhaDuelo(const Recursos *r, int i, const char *nomeSentinela,
                               int poderSentinela, int poderHeroi, bool atacando, float tempo,
                               Color tintaSentinela, Color tintaHeroi) {
    Rectangle sentinela = spriteSentinela(i);
    Rectangle heroi = spriteHeroi(i);

    desenharPersonagem(r->sentinela, r->sentinelaAtaque, atacando, tempo, sentinela, tintaSentinela);
    desenharBarraPoder(sentinela, poderSentinela, COR_SENTINELA);
    DrawText(nomeSentinela, 40, (int)sentinela.y + 45, 20, tintaSentinela);

    desenharPersonagem(r->heroi[i], r->heroiAtaque[i], atacando, tempo, heroi, tintaHeroi);
    desenharBarraPoder(heroi, poderHeroi, COR_HEROI);
    DrawText(TextFormat("Heroi %d", i + 1), 1070, (int)heroi.y + 45, 20, tintaHeroi);
}

static void desenharTextoMeio(int i, const char *texto, int tamanho, Color cor) {
    int y = (int)spriteSentinela(i).y + 55 - tamanho / 2;
    DrawText(texto, (GetScreenWidth() - MeasureText(texto, tamanho)) / 2, y, tamanho, cor);
}

static const char *instrucaoFase1(const Fase1 *fase) {
    switch (fase->etapa) {
        case ETAPA_DUELOS:
            return TextFormat("Duelo %d de %d", fase->dueloAtual + 1, QTD_DUELOS);
        case ETAPA_RESULTADO:
            return "";
        case ETAPA_PREPARACAO:
            break;
    }
    if (fase->nucleoSelecionado != -1) {
        return TextFormat("Clique no Sentinela que vai receber o nucleo +%d",
                          fase->nucleos[fase->nucleoSelecionado]);
    }
    if (fase->sentinelaSelecionado != -1) {
        return "Clique em outro Sentinela para trocar de posicao (ou no mesmo para cancelar)";
    }
    if (!podeIniciarDuelos(fase)) {
        return "Clique num nucleo e depois no Sentinela que vai recebe-lo. Clique em dois Sentinelas para troca-los.";
    }
    return "Tudo pronto? Clique em Iniciar duelos.";
}

static void desenharPreparacao(const Fase1 *fase, const Recursos *r) {
    Vector2 mouse = GetMousePosition();
    No *sentinela = fase->sentinelas->inicio;
    No *heroi = fase->herois->inicio;

    for (int i = 0; i < QTD_DUELOS; i++) {
        desenharLinhaDuelo(r, i, sentinela->nome, sentinela->poder, heroi->poder,
                           false, 0.0f, WHITE, WHITE);
        desenharTextoMeio(i, "VS", 30, LIGHTGRAY);

        /* destaque: Sentinela escolhido para troca ou sob o mouse */
        Rectangle area = areaSentinela(i);
        if (fase->sentinelaSelecionado == i) {
            DrawRectangleLinesEx(area, 3, GOLD);
        } else if (CheckCollisionPointRec(mouse, area)) {
            DrawRectangleLinesEx(area, 2, Fade(WHITE, 0.5f));
        }

        sentinela = sentinela->prox;
        heroi = heroi->prox;
    }

    DrawText("Nucleos de energia", 50, 595, 18, WHITE);
    for (int i = 0; i < QTD_NUCLEOS; i++) {
        Rectangle area = areaNucleo(i);
        Vector2 centro = { area.x + area.width / 2, area.y + area.height / 2 };
        const char *texto = TextFormat("+%d", fase->nucleos[i]);
        int x = (int)centro.x - MeasureText(texto, 26) / 2;

        if (fase->nucleoUsado[i]) {
            DrawCircleV(centro, 36, Fade(GRAY, 0.4f));
            DrawText(texto, x, (int)centro.y - 13, 26, GRAY);
        } else {
            DrawCircleV(centro, 36, COR_NUCLEO);
            if (fase->nucleoSelecionado == i) DrawRing(centro, 38, 44, 0, 360, 36, WHITE);
            DrawText(texto, x, (int)centro.y - 13, 26, BLACK);
        }
    }

    desenharBotao(BOTAO_INICIAR_DUELOS, "Iniciar duelos", podeIniciarDuelos(fase));
}

static void desenharDuelos(const Fase1 *fase, const Recursos *r) {
    for (int i = 0; i < QTD_DUELOS; i++) {
        const Duelo *duelo = &fase->duelos[i];
        bool atacando = fase->etapa == ETAPA_DUELOS && i == fase->dueloAtual && !duelo->resolvido;

        /* quem perdeu fica apagado */
        Color tintaSentinela = (duelo->resolvido && !duelo->sentinelaVenceu) ? Fade(WHITE, 0.3f) : WHITE;
        Color tintaHeroi = (duelo->resolvido && duelo->sentinelaVenceu) ? Fade(WHITE, 0.3f) : WHITE;

        desenharLinhaDuelo(r, i, duelo->nomeSentinela, duelo->poderSentinela, duelo->poderHeroi,
                           atacando, fase->tempoDuelo, tintaSentinela, tintaHeroi);

        if (duelo->resolvido) {
            desenharTextoMeio(i, duelo->sentinelaVenceu ? "VITORIA" : "DERROTA", 34,
                              duelo->sentinelaVenceu ? LIME : RED);
        } else {
            desenharTextoMeio(i, "VS", atacando ? 40 : 30, atacando ? GOLD : LIGHTGRAY);
        }
    }
}

static void desenharResultadoFase1(const Fase1 *fase) {
    bool avancou = fase->vitorias >= 2;
    Rectangle painel = { 340, 250, 600, 220 };
    DrawRectangleRec(painel, Fade(BLACK, 0.85f));
    DrawRectangleLinesEx(painel, 3, avancou ? LIME : RED);

    desenharTextoCentralizado(TextFormat("Duelos vencidos: %d de %d", fase->vitorias, QTD_DUELOS),
                              285, 36, WHITE);
    if (avancou) {
        desenharTextoCentralizado("Destino segue para o universo do Loki!", 345, 26, LIME);
    } else {
        desenharTextoCentralizado("Os herois venceram. Destino fica sem protecao...", 345, 26, RED);
    }
    desenharTextoCentralizado("Clique ou aperte ENTER para continuar", 420, 20, GRAY);
}

void desenharFase1(const Fase1 *fase, const Recursos *r) {
    desenharFundo(r->cenarioBatalha);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.35f));

    DrawText("FASE 1 - O EXERCITO DO DESTINO", 20, 15, 28, WHITE);
    DrawText(instrucaoFase1(fase), 20, 52, 20, GOLD);

    if (fase->etapa == ETAPA_PREPARACAO) {
        const char *trocas = TextFormat("Trocas restantes: %d", fase->trocasRestantes);
        DrawText(trocas, GetScreenWidth() - MeasureText(trocas, 24) - 20, 18, 24, WHITE);
        desenharPreparacao(fase, r);
    } else {
        desenharDuelos(fase, r);
        if (fase->etapa == ETAPA_RESULTADO) desenharResultadoFase1(fase);
    }
}

void desenharFase2(const Fase2 *fase, const Recursos *r) {
    desenharFundo(r->cenarioLoki);
    DrawText("FASE 2", 20, 20, 30, WHITE);
    DrawText(TextFormat("Vidas do Destino: %d", fase->vidasDestino), 20, 60, 24, WHITE);
    DrawText(TextFormat("Vida do Loki: %d", fase->vidaLoki), 20, 90, 24, WHITE);
    DrawText(TextFormat("Acoes na pilha: %d", fase->acoes ? fase->acoes->tamanho : 0), 20, 120, 24, WHITE);
    DrawText("(provisorio) A = empilhar ataque   ESPACO = resolver   D = perder vida", 20, 680, 20, YELLOW);
}

/* Faixa com o nome e os pontos da partida, nas telas finais. */
static void desenharPontuacaoFinal(const char *nome, int pontos) {
    DrawRectangle(0, 520, GetScreenWidth(), 60, Fade(BLACK, 0.6f));
    desenharTextoCentralizado(TextFormat("%s: %d pontos", nome, pontos), 535, 32, GOLD);
}

void desenharVitoria(const Recursos *r, const char *nome, int pontos) {
    desenharFundo(r->telaVitoria);
    desenharPontuacaoFinal(nome, pontos);
    desenharBotao(BOTAO_VOLTAR_MENU, "Menu", true);
}

void desenharDerrota(const Recursos *r, const char *nome, int pontos) {
    desenharFundo(r->telaDerrota);
    desenharPontuacaoFinal(nome, pontos);
    desenharBotao(BOTAO_VOLTAR_MENU, "Menu", true);
}
