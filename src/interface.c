#include "interface.h"

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

static void desenharBotao(Rectangle area, const char *texto) {
    bool mouseEmCima = CheckCollisionPointRec(GetMousePosition(), area);
    DrawRectangleRec(area, mouseEmCima ? DARKGREEN : DARKGRAY);
    DrawRectangleLinesEx(area, 3, WHITE);
    int largura = MeasureText(texto, 28);
    DrawText(texto, (int)(area.x + (area.width - largura) / 2),
             (int)(area.y + (area.height - 28) / 2), 28, WHITE);
}

/* ------------------------------------------------------------------
 * Telas
 * ------------------------------------------------------------------ */

void desenharMenu(const Recursos *r) {
    desenharFundo(r->cenarioBatalha);
    desenharTextoCentralizado("DESTINO", 150, 90, GREEN);
    desenharTextoCentralizado("O Duelo do Tempo", 260, 40, RAYWHITE);
    desenharBotao(BOTAO_JOGAR, "Jogar");
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

void desenharFase1(const Fase1 *fase, const Recursos *r) {
    desenharFundo(r->cenarioBatalha);
    DrawText("FASE 1", 20, 20, 30, WHITE);
    DrawText(TextFormat("Sentinelas: %d", contarNos(fase->sentinelas)), 20, 60, 24, WHITE);
    DrawText("(provisorio) R = perder Sentinela   ENTER = terminar fase", 20, 680, 20, YELLOW);
}

void desenharFase2(const Fase2 *fase, const Recursos *r) {
    desenharFundo(r->cenarioLoki);
    DrawText("FASE 2", 20, 20, 30, WHITE);
    DrawText(TextFormat("Vidas do Destino: %d", fase->vidasDestino), 20, 60, 24, WHITE);
    DrawText(TextFormat("Vida do Loki: %d", fase->vidaLoki), 20, 90, 24, WHITE);
    DrawText(TextFormat("Acoes na pilha: %d", fase->acoes ? fase->acoes->tamanho : 0), 20, 120, 24, WHITE);
    DrawText("(provisorio) A = empilhar ataque   ESPACO = resolver   D = perder vida", 20, 680, 20, YELLOW);
}

void desenharVitoria(const Recursos *r) {
    desenharFundo(r->telaVitoria);
    desenharBotao(BOTAO_VOLTAR_MENU, "Menu");
}

void desenharDerrota(const Recursos *r) {
    desenharFundo(r->telaDerrota);
    desenharBotao(BOTAO_VOLTAR_MENU, "Menu");
}
