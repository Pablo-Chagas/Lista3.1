#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_TITULO 30
#define MAX_CONSOLE 15
#define MAX_STR 100
#define TAM_DATA 11

typedef struct {
    char data[TAM_DATA];
    char nomePessoa[MAX_STR];
    char emprestado; /* 'S' ou 'N' */
} Emprestimo;

typedef struct {
    char titulo[MAX_TITULO + 1];
    char console[MAX_CONSOLE + 1];
    int ano;
    int ranking;
    Emprestimo emprestimo;
} Jogo;

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void lerLinha(const char *prompt, char *destino, size_t tamanho) {
    printf("%s", prompt);
    if (fgets(destino, (int)tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            limparBuffer();
        }
    }
}

int lerInteiro(const char *prompt) {
    int valor;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &valor) == 1) {
            limparBuffer();
            return valor;
        }
        printf("Entrada invalida. Digite um numero inteiro.\n");
        limparBuffer();
    }
}

void obterDataAtual(char *destino, size_t tamanho) {
    time_t agora = time(NULL);
    struct tm *infoTempo = localtime(&agora);
    if (infoTempo != NULL) {
        strftime(destino, tamanho, "%d/%m/%Y", infoTempo);
    } else {
        snprintf(destino, tamanho, "00/00/0000");
    }
}

void exibirJogo(const Jogo *j, int indice) {
    printf("\n--- Jogo #%d ---\n", indice + 1);
    printf("Titulo:     %s\n", j->titulo);
    printf("Console:    %s\n", j->console);
    printf("Ano:        %d\n", j->ano);
    printf("Ranking:    #%d\n", j->ranking);
    printf("Emprestado: %c\n", j->emprestimo.emprestado);
    if (j->emprestimo.emprestado == 'S') {
        printf("Pessoa:     %s\n", j->emprestimo.nomePessoa);
        printf("Data:       %s\n", j->emprestimo.data);
    }
}

int buscarIndicePorTitulo(const Jogo *jogos, int qtd, const char *tituloBuscado) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(jogos[i].titulo, tituloBuscado) == 0) {
            return i;
        }
    }
    return -1;
}

/* Cadastrar n jogos */
void cadastrarJogos(Jogo **jogos, int *qtd) {
    int n = lerInteiro("Quantos jogos deseja cadastrar? ");
    if (n <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    Jogo *novoArray = realloc(*jogos, (size_t)(*qtd + n) * sizeof(Jogo));
    if (novoArray == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }
    *jogos = novoArray;

    for (int k = 0; k < n; k++) {
        Jogo *j = &(*jogos)[*qtd];
        printf("\nCadastrando jogo %d de %d:\n", k + 1, n);
        lerLinha("Titulo (ate 30 caracteres): ", j->titulo, sizeof(j->titulo));
        lerLinha("Console (ate 15 caracteres): ", j->console, sizeof(j->console));
        j->ano = lerInteiro("Ano de lancamento: ");
        j->ranking = lerInteiro("Posicao no ranking: ");

        j->emprestimo.emprestado = 'N';
        j->emprestimo.nomePessoa[0] = '\0';
        j->emprestimo.data[0] = '\0';

        (*qtd)++;
    }

    printf("\n%d jogo(s) cadastrado(s) com sucesso!\n", n);
}

/* a) Procurar por titulo ou listar todos os jogos de um console */
void buscarPorTitulo(const Jogo *jogos, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum jogo cadastrado.\n");
        return;
    }

    char tituloBuscado[MAX_TITULO + 1];
    lerLinha("Digite o titulo do jogo: ", tituloBuscado, sizeof(tituloBuscado));

    int idx = buscarIndicePorTitulo(jogos, qtd, tituloBuscado);
    if (idx != -1) {
        exibirJogo(&jogos[idx], idx);
    } else {
        printf("Jogo '%s' nao encontrado.\n", tituloBuscado);
    }
}

void listarPorConsole(const Jogo *jogos, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum jogo cadastrado.\n");
        return;
    }

    char consoleBuscado[MAX_CONSOLE + 1];
    lerLinha("Digite o nome do console: ", consoleBuscado, sizeof(consoleBuscado));

    int encontrados = 0;
    for (int i = 0; i < qtd; i++) {
        if (strcmp(jogos[i].console, consoleBuscado) == 0) {
            exibirJogo(&jogos[i], i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum jogo encontrado para o console '%s'.\n", consoleBuscado);
    }
}

/* b) Realizar emprestimo (data atual, nomePessoa, emprestado = 'S') */
void realizarEmprestimo(Jogo *jogos, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum jogo cadastrado.\n");
        return;
    }

    char tituloBuscado[MAX_TITULO + 1];
    lerLinha("Digite o titulo do jogo para emprestar: ", tituloBuscado, sizeof(tituloBuscado));

    int idx = buscarIndicePorTitulo(jogos, qtd, tituloBuscado);
    if (idx == -1) {
        printf("Jogo '%s' nao encontrado.\n", tituloBuscado);
        return;
    }

    if (jogos[idx].emprestimo.emprestado == 'S') {
        printf("Este jogo ja esta emprestado para %s desde %s.\n",
               jogos[idx].emprestimo.nomePessoa,
               jogos[idx].emprestimo.data);
        return;
    }

    char dataAtual[TAM_DATA];
    obterDataAtual(dataAtual, sizeof(dataAtual));

    lerLinha("Nome da pessoa que pegou o jogo: ", jogos[idx].emprestimo.nomePessoa, MAX_STR);
    snprintf(jogos[idx].emprestimo.data, sizeof(jogos[idx].emprestimo.data), "%s", dataAtual);
    jogos[idx].emprestimo.emprestado = 'S';

    printf("Emprestimo registrado com sucesso! Data: %s | Pessoa: %s\n",
           jogos[idx].emprestimo.data,
           jogos[idx].emprestimo.nomePessoa);
}

/* c) Devolver jogo (emprestado = 'N') */
void devolverJogo(Jogo *jogos, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum jogo cadastrado.\n");
        return;
    }

    char tituloBuscado[MAX_TITULO + 1];
    lerLinha("Digite o titulo do jogo a ser devolvido: ", tituloBuscado, sizeof(tituloBuscado));

    int idx = buscarIndicePorTitulo(jogos, qtd, tituloBuscado);
    if (idx == -1) {
        printf("Jogo '%s' nao encontrado.\n", tituloBuscado);
        return;
    }

    if (jogos[idx].emprestimo.emprestado == 'N') {
        printf("O jogo '%s' ja consta como disponivel (N).\n", jogos[idx].titulo);
        return;
    }

    jogos[idx].emprestimo.emprestado = 'N';
    jogos[idx].emprestimo.nomePessoa[0] = '\0';
    jogos[idx].emprestimo.data[0] = '\0';

    printf("Devolucao concluida! O jogo '%s' agora esta disponivel (N).\n", jogos[idx].titulo);
}

/* d) Mostrar todos os jogos emprestados e para quem */
void listarEmprestados(const Jogo *jogos, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum jogo cadastrado.\n");
        return;
    }

    int encontrados = 0;
    printf("\n=========== JOGOS EMPRESTADOS ===========\n");
    for (int i = 0; i < qtd; i++) {
        if (jogos[i].emprestimo.emprestado == 'S') {
            printf("- Titulo: %s | Console: %s | Pessoa: %s | Data: %s\n",
                   jogos[i].titulo,
                   jogos[i].console,
                   jogos[i].emprestimo.nomePessoa,
                   jogos[i].emprestimo.data);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum jogo esta emprestado no momento.\n");
    }
    printf("=========================================\n");
}

void exibirMenu(void) {
    printf("\n========= CATALOGO E CONTROLE DE JOGOS =========\n");
    printf("1. Cadastrar n jogos\n");
    printf("2. Procurar jogo por titulo\n");
    printf("3. Listar todos os jogos de um console\n");
    printf("4. Realizar emprestimo de um jogo\n");
    printf("5. Devolver jogo\n");
    printf("6. Mostrar todos os jogos emprestados e para quem\n");
    printf("0. Sair\n");
    printf("================================================\n");
}

int main(void) {
    Jogo *jogos = NULL;
    int qtd = 0;
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarJogos(&jogos, &qtd);
                break;
            case 2:
                buscarPorTitulo(jogos, qtd);
                break;
            case 3:
                listarPorConsole(jogos, qtd);
                break;
            case 4:
                realizarEmprestimo(jogos, qtd);
                break;
            case 5:
                devolverJogo(jogos, qtd);
                break;
            case 6:
                listarEmprestados(jogos, qtd);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    free(jogos);
    return 0;
}