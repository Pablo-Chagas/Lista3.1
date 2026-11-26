#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define ARQUIVO_DADOS "consumo.dat"
#define DIAS_MES 30.0

typedef struct {
    char nome[MAX_STR];
    double potenciaKw;
    double horasPorDia;
} Eletrodomestico;

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

double consumoDiarioKwh(const Eletrodomestico *e) {
    return e->potenciaKw * e->horasPorDia;
}

double consumoMensalKwh(const Eletrodomestico *e) {
    return consumoDiarioKwh(e) * DIAS_MES;
}

void exibirEletrodomestico(const Eletrodomestico *e, int indice) {
    double diario = consumoDiarioKwh(e);
    double mensal = consumoMensalKwh(e);

    printf("\n--- Eletrodomestico #%d ---\n", indice + 1);
    printf("Nome:               %s\n", e->nome);
    printf("Potencia:           %.2f kW\n", e->potenciaKw);
    printf("Tempo ativo/dia:    %.2f h\n", e->horasPorDia);
    printf("Consumo diario:     %.4f kWh\n", diario);
    printf("Consumo mensal:     %.4f kWh\n", mensal);
}

/* a) Preencher / cadastrar eletrodomestico */
void cadastrarEletrodomestico(Eletrodomestico **itens, int *qtd) {
    Eletrodomestico *novoArray = realloc(*itens, (size_t)(*qtd + 1) * sizeof(Eletrodomestico));
    if (novoArray == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }
    *itens = novoArray;

    Eletrodomestico *e = &(*itens)[*qtd];
    lerLinha("Nome do eletrodomestico: ", e->nome, MAX_STR);
    e->potenciaKw = lerDouble("Potencia (em kW): ");
    e->horasPorDia = lerDouble("Tempo medio ativo por dia (em horas): ");

    (*qtd)++;
    printf("Eletrodomestico cadastrado com sucesso!\n");
}

/* b) Listar em tela */
void listarEletrodomesticos(const Eletrodomestico *itens, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum eletrodomestico cadastrado no momento.\n");
        return;
    }
    for (int i = 0; i < qtd; i++) {
        exibirEletrodomestico(&itens[i], i);
    }
}

/* b) Salvar em arquivo */
void salvarArquivo(const Eletrodomestico *itens, int qtd, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "wb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo '%s' para escrita.\n", nomeArquivo);
        return;
    }

    if (fwrite(&qtd, sizeof(int), 1, fp) != 1 ||
        (qtd > 0 && fwrite(itens, sizeof(Eletrodomestico), (size_t)qtd, fp) != (size_t)qtd)) {
        printf("Erro ao gravar dados no arquivo.\n");
        fclose(fp);
        return;
    }

    fclose(fp);
    printf("Dados salvos em '%s' (%d item(ns)).\n", nomeArquivo, qtd);
}

/* f) Carregar dados ja gravados */
void carregarArquivo(Eletrodomestico **itens, int *qtd, const char *nomeArquivo) {
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

    Eletrodomestico *novosItens = NULL;
    if (novoQtd > 0) {
        novosItens = malloc((size_t)novoQtd * sizeof(Eletrodomestico));
        if (novosItens == NULL) {
            printf("Erro ao alocar memoria ao carregar arquivo.\n");
            fclose(fp);
            return;
        }
        if (fread(novosItens, sizeof(Eletrodomestico), (size_t)novoQtd, fp) != (size_t)novoQtd) {
            printf("Erro ao ler registros do arquivo.\n");
            free(novosItens);
            fclose(fp);
            return;
        }
    }

    fclose(fp);
    free(*itens);
    *itens = novosItens;
    *qtd = novoQtd;
    printf("Dados carregados de '%s' (%d item(ns)).\n", nomeArquivo, *qtd);
}

/* c) Buscar pelo nome */
void buscarPorNome(const Eletrodomestico *itens, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum eletrodomestico cadastrado.\n");
        return;
    }

    char nomeBuscado[MAX_STR];
    lerLinha("Digite o nome do eletrodomestico: ", nomeBuscado, MAX_STR);

    int encontradas = 0;
    for (int i = 0; i < qtd; i++) {
        if (strcmp(itens[i].nome, nomeBuscado) == 0) {
            exibirEletrodomestico(&itens[i], i);
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("Nenhum eletrodomestico encontrado com o nome '%s'.\n", nomeBuscado);
    }
}

/* d) Buscar eletrodomesticos que gastam mais que um valor X */
void buscarAcimaDeX(const Eletrodomestico *itens, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum eletrodomestico cadastrado.\n");
        return;
    }

    printf("\nDeseja comparar X com o consumo:\n");
    printf("1. Diario (kWh)\n");
    printf("2. Mensal (kWh)\n");
    int tipo = lerInteiro("Escolha (1 ou 2): ");

    double limiteX = lerDouble("Digite o valor X (em kWh): ");
    int encontradas = 0;

    for (int i = 0; i < qtd; i++) {
        double consumo = (tipo == 2) ? consumoMensalKwh(&itens[i]) : consumoDiarioKwh(&itens[i]);
        if (consumo > limiteX) {
            exibirEletrodomestico(&itens[i], i);
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("Nenhum eletrodomestico acima de %.4f kWh encontrado.\n", limiteX);
    }
}

/* e) Calcular e mostrar consumo diario e mensal da casa em kWh e R$ */
void calcularConsumoCasa(const Eletrodomestico *itens, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum eletrodomestico cadastrado para calcular o consumo.\n");
        return;
    }

    double valorKwh = lerDouble("Informe o valor do kWh em R$ (ex.: 0.85): ");
    double totalDiarioKwh = 0.0;
    double totalMensalKwh = 0.0;

    for (int i = 0; i < qtd; i++) {
        totalDiarioKwh += consumoDiarioKwh(&itens[i]);
        totalMensalKwh += consumoMensalKwh(&itens[i]);
    }

    double totalDiarioReais = totalDiarioKwh * valorKwh;
    double totalMensalReais = totalMensalKwh * valorKwh;

    printf("\n========== CONSUMO TOTAL DA CASA ==========\n");
    printf("Consumo diario:  %.4f kWh  |  R$ %.2f\n", totalDiarioKwh, totalDiarioReais);
    printf("Consumo mensal:  %.4f kWh  |  R$ %.2f\n", totalMensalKwh, totalMensalReais);
    printf("===========================================\n");
}

/* f) Menu principal */
void exibirMenu(void) {
    printf("\n========= CONTROLE DE ENERGIA DA CASA =========\n");
    printf("1. Cadastrar eletrodomestico\n");
    printf("2. Listar eletrodomesticos na tela\n");
    printf("3. Salvar dados em arquivo\n");
    printf("4. Carregar dados do arquivo\n");
    printf("5. Buscar eletrodomestico por nome\n");
    printf("6. Buscar eletrodomesticos com consumo maior que X\n");
    printf("7. Calcular consumo diario e mensal da casa (kWh e R$)\n");
    printf("0. Sair\n");
    printf("===============================================\n");
}

int main(void) {
    Eletrodomestico *itens = NULL;
    int qtd = 0;
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarEletrodomestico(&itens, &qtd);
                break;
            case 2:
                listarEletrodomesticos(itens, qtd);
                break;
            case 3:
                salvarArquivo(itens, qtd, ARQUIVO_DADOS);
                break;
            case 4:
                carregarArquivo(&itens, &qtd, ARQUIVO_DADOS);
                break;
            case 5:
                buscarPorNome(itens, qtd);
                break;
            case 6:
                buscarAcimaDeX(itens, qtd);
                break;
            case 7:
                calcularConsumoCasa(itens, qtd);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    free(itens);
    return 0;
}