#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {PICCHE, FIORI, CUORI, QUADRI} Seme;
typedef struct{
    Seme seme;
    int valore;
    int stato;
}Carta;

typedef struct{
    char nome[20];
    int vite;
    Carta coperta;
    Carta scoperta;
}giocatore;

typedef struct{
    Carta carte[40];
    size_t index;    
}mazzo;

typedef struct{
    int vite_in_campo;
}campo;


int main(){
    size_t num_giocatori=0;
    printf("Numero Giocatori:");
    scanf("%zu", &num_giocatori);
    
    
}
