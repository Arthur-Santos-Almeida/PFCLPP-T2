#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

// Define a struct Conflito
typedef struct {
    int i;
    int j;
    int a;
    int b;
    double jaccardIndex;
} Conflito;

void ler_N_P(const char *caminho_arquivo, int *N, int *P) {
    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    fscanf(arquivo, "%d", N);  // Lê N
    fscanf(arquivo, "%d", P);  // Lê P
    fclose(arquivo);
}

int ler_num_conflitos(const char *caminho_arquivo, int N, int P) {
    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    // Ignora as duas primeiras linhas (já lidas em ler_N_P)
    fscanf(arquivo, "%*d");
    fscanf(arquivo, "%*d");

    int numConflitos = 0;
    int numConflitosTotal = 0;
    char bufferDescartavel[2000];

    for (int i = 1; i <= N; i++) {
        for (int a = 1; a <= P; a++) {

            fscanf(arquivo, "%d\n", &numConflitos);  // Lê o número de conflitos, SE VOCÊ NÃO EXPLICITA O \n O FSCANF NÃO CONSOME ESSE CARACTERE E DÁ ERRADO!!

            if (numConflitos != 0) {
                fgets(bufferDescartavel, sizeof(bufferDescartavel), arquivo);
                fgets(bufferDescartavel, sizeof(bufferDescartavel), arquivo);
            }

            numConflitosTotal += numConflitos;
            // if(numConflitosTotal < 20000){
            //     printf("%d\n", numConflitosTotal);
            //     FILE *arquivoEscrita = fopen("C:/OCII/PFCLPP/T2/escrita/c.txt", "a");
            //     fprintf(arquivoEscrita, "%d\n", numConflitosTotal);
            //     fclose(arquivoEscrita);
            // }
        }
    }

    fclose(arquivo);
    return numConflitosTotal;
}

Conflito* ler_coeficientes(int numConflitosComRedundancia, const char *caminho_arquivo, int N, int P) {
    Conflito *conflitos = malloc( numConflitosComRedundancia * sizeof(Conflito) );
    
    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    // Ignora as duas primeiras linhas (já lidas em ler_N_P)
    fscanf(arquivo, "%*d");
    fscanf(arquivo, "%*d");

    int numConflitos = 0;
    int conflitoIndex = 0;
    char bufferDescartavel[2000];

    for (int i = 1; i <= N; i++) {
        for (int a = 1; a <= P; a++) {
            fscanf(arquivo, "%d\n", &numConflitos);  // Lê o número de conflitos, SE VOCÊ NÃO EXPLICITA O \n O FSCANF NÃO CONSOME ESSE CARACTERE E DÁ ERRADO!!
            
            if (numConflitos != 0) {
                fgets(bufferDescartavel, sizeof(bufferDescartavel), arquivo);  // Ignora a linha de distância Euclidiana
            }

            // As duas linhas em branco em caso de numConflitos == 0 não precisam
            // explicitamente ser puladas. O programa já faz sozinho, sabe Deus como
            
            for (int c = 0; c < numConflitos; c++) {
                // Lê a linha com o índice de Jaccard
                int idLabelEmConflito;
                double jaccardIndex;
                fscanf(arquivo, "%d %lf", &idLabelEmConflito, &jaccardIndex);

                int j = (idLabelEmConflito % P != 0)
                            ? (int)ceil((double)idLabelEmConflito / P)
                            : (idLabelEmConflito / P) + 1;

                double parte_decimal = fmod((double)idLabelEmConflito / P, 1.0);

                for (int p = 1; p <= P; p++) {
                    if (fabs(parte_decimal * ((double)P / (double)p) - 1.0) < 1e-6) {  // Compara com tolerância para evitar problemas de ponto flutuante
                        int b = p + 1;
                        conflitos[conflitoIndex].i = i;
                        conflitos[conflitoIndex].j = j;
                        conflitos[conflitoIndex].a = a;
                        conflitos[conflitoIndex].b = b;
                        conflitos[conflitoIndex].jaccardIndex = (double)jaccardIndex;
                        conflitoIndex++;
                        break;
                    } else if (parte_decimal == 0) {  // Caso seja exato
                        int b = 1;
                        conflitos[conflitoIndex].i = i;
                        conflitos[conflitoIndex].j = j;
                        conflitos[conflitoIndex].a = a;
                        conflitos[conflitoIndex].b = b;
                        conflitos[conflitoIndex].jaccardIndex = (double)jaccardIndex;
                        conflitoIndex++;
                        break;
                    }
                }

                // printf("%d %d %d %d %f\n", conflitos[conflitoIndex-1].i, conflitos[conflitoIndex-1].j, conflitos[conflitoIndex-1].a, conflitos[conflitoIndex-1].b, conflitos[conflitoIndex-1].jaccardIndex);
                // FILE *arquivoEscrita = fopen("C:/OCII/PFCLPP/T2/escrita/c.txt", "a");
                // fprintf(arquivoEscrita, "%d %d %d %d %f\n", conflitos[conflitoIndex-1].i, conflitos[conflitoIndex-1].j, conflitos[conflitoIndex-1].a, conflitos[conflitoIndex-1].b, conflitos[conflitoIndex-1].jaccardIndex);
                // fclose(arquivoEscrita);
            }
        }
    }

    fclose(arquivo);
    return conflitos;
}

bool conflito_igual(Conflito c1, Conflito c2) {
    return c1.i == c2.i && c1.j == c2.j && c1.a == c2.a && c1.b == c2.b && c1.jaccardIndex == c2.jaccardIndex;
}

int remover_redundantes(Conflito listaConflitos[], int tamanho) {
    bool *redundantes = (bool*) malloc( tamanho * sizeof(bool) );
    for (int i = 0; i < tamanho; i++) {
        redundantes[i] = false; // Inicializa todos como não redundantes
    }

    for (int i = 0; i < tamanho; i++) {
        if (redundantes[i]) continue; // Pula se já marcado como redundante
        for (int j = i + 1; j < tamanho; j++) {
            if (listaConflitos[i].i == listaConflitos[j].j &&
                listaConflitos[i].j == listaConflitos[j].i &&
                listaConflitos[i].a == listaConflitos[j].b &&
                listaConflitos[i].b == listaConflitos[j].a &&
                listaConflitos[i].jaccardIndex == listaConflitos[j].jaccardIndex) {
                redundantes[j] = true;
            }
        }
    }

    // Cria um novo array sem os conflitos redundantes
    int novoTamanho = 0;
    for (int i = 0; i < tamanho; i++) {
        if (!redundantes[i]) {
            listaConflitos[novoTamanho++] = listaConflitos[i];
        }
    }

    return novoTamanho;
}

int* construtiva_aleatoria(int numPontos) {

    int *solucao = malloc(numPontos * sizeof(int));

    for (int i = 0; i < numPontos; i++) {
        solucao[i] = rand() % 4;
    }

    return solucao;
}

double calc_FO(int numConflitos, Conflito *conflitos, int *solucao) {
    // Nota: em "conflitos" i, j, a, b começam de "1",
    // Já em "solucao", i é representado pelas posições do vetor, que começa em "0".
    // Ainda em "solucao, as posições p começam de "0".

    double FO = 0;
    for( int c = 0; c < numConflitos; c++ ) {
        int i = conflitos[c].i;
        int j = conflitos[c].j;
        int a = conflitos[c].a - 1;
        int b = conflitos[c].b - 1;

        if( solucao[i] == a && solucao[j] == b ) {
            FO += conflitos[c].jaccardIndex;
        }
    }

    return FO;
}

void perturba_solucao(int numPontos, int* solucao, int maxPorCentoPerturbacoes) {
    // Pega um ponto "i" aleatório e troca sua posição por uma NOVA posição aleatória

    int porCentoPerturbacoes = rand() % maxPorCentoPerturbacoes;
    int numPontosPerturbados = ceil( numPontos * (0.01 * porCentoPerturbacoes) );

    for( int i = 0; i < numPontosPerturbados; i++ ) {

        int indexPonto = rand() % numPontos;
        int velhaPosicaoDoPonto = solucao[indexPonto];

        while(solucao[indexPonto] == velhaPosicaoDoPonto) {
            solucao[indexPonto] = rand() % 4;
        }
    }
}

void printa_solucao(int *solucao, int numPontos) {

    printf("Solucao:\n");
    for( int i = 0; i < numPontos; i++ ) {
        printf("%d ", solucao[i]);
    }

    printf("\n");
}

double simmulated_annealing(float taxaResfriamento, int SAMax, float temperaturaInicial, float temperaturaCongelamento, int *solucao, int numConflitos, Conflito *conflitos, int numPontos, int kPerturbacoes) {
    calc_FO(numConflitos, conflitos, solucao);

    int *solucaoMelhor = malloc(numPontos * sizeof(int));
    memcpy(solucaoMelhor, solucao, numPontos * sizeof(int));
    double FOMelhor = calc_FO(numConflitos, conflitos, solucaoMelhor);

    int iterT = 0;
    int temperatura = temperaturaInicial;
    
    while( temperatura > temperaturaCongelamento ) {

        while( iterT < SAMax ) {
            iterT++;

            // Gerar um vizinho
            int *solucaoDepois = malloc(numPontos * sizeof(int));
            memcpy(solucaoDepois, solucao, numPontos * sizeof(int));
            perturba_solucao(numPontos, solucaoDepois, kPerturbacoes);

            double FO = calc_FO(numConflitos, conflitos, solucao);
            double FODepois = calc_FO(numConflitos, conflitos, solucaoDepois);
            double delta = FODepois - FO;

            // Delta negativo: FODepois é menor (e melhor) que FO
            if( delta < 0 ) {
                solucao = solucaoDepois;

                if( FODepois < FOMelhor ) {
                    solucaoMelhor = solucaoDepois;
                    FOMelhor = FODepois;
                }
            }
            else {
                // Como RAND_MAX é o maior valor aleatório possível, dividir por RAND_MAX garante que o resultado seja entre 0 e 1
                double x = (double)rand() / RAND_MAX;
                double probabilidade = exp(-delta / temperatura);

                if( x < probabilidade ) {
                    solucao = solucaoDepois;
                }
            }
        }

        temperatura = taxaResfriamento * temperatura;
        iterT = 0;
    }

    return FOMelhor;
}

int main(int argc, char *argv[]) {

    // Início do relógio
    clock_t start = clock();

    if (argc < 6) {
        // Exemplo de uso: .\heuristic.exe h2_w24_58_all 0.99 200 10 0.01 50
        printf("Uso: %s <instancia> <taxaResfriamento> <SAMax> <temperaturaInicial> <temperaturaCongelamento> <kPerturbacoes>\n", argv[0]);
        // return 1;
    }

    /* Parâmetros recebidos por argumento na linha de comando */
    const char *instancia = argv[1];
    float taxaResfriamento = atof(argv[2]);  // Entre 0 e 1
    int SAMax = atoi(argv[3]); // Número máximo de iterações por temperatura
    float temperaturaInicial = atof(argv[4]); 
    float temperaturaCongelamento = atof(argv[5]);// Próxima a 0 (0.01, 0.001)
    int kPerturbacoes = atoi(argv[6]); // Número de pontos que eu perturbo a posição do label

    /* Descomente as linhas abaixo para setar os parâmetros em código. Facilita a debugar o algoritmo. */
    // const char *instancia = "h2_w24_58_all";
    // float taxaResfriamento = 0.99;  // Entre 0 e 1
    // int SAMax = 200; // Número máximo de iterações por temperatura
    // float temperaturaInicial = 10; 
    // float temperaturaCongelamento = 0.01;// Próxima a 0 (0.01, 0.001)
    // int kPerturbacoes = 50; // Máxima porcentagem de pontos a qual poderão ser perturbados a posição do label

    int N, P;
    char caminho_arquivo[100];
    snprintf(caminho_arquivo, sizeof(caminho_arquivo), "./Instances/%s.confl", instancia);

    srand(time(NULL));

    ler_N_P(caminho_arquivo, &N, &P);
    int numConflitosComRedundancia = ler_num_conflitos(caminho_arquivo, N, P);
    Conflito *conflitos = ler_coeficientes(numConflitosComRedundancia, caminho_arquivo, N, P);
    int numConflitos = remover_redundantes(conflitos, numConflitosComRedundancia);
    int *solucao = construtiva_aleatoria(N); 
    double FO = simmulated_annealing(taxaResfriamento, SAMax, temperaturaInicial, temperaturaCongelamento, solucao, numConflitos, conflitos, N, kPerturbacoes);

    // Fim do relógio
    clock_t end = clock();
    double tempo_exec = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%.6f %.6f\n", FO, tempo_exec);

    return 0;
}
