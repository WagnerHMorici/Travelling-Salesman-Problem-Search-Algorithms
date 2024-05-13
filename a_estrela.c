#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <locale.h>

#define MAX_CITIES 15
#define MAX_LINE_LENGTH 1024

// Estrutura para representar uma cidade
typedef struct {
    int x;
    int y;
} City;

// Função para calcular a distância entre duas cidades
int distance(City city1, City city2) {
    return abs(city1.x - city2.x) + abs(city1.y - city2.y);
}

// Função para calcular o custo total de um percurso
int calculate_cost(int *path, int num_cities, int cost_table[][MAX_CITIES]) {
    int cost = 0;
    for (int i = 0; i < num_cities - 1; i++) {
        cost += cost_table[path[i]][path[i + 1]];
    }
    cost += cost_table[path[num_cities - 1]][path[0]]; // Voltando para a cidade inicial
    return cost;
}

// Função para trocar duas cidades em um percurso
void swap_cities(int *path, int i, int j) {
    int temp = path[i];
    path[i] = path[j];
    path[j] = temp;
}

// Função para encontrar o próximo permutação lexicograficamente
int next_permutation(int *array, int size) {
    int i = size - 1;
    while (i > 0 && array[i - 1] >= array[i]) {
        i--;
    }

    if (i <= 0) {
        return 0;
    }

    int j = size - 1;
    while (array[j] <= array[i - 1]) {
        j--;
    }

    int temp = array[i - 1];
    array[i - 1] = array[j];
    array[j] = temp;

    j = size - 1;
    while (i < j) {
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
        i++;
        j--;
    }

    return 1;
}

// Função para encontrar o percurso ótimo pelo método de força bruta
void find_optimal_path(int num_cities, int cost_table[][MAX_CITIES]) {
    int cities[num_cities]; // Array para armazenar as cidades
    for (int i = 0; i < num_cities; i++) {
        cities[i] = i; // Inicializando o array de cidades
    }

    int optimal_path[num_cities]; // Array para armazenar o percurso ótimo
    int min_cost = INT_MAX; // Inicializando o custo mínimo com um valor grande

    // Gerando todas as permutações possíveis dos caminhos
    do {
        int cost = calculate_cost(cities, num_cities, cost_table); // Calculando o custo do percurso atual
        printf("Percurso: ");
        for (int i = 0; i < num_cities; i++) {
            printf("%d ", cities[i]);
        }
        printf("\nCusto: %d\n", cost);

        if (cost < min_cost) {
            min_cost = cost; // Atualizando o custo mínimo
            memcpy(optimal_path, cities, sizeof(cities)); // Copiando o percurso atual como o ótimo
        }
    } while (next_permutation(cities, num_cities)); // Gerando a próxima permutação

    // Imprimindo o percurso ótimo e seu custo
    printf("Caminho ótimo: ");
    for (int i = 0; i < num_cities; i++) {
        printf("%d ", optimal_path[i]);
    }
    printf("\nCusto mínimo: %d\n", min_cost);
}

int main() {
    setlocale(LC_ALL, "Portuguese");

    FILE *file;
    char filename[] = "cidades1.csv";
    char line[MAX_LINE_LENGTH];
    int cost_table[MAX_CITIES][MAX_CITIES];
    int num_cities = 0;

    // Abrir o arquivo
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", filename);
        return 1;
    }

    // Ler o arquivo e construir a tabela de custos
    while (fgets(line, sizeof(line), file) && num_cities < MAX_CITIES) {
        char *token;
        int column = 0;

        token = strtok(line, ",");
        while (token != NULL && column < MAX_CITIES) {
            cost_table[num_cities][column] = atoi(token);
            token = strtok(NULL, ",");
            column++;
        }

        num_cities++;
    }

    // Fechar o arquivo
    fclose(file);

    // Encontrar o percurso ótimo
    find_optimal_path(num_cities, cost_table);

    return 0;
}
