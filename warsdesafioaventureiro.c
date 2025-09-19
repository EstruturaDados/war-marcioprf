#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Defini��o da estrutura do Territ�rio
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- Fun��es auxiliares --- //

// Cadastra os territ�rios
int i;
void cadastrarTerritorios(Territorio* mapa, int qtd) {
    for (i = 0; i < qtd; i++) {
        printf("\nCadastro do territ�rio %d: \n", i + 1);

        printf("Digite o nome do territ�rio: ");
        scanf(" %[^\n]", mapa[i].nome); // permite espa�os

        printf("Digite a cor do ex�rcito: ");
        scanf(" %[^\n]", mapa[i].cor);

        printf("Digite o n�mero de tropas: ");
        scanf("%d", &mapa[i].tropas);
    }
}

// Exibe todos os territ�rios cadastrados
void exibirTerritorios(Territorio* mapa, int qtd) {
    printf("\n--- LISTA DE TERRIT�RIOS ---\n");
    int i;
    for (i = 0; i < qtd; i++) {
        printf("\nTerrit�rio %d:\n", i + 1);
        printf(" Nome: %s\n", mapa[i].nome);
        printf(" Cor: %s\n", mapa[i].cor);
        printf(" Tropas: %d\n", mapa[i].tropas);
    }
}

// Fun��o de ataque entre dois territ�rios
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\n[ERRO] Voc� n�o pode atacar territ�rios da mesma cor!\n");
        return;
    }

    if (atacante->tropas < 1) {
        printf("\n[ERRO] O territ�rio atacante n�o tem tropas suficientes!\n");
        return;
    }

    // Rolagem de dados
    int dadoAtacante = (rand() % 6) + 1; // 1 a 6
    int dadoDefensor = (rand() % 6) + 1;

    printf("\nAtaque iniciado!\n");
    printf(" %s (%s) rolou: %d\n", atacante->nome, atacante->cor, dadoAtacante);
    printf(" %s (%s) rolou: %d\n", defensor->nome, defensor->cor, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("\n>>> O atacante venceu a batalha!\n");
        strcpy(defensor->cor, atacante->cor);       // defensor muda de dono
        defensor->tropas = atacante->tropas / 2;    // metade das tropas migram
    } else {
        printf("\n>>> O defensor resistiu ao ataque!\n");
        atacante->tropas -= 1; // atacante perde uma tropa
    }
}

// Libera mem�ria alocada
void liberarMemoria(Territorio* mapa) {
    free(mapa);
    printf("\nMem�ria liberada!\n");
}

// --- Fun��o principal --- //
int main() {
    srand(time(NULL)); // inicializa aleatoriedade

    int qtdTerritorios;
    printf("Digite o n�mero de territ�rios: ");
    scanf("%d", &qtdTerritorios);

    // Aloca��o din�mica
    Territorio* mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));

    // Cadastro
    cadastrarTerritorios(mapa, qtdTerritorios);
    exibirTerritorios(mapa, qtdTerritorios);

    // Escolher ataque
    int atk, def;
    printf("\nEscolha o n�mero do territ�rio atacante: ");
    scanf("%d", &atk);
    printf("Escolha o n�mero do territ�rio defensor: ");
    scanf("%d", &def);

    if (atk > 0 && atk <= qtdTerritorios && def > 0 && def <= qtdTerritorios) {
        atacar(&mapa[atk - 1], &mapa[def - 1]);
    } else {
        printf("\n[ERRO] �ndices inv�lidos!\n");
    }

    // Exibir resultado ap�s ataque
    exibirTerritorios(mapa, qtdTerritorios);

    // Libera mem�ria
    liberarMemoria(mapa);
    return 0;
}

