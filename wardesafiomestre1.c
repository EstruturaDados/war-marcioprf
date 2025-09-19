#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Estrutura do Territ�rio --- //
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- Estrutura do Jogador --- //
typedef struct {
    char nome[30];
    char cor[10];    // cor escolhida pelo jogador (ex: "vermelha", "azul")
    char* missao;    // miss�o alocada dinamicamente (malloc)
} Jogador;

// --- Prototipos --- //
void cadastrarTerritorios(Territorio* mapa, int qtd);
void exibirTerritorios(Territorio* mapa, int qtd);
void atacar(Territorio* atacante, Territorio* defensor);
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes, const char* corJogador);
void exibirMissao(const char* missao);
int verificarMissao(const char* missao, Territorio* mapa, int tamanho);
void liberarMemoria(Territorio* mapa, Jogador* jogadores, int qtdJogadores);

// --- Implementa��es --- //

// Cadastro de territ�rios (entrada segura com limites)
void cadastrarTerritorios(Territorio* mapa, int qtd) {
	int i;
    for (i = 0; i < qtd; i++) {
        printf("\nCadastro do territ�rio %d: \n", i + 1);

        printf("Digite o nome do territ�rio: ");
        scanf(" %29[^\n]", mapa[i].nome); // limite 29 + '\0'

        printf("Digite a cor do ex�rcito (ex: vermelha): ");
        scanf(" %9[^\n]", mapa[i].cor); // limite 9 + '\0'

        printf("Digite o n�mero de tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 0) {
            printf("Valor inv�lido. Digite um n�mero inteiro >= 0: ");
            // limpa buffer at� newline
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
        }
    }
}

// Exibe todos os territ�rios cadastrados
void exibirTerritorios(Territorio* mapa, int qtd) {
    printf("\n--- LISTA DE TERRIT�RIOS ---\n");
    int i;
    for ( i = 0; i < qtd; i++) {
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

    if (atacante->tropas < 2) { // precisa deixar pelo menos 1 tropa
        printf("\n[ERRO] O territ�rio atacante n�o tem tropas suficientes para atacar (necess�rio >= 2)!\n");
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

        // metade das tropas migram para o defensor; e s�o subtra�das do atacante
        int tropasMigradas = atacante->tropas / 2; // se 1 a� n�o ataca, ent�o >=2
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

// Atribui uma miss�o ao jogador: copia e personaliza com a cor do jogador
// destino: ponteiro para char* (aponta para o buffer alocado)
// missoes[]: vetores de templates (alguns com %s para cor)
// totalMissoes: quantidade de templates
// corJogador: cor escolhida pelo jogador para personalizar a miss�o
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes, const char* corJogador) {
    int sorteio = rand() % totalMissoes;
    const char* template = missoes[sorteio];

    // Se o template cont�m "%s", colocamos a cor do jogador.
    // Calculamos o tamanho necess�rio e alocamos.
    char buffer[256];
    if (strstr(template, "%s") != NULL) {
        snprintf(buffer, sizeof(buffer), template, corJogador);
    } else {
        // Caso n�o tenha placeholder, anexamos a cor no final para refer�ncia.
        snprintf(buffer, sizeof(buffer), "%s (cor alvo: %s)", template, corJogador);
    }

    size_t tam = strlen(buffer) + 1;
    *destino = (char*) malloc(tam);
    if (*destino == NULL) {
        fprintf(stderr, "Erro: falha ao alocar mem�ria para miss�o.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(*destino, buffer);
}

// Exibe a miss�o (passagem por valor: const char*)
void exibirMissao(const char* missao) {
    printf("\n--- MISS�O ATRIBU�DA ---\n%s\n", missao);
}

// Verifica se a miss�o foi cumprida.
// Implementa��o simples: procura por palavras-chave na miss�o e executa verifica��o
// Observa��o: assumimos que a miss�o foi personalizada com a cor do jogador (ex: "... vermelha")
int verificarMissao(const char* missao, Territorio* mapa, int tamanho) {
    // Verifica��o 1: "Conquistar 3 territorios" (interpreta-se como controlar 3 territ�rios com a cor do jogador)
    if (strstr(missao, "Conquistar 3 territorios") != NULL) {
        // extra�mos a cor do jogador no final da string (se existir)
        // vamos procurar a �ltima palavra, assumindo que a cor foi inserida no texto.
        // Exemplo: "Conquistar 3 territorios da cor vermelha"
        char copia[256];
        strncpy(copia, missao, sizeof(copia)-1);
        copia[sizeof(copia)-1] = '\0';

        // procuramos a �ltima palavra (considerando "vermelha", "azul", etc.)
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

    // Verifica��o 2: "Eliminar todas as tropas da cor X"
    if (strstr(missao, "Eliminar todas as tropas") != NULL) {
        // esperamos algo como "... da cor vermelha" no texto
        char* pos = strstr(missao, "cor");
        if (pos != NULL) {
            // pega a palavra ap�s "cor "
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

    // Verifica��o 3: "Controlar 5 territorios simultaneamente" (assumimos cor do jogador no final)
    if (strstr(missao, "Controlar 5 territorios") != NULL) {
        // extrai cor no final (mesma estrat�gia)
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

    // Verifica��o 4: miss�o gen�rica: "Conquistar o territorio inimigo X" -> interpretamos como
    // verificar se existe ao menos um territ�rio com a cor do jogador (simplifica��o)
    if (strstr(missao, "Conquistar o territorio inimigo") != NULL) {
        // se a miss�o personalizada tiver "(cor alvo: X)" no final, usamos X
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

    // Se n�o reconheceu a miss�o, considera n�o cumprida (pode implementar mais l�gicas)
    return 0;
}

// Libera mem�ria alocada para mapa e jogadores (miss�es)
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

    printf("\nMem�ria liberada!\n");
}

// --- Fun��o principal --- //
int main() {
    srand((unsigned int) time(NULL)); // inicializa aleatoriedade

    // Templates de miss�es (alguns com %s para a cor do jogador)
    const char* missoesPreDefinidas[] = {
        "Conquistar 3 territorios da cor %s",
        "Eliminar todas as tropas da cor %s",
        "Controlar 5 territorios da cor %s",
        "Manter 2 territorios sem perder tropas por 3 turnos (meta simplificada) da cor %s",
        "Conquistar o territorio inimigo (objetivo simples) - cor alvo: %s"
    };
    int totalMissoes = sizeof(missoesPreDefinidas) / sizeof(missoesPreDefinidas[0]);

    int qtdTerritorios;
    printf("Digite o n�mero de territ�rios: ");
    while (scanf("%d", &qtdTerritorios) != 1 || qtdTerritorios <= 0) {
        printf("Valor inv�lido. Digite um n�mero inteiro positivo: ");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
    }

    // Aloca mapa dinamicamente
    Territorio* mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha ao alocar mem�ria para mapa.\n");
        return EXIT_FAILURE;
    }

    // Cadastro dos territ�rios
    cadastrarTerritorios(mapa, qtdTerritorios);
    exibirTerritorios(mapa, qtdTerritorios);

    // Quantidade de jogadores
    int qtdJogadores;
    printf("\nDigite o n�mero de jogadores (2 a 4 recomendado): ");
    while (scanf("%d", &qtdJogadores) != 1 || qtdJogadores <= 0) {
        printf("Valor inv�lido. Digite um n�mero inteiro: ");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
    }

    // Aloca jogadores
    Jogador* jogadores = (Jogador*) calloc(qtdJogadores, sizeof(Jogador));
    if (jogadores == NULL) {
        fprintf(stderr, "Erro: falha ao alocar mem�ria para jogadores.\n");
        free(mapa);
        return EXIT_FAILURE;
    }

    // Cadastro de jogadores: nome e cor; atribui��o de miss�o (demonstra��o)
    int j;
    for ( j = 0; j < qtdJogadores; j++) {
        printf("\n--- Jogador %d ---\n", j + 1);
        printf("Digite o nome do jogador %d: ", j + 1);
        scanf(" %29[^\n]", jogadores[j].nome);

        printf("Digite a cor do jogador %d (ex: vermelha): ", j + 1);
        scanf(" %9[^\n]", jogadores[j].cor);

        // Atribui miss�o (personalizada com a cor do jogador)
        jogadores[j].missao = NULL;
        atribuirMissao(&jogadores[j].missao, missoesPreDefinidas, totalMissoes, jogadores[j].cor);

        // Exibe miss�o apenas uma vez (in�cio)
        exibirMissao(jogadores[j].missao);
    }

    // Loop principal de turnos (simples): cada jogador faz uma a��o de ataque por turno
    int turno = 0;
    int vencedor = -1;
    // Vari�vel para permitir sa�da do loop manualmente
    printf("\nIniciando jogo. Digite 0 em qualquer prompt de escolha para encerrar o jogo.\n");

    while (vencedor == -1) {
        int jogadorAtual = turno % qtdJogadores;
        printf("\n--- Turno do jogador %s (cor %s) ---\n", jogadores[jogadorAtual].nome, jogadores[jogadorAtual].cor);
        exibirTerritorios(mapa, qtdTerritorios);

        int atk, def;
        printf("\nEscolha o n�mero do territ�rio atacante (0 para sair): ");
        if (scanf("%d", &atk) != 1) break;
        if (atk == 0) break;
        printf("Escolha o n�mero do territ�rio defensor (0 para sair): ");
        if (scanf("%d", &def) != 1) break;
        if (def == 0) break;

        // Valida��o de �ndices
        if (!(atk > 0 && atk <= qtdTerritorios && def > 0 && def <= qtdTerritorios)) {
            printf("\n[ERRO] �ndices inv�lidos!\n");
        } else {
            Territorio* atacante = &mapa[atk - 1];
            Territorio* defensor = &mapa[def - 1];

            // Valida��o: o territ�rio atacante deve pertencer � cor do jogador atual
            if (strcmp(atacante->cor, jogadores[jogadorAtual].cor) != 0) {
                printf("\n[ERRO] Voc� s� pode atacar com territ�rios da sua cor (%s).\n", jogadores[jogadorAtual].cor);
            } else {
                // Realiza ataque
                atacar(atacante, defensor);
            }
        }

        // Ao final do turno, verifica se algum jogador cumpriu sua miss�o
        int j;
        for ( j = 0; j < qtdJogadores; j++) {
            if (verificarMissao(jogadores[j].missao, mapa, qtdTerritorios)) {
                printf("\n>>> %s (cor %s) CUMPRIU A MISS�O! VIT�RIA! <<<\n", jogadores[j].nome, jogadores[j].cor);
                vencedor = j;
                break;
            }
        }

        // Pr�ximo turno
        turno++;
    }

    if (vencedor == -1) {
        printf("\nJogo encerrado sem vencedor definido (sa�da manual ou erro de entrada).\n");
    } else {
        printf("\nParab�ns ao vencedor: %s (cor %s)!\n", jogadores[vencedor].nome, jogadores[vencedor].cor);
    }

    // Libera mem�ria
    liberarMemoria(mapa, jogadores, qtdJogadores);

    return 0;
}

