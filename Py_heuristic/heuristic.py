import pprint
import numpy as np

def ler_N_P(caminho_arquivo):

    with open(caminho_arquivo, "r") as arquivo:
        N = int(arquivo.readline().strip())
        P = int(arquivo.readline().strip())
    return N, P

def ler_coeficientes(caminho_arquivo, listaConflitos, N, P):

    with open(caminho_arquivo, "r") as arquivo:
        
        # Ignora as duas primeiras linhas já lidas em ler_N_P()
        arquivo.readline()
        arquivo.readline()

        for i in range(1, N + 1):
            for a in range(1, P + 1):
                
                numConflitos = int( arquivo.readline().strip() )
                linhaEuclidiana = arquivo.readline()    # Linha da distância euclidiana ignorada
                linhaJaccard = arquivo.readline().strip().split()

                # Essa groselha toda só pra percorrer a linha de Jaccard de 2 em 2
                for conflito in range( 0, numConflitos * 2, 2 ):
                    idLabelEmConflito = int( linhaJaccard[conflito] )
                    jaccardIndex = float( linhaJaccard[conflito + 1] )

                    if idLabelEmConflito % P != 0:
                        j = int( np.ceil( idLabelEmConflito / P ) )
                    else:
                        j = int( idLabelEmConflito / P ) + 1
                    
                    parte_decimal, _ = np.modf( idLabelEmConflito / P )

                    for p in range(1, P + 1):
                        if parte_decimal * (P/p) == 1:
                            b = p + 1
                            listaConflitos.append([i,j,a,b,jaccardIndex])
                        elif parte_decimal == 0:
                            b = 1
                            listaConflitos.append([i,j,a,b,jaccardIndex])
                    
                    print(f"{i} {j} {a} {b} {jaccardIndex}")

def remover_duplicatas(listaConflitos):

    semDuplicatas = []

    for conflito in listaConflitos:
        if conflito not in semDuplicatas:
            semDuplicatas.append(conflito)

    return semDuplicatas

# Removendo conflitos que não são duplicatas mas são redundantes
# Ex: [1, 3, 2, 4, 0.25] e [3, 1, 4, 2, 0.25]
def remover_redundantes(listaConflitos):

    indexEquivalentes = []

    for index1 in range(0, len(listaConflitos) ):
        for index2 in range(index1 + 1, len(listaConflitos) ):
            if listaConflitos[index1][0] == listaConflitos[index2][1] and listaConflitos[index1][1] == listaConflitos[index2][0] and listaConflitos[index1][2] == listaConflitos[index2][3] and listaConflitos[index1][3] == listaConflitos[index2][2]:
                indexEquivalentes.append(index2)

    indexEquivalentes.sort(reverse = True)

    for index in indexEquivalentes:
        listaConflitos.pop(index)

    return listaConflitos

# Configuração
nome_instancia = "h2_w24_58_all"
caminho_arquivo = f"../Instances/{nome_instancia}.confl"
arquivo_saida = f"../Model/{nome_instancia}.lp"

# Parâmetros e lista auxiliar de conflitos
N, P = ler_N_P(caminho_arquivo)
listaConflitos = []

ler_coeficientes(caminho_arquivo, listaConflitos, N, P)
listaConflitos = remover_duplicatas(listaConflitos)
listaConflitos = remover_redundantes(listaConflitos)

pprint.pp(listaConflitos)
print(len(listaConflitos))