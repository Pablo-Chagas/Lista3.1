#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STR 50
#define ARQUIVO_DADOS "fazenda.dat"

typedef struct {
    int mes;
    int ano;
} DataNascimento;

typedef struct {
    char codigo[MAX_STR];
    double leite;      /* litros produzidos por semana */
    double alim;       /* kg de alimento ingeridos por semana */
    DataNascimento nasc;
    char abate;        /* 'S' ou 'N' */
} Gado;

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

double lerDouble(const char *prompt) {
    double valor;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &valor) == 1 && valor >= 0.0) {
            limparBuffer();
            return valor;
        }
        printf("Entrada invalida. Digite um numero valido (>= 0).\n");
        limparBuffer();
    }
}

int lerMes(void) {
    int mes;
    while (1) {
        mes = lerInteiro("Mes de nascimento (1 a 12): ");
        if (mes >= 1 && mes <= 12) {
            return mes;
        }
        printf("Mes invalido. Digite um valor entre 1 e 12.\n");
    }
}

int lerAno(void) {
    int ano;
    while (1) {
        ano = lerInteiro("Ano de nascimento (ex.: 2020): ");
        if (ano > 1900 && ano <= 9999) {
            return ano;
        }
        printf("Ano invalido. Digite um ano valido.\n");
    }
}

int calcularIdadeAnos(DataNascimento nasc) {
    time_t agora = time(NULL);
    struct tm *hoje = localtime(&agora);
    int anoAtual = (hoje != NULL) ? (hoje->tm_year + 1900) : 2026;
    int mesAtual = (hoje != NULL) ? (hoje->tm_mon + 1) : 11;

    int idade = anoAtual - nasc.ano;
    if (mesAtual < nasc.mes) {
        idade--;
    }
    return (idade < 0) ? 0 : idade;
}

/* b) Preencher campo abate conforme regras */
void atualizarAbate(Gado *animal) {
    int idade = calcularIdadeAnos(animal->nasc);
    if (idade > 5 || animal->leite < 40.0) {
        animal->abate = 'S';
    } else {
        animal->abate = 'N';
    }
}

void exibirCabecaGado(const Gado *g, int indice) {
    int idade = calcularIdadeAnos(g->nasc);
    printf("\n--- Cabeca #%d ---\n", indice + 1);
    printf("Codigo:         %s\n", g->codigo);
    printf("Leite/semana:   %.2f L\n", g->leite);
    printf("Alim./semana:   %.2f kg\n", g->alim);
    printf("Nascimento:     %02d/%d (%d ano(s))\n", g->nasc.mes, g->nasc.ano, idade);
    printf("Abate:          %c\n", g->abate);
}

/* a) Ler a base de dados com N cabecas de gado */
void lerBaseDados(Gado **rebanho, int *qtd) {
    int n = lerInteiro("Quantas cabecas de gado deseja cadastrar? ");
    if (n <= 0) {
        printf("Quantidade invalida.\n");
        return;
    }

    Gado *novoArray = realloc(*rebanho, (size_t)(*qtd + n) * sizeof(Gado));
    if (novoArray == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }
    *rebanho = novoArray;

    for (int i = 0; i < n; i++) {
        Gado *g = &(*rebanho)[*qtd];
        printf("\nCadastrando cabeca %d de %d:\n", i + 1, n);
        lerLinha("Codigo: ", g->codigo, MAX_STR);
        g->leite = lerDouble("Litros de leite produzidos por semana: ");
        g->alim = lerDouble("Quantidade de alimento por semana (kg): ");
        g->nasc.mes = lerMes();
        g->nasc.ano = lerAno();

        atualizarAbate(g);
        (*qtd)++;
    }

    printf("\n%d cabeca(s) de gado cadastrada(s) e avaliada(s) para abate!\n", n);
}

/* c) Retornar quantidade total de leite produzida por semana */
double totalLeiteSemana(const Gado *rebanho, int qtd) {
    double total = 0.0;
    for (int i = 0; i < qtd; i++) {
        total += rebanho[i].leite;
    }
    return total;
}

/* d) Retornar quantidade total de alimento consumido por semana */
double totalAlimentoSemana(const Gado *rebanho, int qtd) {
    double total = 0.0;
    for (int i = 0; i < qtd; i++) {
        total += rebanho[i].alim;
    }
    return total;
}

/* e) Listar animais que devem ir para o abate */
void listarParaAbate(const Gado *rebanho, int qtd) {
    if (qtd == 0) {
        printf("\nNenhuma cabeca de gado cadastrada.\n");
        return;
    }

    int encontrados = 0;
    printf("\n========== ANIMAIS PARA ABATE ('S') ==========\n");
    for (int i = 0; i < qtd; i++) {
        if (rebanho[i].abate == 'S') {
            exibirCabecaGado(&rebanho[i], i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhuma cabeca de gado marcada para abate no momento.\n");
    }
    printf("==============================================\n");
}

/* f) Salvar dados em arquivo */
void salvarArquivo(const Gado *rebanho, int qtd, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "wb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo '%s' para escrita.\n", nomeArquivo);
        return;
    }

    if (fwrite(&qtd, sizeof(int), 1, fp) != 1 ||
        (qtd > 0 && fwrite(rebanho, sizeof(Gado), (size_t)qtd, fp) != (size_t)qtd)) {
        printf("Erro ao gravar dados no arquivo.\n");
        fclose(fp);
        return;
    }

    fclose(fp);
    printf("Dados salvos em '%s' (%d registro(s)).\n", nomeArquivo, qtd);
}

/* f) Carregar dados do arquivo */
void carregarArquivo(Gado **rebanho, int *qtd, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf("Arquivo '%s' nao encontrado ou nao pode ser aberto.\n", nomeArquivo);
        return;
    }

    int novoQtd = 0;
    if (fread(&novoQtd, sizeof(int), 1, fp) != 1 || novoQtd < 0) {
        printf("Arquivo corrompido ou formato invalido.\n");
        fclose(fp);
        return;
    }

    Gado *novoRebanho = NULL;
    if (novoQtd > 0) {
        novoRebanho = malloc((size_t)novoQtd * sizeof(Gado));
        if (novoRebanho == NULL) {
            printf("Erro ao alocar memoria ao carregar arquivo.\n");
            fclose(fp);
            return;
        }
        if (fread(novoRebanho, sizeof(Gado), (size_t)novoQtd, fp) != (size_t)novoQtd) {
            printf("Erro ao ler registros do arquivo.\n");
            free(novoRebanho);
            fclose(fp);
            return;
        }
    }

    for (int i = 0; i < novoQtd; i++) {
        atualizarAbate(&novoRebanho[i]);
    }

    fclose(fp);
    free(*rebanho);
    *rebanho = novoRebanho;
    *qtd = novoQtd;
    printf("Dados carregados de '%s' (%d registro(s)).\n", nomeArquivo, *qtd);
}

void exibirMenu(void) {
    printf("\n========= CONTROLE DA FAZENDA DE GADO =========\n");
    printf("1. Cadastrar base de dados (N cabecas)\n");
    printf("2. Total de leite produzido por semana\n");
    printf("3. Total de alimento consumido por semana\n");
    printf("4. Listar animais que devem ir para o abate\n");
    printf("5. Salvar dados em arquivo\n");
    printf("6. Carregar dados do arquivo\n");
    printf("0. Sair\n");
    printf("===============================================\n");
}

int main(void) {
    Gado *rebanho = NULL;
    int qtd = 0;
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                lerBaseDados(&rebanho, &qtd);
                break;
            case 2:
                printf("\nTotal de leite produzido por semana: %.2f litros\n",
                       totalLeiteSemana(rebanho, qtd));
                break;
            case 3:
                printf("\nTotal de alimento consumido por semana: %.2f kg\n",
                       totalAlimentoSemana(rebanho, qtd));
                break;
            case 4:
                listarParaAbate(rebanho, qtd);
                break;
            case 5:
                salvarArquivo(rebanho, qtd, ARQUIVO_DADOS);
                break;
            case 6:
                carregarArquivo(&rebanho, &qtd, ARQUIVO_DADOS);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    free(rebanho);
    return 0;
}