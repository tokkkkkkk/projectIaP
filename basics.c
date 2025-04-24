#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

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
            mazzo->carte[i * 10 + j].stato = 0;
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
            // printf("%c", mazzo->carte[i * 10 + j].valore);
            // printf(" di %s\n", (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[mazzo->carte[i * 10 + j].seme]);
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

void assign_card(mazzo *deck, giocatore giocatori[], size_t num_giocatori)
{
    int j = 0;
    for (size_t i = 0; i < num_giocatori * 2; i++)
    {
        giocatori[j].coperta = deck->carte[i++];
        giocatori[j].coperta.stato = 0; // Face down card
        giocatori[j].scoperta = deck->carte[i];
        giocatori[j].scoperta.stato = 1; // Face up card
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
    create_deck(deck);
    campo->first_player = rand() % num_giocatori;
}

// phase part

void print_field(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    printf("Vite nel campo: %d\n", campo->vite_in_campo);
    for (size_t i = 0; i < num_giocatori; i++)
    {
        printf("Vite giocatore %s: %d\n", giocatori[i].nome, giocatori[i].vite);
        printf("Carta scoperta: %c di %s\n",
               giocatori[i].scoperta.valore,
               (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[giocatori[i].scoperta.seme]);

        if (giocatori[i].coperta.stato)
        {
            printf("Carta coperta: %c di %s\n",
                   giocatori[i].coperta.valore,
                   (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[giocatori[i].coperta.seme]);
        }
        printf("\n");
    }
}

void check_hp(giocatore giocatori[], size_t *num_giocatori, size_t index)
{
    if (giocatori[index].vite == 0)
    {
        printf("Giocatore %s è morto\n", giocatori[index].nome);

        // Save the player to remove
        giocatore removed = giocatori[index];

        // Shift all elements after index one position to the left
        for (size_t i = index; i < (*num_giocatori - 1); i++)
        {
            giocatori[i] = giocatori[i + 1];
        }

        // Place the removed player at the end
        giocatori[*num_giocatori - 1] = removed;

        // Decrement the count of active players
        (*num_giocatori)--;
    }
}

void manage_effect(giocatore giocatori[], size_t *num_giocatori, campo *campo, int index, Carta carta)
{
    bool ultimo = false;
    if (index == (*num_giocatori) - 1)
    {
        ultimo = true;
    }

    switch (carta.valore)
    {
    case '1':
        giocatori[index].vite -= 1;
        campo->vite_in_campo += 1;
        break;
    case '7':
        if (ultimo)
            index = -1;
        if (giocatori[index + 1].coperta.stato == 0)
        {
            giocatori[index + 1].coperta.stato = 1;
            manage_effect(giocatori, num_giocatori, campo, index + 1, giocatori[index + 1].coperta);
        }
        else
        {
            printf("carta già scoperta");
        }
        break;
    case 'J':
        giocatori[index].vite -= 1;
        giocatori[index - 1].vite += 1;
        break;
    case 'Q':

        break;
    case 'K':
        /* code */
        break;
    default:
        break;
    }
}

void manage_phase(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    mix_deck(deck);
    assign_card(deck, giocatori, num_giocatori);
    for (size_t i = 0; i < num_giocatori; i++)
    {
        print_field(giocatori, num_giocatori, campo, deck);
        // risolvere effetto carta scoperta
        printf("Risolvi effetto carta scoperta maledetto cancaro");
        manage_effect(giocatori, &num_giocatori, campo, i, giocatori[i].scoperta);
        // gestire carta coperta
        if (giocatori[i].coperta.stato == 0)
        {
            int c;
            do
            {
                char scelta;
                printf("Vuoi scoprire la carta?(s/n)\n");
                scanf(" %c", &scelta);
                c = 0;
                switch (scelta)
                {
                case 's':
                    giocatori[i].coperta.stato = 1;
                    manage_effect(giocatori, &num_giocatori, campo, i, giocatori[i].coperta);
                    break;
                case 'n':
                    // end turn
                    break;

                default:
                    printf("scelta errata\n");
                    c = 1;
                    break;
                }
            } while (c == 1);
        }
        check_hp(giocatori, &num_giocatori, i);//maybe sbagliato
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
    manage_phase(giocatori, num_giocatori, &match_campo, &deck);
}
