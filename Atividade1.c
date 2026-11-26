#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define ARQUIVO_DADOS "bandas.dat"

typedef struct {
    char nome[MAX_STR];
    char genero[MAX_STR];
    int integrantes;
    int ranking;
} Banda;

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

void exibirBanda(const Banda *b, int indice) {
    printf("\n--- Banda #%d ---\n", indice + 1);
    printf("Nome:        %s\n", b->nome);
    printf("Genero:      %s\n", b->genero);
    printf("Integrantes: %d\n", b->integrantes);
    printf("Ranking:     #%d\n", b->ranking);
}

/* a) Cadastrar uma nova banda */
void cadastrarBanda(Banda **bandas, int *qtd) {
    Banda *novoArray = realloc(*bandas, (size_t)(*qtd + 1) * sizeof(Banda));
    if (novoArray == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }
    *bandas = novoArray;

    Banda *b = &(*bandas)[*qtd];
    lerLinha("Nome da banda: ", b->nome, MAX_STR);
    lerLinha("Tipo/genero de musica: ", b->genero, MAX_STR);
    b->integrantes = lerInteiro("Numero de integrantes: ");
    b->ranking = lerInteiro("Posicao no ranking de favoritas: ");

    (*qtd)++;
    printf("Banda cadastrada com sucesso!\n");
}

/* b) Listar todas as bandas cadastradas */
void listarBandas(const Banda *bandas, int qtd) {
    if (qtd == 0) {
        printf("\nNenhuma banda cadastrada no momento.\n");
        return;
    }
    for (int i = 0; i < qtd; i++) {
        exibirBanda(&bandas[i], i);
    }
}

/* c) Salvar dados em arquivo */
void salvarArquivo(const Banda *bandas, int qtd, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "wb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo '%s' para escrita.\n", nomeArquivo);
        return;
    }

    if (fwrite(&qtd, sizeof(int), 1, fp) != 1 ||
        (qtd > 0 && fwrite(bandas, sizeof(Banda), (size_t)qtd, fp) != (size_t)qtd)) {
        printf("Erro ao gravar os dados no arquivo.\n");
        fclose(fp);
        return;
    }

    fclose(fp);
    printf("Dados salvos em '%s' (%d banda(s)).\n", nomeArquivo, qtd);
}

/* c) Carregar dados de um arquivo */
void carregarArquivo(Banda **bandas, int *qtd, const char *nomeArquivo) {
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

    Banda *novasBandas = NULL;
    if (novoQtd > 0) {
        novasBandas = malloc((size_t)novoQtd * sizeof(Banda));
        if (novasBandas == NULL) {
            printf("Erro ao alocar memoria ao carregar arquivo.\n");
            fclose(fp);
            return;
        }
        if (fread(novasBandas, sizeof(Banda), (size_t)novoQtd, fp) != (size_t)novoQtd) {
            printf("Erro ao ler registros do arquivo.\n");
            free(novasBandas);
            fclose(fp);
            return;
        }
    }

    fclose(fp);
    free(*bandas);
    *bandas = novasBandas;
    *qtd = novoQtd;
    printf("Dados carregados de '%s' (%d banda(s)).\n", nomeArquivo, *qtd);
}

/* d) Exibir bandas pela posicao solicitada no ranking */
void buscarPorRanking(const Banda *bandas, int qtd) {
    int pos = lerInteiro("Digite a posicao do ranking desejada: ");
    int encontradas = 0;

    for (int i = 0; i < qtd; i++) {
        if (bandas[i].ranking == pos) {
            exibirBanda(&bandas[i], i);
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("Nenhuma banda encontrada na posicao #%d do ranking.\n", pos);
    }
}

/* e) Exibir bandas de um genero musical */
void buscarPorGenero(const Banda *bandas, int qtd) {
    char generoBuscado[MAX_STR];
    lerLinha("Digite o genero musical: ", generoBuscado, MAX_STR);

    int encontradas = 0;
    for (int i = 0; i < qtd; i++) {
        if (strcmp(bandas[i].genero, generoBuscado) == 0) {
            exibirBanda(&bandas[i], i);
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("Nenhuma banda encontrada com o genero '%s'.\n", generoBuscado);
    }
}

/* f) Buscar banda pelo nome e exibir todos os dados */
int buscarIndicePorNome(const Banda *bandas, int qtd, const char *nomeBuscado) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(bandas[i].nome, nomeBuscado) == 0) {
            return i;
        }
    }
    return -1;
}

void buscarPorNome(const Banda *bandas, int qtd) {
    char nomeBuscado[MAX_STR];
    lerLinha("Digite o nome da banda: ", nomeBuscado, MAX_STR);

    int idx = buscarIndicePorNome(bandas, qtd, nomeBuscado);
    if (idx != -1) {
        exibirBanda(&bandas[idx], idx);
    } else {
        printf("Banda '%s' nao encontrada.\n", nomeBuscado);
    }
}

/* g) Excluir uma banda */
void excluirBanda(Banda **bandas, int *qtd) {
    if (*qtd == 0) {
        printf("Nenhuma banda cadastrada para excluir.\n");
        return;
    }

    char nomeBuscado[MAX_STR];
    lerLinha("Digite o nome da banda a ser excluida: ", nomeBuscado, MAX_STR);

    int idx = buscarIndicePorNome(*bandas, *qtd, nomeBuscado);
    if (idx == -1) {
        printf("Banda '%s' nao encontrada.\n", nomeBuscado);
        return;
    }

    for (int i = idx; i < *qtd - 1; i++) {
        (*bandas)[i] = (*bandas)[i + 1];
    }

    (*qtd)--;
    if (*qtd == 0) {
        free(*bandas);
        *bandas = NULL;
    } else {
        Banda *reduzido = realloc(*bandas, (size_t)(*qtd) * sizeof(Banda));
        if (reduzido != NULL) {
            *bandas = reduzido;
        }
    }

    printf("Banda '%s' removida com sucesso.\n", nomeBuscado);
}

/* h) Alterar dados de uma banda (ler novamente todos os campos apos encontrar) */
void alterarBanda(Banda *bandas, int qtd) {
    if (qtd == 0) {
        printf("Nenhuma banda cadastrada para alterar.\n");
        return;
    }

    char nomeBuscado[MAX_STR];
    lerLinha("Digite o nome da banda que deseja alterar: ", nomeBuscado, MAX_STR);

    int idx = buscarIndicePorNome(bandas, qtd, nomeBuscado);
    if (idx == -1) {
        printf("Banda '%s' nao encontrada.\n", nomeBuscado);
        return;
    }

    printf("Banda encontrada! Informe novamente todos os dados:\n");
    lerLinha("Novo nome da banda: ", bandas[idx].nome, MAX_STR);
    lerLinha("Novo tipo/genero de musica: ", bandas[idx].genero, MAX_STR);
    bandas[idx].integrantes = lerInteiro("Novo numero de integrantes: ");
    bandas[idx].ranking = lerInteiro("Nova posicao no ranking: ");

    printf("Dados da banda atualizados com sucesso!\n");
}

/* i) Menu principal */
void exibirMenu(void) {
    printf("\n================ MENU - BANDAS ================\n");
    printf("1. Cadastrar banda\n");
    printf("2. Listar todas as bandas\n");
    printf("3. Salvar dados em arquivo\n");
    printf("4. Carregar dados do arquivo\n");
    printf("5. Buscar banda por posicao no ranking\n");
    printf("6. Buscar bandas por genero musical\n");
    printf("7. Buscar banda por nome\n");
    printf("8. Excluir banda\n");
    printf("9. Alterar dados de uma banda\n");
    printf("0. Sair\n");
    printf("===============================================\n");
}

int main(void) {
    Banda *bandas = NULL;
    int qtd = 0;
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarBanda(&bandas, &qtd);
                break;
            case 2:
                listarBandas(bandas, qtd);
                break;
            case 3:
                salvarArquivo(bandas, qtd, ARQUIVO_DADOS);
                break;
            case 4:
                carregarArquivo(&bandas, &qtd, ARQUIVO_DADOS);
                break;
            case 5:
                buscarPorRanking(bandas, qtd);
                break;
            case 6:
                buscarPorGenero(bandas, qtd);
                break;
            case 7:
                buscarPorNome(bandas, qtd);
                break;
            case 8:
                excluirBanda(&bandas, &qtd);
                break;
            case 9:
                alterarBanda(bandas, qtd);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    free(bandas);
    return 0;
}