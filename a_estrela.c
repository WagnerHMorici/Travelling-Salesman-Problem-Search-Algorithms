#include <stdio.h>       // Inclui a biblioteca padrão de entrada e saída
#include <stdlib.h>      // Inclui a biblioteca padrão de funções utilitárias, como malloc e free
#include <limits.h>      // Inclui a biblioteca para definir valores de limites, como INT_MAX
#include <string.h>      // Inclui a biblioteca para manipulação de strings
#include <locale.h>      // Inclui a biblioteca para configuração de localização
#include <time.h>        // Inclui a biblioteca para manipulação de tempo
#include <math.h>        // Inclui a biblioteca para cálculos matemáticos

#define MAX_CITIES 15            // Define o número máximo de cidades
#define MAX_LINE_LENGTH 1024     // Define o comprimento máximo de uma linha do arquivo

// Estrutura para representar um estado no A*
typedef struct {
    int *path;           // Array de inteiros para armazenar o caminho atual
    int path_length;     // Comprimento do caminho atual
    int g;               // Custo acumulado até o estado atual
    int f;               // Custo total estimado (g + h)
} State;

// Função para calcular a distância entre duas cidades
int distance(int city1, int city2, int cost_table[MAX_CITIES][MAX_CITIES]) {
    return cost_table[city1][city2];   // Retorna a distância entre city1 e city2 da tabela de custos
}

// Função heurística (distância mínima ao próximo vizinho)
int heuristic(int city, int num_cities, int cost_table[MAX_CITIES][MAX_CITIES], int *visited) {
    int min_distance = INT_MAX;   // Inicializa a distância mínima como o valor máximo de int
    for (int i = 0; i < num_cities; i++) {  // Itera por todas as cidades
        if (!visited[i] && cost_table[city][i] < min_distance) {  // Se a cidade não foi visitada e a distância é menor
            min_distance = cost_table[city][i];  // Atualiza a distância mínima
        }
    }
    return (min_distance == INT_MAX) ? 0 : min_distance;  // Retorna a distância mínima ou 0 se não houver cidade não visitada
}

// Função para criar um novo estado
State *create_state(int *path, int path_length, int g, int f) {
    State *state = (State *)malloc(sizeof(State));  // Aloca memória para um novo estado
    state->path = (int *)malloc(path_length * sizeof(int));  // Aloca memória para o caminho
    memcpy(state->path, path, path_length * sizeof(int));  // Copia o caminho para o novo estado
    state->path_length = path_length;  // Define o comprimento do caminho
    state->g = g;  // Define o custo acumulado
    state->f = f;  // Define o custo total estimado
    return state;  // Retorna o novo estado
}

// Função para liberar a memória de um estado
void free_state(State *state) {
    free(state->path);  // Libera a memória alocada para o caminho
    free(state);        // Libera a memória alocada para o estado
}

// Função para comparar dois estados na priority queue
int compare_states(const void *a, const void *b) {
    return ((*(State **)a)->f - (*(State **)b)->f);  // Compara os custos totais estimados (f) de dois estados
}

// Função para encontrar o percurso ótimo pelo método A*
void find_optimal_path(int num_cities, int cost_table[MAX_CITIES][MAX_CITIES]) {
    // Inicializando a lista aberta como uma priority queue
    State **open_list = (State **)malloc(num_cities * num_cities * sizeof(State *));  // Aloca memória para a lista aberta
    int open_list_size = 0;  // Inicializa o tamanho da lista aberta

    // Estado inicial (começando da cidade 0)
    int initial_path[1] = {0};  // Define o caminho inicial começando da cidade 0
    int initial_g = 0;          // Define o custo acumulado inicial como 0
    int visited[MAX_CITIES] = {0};  // Array para acompanhar as cidades visitadas
    visited[0] = 1;             // Marca a cidade 0 como visitada
    int initial_h = heuristic(0, num_cities, cost_table, visited);  // Calcula a heurística para o estado inicial
    State *initial_state = create_state(initial_path, 1, initial_g, initial_g + initial_h);  // Cria o estado inicial
    open_list[open_list_size++] = initial_state;  // Adiciona o estado inicial à lista aberta

    int min_cost = INT_MAX;  // Inicializa o custo mínimo como o valor máximo de int
    int *optimal_path = NULL;  // Ponteiro para armazenar o caminho ótimo
    clock_t start_time = clock();  // Marca o tempo de início

    while (open_list_size > 0) {  // Enquanto houver estados na lista aberta
        // Ordenando a lista aberta para obter o estado com menor f(n)
        qsort(open_list, open_list_size, sizeof(State *), compare_states);  // Ordena a lista aberta
        State *current_state = open_list[--open_list_size];  // Obtém o estado com menor f(n) e remove da lista aberta

        // Verificando se todas as cidades foram visitadas e retornando à inicial
        if (current_state->path_length == num_cities) {  // Se todas as cidades foram visitadas
            int final_cost = current_state->g + distance(current_state->path[num_cities - 1], 0, cost_table);  // Calcula o custo final retornando à cidade inicial
            if (final_cost < min_cost) {  // Se o custo final é menor que o custo mínimo
                min_cost = final_cost;  // Atualiza o custo mínimo
                if (optimal_path) free(optimal_path);  // Libera a memória do caminho ótimo anterior, se houver
                optimal_path = (int *)malloc((num_cities + 1) * sizeof(int));  // Aloca memória para o novo caminho ótimo
                memcpy(optimal_path, current_state->path, num_cities * sizeof(int));  // Copia o caminho atual para o caminho ótimo
                optimal_path[num_cities] = 0; // Adiciona a cidade inicial ao final do caminho
            }
            free_state(current_state);  // Libera a memória do estado atual
            continue;  // Continua para o próximo estado na lista aberta
        }

        // Expandindo os vizinhos
        for (int i = 0; i < num_cities; i++) {  // Itera por todas as cidades
            int already_visited = 0;  // Variável para verificar se a cidade já foi visitada
            for (int j = 0; j < current_state->path_length; j++) {  // Verifica o caminho atual
                if (current_state->path[j] == i) {  // Se a cidade já foi visitada
                    already_visited = 1;  // Marca como já visitada
                    break;  // Sai do loop
                }
            }
            if (already_visited) continue;  // Se a cidade já foi visitada, pula para a próxima

            int new_g = current_state->g + distance(current_state->path[current_state->path_length - 1], i, cost_table);  // Calcula o novo custo acumulado
            int new_path[current_state->path_length + 1];  // Array para o novo caminho
            memcpy(new_path, current_state->path, current_state->path_length * sizeof(int));  // Copia o caminho atual para o novo caminho
            new_path[current_state->path_length] = i;  // Adiciona a nova cidade ao caminho

            memset(visited, 0, sizeof(visited));  // Reseta o array de cidades visitadas
            for (int k = 0; k <= current_state->path_length; k++) {  // Marca as cidades do novo caminho como visitadas
                visited[new_path[k]] = 1;
            }
            int new_h = heuristic(i, num_cities, cost_table, visited);  // Calcula a heurística para o novo estado
            State *new_state = create_state(new_path, current_state->path_length + 1, new_g, new_g + new_h);  // Cria o novo estado
            open_list[open_list_size++] = new_state;  // Adiciona o novo estado à lista aberta
        }

        free_state(current_state);  // Libera a memória do estado atual
    }

    if (optimal_path) {  // Se foi encontrado um caminho ótimo
        printf("Caminho ótimo: ");
        for (int i = 0; i <= num_cities; i++) {  // Imprime o caminho ótimo
            printf("%d ", optimal_path[i]);
        }
        printf("\nCusto mínimo: %d\n", min_cost);  // Imprime o custo mínimo
        free(optimal_path);  // Libera a memória do caminho ótimo
    } else {
        printf("Nenhum caminho encontrado.\n");  // Se não foi encontrado nenhum caminho ótimo
    }

    for (int i = 0; i < open_list_size; i++) {  // Libera a memória de todos os estados restantes na lista aberta
        free_state(open_list[i]);
    }
    free(open_list);  // Libera a memória da lista aberta

    clock_t end_time = clock();  // Marca o tempo de término
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;  // Calcula o tempo de execução
    printf("Tempo total de execução: %.2f segundos\n", execution_time);  // Imprime o tempo total de execução
}

int main() {
    setlocale(LC_ALL, ("Portuguese"));  // Configura a localização para português

    FILE *file;
    char filename[] = "cidades12.csv";  // Nome do arquivo de entrada
    char line[MAX_LINE_LENGTH];  // Array para armazenar cada linha do arquivo
    int cost_table[MAX_CITIES][MAX_CITIES];  // Matriz de custos entre as cidades
    int num_cities = 0;  // Número de cidades

    // Abrir o arquivo
    file = fopen(filename, "r");  // Abre o arquivo para leitura

    if (file == NULL) {  // Verifica se o arquivo foi aberto com sucesso
        fprintf(stderr, "Não foi possível abrir o arquivo %s\n", filename);  // Imprime mensagem de erro
        return 1;  // Retorna com erro
    }

    // Ler o arquivo e construir a tabela de custos
    while (fgets(line, sizeof(line), file) && num_cities < MAX_CITIES) {  // Lê cada linha do arquivo
        char *token;
        int column = 0;
        token = strtok(line, ",");  // Divide a linha em tokens usando vírgula como delimitador
        while (token != NULL && column < MAX_CITIES) {  // Itera pelos tokens da linha
            cost_table[num_cities][column] = atoi(token);  // Converte o token para inteiro e armazena na tabela de custos
            token = strtok(NULL, ",");  // Obtém o próximo token
            column++;  // Incrementa a coluna
        }
        num_cities++;  // Incrementa o número de cidades
    }

    printf("Número de cidades: %d\n", num_cities);

    fclose(file);  // Fecha o arquivo

    find_optimal_path(num_cities, cost_table);  // Chama a função para encontrar o percurso ótimo

    return 0;  // Retorna 0 indicando sucesso
}
