#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_CIDADES 15
#define NOME_ARQUIVO "cidades1.csv"
#define VALOR_INVALIDO -1

typedef struct {
    int custos[MAX_CIDADES][MAX_CIDADES];
    int tamanho;
} Grafo;

// Função para ler os custos entre as cidades a partir de um arquivo CSV
Grafo ler_custos(const char* nome_arquivo) {
    FILE* file = fopen(nome_arquivo, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    Grafo grafo;
    grafo.tamanho = 0;

    char linha[BUFSIZ];
    while (fgets(linha, sizeof(linha), file)) {
        int cidade_atual = 0;
        char* token = strtok(linha, ",");
        while (token != NULL) {
            grafo.custos[grafo.tamanho][cidade_atual++] = atoi(token);
            token = strtok(NULL, ",");
        }
        grafo.tamanho++;
    }

    fclose(file);
    return grafo;
}

// Função auxiliar para encontrar o próximo vizinho não visitado mais próximo
int proximo_vizinho(int cidade_atual, const Grafo* grafo, bool* visitados) {
    int prox_cidade = -1;
    int menor_custo = INT_MAX;

    for (int i = 0; i < grafo->tamanho; i++) {
        if (!visitados[i] && grafo->custos[cidade_atual][i] < menor_custo) {
            prox_cidade = i;
            menor_custo = grafo->custos[cidade_atual][i];
        }
    }

    return prox_cidade;
}

// Função recursiva para encontrar a melhor rota usando busca em profundidade
void tsp_dfs(int cidade_atual, int custo_atual, const Grafo* grafo, bool* visitados, int* melhor_custo, int* melhor_rota, int* rota_atual) {
    // Se o custo atual já é maior ou igual ao melhor custo encontrado até agora, retorne
    if (custo_atual >= *melhor_custo)
        return;

    // Se todas as cidades foram visitadas, atualize o melhor custo e a melhor rota
    bool todas_visitadas = true;
    for (int i = 0; i < grafo->tamanho; i++) {
        if (!visitados[i]) {
            todas_visitadas = false;
            break;
        }
    }

    if (todas_visitadas) {
        *melhor_custo = custo_atual;
        memcpy(melhor_rota, rota_atual, grafo->tamanho * sizeof(int));
        return;
    }

    // Encontre o próximo vizinho não visitado mais próximo
    int prox_cidade = proximo_vizinho(cidade_atual, grafo, visitados);

    // Se não houver próximo vizinho válido, retorne
    if (prox_cidade == -1)
        return;

    // Marque a cidade como visitada
    visitados[prox_cidade] = true;
    // Adicione a cidade à rota atual
    rota_atual[cidade_atual] = prox_cidade;
    // Chame recursivamente a função para a próxima cidade
    tsp_dfs(prox_cidade, custo_atual + grafo->custos[cidade_atual][prox_cidade], grafo, visitados, melhor_custo, melhor_rota, rota_atual);
    // Desmarque a cidade como visitada para explorar outras ramificações
    visitados[prox_cidade] = false;
}

// Função para escrever os resultados em um arquivo
void escrever_resultados(const char* nome_arquivo, int melhor_custo, const int* melhor_rota, int tamanho, double tempo_execucao) {
    FILE* file = fopen(nome_arquivo, "w");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return;
    }

    fprintf(file, "Melhor custo encontrado: %d\n", melhor_custo);
    fprintf(file, "Melhor rota encontrada: ");
    for (int i = 0; i < tamanho; i++) {
        fprintf(file, "%d ", melhor_rota[i] + 1);
    }
    fprintf(file, "\nTempo de execução: %.2f segundos\n", tempo_execucao);

    fclose(file);
}

// Função principal para encontrar a melhor rota
void encontrar_melhor_rota(const Grafo* grafo) {
    bool visitados[MAX_CIDADES] = {false};
    int melhor_custo = INT_MAX;
    int melhor_rota[MAX_CIDADES];
    int rota_atual[MAX_CIDADES] = {0};

    visitados[0] = true;

    clock_t inicio = clock();

    tsp_dfs(0, 0, grafo, visitados, &melhor_custo, melhor_rota, rota_atual);

    clock_t fim = clock();
    double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    escrever_resultados("Profundidade_resultado.txt", melhor_custo, melhor_rota, grafo->tamanho, tempo_execucao);

    printf("Melhor custo encontrado: %d\n", melhor_custo);
    printf("Melhor rota encontrada: ");
    for (int i = 0; i < grafo->tamanho; i++) {
        printf("%d ", melhor_rota[i] + 1);
    }
    printf("\nTempo de execução: %.2f segundos\n", tempo_execucao);
}

// Função principal
int main() {
    // Ler os custos entre as cidades do arquivo CSV
    Grafo grafo = ler_custos(NOME_ARQUIVO);

    // Encontrar a melhor rota
    encontrar_melhor_rota(&grafo);

    return 0;
}
