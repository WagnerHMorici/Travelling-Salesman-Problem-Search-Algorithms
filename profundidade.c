#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Adicionando biblioteca para manipulação de tempo

#define INFINITO 999999 // Definindo um valor grande para representar infinito

// Função para ler os custos entre as cidades a partir de um arquivo CSV
int** ler_custos(const char* nome_arquivo, int* tamanho) {
    
    FILE* file = fopen(nome_arquivo, "r"); 
    if (!file) { 
        
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);

    }

    int** custos; 
    int linhas = 0; 
    int colunas = 0;
    char linha[BUFSIZ]; 

    // Contando o número de linhas e colunas no arquivo
    while (fgets(linha, sizeof(linha), file)) {

        linhas++; 

        colunas = 0; 

        char* token;

        token = strtok(linha, ","); 

        while (token != NULL) {
            colunas++; 
            token = strtok(NULL, ","); 
        }
    }

    *tamanho = linhas; // Definindo o tamanho da matriz (número de linhas)

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

        token = strtok(linha, ",");

        while (token != NULL) {

            custos[i][j++] = atoi(token);
             
            token = strtok(NULL, ",");
        }
        i++;
    }

    fclose(file); 
    return custos;
}

// Função recursiva para encontrar a melhor rota através de busca em profundidade
void tsp_dfs(int cidade_atual, int custo_atual, int* cidades_visitadas, int** custos, bool* visitados, int n, int* melhor_custo, int* melhor_rota, int* rota_atual) {

    // Verificando se todas as cidades foram visitadas
    if (*cidades_visitadas == n) {

        // Verificando se o custo atual é melhor do que o melhor custo encontrado até agora
        if (custo_atual + custos[cidade_atual][0] < *melhor_custo) {

            // Atualizando o melhor custo e a melhor rota
            *melhor_custo = custo_atual + custos[cidade_atual][0];

            for (int i = 0; i < n; i++) {

                melhor_rota[i] = rota_atual[i];

            }
        }
        return;
    }

    // Explorando todas as possíveis cidades seguintes
    for (int prox_cidade = 0; prox_cidade < n; prox_cidade++) {

        if (!visitados[prox_cidade]) {

            visitados[prox_cidade] = true; 
           
            rota_atual[*cidades_visitadas] = prox_cidade; 
            (*cidades_visitadas)++; 

            // Chamando recursivamente a função para a próxima cidade
            tsp_dfs(prox_cidade, custo_atual + custos[cidade_atual][prox_cidade], cidades_visitadas, custos, visitados, n, melhor_custo, melhor_rota, rota_atual);

            (*cidades_visitadas)--; 

            visitados[prox_cidade] = false;
        }
    }
}

// Função para escrever os resultados em um arquivo de texto
void escrever_resultados(const char* nome_arquivo, int melhor_custo, int* melhor_rota, double tempo_execucao) {

    FILE* file = fopen(nome_arquivo, "w"); 

    if (!file) { 

        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);

        return;
    }
    // Escrevendo os resultados no arquivo
    fprintf(file, "Melhor custo encontrado: %d\n", melhor_custo);
    fprintf(file, "Melhor rota encontrada: ");

    for (int i = 0; i < sizeof(melhor_rota) / sizeof(melhor_rota[0]); i++) {

        fprintf(file, "%d ", melhor_rota[i]);

    }
    fprintf(file, "\nTempo de execução: %.2f segundos\n", tempo_execucao);

    fclose(file); 
}

// Função para encontrar a melhor rota a partir do arquivo de custos entre as cidades
void encontrar_melhor_rota(const char* nome_arquivo) {

    int n;

    int** custos = ler_custos(nome_arquivo, &n);

    bool visitados[n]; 

    int melhor_custo = INFINITO; 

    int melhor_rota[n]; 

    int rota_atual[n]; 

    int cidades_visitadas = 1; 

    for (int i = 0; i < n; i++) {
        visitados[i] = false;
    }
    visitados[0] = true; 
    
    clock_t inicio = clock(); 
    
    // Chamando a função recursiva para encontrar a melhor rota
    tsp_dfs(0, 0, &cidades_visitadas, custos, visitados, n, &melhor_custo, melhor_rota, rota_atual);
    
    clock_t fim = clock(); 

    double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC; 

    // Escrevendo os resultados em um arquivo de texto
    escrever_resultados("Profundidade_resultado.txt", melhor_custo, melhor_rota, tempo_execucao);



    printf("Melhor custo encontrado: %d\n", melhor_custo);
    printf("Melhor rota encontrada: ");

    for (int i = 0; i < n; i++) {

        printf("%d ", melhor_rota[i]);

    }
    printf("\nTempo de execução: %.2f segundos\n", tempo_execucao);

}


// Função principal
int main() {
    const char* nome_arquivo = "cidades.csv"; 

    encontrar_melhor_rota(nome_arquivo); 
    
    return 0;
}
