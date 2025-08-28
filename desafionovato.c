#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// definindo extrutura
typedef struct {
	char nome[30];
	char cor[10];
	int tropas;
} Territorio;


//declaração do vetor de structs

int main(){

    Territorio estrutura[5]; //vetor
    int i = 0;
//criar um vetor pra armazenar 5 territorios
///deve cadastrar os 5 teritorios
//exibir a lista completa
	
//estrada de dados
printf("---CADASTRO DE TERRITORIOS---\n");
//loop
for (i = 0; i < 5; i++) { 
	printf("\nCadastro do territorio %d: \n", i+1);
	

	printf("Digite o nome do territorio: ");
	scanf("%s", estrutura[i].nome);
	
	printf("Digite a cor do exército: ");
	scanf("%s", estrutura[i].cor);
	
	printf("Digite o numero de tropas: ");
	scanf("%d", &estrutura[i].tropas);

}
//exibir territorios
printf("\n--- TERRIRTORIOS CADASTRADOS---\n");
for (i = 0; i < 5; i++) {
	  printf("\nTerritório %d:\n", i + 1);
        printf(" Nome: %s\n", estrutura[i].nome);
        printf(" Cor do exército: %s\n", estrutura[i].cor);
        printf(" Tropas: %d\n", estrutura[i].tropas);
}
return 0;

}


