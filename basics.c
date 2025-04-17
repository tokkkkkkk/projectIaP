#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum
{
    PICCHE,
    FIORI,
    CUORI,
    QUADRI
} Seme;
typedef struct
{
    Seme seme;
    char valore;
    int stato;
} Carta;

typedef struct
{
    char nome[20];
    int vite;
    Carta coperta;
    Carta scoperta;
} giocatore;

typedef struct
{
    Carta carte[40];
    size_t index;
} mazzo;

typedef struct
{
    int vite_in_campo;
} campo;

void create_deck(mazzo *mazzo)
{
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 10; j++)
        {
            mazzo->carte[i * 10 + j].seme = i;
            switch (j)
            {
            case 9:
                mazzo->carte[i * 10 + j].valore = 'K';
                break;
            case 8:
                mazzo->carte[i * 10 + j].valore = 'Q';
                break;
            case 7:
                mazzo->carte[i * 10 + j].valore = 'J';
                break;

            default:
                mazzo->carte[i * 10 + j].valore = '0' + j + 1;
                break;
            }
            printf("%c", mazzo->carte[i * 10 + j].valore);
            printf(" di %s\n", (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[mazzo->carte[i * 10 + j].seme]);
        }
    }
}

void create_match(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo deck)
{
    for (size_t i = 0; i < num_giocatori; i++)
    {
        printf("Nome Giocatore %zu:", i + 1);
        scanf("%s", giocatori[i].nome);
        giocatori[i].vite = 2;
    }
    campo->vite_in_campo = 0;
    create_deck(&deck);
}

int main()
{
    size_t num_giocatori = 0;
    printf("Numero Giocatori:");
    scanf("%zu", &num_giocatori);
    giocatore giocatori[num_giocatori];
    campo match_campo;
    mazzo deck;
    create_match(giocatori, num_giocatori, &match_campo, deck);
}
