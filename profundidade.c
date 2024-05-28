#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Biblioteca para manipulação de tempo

#define INFINITO 999999 // Definindo um valor grande para representar infinito

// Função para ler os custos entre as cidades a partir de um arquivo CSV
int** ler_custos(const char* nome_arquivo, int* tamanho) {
    
    // Abre o arquivo para leitura
    FILE* file = fopen(nome_arquivo, "r"); 
    if (!file) { 
        // Se ocorrer um erro ao abrir o arquivo, exibe uma mensagem e encerra o programa
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    int** custos; // Declaração da matriz de custos
    int linhas = 0; // Contador de linhas
    int colunas = 0; // Contador de colunas
    char linha[BUFSIZ]; // Buffer para leitura de linhas

    // Contando o número de linhas e colunas no arquivo
    while (fgets(linha, sizeof(linha), file)) {
        linhas++; // Incrementa o contador de linhas
        colunas = 0; // Reinicia o contador de colunas para cada linha

        char* token;

        // Tokeniza a linha com base na vírgula
        token = strtok(linha, ","); 

        while (token != NULL) {
            colunas++; // Incrementa o contador de colunas
            token = strtok(NULL, ","); 
        }
    }

    // Define o tamanho da matriz (número de linhas)
    *tamanho = linhas; 

    // Alocando espaço para a matriz de custos
    custos = (int**)malloc(linhas * sizeof(int*));
    for (int i = 0; i < linhas; i++) {
        custos[i] = (int*)malloc(colunas * sizeof(int));
    }

    // Voltando ao início do arquivo
    rewind(file);

    int i = 0;
    // Preenchendo a matriz de custos com os valores do arquivo
    while (fgets(linha, sizeof(linha), file)) {
        int j = 0;
        char* token;

        // Tokeniza a linha com base na vírgula
        token = strtok(linha, ",");
        while (token != NULL) {
            custos[i][j++] = atoi(token); // Converte o token para inteiro e armazena na matriz
            token = strtok(NULL, ",");
        }
        i++;
    }

    // Fecha o arquivo
    fclose(file); 

    // Retorna a matriz de custos
    return custos;
}

// Função recursiva para encontrar a melhor rota através de busca em profundidade
// Função recursiva para encontrar a melhor rota através de busca em profundidade
// Função recursiva para encontrar a melhor rota através de busca em profundidade
void tsp_dfs(int cidade_atual, int custo_atual, int** custos, bool* visitados, int n, int* melhor_custo, int* melhor_rota, int* rota_atual) {

    // Verifica se todas as cidades foram visitadas
    bool todas_visitadas = true;
    for (int i = 0; i < n; i++) {
        if (!visitados[i]) {
            todas_visitadas = false;
            break;
        }
    }

    // Se todas as cidades foram visitadas
    if (todas_visitadas) {
        // Verifica se o custo atual é menor que o melhor custo encontrado até agora
        if (custo_atual < *melhor_custo) {
            *melhor_custo = custo_atual; // Atualiza o melhor custo
            // Copia a rota atual para a melhor rota
            for (int i = 0; i < n; i++) {
                melhor_rota[i] = rota_atual[i];
            }
        }
        return; // Retorna da função recursiva
    }

    // Percorre todas as cidades
    for (int prox_cidade = 0; prox_cidade < n; prox_cidade++) {
        // Se a próxima cidade ainda não foi visitada
        if (!visitados[prox_cidade]) {
            visitados[prox_cidade] = true; // Marca a cidade como visitada
            rota_atual[cidade_atual] = prox_cidade; // Adiciona a cidade na rota atual

            // Chama recursivamente a função para a próxima cidade
            tsp_dfs(prox_cidade, custo_atual + custos[cidade_atual][prox_cidade], custos, visitados, n, melhor_custo, melhor_rota, rota_atual);

            visitados[prox_cidade] = false; // Desmarca a cidade como visitada
        }
    }
}

// Função para escrever os resultados em um arquivo

void escrever_resultados(const char* nome_arquivo, int melhor_custo, int* melhor_rota, int n, double tempo_execucao) {
    // Abre o arquivo para escrita
    FILE* file = fopen(nome_arquivo, "w");
    if (!file) {
        // Se ocorrer um erro ao abrir o arquivo, exibe uma mensagem
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return;
    }

    // Escreve o melhor custo encontrado
    fprintf(file, "Melhor custo encontrado: %d\n", melhor_custo);
    fprintf(file, "Melhor rota encontrada: ");

    // Escreve a melhor rota encontrada
    for (int i = 0; i < n; i++) {
        fprintf(file, "%d ", melhor_rota[i] + 1); // Incrementa o índice em 1 para obter a cidade real
    }
    fprintf(file, "\nTempo de execução: %.2f segundos\n", tempo_execucao); // Escreve o tempo de execução

    // Fecha o arquivo
    fclose(file);
}

// Função principal para encontrar a melhor rota
void encontrar_melhor_rota(const char* nome_arquivo) {
    int n;

    // Lê a matriz de custos do arquivo
    int** custos = ler_custos(nome_arquivo, &n);

    // Declaração de variáveis
    bool visitados[n];
    int melhor_custo = INFINITO;
    int melhor_rota[n];
    int rota_atual[n];
    int cidades_visitadas = 1; // Movido para fora da função encontrar_melhor_rota

    // Inicializa o vetor de cidades visitadas
    for (int i = 0; i < n; i++) {
        visitados[i] = false;
    }

    visitados[0] = true; // Marca a cidade inicial como visitada

    // Inicia a contagem do tempo de execução
    clock_t inicio = clock();

    // Chama a função recursiva para encontrar a melhor rota
    tsp_dfs(0, 0, custos, visitados, n, &melhor_custo, melhor_rota, rota_atual);

    // Finaliza a contagem do tempo de execução
    clock_t fim = clock();
    double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC; // Calcula o tempo de execução

    // Escreve os resultados no arquivo
    escrever_resultados("Profundidade_resultado.txt", melhor_custo, melhor_rota, n, tempo_execucao);

    // Exibe os resultados no console
    printf("Melhor custo encontrado: %d\n", melhor_custo);
    printf("Melhor rota encontrada: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", melhor_rota[i]);
    }
    printf("\nTempo de execução: %.2f segundos\n", tempo_execucao);
}
// Função principal
int main() {
    const char* nome_arquivo = "cidades1.csv"; // Nome do arquivo CSV contendo os custos

    // Chama a função para encontrar a melhor rota
    encontrar_melhor_rota(nome_arquivo); 
    
    return 0;
}

