#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definição da estrutura do Território
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- Funções auxiliares --- //

// Cadastra os territórios
int i;
void cadastrarTerritorios(Territorio* mapa, int qtd) {
    for (i = 0; i < qtd; i++) {
        printf("\nCadastro do território %d: \n", i + 1);

        printf("Digite o nome do território: ");
        scanf(" %[^\n]", mapa[i].nome); // permite espaços

        printf("Digite a cor do exército: ");
        scanf(" %[^\n]", mapa[i].cor);

        printf("Digite o número de tropas: ");
        scanf("%d", &mapa[i].tropas);
    }
}

// Exibe todos os territórios cadastrados
void exibirTerritorios(Territorio* mapa, int qtd) {
    printf("\n--- LISTA DE TERRITÓRIOS ---\n");
    int i;
    for (i = 0; i < qtd; i++) {
        printf("\nTerritório %d:\n", i + 1);
        printf(" Nome: %s\n", mapa[i].nome);
        printf(" Cor: %s\n", mapa[i].cor);
        printf(" Tropas: %d\n", mapa[i].tropas);
    }
}

// Função de ataque entre dois territórios
void atacar(Territorio* atacante, Territorio* defensor) {
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\n[ERRO] Você não pode atacar territórios da mesma cor!\n");
        return;
    }

    if (atacante->tropas < 1) {
        printf("\n[ERRO] O território atacante não tem tropas suficientes!\n");
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

// Libera memória alocada
void liberarMemoria(Territorio* mapa) {
    free(mapa);
    printf("\nMemória liberada!\n");
}

// --- Função principal --- //
int main() {
    srand(time(NULL)); // inicializa aleatoriedade

    int qtdTerritorios;
    printf("Digite o número de territórios: ");
    scanf("%d", &qtdTerritorios);

    // Alocação dinâmica
    Territorio* mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));

    // Cadastro
    cadastrarTerritorios(mapa, qtdTerritorios);
    exibirTerritorios(mapa, qtdTerritorios);

    // Escolher ataque
    int atk, def;
    printf("\nEscolha o número do território atacante: ");
    scanf("%d", &atk);
    printf("Escolha o número do território defensor: ");
    scanf("%d", &def);

    if (atk > 0 && atk <= qtdTerritorios && def > 0 && def <= qtdTerritorios) {
        atacar(&mapa[atk - 1], &mapa[def - 1]);
    } else {
        printf("\n[ERRO] Índices inválidos!\n");
    }

    // Exibir resultado após ataque
    exibirTerritorios(mapa, qtdTerritorios);

    // Libera memória
    liberarMemoria(mapa);
    return 0;
}

