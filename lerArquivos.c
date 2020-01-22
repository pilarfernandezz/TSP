#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lerArquivos.h"

// Programa responsável pela leitura dos aquivos e retorno das distâncias entre cidades

FILE *file; // arquivo a ser lido com as cidades e suas respectivas distâncias
int** matDist; // Matriz contendo as distâncias entre as cidades
int quantidadeCidades; // Numero de cidades contidas no arquivo

char textString[30];    // texto do arquivo convertido em string

void leMatrizInferior();
void leMatrizSuperior();

// Permite o usuário escolher qual arquivo ele deseja ler
void escolherArquivo() {
    int n;

    printf("\nArquivos disponiveis:\n1 - brazil58.tsp\n2 - gr24.tsp\n3 - gr120.tsp\n4 - hk48.tsp\n5 - si175.tsp\n");
	printf("\nEscolha o arquivo que deseja ler: ");
    scanf("%d", &n);
	printf("\n");

	if(n == 1)      file = fopen("brazil58.tsp","rt");
    else if(n == 2) file = fopen("gr24.tsp","rt"); 
    else if(n == 3) file = fopen("gr120.tsp","rt"); 
    else if(n == 4) file = fopen("hk48.tsp","rt"); 
    else if(n == 5) file = fopen("si175.tsp","rt");
    else {
    	puts("Arquivo não existente");
    	exit(1);
    }
}

// Retorna um ponteiro para a matriz de distancia entre cidades
int** leMatrizdistancias(){

    // Verifica se não há erro na leitura 
    if(file == NULL) {
		printf("Erro na leitura do arquivo\n");
		exit(1);
	}

    // recebe o número de cidades contidas no problema
    while(quantidadeCidades == 0) { 
		fscanf(file,"%s",&textString);
		if(strcmp(textString, "DIMENSION:") == 0) {
			fscanf(file,"%d",&quantidadeCidades);
		}
	}

    // Aloca espaço para a matriz de distancia entre cidades
	matDist = (int **) malloc(quantidadeCidades * sizeof(int**)); 
	for(int i = 0; i < quantidadeCidades; i++) {
		matDist[i] = (int*) malloc(quantidadeCidades * sizeof(int));
		for(int j = 0; j < quantidadeCidades; j++) {
			matDist[i][j] = 0;
		}
	}

    // verifica o formato que a matriz está apresentada
	while(strcmp(textString,"EDGE_WEIGHT_FORMAT:") != 0) 
		fscanf(file,"%s",&textString);

    int cont = ((quantidadeCidades * quantidadeCidades)/2) + (quantidadeCidades/2);

    fscanf(file,"%s",&textString);
	if (strcmp(textString,"LOWER_DIAG_ROW") == 0 || strcmp(textString,"LOWER_ROW") == 0) { 
        leMatrizInferior(cont);

    } else if (strcmp(textString,"UPPER_DIAG_ROW") == 0 || strcmp(textString,"UPPER_ROW") == 0) {
        leMatrizSuperior(cont);
    }

    return matDist;
}

int leNumeroCidades(){

    return quantidadeCidades;
}

void fechaArquivo(){
    fclose(file);
}

// Escolhe qual forma de leitura será feita da matriz de acordo com o formato dela no arquivo
void leMatrizInferior(int cont){
    int i = 0;
    int j = 0;

    while(strcmp(textString,"EDGE_WEIGHT_SECTION") != 0) 
		fscanf(file,"%s",&textString);

    while(cont > 0) { 
        fscanf(file,"%d",&matDist[i][j]);
        matDist[j][i] = matDist[i][j];

        if(matDist[i][j] == 0) {
            i++;
            j = 0;
        } else {
            j++;
        }
        cont--;
    }
}

void leMatrizSuperior(int cont){
    int i = 0;
    int j = 0;

    int hasZeros = 0;

    fscanf(file,"%s",&textString);
    if (strcmp(textString,"DISPLAY_DATA_TYPE:") == 0){
        hasZeros = 1;
    }

    while(strcmp(textString,"EDGE_WEIGHT_SECTION") != 0) 
		fscanf(file,"%s",&textString);

    while(cont > 0) {  

        if(i > j) {
            if (j < quantidadeCidades){
                j++;
            }

        } else if (j == quantidadeCidades) {
            i++;
            j = 0;

        } else {

            if (hasZeros){
                
                fscanf(file,"%d",&matDist[i][j]);
                matDist[j][i] = matDist[i][j];

            } else {

                if(i==j){
                    matDist[j][i] = 0;
                } else {
                    fscanf(file,"%d",&matDist[i][j]);
                    matDist[j][i] = matDist[i][j];
                }
            }

            cont--;
            j++;
        }

    }

}