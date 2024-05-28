#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define NUM_CITIES 15 // Número de cidades
#define POP_SIZE 100 // Tamanho da população
#define MAX_GENERATIONS 500 // Número máximo de gerações
#define MUTATION_RATE 0.01 // Taxa de mutação
#define INTERVALO_TEMPO 60 // Intervalo de tempo para exportar informações (em segundos)

// Definição da estrutura de um indivíduo (solução)
typedef struct {
    int route[NUM_CITIES]; 
    double fitness; 
} Individual;

double cost_matrix[NUM_CITIES][NUM_CITIES]; // Matriz de custos

// Função para inicializar a matriz de custos a partir de um arquivo CSV
void initialize_cost_matrix_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }

    char line[1024];

    for (int i = 0; i < NUM_CITIES; i++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Erro ao ler o arquivo\n");
            exit(1);
        }

        char *token = strtok(line, ",");
        for (int j = 0; j < NUM_CITIES; j++) {
            if (token == NULL) {
                fprintf(stderr, "Erro ao ler o arquivo\n");
                exit(1);
            }
            cost_matrix[i][j] = atof(token);
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

// Função para inicializar um indivíduo com uma rota aleatória
void initialize_individual(Individual *individual) {
    for (int i = 0; i < NUM_CITIES; i++) {
        individual->route[i] = i;
    }

    // Embaralhe a rota aleatoriamente
    for (int i = 0; i < NUM_CITIES; i++) {
        int j = rand() % NUM_CITIES;
        int temp = individual->route[i];
        individual->route[i] = individual->route[j];
        individual->route[j] = temp;
    }

    individual->fitness = 0; 
}

// Função para calcular o valor de fitness de um indivíduo
void calculate_fitness(Individual *individual) {
    double total_distance = 0;

    // Calcule a distância total da rota
    for (int i = 0; i < NUM_CITIES - 1; i++) {
        total_distance += cost_matrix[individual->route[i]][individual->route[i + 1]];
    }
    // Adicione a distância da última cidade de volta para a primeira
    total_distance += cost_matrix[individual->route[NUM_CITIES - 1]][individual->route[0]];
    
    // O fitness é o inverso da distância total (menor distância = fitness maior)
    individual->fitness = 1.0 / total_distance;
}

// Função para executar a seleção de pais usando o método de torneio
void select_parents(Individual population[], Individual *parent1, Individual *parent2) {
    int tournament_size = 5; 
    Individual tournament[tournament_size];

    // Escolha aleatoriamente participantes do torneio
    for (int i = 0; i < tournament_size; i++) {
        tournament[i] = population[rand() % POP_SIZE];
    }

    // Encontre os dois indivíduos com o maior fitness no torneio
    *parent1 = tournament[0];
    *parent2 = tournament[0];

    for (int i = 1; i < tournament_size; i++) {
        if (tournament[i].fitness > parent1->fitness) {
            *parent2 = *parent1;
            *parent1 = tournament[i];
        } else if (tournament[i].fitness > parent2->fitness) {
            *parent2 = tournament[i];
        }
    }
}

// Função para realizar crossover entre dois pais para produzir um filho
void crossover(Individual parent1, Individual parent2, Individual *child) {
    int crossover_point = rand() % NUM_CITIES;

    // Copie a seção antes do ponto de crossover do pai 1
    for (int i = 0; i < crossover_point; i++) {
        child->route[i] = parent1.route[i];
    }

    // Copie as cidades restantes do pai 2, mantendo a ordem
    int index = crossover_point;

    for (int i = 0; i < NUM_CITIES; i++) {
        int city = parent2.route[i];
        int j;

        for (j = 0; j < crossover_point; j++) {
            if (child->route[j] == city) break; 
        }

        if (j == crossover_point) { 
            child->route[index++] = city;
        }
    }
}

// Função para realizar mutação em um filho
void mutate(Individual *child) {
    // Aplique mutação de troca de duas cidades
    int city1 = rand() % NUM_CITIES;
    int city2 = rand() % NUM_CITIES;

    int temp = child->route[city1];
    child->route[city1] = child->route[city2];
    child->route[city2] = temp;
}

// Função para exportar os resultados para um arquivo de texto
void export_results_to_file(const char *filename, Individual best_individual, double total_distance, double simulation_time) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }

    fprintf(file, "Melhor rota encontrada:\n");
    for (int i = 0; i < NUM_CITIES; i++) {
        fprintf(file, "%d ", best_individual.route[i]);
    }
    fprintf(file, "\nValor de fitness: %f\n", best_individual.fitness);
    fprintf(file, "Custo do caminho escolhido: %f\n", total_distance);
    fprintf(file, "Tempo de simulação: %f segundos\n", simulation_time);

    fclose(file);
}

// Função para executar o algoritmo genético
void genetic_algorithm() {
    Individual population[POP_SIZE]; 
    Individual offspring[POP_SIZE]; 

    srand(time(NULL)); 
    initialize_cost_matrix_from_file("cidades.csv"); 

    // Inicialize a população
    for (int i = 0; i < POP_SIZE; i++) {
        initialize_individual(&population[i]);
        calculate_fitness(&population[i]);
    }

    int generation = 0;
    clock_t start_time = clock(); // Iniciar a medição do tempo

    while (generation < MAX_GENERATIONS) {
        // Execute seleção, crossover e mutação para criar a próxima geração
        for (int i = 0; i < POP_SIZE; i++) {
            Individual parent1, parent2;
            select_parents(population, &parent1, &parent2);
            crossover(parent1, parent2, &offspring[i]);

            if (rand() < RAND_MAX * MUTATION_RATE) {
                mutate(&offspring[i]);
            }

            calculate_fitness(&offspring[i]);
        }

        // Substitua a população atual pela nova geração
        for (int i = 0; i < POP_SIZE; i++) {
            population[i] = offspring[i];
        }

        generation++;
    }

    // Encontre o melhor indivíduo na última geração
    Individual best_individual = population[0];

    for (int i = 1; i < POP_SIZE; i++) {
        if (population[i].fitness > best_individual.fitness) {
            best_individual = population[i];
        }
    }

    // Calcular o custo do caminho escolhido
    double total_distance = 0;
    for (int i = 0; i < NUM_CITIES - 1; i++) {
        total_distance += cost_matrix[best_individual.route[i]][best_individual.route[i + 1]];
    }
    // Adicione a distância da última cidade de volta para a primeira
    total_distance += cost_matrix[best_individual.route[NUM_CITIES - 1]][best_individual.route[0]];

    clock_t end_time = clock(); // Finalizar a medição do tempo
    double simulation_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Imprima a melhor rota e seu valor de fitness
    printf("Melhor rota encontrada:\n");
    for (int i = 0; i < NUM_CITIES; i++) {
        printf("%d ", best_individual.route[i]);
    }
    printf("\nValor de fitness: %f\n", best_individual.fitness);
    printf("Custo do caminho escolhido: %f\n", total_distance);
    printf("Tempo de simulação: %f segundos\n", simulation_time);

    // Exportar resultados para um arquivo de texto
    export_results_to_file("resultados_geneticos.txt", best_individual, total_distance, simulation_time);
}

int main() {
    genetic_algorithm();
    return 0;
}
