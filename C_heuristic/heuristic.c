#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// Define a struct Conflito
typedef struct {
    int i;
    int j;
    int a;
    int b;
    float jaccardIndex;
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

void ler_coeficientes(const char *caminho_arquivo, Conflito *conflitos, int *numConflitosGerados, int N, int P) {
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
    char bufferDescartavel[256];

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
                    if (fabs(parte_decimal * ((float)P / (float)p) - 1.0) < 1e-6) {  // Compara com tolerância para evitar problemas de ponto flutuante
                        int b = p + 1;
                        conflitos[conflitoIndex].i = i;
                        conflitos[conflitoIndex].j = j;
                        conflitos[conflitoIndex].a = a;
                        conflitos[conflitoIndex].b = b;
                        conflitos[conflitoIndex].jaccardIndex = (float)jaccardIndex;
                        conflitoIndex++;
                    } else if (parte_decimal == 0) {  // Caso seja exato
                        int b = 1;
                        conflitos[conflitoIndex].i = i;
                        conflitos[conflitoIndex].j = j;
                        conflitos[conflitoIndex].a = a;
                        conflitos[conflitoIndex].b = b;
                        conflitos[conflitoIndex].jaccardIndex = (float)jaccardIndex;
                        conflitoIndex++;
                    }
                }

                // printf("%d\n", conflitoIndex);
                // printf("%d %d %d %d %f\n", conflitos[conflitoIndex-1].i, conflitos[conflitoIndex-1].j, conflitos[conflitoIndex-1].a, conflitos[conflitoIndex-1].b, conflitos[conflitoIndex-1].jaccardIndex);
            }
        }
    }

    *numConflitosGerados = conflitoIndex;  // Atualiza o total de conflitos gerados
    fclose(arquivo);
}

bool conflito_igual(Conflito c1, Conflito c2) {
    return c1.i == c2.i && c1.j == c2.j && c1.a == c2.a && c1.b == c2.b && c1.jaccardIndex == c2.jaccardIndex;
}

int remover_duplicatas(Conflito listaConflitos[], int tamanho, Conflito resultado[]) {
    int novoTamanho = 0;

    for (int i = 0; i < tamanho; i++) {
        bool encontrado = false;
        for (int j = 0; j < novoTamanho; j++) {
            if (conflito_igual(listaConflitos[i], resultado[j])) {
                encontrado = true;
                break;
            }
        }
        if (!encontrado) {
            resultado[novoTamanho++] = listaConflitos[i];
        }
    }
    return novoTamanho;
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

int main() {
    const char *nome_instancia = "h2_w24_58_all";
    char caminho_arquivo[100];
    snprintf(caminho_arquivo, sizeof(caminho_arquivo), "../Instances/%s.confl", nome_instancia);

    int N, P;
    ler_N_P(caminho_arquivo, &N, &P);

    Conflito conflitos[2000];  // Capacidade máxima arbitrária para os conflitos
    int numConflitosGerados = 0;

    ler_coeficientes(caminho_arquivo, conflitos, &numConflitosGerados, N, P);

    Conflito *conflitosUnicos = (Conflito*) malloc( numConflitosGerados * sizeof(Conflito) );
    int tamanhoSemDuplicatas = remover_duplicatas(conflitos, numConflitosGerados, conflitosUnicos);
    int tamanhoFinal = remover_redundantes(conflitosUnicos, tamanhoSemDuplicatas);

    // Exibe a lista de conflitos
    for (int i = 0; i < tamanhoFinal; i++) {
        printf("[%d, %d, %d, %d, %.6f]\n",
               conflitosUnicos[i].i, conflitosUnicos[i].j,
               conflitosUnicos[i].a, conflitosUnicos[i].b,
               conflitosUnicos[i].jaccardIndex);
    }
    printf("\nTotal de conflitos gerados: %d\n", tamanhoFinal);

    return 0;
}