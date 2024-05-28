#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <stdarg.h>

#define MAX_CITIES 15
#define MAX_LINE_LENGTH 1024

// Estrutura para armazenar o estado do caminho atual
typedef struct {
    int *path;              // Caminho atual
    int path_length;        // Comprimento do caminho atual
    int g;                  // Custo do caminho percorrido até agora
    int f;                  // Custo estimado total (g + h)
    int visited[MAX_CITIES]; // Marcador de cidades visitadas
} State;

// Função para calcular a distância entre duas cidades
int distance(int city1, int city2, int cost_table[MAX_CITIES][MAX_CITIES]) {
    return cost_table[city1][city2];
}

// Função heurística para estimar o custo restante até o objetivo
int heuristic(int city, int num_cities, int cost_table[MAX_CITIES][MAX_CITIES], int *visited) {
    int min_distance = INT_MAX;
    for (int i = 0; i < num_cities; i++) {
        if (!visited[i] && cost_table[city][i] < min_distance) {
            min_distance = cost_table[city][i];
        }
    }
    return (min_distance == INT_MAX) ? 0 : min_distance;
}

// Função para criar um novo estado
State *create_state(int *path, int path_length, int g, int f, int *visited) {
    State *state = (State *)malloc(sizeof(State));
    state->path = (int *)malloc(path_length * sizeof(int));
    memcpy(state->path, path, path_length * sizeof(int));
    state->path_length = path_length;
    state->g = g;
    state->f = f;
    memcpy(state->visited, visited, MAX_CITIES * sizeof(int));
    return state;
}

// Função para liberar a memória de um estado
void free_state(State *state) {
    free(state->path);
    free(state);
}

// Função de comparação de estados para ordenação
int compare_states(const void *a, const void *b) {
    return ((*(State **)a)->f - (*(State **)b)->f);
}

// Função para imprimir tanto no terminal quanto em um arquivo
void print_to_both(FILE *file, const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args); // Imprime no terminal
    va_end(args);

    va_start(args, format);
    vfprintf(file, format, args); // Imprime no arquivo
    va_end(args);
}

// Função para encontrar o caminho ótimo usando o algoritmo A*
void find_optimal_path(int num_cities, int cost_table[MAX_CITIES][MAX_CITIES], FILE *output_file) {
    State **open_list = (State **)malloc(num_cities * num_cities * sizeof(State *));
    int open_list_size = 0;

    int initial_path[1] = {0};
    int initial_g = 0;
    int visited[MAX_CITIES] = {0};
    visited[0] = 1; // Marca a cidade inicial como visitada
    int initial_h = heuristic(0, num_cities, cost_table, visited);
    State *initial_state = create_state(initial_path, 1, initial_g, initial_g + initial_h, visited);
    open_list[open_list_size++] = initial_state;

    int min_cost = INT_MAX;
    int *optimal_path = NULL;
    clock_t start_time = clock();

    // Loop principal do algoritmo A*
    while (open_list_size > 0) {
        qsort(open_list, open_list_size, sizeof(State *), compare_states); // Ordena a lista de estados abertos
        State *current_state = open_list[--open_list_size];

        // Se todas as cidades foram visitadas, calcula o custo final do caminho
        if (current_state->path_length == num_cities) {
            int final_cost = current_state->g + distance(current_state->path[num_cities - 1], 0, cost_table);
            if (final_cost < min_cost) {
                min_cost = final_cost;
                if (optimal_path) free(optimal_path);
                optimal_path = (int *)malloc((num_cities + 1) * sizeof(int));
                memcpy(optimal_path, current_state->path, num_cities * sizeof(int));
                optimal_path[num_cities] = 0; // Adiciona a cidade inicial no final para completar o ciclo
            }
            free_state(current_state);
            continue;
        }

        // Expande os nós vizinhos
        for (int i = 0; i < num_cities; i++) {
            if (current_state->path_length == 1 && i == 0) continue;  // Evita revisitar a cidade inicial no segundo passo
            if (i == current_state->path[current_state->path_length - 1]) continue;  // Evita revisitar a mesma cidade
            if (current_state->visited[i]) continue; // Evita expandir nós com cidades já visitadas

            int new_g = current_state->g + distance(current_state->path[current_state->path_length - 1], i, cost_table);
            if (new_g >= min_cost) continue;  // Evita expandir nós com custo maior que o menor custo encontrado até agora

            int new_path[current_state->path_length + 1];
            memcpy(new_path, current_state->path, current_state->path_length * sizeof(int));
            new_path[current_state->path_length] = i;

            int new_visited[MAX_CITIES];
            memcpy(new_visited, current_state->visited, MAX_CITIES * sizeof(int));
            new_visited[i] = 1; // Marca a cidade como visitada

            int new_h = heuristic(i, num_cities, cost_table, new_visited);
            State *new_state = create_state(new_path, current_state->path_length + 1, new_g, new_g + new_h, new_visited);
            open_list[open_list_size++] = new_state;
        }

        free_state(current_state);
    }

    // Imprime o resultado final
    if (optimal_path) {
        print_to_both(output_file, "Caminho ótimo: ");
        for (int i = 0; i <= num_cities; i++) {
            print_to_both(output_file, "%d ", optimal_path[i]);
        }
        print_to_both(output_file, "\nCusto mínimo: %d\n", min_cost);
        free(optimal_path);
    } else {
        print_to_both(output_file, "Nenhum caminho encontrado.\n");
    }

    for (int i = 0; i < open_list_size; i++) {
        free_state(open_list[i]);
    }
    free(open_list);

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    print_to_both(output_file, "Tempo total de execução: %.2f segundos\n", execution_time);
}

int main() {
    setlocale(LC_ALL, ("Portuguese"));

    FILE *file;
    char filename[] = "cidades.csv";
    char line[MAX_LINE_LENGTH];
    int cost_table[MAX_CITIES][MAX_CITIES];
    int num_cities = 0;

    // Abre o arquivo CSV para ler a tabela de custos entre as cidades
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", filename);
        return 1;
    }

    // Lê a tabela de custos do arquivo CSV
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

    printf("Número de cidades: %d\n", num_cities);
    fclose(file);

    // Abre o arquivo de saída para escrever os resultados
    FILE *output_file = fopen("a_estrela_result.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo output.txt\n");
        return 1;
    }

    // Encontra o caminho ótimo usando o algoritmo A*
    find_optimal_path(num_cities, cost_table, output_file);
    fclose(output_file);

    return 0;
}
