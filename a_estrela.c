#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#define MAX_CITIES 15
#define MAX_LINE_LENGTH 1024

typedef struct {
    int *path;
    int path_length;
    int g;
    int f;
    int visited[MAX_CITIES]; // Marcador de cidades visitadas
} State;

int distance(int city1, int city2, int cost_table[MAX_CITIES][MAX_CITIES]) {
    return cost_table[city1][city2];
}

int heuristic(int city, int num_cities, int cost_table[MAX_CITIES][MAX_CITIES], int *visited) {
    int min_distance = INT_MAX;
    for (int i = 0; i < num_cities; i++) {
        if (!visited[i] && cost_table[city][i] < min_distance) {
            min_distance = cost_table[city][i];
        }
    }
    return (min_distance == INT_MAX) ? 0 : min_distance;
}

State *create_state(int *path, int path_length, int g, int f, int *visited) {
    State *state = (State *)malloc(sizeof(State));
    state->path = (int *)malloc(path_length * sizeof(int));
    memcpy(state->path, path, path_length * sizeof(int));
    state->path_length = path_length;
    state->g = g;
    state->f = f;
    memcpy(state->visited, visited, MAX_CITIES * sizeof(int)); // Copia o marcador de cidades visitadas
    return state;
}

void free_state(State *state) {
    free(state->path);
    free(state);
}

int compare_states(const void *a, const void *b) {
    return ((*(State **)a)->f - (*(State **)b)->f);
}

void find_optimal_path(int num_cities, int cost_table[MAX_CITIES][MAX_CITIES]) {
    State **open_list = (State **)malloc(num_cities * num_cities * sizeof(State *));
    int open_list_size = 0;

    int initial_path[1] = {0};
    int initial_g = 0;
    int visited[MAX_CITIES] = {0};
    visited[0] = 1;
    int initial_h = heuristic(0, num_cities, cost_table, visited);
    State *initial_state = create_state(initial_path, 1, initial_g, initial_g + initial_h, visited);
    open_list[open_list_size++] = initial_state;

    int min_cost = INT_MAX;
    int *optimal_path = NULL;
    clock_t start_time = clock();

    while (open_list_size > 0) {
        qsort(open_list, open_list_size, sizeof(State *), compare_states);
        State *current_state = open_list[--open_list_size];

        if (current_state->path_length == num_cities) {
            int final_cost = current_state->g + distance(current_state->path[num_cities - 1], 0, cost_table);
            if (final_cost < min_cost) {
                min_cost = final_cost;
                if (optimal_path) free(optimal_path);
                optimal_path = (int *)malloc((num_cities + 1) * sizeof(int));
                memcpy(optimal_path, current_state->path, num_cities * sizeof(int));
                optimal_path[num_cities] = 0;
            }
            free_state(current_state);
            continue;
        }

        for (int i = 0; i < num_cities; i++) {
            if (current_state->path_length == 1 && i == 0) continue;  // Evita revisitar a cidade inicial
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
    char filename[] = "cidades.csv";
    char line[MAX_LINE_LENGTH];
    int cost_table[MAX_CITIES][MAX_CITIES];
    int num_cities = 0;

    file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", filename);
        return 1;
    }

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

    find_optimal_path(num_cities, cost_table);

    return 0;
}
