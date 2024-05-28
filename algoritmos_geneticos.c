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

// Definição da estrutura de uma cidade
typedef struct {
    int x, y; 
} City;

// Definição da estrutura de um indivíduo (solução)
typedef struct {
    int route[NUM_CITIES]; 
    double fitness; 
} Individual;

City cities[NUM_CITIES]; // Array de cidades

// Função para calcular a distância euclidiana entre duas cidades
double calc_distance(City city1, City city2) {
    return sqrt(pow(city1.x - city2.x, 2) + pow(city1.y - city2.y, 2));
}

// Função para inicializar as cidades a partir de um arquivo CSV
void initialize_cities_from_file(const char *filename) {
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
        
        sscanf(line, "%d,%d", &cities[i].x, &cities[i].y);
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
        total_distance += calc_distance(cities[individual->route[i]], cities[individual->route[i + 1]]);
    }
    // Adicione a distância da última cidade de volta para a primeira
    total_distance += calc_distance(cities[individual->route[NUM_CITIES - 1]], cities[individual->route[0]]);
    
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

// Função para executar o algoritmo genético
void genetic_algorithm() {
    Individual population[POP_SIZE]; 
    Individual offspring[POP_SIZE]; 

    srand(time(NULL)); 
    initialize_cities_from_file("cidades.csv"); 

    // Inicialize a população
    for (int i = 0; i < POP_SIZE; i++) {
        initialize_individual(&population[i]);
        calculate_fitness(&population[i]);
    }

    int generation = 0;

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

    // Imprima a melhor rota e seu valor de fitness
    printf("Melhor rota encontrada:\n");
    for (int i = 0; i < NUM_CITIES; i++) {
        printf("%d ", best_individual.route[i]);
    }
    printf("\nValor de fitness: %f\n", best_individual.fitness);

    // Calcular o custo do caminho escolhido
    double total_distance = 0;
    for (int i = 0; i < NUM_CITIES - 1; i++) {
        total_distance += calc_distance(cities[best_individual.route[i]], cities[best_individual.route[i + 1]]);
    }
    // Adicione a distância da última cidade de volta para a primeira
    total_distance += calc_distance(cities[best_individual.route[NUM_CITIES - 1]], cities[best_individual.route[0]]);
    printf("Custo do caminho escolhido: %f\n", total_distance);
}

int main() {
    genetic_algorithm();
    return 0;
}
