#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

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
    int stato; // 0=coperta 1=scoperta
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
    size_t first_player;
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

/*
Seeds the random number generator with the current time
Iterates through the deck from the last card to the first
For each card, selects a random position from the beginning up to the current position
Swaps the current card with the randomly selected one
Resets the deck's index to 0 so cards can be drawn from the beginning
*/
void mix_deck(mazzo *deck)
{
    // Fisher-Yates shuffle algorithm
    srand((unsigned int)time(NULL)); // Seed the random number generator

    for (size_t i = 39; i > 0; i--)
    {
        // Generate a random index between 0 and i (inclusive)
        size_t j = rand() % (i + 1);

        // Swap cards at positions i and j
        Carta temp = deck->carte[i];
        deck->carte[i] = deck->carte[j];
        deck->carte[j] = temp;
    }
    // Reset the deck index to start drawing from the beginning
    deck->index = 0;
}

void assign_card(mazzo deck, giocatore giocatori[], size_t num_giocatori)
{
    int j = 0;
    for (size_t i = 0; i < num_giocatori * 2; i++)
    {
        giocatori[j].coperta = deck.carte[i++];
        giocatori[j].scoperta = deck.carte[i];
        j++;
    }
}

void create_match(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    for (size_t i = 0; i < num_giocatori; i++)
    {
        printf("Nome Giocatore %zu:", i + 1);
        scanf("%s", giocatori[i].nome);
        giocatori[i].vite = 2;
    }
    campo->vite_in_campo = 0;
    create_deck(&deck);
    mix_deck(&deck);
    srand((unsigned int)time(NULL));
    campo->first_player = rand() % num_giocatori;
}

// phase part
void manage_effect(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
}

void manage_phase(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    for (size_t i = 0; i < num_giocatori; i++)
    {
        // risolvere effetto carta scoperta
        manage_effect(giocatori, num_giocatori, &campo, &deck);
        // gestire carta coperta
        if (giocatori[i].coperta.stato == 0)
        {
            char scelta;
            printf("Vuoi scopare (tua madre) la carta?(Rispondi con si o no)");
            scanf("%s", scelta);
            int c;
            do
            {
                c = 0;
                switch (scelta)
                {
                case 's':
                    manage_effect(giocatori, num_giocatori, &campo, &deck);
                    break;
                case 'n':

                    break;

                default:
                    printf("scelta errata");
                    c = 1;
                    break;
                }
            } while (c == 1);
        }
    }
}

int main()
{
    size_t num_giocatori = 0;
    printf("Numero Giocatori:");
    scanf("%zu", &num_giocatori);
    giocatore giocatori[num_giocatori];
    campo match_campo;
    mazzo deck;
    create_match(giocatori, num_giocatori, &match_campo, &deck);
}
