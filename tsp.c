#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "lerArquivos.h"

#define FLOAT_TO_INT(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

// Simulated Annealing 

int temperature;
int iteration;

int objetivo (int * sol, int **mat, int t){
	int cont = 0;
	for (int i = 0; i < t-1; i++){
		
		cont += mat[sol[i]][sol[i+1]];
	}
	return cont;
}

int simulatedAnnealing(int delta, int **mat, int t){
	double deltatemp = -1 * delta/temperature;
	double p = exp(deltatemp);

	if (p < 1) return 0;
	else return 1;
}

int *geraSolInicialRandom(int t){

	int posicionou = 0;

	int *v = (int *) malloc (t* sizeof(int));
	for (int i=t-1; i >= 0; i--){

		while(posicionou == 0){
			int num = (rand() % (t-1 + 1 - 0) + 0);

			if (v[num] == 0){
				v[num] = i;
				posicionou = 1;
			}
		}
		posicionou = 0;
	}
	return v;
}

int *geraSolInicial(int t) {

	int *v = (int *) malloc (t* sizeof(int));
	for (int i=0; i< t; i++){
		v[i] = i;
	}
	return v;
}

// Realiza o Swap completo e retorna o melhor vizinho
int *bestNeighbour(int **m, int t, int *s){ 

	int *vizinhanca = (int *) malloc (t* sizeof(int));
	int *melhorVizinhanca = (int *) malloc (t* sizeof(int));
	int VizinhoTrocado;
	int contIteration = 0;

	for (int h=0; h<t; h++)
		melhorVizinhanca[h] = s[h];

	// pega um valor do vetor original
	for (int i=0; i<t; i++){

		VizinhoTrocado = s[i];

		// faz a troca desse valor em cada uma das posicoes do vetor
		for (int j=0; j<t; j++){

			contIteration++;
			if (contIteration == iteration){
				temperature -= 1; // diminui valor da temperatura a iterações do numero de cidades
				contIteration = 0;
				if(temperature == 0) break;
			}

			// reinicia o vetor a cada loop de troca 
			for (int h=0; h<t; h++)
				vizinhanca[h] = s[h];

			// Faz a troca sem repetição (>)
			// Faz a funcao objetiva do vetor original uma única vez (j=i && i=0)
			if (j > i || (j==i && i==0)){ 	
				
				vizinhanca[i] = vizinhanca[j];
				vizinhanca[j] = VizinhoTrocado;
				int objVizinho = objetivo(vizinhanca,m,t);
				int objMelhorVizinho = objetivo(melhorVizinhanca,m,t);
				// Dado o vetor de vizinhos criado, compara com a melhor vizinhança

				if (objVizinho <= objMelhorVizinho){
					for (int i = 0; i < t; i++){
						melhorVizinhanca[i] = vizinhanca[i];
					}
				} else {
					//chama simulated pra ver se aceita com a probabilidade menor
					int delta = objVizinho - objMelhorVizinho;
					if(simulatedAnnealing(delta, m, t)){
						for (int i = 0; i < t; i++){
							melhorVizinhanca[i] = vizinhanca[i];
						}
					}
				}
			}
		}
		if(temperature == 0) break;
	}

	free(vizinhanca);
	return melhorVizinhanca;
}

// Genetic Algorithm
int *bubbleSort(int *v, int tam){ 
  	int aux; 

  	for(int j=tam-1; j >= 0; j--) { 
    	for(int i=0; i < j; i++) { 
			if(v[i] > v[i+1]) { 
				aux=v[i]; 
				v[i]=v[i+1]; 
				v[i+1]=aux; 		
			} 
    	} 
  	} 

  	return v;
}

// Sorteia os dois pais dentro da proporção de sua solução
int *roulette(int objindividuo1, int objindividuo2, int objindividuo3, int objindividuo4){

	int vet[4] = {objindividuo1, objindividuo2, objindividuo3, objindividuo4};
	int *ordenadoInv = bubbleSort(vet, 4);	// vetor invertidamente ordenado

	int soma = 0;
	for (int i=0; i<4; i++) {
		soma += ordenadoInv[i];
	}

	// encontra a percentagem de participação de cada valor da solução para o próximo filho
	float *prob = (float *) malloc (4* sizeof(float));
	for(int i = 0; i < 4; i++){
		prob[3-i] = FLOAT_TO_INT((ordenadoInv[i]*100.0)/soma);
	}

	// verifica se passa do numero maximo de cidades (ou é menor)
	int somaInt = 0;
	for(int i=0; i<4; i++){
		somaInt += prob[i];
	}
	if (somaInt > 100)
		prob[0] -= 1;
	else if (somaInt < 100)
		prob[0] += 1;

	// Faz o range de cada proporção
	int *rangeInRoulette = (int *) malloc (4* sizeof(int));
	int num = 0;
	for (int i=0; i<4; i++){
		num += prob[i];
		rangeInRoulette[i] = num;
	}
	
	// Sorteia 
	int* parents = (int *) malloc (2* sizeof(int));
	int numParents = 0;
	while(numParents < 2){
		int sorte = (rand() % (100 + 1 - 0) + 0);

		for(int i=0; i<4; i++){
			if (i==0){
				if (sorte < rangeInRoulette[i]) {
					// evita repetir o pai
					if(numParents == 0){
						parents[0] = 0;
						numParents ++;
						break;
					} else {
						if (parents[0] != 0){
							parents[1] = 0;
							numParents ++;
							break;
						}
					}
				}
			} else {
				if ((sorte > rangeInRoulette[i-1]) && (sorte < rangeInRoulette[i])){
					// evita repetir o pai
					if(numParents == 0){
						parents[0] = i;
						numParents ++;
						break;
					} else {
						if (parents[0] != i){
							parents[1] = i;
							numParents ++;
							break;
						}
					}
					
				}
			}
		}

	}

	return parents;
}	


int* crossoverOrdem1(int t, int *pai1, int *pai2){

    int init = (rand() % (t/2 + 1 - 0) + 0);
    int final = init+t/2;

	int* filho = (int*) malloc (t *sizeof(int));

	// copia a parte do pai 1
    for(int i = 0; i < t/2; i ++){
        filho[init + i] = pai1[init + i];
    }

	int isEqual = 0;
	int finalFilho = final;
	int indexFilho = 0;
	int finalPai = final;
	int indexPai = 0;

	// copiando a parte do pai 2
    for(int i = 0; i < t; i ++){

		// quando chega no final do vetor, retorna para preencher o inicio
			if (finalFilho + indexFilho == t){
				finalFilho = 0;
				indexFilho = 0;
			}

			if (finalPai + i == t){
				finalPai = 0;
				indexPai = 0;
			}

			if(finalFilho + indexFilho == init)
				break;

		// verifica se o pai tem valor repetido aos valores ja copiados do pai 1
        for(int j = init; j < final ; j++){
            if(pai1[j] == pai2[finalPai + indexPai]){
				isEqual = 1;
				break;
            }
        }

		if(!isEqual){

			filho[finalFilho + indexFilho] = pai2[finalPai + indexPai];
			indexFilho ++;

		} else {
			isEqual = 0;
		}	

		indexPai++;
    }

    return filho;
}

int* Mutacao(int t, int* indiv){

	int rand1 = 0, rand2 = 0, aux = 0;


	do {
		rand1 = (rand() % (t - 0) + 0);
		rand2 = (rand() % (t - 0) + 0);

		if(rand1 != rand2)
			break;

	} while(rand1 == rand2);


	int* v = (int*) malloc (t *sizeof(int));

	for(int i=0;i<t;i++){
		v[i] = indiv[i];
	}

	aux = v[rand1];
	v[rand1] = v[rand2];
	v[rand2] = aux;

	return v;

}


int main(int argc, char *argv[]) {
	int **matrizDistancias; // Matriz contendo a distância entre as cidades
	int quantidadeCidades = 0; // Numero de cidades envolvidas no arquivo
	int *solCorrente;
	int metaheuristica = 0;
	clock_t Ticks[2];
    printf("1 - Simulated Annealing\n2 - Genetic Algorithm\n");
	printf("\nEscolha o método que deseja utilizar para resolver o Problema do Caxeiro Viajante: ");
    scanf("%d", &metaheuristica);
	printf("\n");


	// faz a leitura, recebe matriz de distâncias do arquivo escolhido e le a quantidade de cidades envolvidas no arquivo
	escolherArquivo();
	matrizDistancias = leMatrizdistancias();
	quantidadeCidades = leNumeroCidades();

	if(metaheuristica == 1){ //SIMULATED ANNEALING
		temperature = quantidadeCidades * 5;
		iteration = pow(quantidadeCidades,2);

	    Ticks[0] = clock();

	    solCorrente = geraSolInicial(quantidadeCidades);

	    while (temperature > 0){

	    	solCorrente = bestNeighbour(matrizDistancias, quantidadeCidades, solCorrente);

			printf("\nobjetivo: %d ", objetivo(solCorrente,matrizDistancias,quantidadeCidades));
	    }
		// Clock da CPU
	    Ticks[1] = clock();

	}
	else if(metaheuristica == 2){ //GENECTIC ALGORITHM
		
		Ticks[0] = clock();
		int *solInit1, *solInit2, *solInit3, *solInit4;
		int resInit1, resInit2, resInit3, resInit4;
		solInit1 = geraSolInicialRandom(quantidadeCidades);
		solInit2 = geraSolInicialRandom(quantidadeCidades);
		solInit3 = geraSolInicialRandom(quantidadeCidades);
		solInit4 = geraSolInicialRandom(quantidadeCidades);

		int cont = 0;

		while(cont < quantidadeCidades * quantidadeCidades){

			resInit1 = objetivo(solInit1, matrizDistancias, quantidadeCidades);
			resInit2 = objetivo(solInit2, matrizDistancias, quantidadeCidades);
			resInit3 = objetivo(solInit3, matrizDistancias, quantidadeCidades);
			resInit4 = objetivo(solInit4, matrizDistancias, quantidadeCidades);

			int nullpai = 0;
			int *pai1, *pai2;
			pai1 = (int *) malloc (quantidadeCidades *sizeof(int));
			pai2 = (int *) malloc (quantidadeCidades *sizeof(int));

			int *index = roulette(resInit1, resInit2, resInit3, resInit4);

			if(index[0] == 0 || index[1] == 0){
				if (nullpai == 0){
					nullpai = 1;
					for(int i = 0; i < quantidadeCidades;i++)
						pai1[i] = solInit1[i];
				}
				else{
					for(int i = 0; i < quantidadeCidades;i++)
						pai2[i] = solInit1[i];
				}
			}
			if(index[0] == 1 || index[1] == 1){
				if (nullpai == 0){
					nullpai = 1;
					for(int i = 0; i < quantidadeCidades;i++)
						pai1[i] = solInit2[i];
				}
				else{
					for(int i = 0; i < quantidadeCidades;i++)
						pai2[i] = solInit2[i];
				}
			}
			if(index[0] == 2 || index[1] == 2){
				if (nullpai == 0){
					nullpai = 1;
					for(int i = 0; i < quantidadeCidades;i++)
						pai1[i] = solInit3[i];
				}
				else{
					for(int i = 0; i < quantidadeCidades;i++)
						pai2[i] = solInit3[i];

				}
			}
			if(index[0] == 3 || index[1] == 3){
				if (nullpai == 0){
					nullpai = 1;
					pai1 = solInit4;
				}
				else{
					pai2 = solInit4;
				}
			}

			int *filho1 = crossoverOrdem1(quantidadeCidades, pai1, pai2);
			int *filho2 = crossoverOrdem1(quantidadeCidades, pai2, pai1);

			filho1 = Mutacao(quantidadeCidades,filho1);
			filho2 = Mutacao(quantidadeCidades,filho2);

			solInit1 = pai1;
			solInit2 = pai2;
			solInit3 = filho1;
			solInit4 = filho2;

			cont++;
		}

		if(resInit1 > resInit2 && resInit1 > resInit3 && resInit1 > resInit4){
			solCorrente = solInit1;
		} else if (resInit2 > resInit1 && resInit2 > resInit3 && resInit2 > resInit4){
			solCorrente = solInit2;
		} else if (resInit3 > resInit1 && resInit3 > resInit2 && resInit3 > resInit4){
			solCorrente = solInit3;
		} else if (resInit4 > resInit1 && resInit4 > resInit2 && resInit4 > resInit3){
			solCorrente = solInit4;
		}

		//Clock da CPU
	    Ticks[1] = clock();
	} 
	else 
		printf("Valor indisponivel\n");
	

	printf("\n\nObjetivo da melhor solução : %d\n", objetivo(solCorrente,matrizDistancias, quantidadeCidades));
	puts("Melhor Solução: ");
	for (int i = 0; i < quantidadeCidades;i++){
		printf("%d ", solCorrente[i]);
	}
	
	double Tempo = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;
	printf("\nTempo gasto: %g ms.\n", Tempo);

	// Fecha o arquivo lido
	fechaArquivo();
	return 0;
}
