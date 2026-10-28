#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "ranking.h"

/* ------------------------------------------------------------------
 * Pontuação e arquivo
 * ------------------------------------------------------------------ */

int calcularPontuacao(int duelosVencidos, int vidasRestantes, bool derrotouLoki) {
    if (vidasRestantes < 0) vidasRestantes = 0;

    int pontos = duelosVencidos * PONTOS_POR_DUELO + vidasRestantes * PONTOS_POR_VIDA;
    if (derrotouLoki) pontos += PONTOS_VITORIA;
    return pontos;
}

/* Acrescenta a partida no fim do arquivo (cria o arquivo se não existir). */
void salvarPontuacao(const char *nome, int pontos) {
    FILE *arquivo = fopen(ARQUIVO_RANKING, "a");
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel salvar a pontuacao em %s.\n", ARQUIVO_RANKING);
        return;
    }
    fprintf(arquivo, "%d;%s\n", pontos, nome);
    fclose(arquivo);
}

/* Lê todas as partidas e devolve a lista da maior para a menor pontuação.
 * Se o arquivo ainda não existe, devolve uma lista vazia. */
Lista *carregarRanking(void) {
    Lista *ranking = criarLista();
    FILE *arquivo = fopen(ARQUIVO_RANKING, "r");
    if (ranking == NULL || arquivo == NULL) return ranking;

    int pontos;
    char nome[30];
    while (fscanf(arquivo, "%d;%29[^\n]\n", &pontos, nome) == 2) {
        inserirNoFim(ranking, nome, pontos);
    }
    fclose(arquivo);

    /* empates ficam na ordem em que foram jogados (o merge sort é estável) */
    mergeSortLista(ranking);
    return ranking;
}

/* ------------------------------------------------------------------
 * Campo de texto do nome
 * ------------------------------------------------------------------ */

/* O nome precisa ter pelo menos uma letra que não seja espaço. */
bool nomeValido(const char *nome) {
    for (int i = 0; nome[i] != '\0'; i++) {
        if (nome[i] != ' ') return true;
    }
    return false;
}

/* Lê o teclado e edita o nome. Retorna true quando o jogador aperta ENTER
 * com um nome válido. Só aceita letras sem acento, números e símbolos
 * comuns, porque a fonte padrão da raylib não tem acentos. */
bool atualizarCampoNome(char *nome, int tamanhoMax) {
    int tamanho = (int) strlen(nome);

    int letra = GetCharPressed();
    while (letra > 0) {
        bool permitido = letra >= 32 && letra <= 126 && letra != ';';  /* ';' separa os campos no arquivo */
        bool espacoNoInicio = letra == ' ' && tamanho == 0;
        if (permitido && !espacoNoInicio && tamanho < tamanhoMax) {
            nome[tamanho++] = (char) letra;
            nome[tamanho] = '\0';
        }
        letra = GetCharPressed();
    }

    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && tamanho > 0) {
        nome[--tamanho] = '\0';
    }

    return IsKeyPressed(KEY_ENTER) && nomeValido(nome);
}
