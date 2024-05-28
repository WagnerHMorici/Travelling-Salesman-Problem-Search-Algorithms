#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#define MAX_CITIES 15
#define MAX_LINE_LENGTH 1024

// Estrutura para representar um estado no A*
typedef struct {
    int *path;
    int path_length;
    int g; // Custo acumulado
    int f; // Custo total estimado (g + h)
} State;

// Função para calcular a distância entre duas cidades
int distance(int city1, int city2, int cost_table[MAX_CITIES][MAX_CITIES]) {
    return cost_table[city1][city2];
}

// Função heurística (distância mínima ao próximo vizinho)
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
State *create_state(int *path, int path_length, int g, int f) {
    State *state = (State *)malloc(sizeof(State));
    state->path = (int *)malloc(path_length * sizeof(int));
    memcpy(state->path, path, path_length * sizeof(int));
    state->path_length = path_length;
    state->g = g;
    state->f = f;
    return state;
}

// Função para liberar a memória de um estado
void free_state(State *state) {
    free(state->path);
    free(state);
}

// Função para comparar dois estados na priority queue
int compare_states(const void *a, const void *b) {
    return ((*(State **)a)->f - (*(State **)b)->f);
}

// Função para encontrar o percurso ótimo pelo método A*
void find_optimal_path(int num_cities, int cost_table[MAX_CITIES][MAX_CITIES]) {
    // Inicializando a lista aberta como uma priority queue
    State **open_list = (State **)malloc(num_cities * num_cities * sizeof(State *));
    int open_list_size = 0;

    // Estado inicial (começando da cidade 0)
    int initial_path[1] = {0};
    int initial_g = 0;
    int visited[MAX_CITIES] = {0};
    visited[0] = 1;
    int initial_h = heuristic(0, num_cities, cost_table, visited);
    State *initial_state = create_state(initial_path, 1, initial_g, initial_g + initial_h);
    open_list[open_list_size++] = initial_state;

    int min_cost = INT_MAX;
    int *optimal_path = NULL;
    clock_t start_time = clock();

    while (open_list_size > 0) {
        // Ordenando a lista aberta para obter o estado com menor f(n)
        qsort(open_list, open_list_size, sizeof(State *), compare_states);
        State *current_state = open_list[--open_list_size];

        // Verificando se todas as cidades foram visitadas e retornando à inicial
        if (current_state->path_length == num_cities) {
            int final_cost = current_state->g + distance(current_state->path[num_cities - 1], 0, cost_table);
            if (final_cost < min_cost) {
                min_cost = final_cost;
                if (optimal_path) free(optimal_path);
                optimal_path = (int *)malloc((num_cities + 1) * sizeof(int));
                memcpy(optimal_path, current_state->path, num_cities * sizeof(int));
                optimal_path[num_cities] = 0; // Voltando à cidade inicial
            }
            free_state(current_state);
            continue;
        }

        // Expandindo os vizinhos
        for (int i = 0; i < num_cities; i++) {
            int already_visited = 0;
            for (int j = 0; j < current_state->path_length; j++) {
                if (current_state->path[j] == i) {
                    already_visited = 1;
                    break;
                }
            }
            if (already_visited) continue;

            int new_g = current_state->g + distance(current_state->path[current_state->path_length - 1], i, cost_table);
            int new_path[current_state->path_length + 1];
            memcpy(new_path, current_state->path, current_state->path_length * sizeof(int));
            new_path[current_state->path_length] = i;
            memset(visited, 0, sizeof(visited));
            for (int k = 0; k <= current_state->path_length; k++) {
                visited[new_path[k]] = 1;
            }
            int new_h = heuristic(i, num_cities, cost_table, visited);
            State *new_state = create_state(new_path, current_state->path_length + 1, new_g, new_g + new_h);
            open_list[open_list_size++] = new_state;
        }

        free_state(current_state);
    }

    if (optimal_path) {
        printf("Caminho ótimo: ");
        for (int i = 0; i <= num_cities; i++) {
            printf("%d ", optimal_path[i]);
        }
        printf("\nCusto mínimo: %d\n", min_cost);
        free(optimal_path);
    } else {
        printf("Nenhum caminho encontrado.\n");
    }

    for (int i = 0; i < open_list_size; i++) {
        free_state(open_list[i]);
    }
    free(open_list);

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Tempo total de execução: %.2f segundos\n", execution_time);
}

int main() {
    setlocale(LC_ALL, ("Portuguese"));

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

    fclose(file);

    find_optimal_path(num_cities, cost_table);

    return 0;
}
