#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100

typedef struct {
    char titulo[MAX_STR];
    char autor[MAX_STR];
    int ano;
    char prateleira[MAX_STR];
} Livro;

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

void exibirLivro(const Livro *l, int indice) {
    printf("\n--- Livro #%d ---\n", indice + 1);
    printf("Titulo:     %s\n", l->titulo);
    printf("Autor:      %s\n", l->autor);
    printf("Ano:        %d\n", l->ano);
    printf("Prateleira: %s\n", l->prateleira);
}

/* b) Cadastrar livro */
void cadastrarLivro(Livro **livros, int *qtd) {
    Livro *novoArray = realloc(*livros, (size_t)(*qtd + 1) * sizeof(Livro));
    if (novoArray == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }
    *livros = novoArray;

    Livro *l = &(*livros)[*qtd];
    lerLinha("Titulo: ", l->titulo, MAX_STR);
    lerLinha("Autor: ", l->autor, MAX_STR);
    l->ano = lerInteiro("Ano de publicacao: ");
    lerLinha("Prateleira: ", l->prateleira, MAX_STR);

    (*qtd)++;
    printf("Livro cadastrado com sucesso!\n");
}

/* c) Procurar livro por titulo e mostrar nome e prateleira */
void buscarPorTitulo(const Livro *livros, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum livro cadastrado no momento.\n");
        return;
    }

    char tituloBuscado[MAX_STR];
    lerLinha("Digite o titulo do livro que deseja buscar: ", tituloBuscado, MAX_STR);

    int encontrados = 0;
    for (int i = 0; i < qtd; i++) {
        if (strcmp(livros[i].titulo, tituloBuscado) == 0) {
            printf("\nLivro encontrado!\n");
            printf("Titulo:     %s\n", livros[i].titulo);
            printf("Prateleira: %s\n", livros[i].prateleira);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum livro encontrado com o titulo '%s'.\n", tituloBuscado);
    }
}

/* d) Mostrar dados de todos os livros cadastrados */
void listarLivros(const Livro *livros, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum livro cadastrado no momento.\n");
        return;
    }

    for (int i = 0; i < qtd; i++) {
        exibirLivro(&livros[i], i);
    }
}

/* e) Ler um ano e exibir todos os livros mais novos que o ano lido */
void buscarMaisNovosQueAno(const Livro *livros, int qtd) {
    if (qtd == 0) {
        printf("\nNenhum livro cadastrado no momento.\n");
        return;
    }

    int anoReferencia = lerInteiro("Digite o ano de referencia: ");
    int encontrados = 0;

    for (int i = 0; i < qtd; i++) {
        if (livros[i].ano > anoReferencia) {
            exibirLivro(&livros[i], i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum livro mais novo que o ano %d encontrado.\n", anoReferencia);
    }
}

void exibirMenu(void) {
    printf("\n============== BIBLIOTECA ==============\n");
    printf("1. Cadastrar livro\n");
    printf("2. Buscar livro por titulo\n");
    printf("3. Listar todos os livros cadastrados\n");
    printf("4. Listar livros mais novos que um ano\n");
    printf("0. Sair\n");
    printf("========================================\n");
}

int main(void) {
    Livro *livros = NULL;
    int qtd = 0;
    int opcao;

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarLivro(&livros, &qtd);
                break;
            case 2:
                buscarPorTitulo(livros, qtd);
                break;
            case 3:
                listarLivros(livros, qtd);
                break;
            case 4:
                buscarMaisNovosQueAno(livros, qtd);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    free(livros);
    return 0;
}