#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void delay_ms(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000); // usleep accetta microsecondi
#endif
}

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
            mazzo->carte[i * 10 + j].seme = (Seme)i;
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
        }
    }
}

/*
imposta il generatore di numeri casuali con l'ora corrente
Scorre il mazzo dall'ultima carta alla prima
Per ogni carta, seleziona una posizione casuale dall'inizio fino alla posizione corrente
Scambia la carta corrente con quella selezionata casualmente
Reimposta l'indice del mazzo a 0 in modo che le carte possano essere pescate dall'inizio
*/
void mix_deck(mazzo *deck)
{
    // algoritmo di Fisher-Yates per il mescolamento
    srand((unsigned int)time(NULL)); // imposta il generatore di numeri casuali

    for (size_t i = 39; i > 0; i--)
    {
        // Genera un indice casuale tra 0 e i (inclusi)
        size_t j = rand() % (i + 1);

        //Scambia le carte nelle posizioni i e j
        Carta temp = deck->carte[i];
        deck->carte[i] = deck->carte[j];
        deck->carte[j] = temp;
    }
    // Reimposta l'indice del mazzo per iniziare a distribuire le carte dall'inizio
    deck->index = 0;
}

void assign_card(mazzo *deck, giocatore giocatori[], size_t num_giocatori)
{
    int j = 0;
    for (size_t i = 0; i < num_giocatori * 2; i++)
    {
        giocatori[j].coperta = deck->carte[i++];
        giocatori[j].coperta.stato = 0; // carta coperta
        giocatori[j].scoperta = deck->carte[i];
        giocatori[j].scoperta.stato = 1; // carta scoperta
        j++;
    }
}

void create_match(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    for (size_t i = 0; i < num_giocatori; i++)
    {
        printf("\n Nome Giocatore %zu:", i + 1);
        scanf("%s", giocatori[i].nome);
        giocatori[i].vite = 2;
    }
    campo->vite_in_campo = 1;
    create_deck(deck);
    campo->first_player = rand() % num_giocatori;
}

// gestione delle fasi di gioco

void print_field(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    printf("\n----------------------------------------\n");
    printf("\n Vite nel campo: %d\n", campo->vite_in_campo);
    printf("\n----------------------------------------\n");
    for (size_t i = 0; i < num_giocatori; i++)
    {
        printf("\n **Giocatore**: %s\n", giocatori[i].nome);
        printf(" Vite: %d\n", giocatori[i].vite);
        printf(" Carta scoperta: %c di %s\n",
               giocatori[i].scoperta.valore,
               (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[giocatori[i].scoperta.seme]);

        if (giocatori[i].coperta.stato)
            printf(" Carta coperta: %c di %s\n",
                   giocatori[i].coperta.valore,
                   (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[giocatori[i].coperta.seme]);
        printf("\n");
        printf("----------------------------------------\n");
        delay_ms(2500);
    }
    printf("\n");
}

void remove_player(giocatore giocatori[], size_t *num_giocatori, size_t index)
{
    if (giocatori[index].vite == 0)
    {
        printf(" **Giocatore %s è morto!**\n", giocatori[index].nome);

        // salva il giocatore da rimuovere
        giocatore removed = giocatori[index];

        // Sposta tutti gli elementi dopo l'indice di una posizione a sinistra
        for (size_t i = index; i < (*num_giocatori - 1); i++)
        {
            giocatori[i] = giocatori[i + 1];
        }

        // posiziona il giocatore da rimuovere alla fine
        giocatori[*num_giocatori - 1] = removed;

        // decrementa il numero di giocatori in gioco
        (*num_giocatori)--;
        printf(" Il giocatore è stato rimosso dal gioco.\n");
    }
}

bool is_alive(giocatore g)
{
    return g.vite > 0;
}

bool warning_hp(giocatore giocatori[], size_t *num_giocatori, size_t index)
{
    char scelta = 's';
    if (giocatori[index].vite == 1)
    {
        printf("\n Questa azione azzererà le tue vite, vuoi continuare? [s/n]");
        scanf(" %c", &scelta);
    }
    if (scelta == 's')
        return true;

    return false;
}

bool warning_hp_reveal(giocatore giocatori[], size_t index, Carta carta) {
    char scelta = 's';
    if (giocatori[index].vite == 1 && carta.stato == 0) {
        printf("\n Questa azione azzererà le tue vite, vuoi continuare? [s/n]");
        scanf(" %c", &scelta);
    }
    return scelta == 's';
}

void manage_effect(giocatore giocatori[], size_t *num_giocatori, campo *campo, int index, Carta carta)
{
    bool ultimo = false;
    if (index == (*num_giocatori) - 1)
    {
        ultimo = true;
    }
    if (!is_alive(giocatori[index]))
        return;

    printf("\n Risolvendo l'effetto della carta: %c di %s\n", carta.valore,
           (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[carta.seme]);

    switch (carta.valore)
    {
    case '1':
    {

        if (carta.stato == 0)
        {
            bool scelta = warning_hp_reveal(giocatori, index, carta);
            if (scelta)
            {
                giocatori[index].vite -= 1;
                campo->vite_in_campo += 1;
            }
            else
            {
                printf("\n Azione annullata. La carta non è stata rivelata.\n");
                return;
            }
        }
        else
        {
            giocatori[index].vite -= 1;
            campo->vite_in_campo += 1;
        }
        printf("\n----------------------------------------");
        printf(" \nEffetto 1 attivato: il giocatore perde 1 vita.\n");
        printf("----------------------------------------\n");

        break;
    }
    case '7':
    {
        if (ultimo)
            index = -1;
        if (giocatori[index + 1].coperta.stato == 0)
        {
            giocatori[index + 1].coperta.stato = 1;
            manage_effect(giocatori, num_giocatori, campo, index + 1, giocatori[index + 1].coperta);
        printf("\n----------------------------------------");
            printf("\n Effetto 7 attivato: la carta coperta del prossimo giocatore è stata scoperta.\n");
        printf("----------------------------------------\n");

        }
        else
        {
            printf("\n carta già  scoperta");
        }
        break;
    }
    case 'J':
    {
        int target_index = (index == 0) ? (*num_giocatori - 1) : (index - 1);

        if (carta.stato == 0)
        {
            bool scelta = warning_hp(giocatori, num_giocatori, index);
            if (scelta)
            {
                giocatori[index].vite -= 1;
                giocatori[target_index].vite += 1;
        printf("\n----------------------------------------");
                printf("\n Effetto J attivato: trasferimento di vite tra il giocatore %s e %s.\n", giocatori[index].nome, giocatori[target_index].nome);
        printf("----------------------------------------\n");

            }
        }
        else
        {
            giocatori[index].vite -= 1;
            giocatori[target_index].vite += 1;
        printf("\n----------------------------------------");
            printf("\n Effetto J attivato: trasferimento di vite tra il giocatore %s e %s.\n", giocatori[index].nome, giocatori[target_index].nome);
        printf("----------------------------------------\n");

        }
        remove_player(giocatori, num_giocatori, index);

        break;
    }
    case 'Q':
    {
        int target_index;
        if (index == *num_giocatori - 1 || index == *num_giocatori - 2)
        {
            target_index = index + 2 - *num_giocatori;
        }
        else
        {
            target_index = index + 2;
        }
        if (carta.stato == 0)
        {
            bool scelta = warning_hp(giocatori, num_giocatori, index);
            if (scelta)
            {
                giocatori[index].vite -= 1;
                giocatori[target_index].vite += 1;
            }
        }
        else
        {
            giocatori[index].vite -= 1;
            giocatori[target_index].vite += 1;
        }
        remove_player(giocatori, num_giocatori, index);
        printf("\n----------------------------------------");
        printf(" \nEffetto Q attivato: trasferimento di vite tra il giocatore %s e %s.\n", giocatori[index].nome, giocatori[target_index].nome);
        printf("----------------------------------------\n");

        break;
    }
    case 'K':
    {
        giocatori[index].vite += campo->vite_in_campo;
        printf("\n----------------------------------------");
        printf(" \nEffetto K attivato: il giocatore %s guadagna %d vite.\n", giocatori[index].nome, campo->vite_in_campo);
        printf("----------------------------------------\n");

        campo->vite_in_campo = 0;
        break;
    }
    default:
    {
        printf("\n----------------------------------------");
        printf("\n Carta senza effetto speciale.\n");
        printf("----------------------------------------\n");

        break;
    }
    }
}

void manage_phase(giocatore giocatori[], size_t *num_giocatori, campo *campo, mazzo *deck)
{
    bool esci = false;
    mix_deck(deck);
    assign_card(deck, giocatori, *num_giocatori);
    int i = campo->first_player;

    // usa *num_giocatori per accedere al valore effettivo 
    for (size_t index = 0; index < *num_giocatori; index++)
    {
        print_field(giocatori, *num_giocatori, campo, deck);
        // risolve effetto carta scoperta
        printf("\n==========================\n");
        printf("  E' IL TURNO DI %s\n", giocatori[i].nome);
        printf("==========================\n");

        printf("\n **Risolvi l'effetto della carta scoperta** di %s:\n", giocatori[i].nome);
        manage_effect(giocatori, num_giocatori, campo, i, giocatori[i].scoperta);

        remove_player(giocatori, num_giocatori, i);
        // Controlla se il numero di giocatori è cambiato dopo manage_effect (a causa dell'eliminazione del giocatore)
        if (index >= *num_giocatori)
            break;
        if (giocatori[i].vite > 0 && giocatori[i].coperta.stato == 0)
        {
            // gestire carta coperta
            if (giocatori[i].coperta.stato == 0)
            {
                int c;
                do
                {
                    char scelta;
                    printf("\n La tua carta coperta: %c di %s\n",
                           giocatori[i].coperta.valore,
                           (const char *[]){"Picche", "Fiori", "Cuori", "Quadri"}[giocatori[i].coperta.seme]);

                    printf("\n Vuoi scoprire la carta?(s/n)\n");
                    scanf(" %c", &scelta);
                    c = 0;
                    switch (scelta)
                    {
                    case 's':
                        giocatori[i].coperta.stato = 1;
                        printf(" **Carta scoperta**: l'effetto della carta verrà risolto subito.\n");
                        manage_effect(giocatori, num_giocatori, campo, i, giocatori[i].coperta);
                        break;
                    case 'n':
                        // fine turno
                        printf(" **Carta non scoperta**: il turno termina.\n");
                        break;
                    default:
                        printf("\n Scelta errata. Per favore, rispondi con 's' per scoprire o 'n' per non scoprire.\n");
                        c = 1;
                        break;
                    }
                } while (c == 1);
            }
        }
        remove_player(giocatori, num_giocatori, i);

        // controlla se il numero giocatori è cambiato
        if (index >= *num_giocatori)
            break;

        i++;
        if (i >= *num_giocatori)
            i = 0;
    }

    // aggiorna il primo giocatore per la prossima fase
    campo->first_player = i;
}

// turno
void manage_game(giocatore giocatori[], size_t num_giocatori, campo *campo, mazzo *deck)
{
    while (num_giocatori != 1)
    {
        printf("\n==========================\n");
        printf(" INIZIO FASE DI GIOCO\n");
        printf("==========================\n");

        manage_phase(giocatori, &num_giocatori, campo, deck);
        printf("\n==========================");
        printf("\n Fine turno\n");
        printf("==========================\n");
    }
    printf("\n **Fine del gioco!** \n");
    printf("==========================\n");
    printf(" **%s  HA VINTO!** \n", giocatori[0].nome);
    printf("==========================\n");

}

int main()
{
    size_t num_giocatori = 0;
    printf(" Numero Giocatori:");
    scanf("%zu", &num_giocatori);
    giocatore *giocatori = (giocatore *)malloc(num_giocatori * sizeof(giocatore));
    if (giocatori == NULL)
    {
        fprintf(stderr, " Errore: memoria insufficiente\n");
        return 1;
    }
    campo match_campo;
    mazzo deck;
    create_match(giocatori, num_giocatori, &match_campo, &deck);
    manage_game(giocatori, num_giocatori, &match_campo, &deck);

    free(giocatori);
    return 0;
}
