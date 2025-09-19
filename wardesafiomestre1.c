#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Estrutura do Território --- //
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- Estrutura do Jogador --- //
typedef struct {
    char nome[30];
    char cor[10];    // cor escolhida pelo jogador (ex: "vermelha", "azul")
    char* missao;    // missão alocada dinamicamente (malloc)
} Jogador;

// --- Prototipos --- //
void cadastrarTerritorios(Territorio* mapa, int qtd);
void exibirTerritorios(Territorio* mapa, int qtd);
void atacar(Territorio* atacante, Territorio* defensor);
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes, const char* corJogador);
void exibirMissao(const char* missao);
int verificarMissao(const char* missao, Territorio* mapa, int tamanho);
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int qtdJogadores);

// --- Implementações --- //

// Cadastro de territórios (entrada segura com limites)
void cadastrarTerritorios(Territorio* mapa, int qtd) {
	int i;
    for (i = 0; i < qtd; i++) {
        printf("\nCadastro do território %d: \n", i + 1);

        printf("Digite o nome do território: ");
        scanf(" %29[^\n]", mapa[i].nome); // limite 29 + '\0'

        printf("Digite a cor do exército (ex: vermelha): ");
        scanf(" %9[^\n]", mapa[i].cor); // limite 9 + '\0'

        printf("Digite o número de tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 0) {
            printf("Valor inválido. Digite um número inteiro >= 0: ");
            // limpa buffer até newline
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
        }
    }
}

// Exibe todos os territórios cadastrados
void exibirTerritorios(Territorio* mapa, int qtd) {
    printf("\n--- LISTA DE TERRITÓRIOS ---\n");
    int i;
    for ( i = 0; i < qtd; i++) {
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

    if (atacante->tropas < 2) { // precisa deixar pelo menos 1 tropa
        printf("\n[ERRO] O território atacante não tem tropas suficientes para atacar (necessário >= 2)!\n");
        return;
    }

    // Rolagem de dados
    int dadoAtacante = (rand() % 6) + 1; // 1 a 6
    int dadoDefensor  = (rand() % 6) + 1;

    printf("\nAtaque iniciado!\n");
    printf(" %s (%s) rolou: %d\n", atacante->nome, atacante->cor, dadoAtacante);
    printf(" %s (%s) rolou: %d\n", defensor->nome, defensor->cor, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("\n>>> O atacante venceu a batalha!\n");

        // metade das tropas migram para o defensor; e são subtraídas do atacante
        int tropasMigradas = atacante->tropas / 2; // se 1 aí não ataca, então >=2
        if (tropasMigradas < 1) tropasMigradas = 1;

        // defensor muda de dono (cor) e recebe as tropas migradas
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = tropasMigradas;

        // atacante perde as tropas migradas
        atacante->tropas -= tropasMigradas;

        printf(" %d tropas migraram do atacante para o defensor.\n", tropasMigradas);
    } else {
        printf("\n>>> O defensor resistiu ao ataque!\n");
        // atacante perde uma tropa
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
        printf(" Atacante perdeu 1 tropa.\n");
    }
}

// Atribui uma missão ao jogador: copia e personaliza com a cor do jogador
// destino: ponteiro para char* (aponta para o buffer alocado)
// missoes[]: vetores de templates (alguns com %s para cor)
// totalMissoes: quantidade de templates
// corJogador: cor escolhida pelo jogador para personalizar a missão
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes, const char* corJogador) {
    int sorteio = rand() % totalMissoes;
    const char* template = missoes[sorteio];

    // Se o template contém "%s", colocamos a cor do jogador.
    // Calculamos o tamanho necessário e alocamos.
    char buffer[256];
    if (strstr(template, "%s") != NULL) {
        snprintf(buffer, sizeof(buffer), template, corJogador);
    } else {
        // Caso não tenha placeholder, anexamos a cor no final para referência.
        snprintf(buffer, sizeof(buffer), "%s (cor alvo: %s)", template, corJogador);
    }

    size_t tam = strlen(buffer) + 1;
    *destino = (char*) malloc(tam);
    if (*destino == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória para missão.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(*destino, buffer);
}

// Exibe a missão (passagem por valor: const char*)
void exibirMissao(const char* missao) {
    printf("\n--- MISSÃO ATRIBUÍDA ---\n%s\n", missao);
}

// Verifica se a missão foi cumprida.
// Implementação simples: procura por palavras-chave na missão e executa verificação
// Observação: assumimos que a missão foi personalizada com a cor do jogador (ex: "... vermelha")
int verificarMissao(const char* missao, Territorio* mapa, int tamanho) {
    // Verificação 1: "Conquistar 3 territorios" (interpreta-se como controlar 3 territórios com a cor do jogador)
    if (strstr(missao, "Conquistar 3 territorios") != NULL) {
        // extraímos a cor do jogador no final da string (se existir)
        // vamos procurar a última palavra, assumindo que a cor foi inserida no texto.
        // Exemplo: "Conquistar 3 territorios da cor vermelha"
        char copia[256];
        strncpy(copia, missao, sizeof(copia)-1);
        copia[sizeof(copia)-1] = '\0';

        // procuramos a última palavra (considerando "vermelha", "azul", etc.)
        char* token = strrchr(copia, ' ');
        if (token != NULL) {
            char corAlvo[16];
            strncpy(corAlvo, token+1, sizeof(corAlvo)-1);
            corAlvo[sizeof(corAlvo)-1] = '\0';

            int contador = 0;
            int i;
            for ( i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, corAlvo) == 0) contador++;
            }
            if (contador >= 3) return 1;
        }
        return 0;
    }

    // Verificação 2: "Eliminar todas as tropas da cor X"
    if (strstr(missao, "Eliminar todas as tropas") != NULL) {
        // esperamos algo como "... da cor vermelha" no texto
        char* pos = strstr(missao, "cor");
        if (pos != NULL) {
            // pega a palavra após "cor "
            pos += 3;
            while (*pos == ' ') pos++;
            char corAlvo[16];
            int i = 0;
            while (*pos != '\0' && *pos != ' ' && *pos != '\n' && i < (int)sizeof(corAlvo)-1) {
                corAlvo[i++] = *pos++;
            }
            corAlvo[i] = '\0';
			int j;
            for ( j = 0; j < tamanho; j++) {
                if (strcmp(mapa[j].cor, corAlvo) == 0 && mapa[j].tropas > 0) {
                    return 0; // ainda existe tropa
                }
            }
            return 1; // nenhuma tropa restante da cor alvo
        }
        return 0;
    }

    // Verificação 3: "Controlar 5 territorios simultaneamente" (assumimos cor do jogador no final)
    if (strstr(missao, "Controlar 5 territorios") != NULL) {
        // extrai cor no final (mesma estratégia)
        char copia[256];
        strncpy(copia, missao, sizeof(copia)-1);
        copia[sizeof(copia)-1] = '\0';
        char* token = strrchr(copia, ' ');
        if (token != NULL) {
            char corAlvo[16];
            strncpy(corAlvo, token+1, sizeof(corAlvo)-1);
            corAlvo[sizeof(corAlvo)-1] = '\0';

            int contador = 0;
            int i;
            for ( i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor, corAlvo) == 0) contador++;
            }
            if (contador >= 5) return 1;
        }
        return 0;
    }

    // Verificação 4: missão genérica: "Conquistar o territorio inimigo X" -> interpretamos como
    // verificar se existe ao menos um território com a cor do jogador (simplificação)
    if (strstr(missao, "Conquistar o territorio inimigo") != NULL) {
        // se a missão personalizada tiver "(cor alvo: X)" no final, usamos X
        char* pos = strstr(missao, "cor alvo:");
        if (pos != NULL) {
            pos += strlen("cor alvo:");
            while (*pos == ' ') pos++;
            char corAlvo[16];
            int i = 0;
            while (*pos != '\0' && *pos != ' ' && *pos != '\n' && i < (int)sizeof(corAlvo)-1) {
                corAlvo[i++] = *pos++;
            }
            corAlvo[i] = '\0';
            // verificar se jogador controla ao menos 1 territorio com corAlvo
            int contador = 0;
            int j;
            for ( j = 0; j < tamanho; j++) {
                if (strcmp(mapa[j].cor, corAlvo) == 0) contador++;
            }
            return (contador >= 1) ? 1 : 0;
        }
        return 0;
    }

    // Se não reconheceu a missão, considera não cumprida (pode implementar mais lógicas)
    return 0;
}

// Libera memória alocada para mapa e jogadores (missões)
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int qtdJogadores) {
    if (mapa != NULL) free(mapa);

    if (jogadores != NULL) {
    	int i;
        for ( i = 0; i < qtdJogadores; i++) {
            if (jogadores[i].missao != NULL) {
                free(jogadores[i].missao);
                jogadores[i].missao = NULL;
            }
        }
        free(jogadores);
    }

    printf("\nMemória liberada!\n");
}

// --- Função principal --- //
int main() {
    srand((unsigned int) time(NULL)); // inicializa aleatoriedade

    // Templates de missões (alguns com %s para a cor do jogador)
    const char* missoesPreDefinidas[] = {
        "Conquistar 3 territorios da cor %s",
        "Eliminar todas as tropas da cor %s",
        "Controlar 5 territorios da cor %s",
        "Manter 2 territorios sem perder tropas por 3 turnos (meta simplificada) da cor %s",
        "Conquistar o territorio inimigo (objetivo simples) - cor alvo: %s"
    };
    int totalMissoes = sizeof(missoesPreDefinidas) / sizeof(missoesPreDefinidas[0]);

    int qtdTerritorios;
    printf("Digite o número de territórios: ");
    while (scanf("%d", &qtdTerritorios) != 1 || qtdTerritorios <= 0) {
        printf("Valor inválido. Digite um número inteiro positivo: ");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
    }

    // Aloca mapa dinamicamente
    Territorio* mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória para mapa.\n");
        return EXIT_FAILURE;
    }

    // Cadastro dos territórios
    cadastrarTerritorios(mapa, qtdTerritorios);
    exibirTerritorios(mapa, qtdTerritorios);

    // Quantidade de jogadores
    int qtdJogadores;
    printf("\nDigite o número de jogadores (2 a 4 recomendado): ");
    while (scanf("%d", &qtdJogadores) != 1 || qtdJogadores <= 0) {
        printf("Valor inválido. Digite um número inteiro: ");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
    }

    // Aloca jogadores
    Jogador* jogadores = (Jogador*) calloc(qtdJogadores, sizeof(Jogador));
    if (jogadores == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória para jogadores.\n");
        free(mapa);
        return EXIT_FAILURE;
    }

    // Cadastro de jogadores: nome e cor; atribuição de missão (demonstração)
    int j;
    for ( j = 0; j < qtdJogadores; j++) {
        printf("\n--- Jogador %d ---\n", j + 1);
        printf("Digite o nome do jogador %d: ", j + 1);
        scanf(" %29[^\n]", jogadores[j].nome);

        printf("Digite a cor do jogador %d (ex: vermelha): ", j + 1);
        scanf(" %9[^\n]", jogadores[j].cor);

        // Atribui missão (personalizada com a cor do jogador)
        jogadores[j].missao = NULL;
        atribuirMissao(&jogadores[j].missao, missoesPreDefinidas, totalMissoes, jogadores[j].cor);

        // Exibe missão apenas uma vez (início)
        exibirMissao(jogadores[j].missao);
    }

    // Loop principal de turnos (simples): cada jogador faz uma ação de ataque por turno
    int turno = 0;
    int vencedor = -1;
    // Variável para permitir saída do loop manualmente
    printf("\nIniciando jogo. Digite 0 em qualquer prompt de escolha para encerrar o jogo.\n");

    while (vencedor == -1) {
        int jogadorAtual = turno % qtdJogadores;
        printf("\n--- Turno do jogador %s (cor %s) ---\n", jogadores[jogadorAtual].nome, jogadores[jogadorAtual].cor);
        exibirTerritorios(mapa, qtdTerritorios);

        int atk, def;
        printf("\nEscolha o número do território atacante (0 para sair): ");
        if (scanf("%d", &atk) != 1) break;
        if (atk == 0) break;
        printf("Escolha o número do território defensor (0 para sair): ");
        if (scanf("%d", &def) != 1) break;
        if (def == 0) break;

        // Validação de índices
        if (!(atk > 0 && atk <= qtdTerritorios && def > 0 && def <= qtdTerritorios)) {
            printf("\n[ERRO] Índices inválidos!\n");
        } else {
            Territorio* atacante = &mapa[atk - 1];
            Territorio* defensor = &mapa[def - 1];

            // Validação: o território atacante deve pertencer à cor do jogador atual
            if (strcmp(atacante->cor, jogadores[jogadorAtual].cor) != 0) {
                printf("\n[ERRO] Você só pode atacar com territórios da sua cor (%s).\n", jogadores[jogadorAtual].cor);
            } else {
                // Realiza ataque
                atacar(atacante, defensor);
            }
        }

        // Ao final do turno, verifica se algum jogador cumpriu sua missão
        int j;
        for ( j = 0; j < qtdJogadores; j++) {
            if (verificarMissao(jogadores[j].missao, mapa, qtdTerritorios)) {
                printf("\n>>> %s (cor %s) CUMPRIU A MISSÃO! VITÓRIA! <<<\n", jogadores[j].nome, jogadores[j].cor);
                vencedor = j;
                break;
            }
        }

        // Próximo turno
        turno++;
    }

    if (vencedor == -1) {
        printf("\nJogo encerrado sem vencedor definido (saída manual ou erro de entrada).\n");
    } else {
        printf("\nParabéns ao vencedor: %s (cor %s)!\n", jogadores[vencedor].nome, jogadores[vencedor].cor);
    }

    // Libera memória
    liberarMemoria(mapa, jogadores, qtdJogadores);

    return 0;
}

