#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *file;
    char filename[] = "cidades.csv";
    char line[1024]; // Supondo comprimento máximo da linha de 1024 caracteres
    char *field;
    char ***matrix = NULL; // Matriz para armazenar os dados do csv (*** é um ponteiro para um array de arrays)
    int rows = 0, cols = 0;

    // Abrir o arquivo
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", filename);
        return 1;
    }

    // Ler a linha de cabeçalho separadamente
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Erro ao ler o arquivo\n");
        return 1;
    }

    // Tokenizar a linha de cabeçalho para determinar o número de colunas
    field = strtok(line, ",");
    while (field != NULL) {
        cols++;
        field = strtok(NULL, ",");
    }

    // Redefinir o ponteiro do arquivo para o início do arquivo
    rewind(file);

    // Alocar memória para a matriz
    matrix = malloc(rows * sizeof(char **));

    // Ler o arquivo linha por linha
    while (fgets(line, sizeof(line), file)) {
        // Remover o caractere de nova linha no final
        line[strcspn(line, "\n")] = '\0';
        // Tokenizar a linha
        field = strtok(line, ",");
        int currentCols = 0;
        char **row = malloc(cols * sizeof(char *));

        while (field != NULL) {
            // Alocar memória para o campo e copiar o valor
            row[currentCols] = malloc((strlen(field) + 1) * sizeof(char));
            strcpy(row[currentCols], field);
            currentCols++;
            // Mover para o próximo token
            field = strtok(NULL, ",");
        }

        // Adicionar a linha à matriz
        matrix = realloc(matrix, (rows + 1) * sizeof(char **));
        matrix[rows] = row;
        rows++;
    }

    // Fechar o arquivo
    fclose(file);

    // Imprimir a matriz incluindo o cabeçalho
    printf("Matriz:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%s\t", matrix[i][j]);
            free(matrix[i][j]); // Liberar memória alocada para cada campo
        }
        printf("\n");
        free(matrix[i]); // Liberar memória alocada para cada linha
    }
    free(matrix); // Liberar memória alocada para a própria matriz

    return 0;
}
