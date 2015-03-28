#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NMAX 20   //fara valori mai mari sau egale cu 1000
#define NMIN 4
#define SMAX 50   //scoruri
#define HMAX 0    //ora
#define MMAX 20   //minut
#define VMAX 1000 //numarul maxim de mutari intr-un joc simulat

typedef struct {
    int gid, col1, col2, type, temp1, temp2, ore, min, sec, tur, A[10][10], played;
    char nume1[20], nume2[20], data[25], titlu[20];
} inreg;

typedef struct {
    char nume[20];
    int scor;
} alltm;

int game_over(inreg temp)
{
    int i, j;
    int a, b;
    int last;
    int full, draw;

    //pe orizontala
    for (i = 1; i <= 6; i++)
    {
        last = 0;
        for (j = 1; j <= 7; j++)
        {
            if (j == 1 || temp.A[i][j] == temp.A[i][j - 1])
                last++;
            else
                last = 1;
            if (last == 4 && temp.A[i][j])
                return 1;
        }
    }

    //pe verticala
    for (j = 1; j <= 7; j++)
    {
        last = 0;
        for (i = 1; i <= 6; i++)
        {
            if (i == 1 || temp.A[i][j] == temp.A[i - 1][j])
                last++;
            else
                last = 1;
            if (last == 4 && temp.A[i][j])
                return 1;
        }
    }

    //oblic NV-SE
    i = 6;
    j = 1;
    while (j <= 7)
    {
        a = i + 1;
        b = j + 1;
        last = 1;
        while (a <= 6 && b <= 7)
        {
            if (temp.A[a][b] == temp.A[a - 1][b - 1])
                last++;
            else
                last = 1;
            if (last == 4 && temp.A[a][b])
                return 1;
            a++;
            b++;
        }
        if (i > 1)
            i--;
        else
            j++;
    }

    //oblic NE-SV
    i = 1;
    j = 1;
    while (i <= 6)
    {
        a = i + 1;
        b = j - 1;
        last = 1;
        while (a <= 6 && b >= 1)
        {
            if (temp.A[a][b] == temp.A[a - 1][b + 1])
                last++;
            else
                last = 1;
            if (last == 4 && temp.A[a][b])
                return 1;
            a++;
            b--;
        }
        if (j < 7)
            j++;
        else
            i++;
    }

    //remiza
    full = 1;
    for (i = 1; i <= 6; i++)
    {
        for (j = 1; j <= 7; j++)
            if (!temp.A[i][j])
            {
                full = 0;
                break;
            }
        if (!full)
            break;
    }
    if (full)
    {
        if (!temp.type)
            return 1;
        else
        {
            draw = 1;
            for (j = 1; j <= 7; j++)
                if (temp.A[6][j] == temp.tur + 1)
                {
                    draw = 0;
                    break;
                }
            if (draw)
                return 1;
        }
    }
    return 0;
}

void proces(inreg *temp, int move)
{
    int i;
    if (move <= 7)
    {
        for (i = 6; i >= 1; i--)
            if ((*temp).A[i][move] == 0)
                break;
        (*temp).A[i][move] = (*temp).tur + 1;
    }
    else
    {
        for (i = 6; i >= 2; i--)
            (*temp).A[i][move - 7] = (*temp).A[i - 1][move - 7];
        (*temp).A[1][move - 7] = 0;
    }
}

int main()
{
    const char *nume[] = {
    "ada", "adela", "adelaida", "adelina", "adina", "adriana", "agata",
    "aglaia", "agripina", "aida", "alberta", "albertina", "alexandra",
    "alexandrina", "alida", "alina", "alis", "alma", "amalia", "amelia",
    "amanda", "ana", "anabela", "anaida", "anamaria", "anastasia", "anca",
    "ancuta", "anda", "andra", "andrada", "andreea", "anemona", "aneta",
    "angela", "anghelina", "anica", "anisoara", "antoaneta", "antonia",
    "antonela", "anuta", "ariadna", "ariana", "arina", "aristita", "artemisa",
    "astrid", "atena", "augustina", "aura", "aurelia", "aureliana", "aurica",
    "aurora", "beatrice", "betina", "bianca", "blanduzia", "bogdana",
    "brandusa", "camelia", "carina", "carla", "carmen", "carmina", "carolina",
    "casandra", "casiana", "catinca", "catrina", "catrinel", "catalina",
    "cecilia", "celia", "cerasela", "cezara", "cipriana", "clara", "clarisa",
    "claudia", "clementina", "cleopatra", "codrina", "codruta", "constantina",
    "constanta", "consuela", "coralia", "corina", "cornelia", "cosmina",
    "crenguta", "crina", "cristina", "daciana", "dafina", "daiana", "dalia",
    "dana", "daniela", "daria", "dariana", "delia", "demetra", "denisa",
    "despina", "diana", "dida", "didina", "dina", "dochia", "doina",
    "domnica", "dora", "doriana", "dorina", "dorli", "draga", "dumitra",
    "dumitrana", "ecaterina", "eftimia", "elena", "eleonora", "eliana",
    "elisabeta", "elisaveta", "eliza", "elodia", "elvira", "emilia",
    "emanuela", "erica", "estera", "eufrosina", "eugenia", "eusebia", "eva",
    "evanghelina", "evelina", "fabia", "fabiana", "felicia", "filofteia",
    "fiona", "flavia", "floare", "floarea", "flora", "floriana", "florica",
    "florina", "florentina", "florenta", "francesca", "frusina", "gabriela",
    "geanina", "georgeta", "georgia", "georgiana", "geta", "gherghina",
    "gianina", "gina", "giorgiana", "gratiana", "gratiela", "hortensia",
    "henrieta", "iasmina", "ica", "ileana", "ilinca", "ilona", "ina", "ioana",
    "iolanda", "ionela", "iosefina", "irina", "iridenta", "iris", "isabela",
    "iulia", "iuliana", "iustina", "ivona", "izabela", "jana", "janeta",
    "janina", "jasmina", "jeana", "julia", "julieta", "larisa", "laura",
    "laurentia", "lavinia", "lacramioara", "leana", "lelia", "leontina",
    "leopoldina", "letitia", "lia", "liana", "lidia", "ligia", "lili",
    "liliana", "lioara", "livia", "loredana", "lorena", "luana", "lucia",
    "luciana", "lucretia", "ludovica", "luiza", "luminita", "magdalena",
    "maia", "manuela", "mara", "marcela", "marga", "margareta", "marcheta",
    "maria", "mariana", "maricica", "marilena", "marina", "marinela",
    "marioara", "marta", "matilda", "malvina", "madalina", "malina",
    "marioara", "mariuca", "melania", "melina", "mihaela", "milena", "mina",
    "minodora", "mioara", "mirabela", "mirela", "miruna", "mona", "monalisa",
    "monica", "nadia", "narcisa", "natalia", "natasa", "nicoleta", "niculina",
    "nora", "norica", "oana", "octavia", "octaviana", "ofelia", "olga",
    "olimpia", "olivia", "ortansa", "otilia", "ozana", "pamela", "paraschiva",
    "paula", "paulica", "paulina", "patricia", "petronela", "petruta",
    "pompilia", "profira", "rada", "rafila", "raluca", "ramona", "rebeca",
    "renata", "rica", "roberta", "robertina", "rodica", "roza", "rozalia",
    "roxana", "ruxanda", "ruxandra", "sabina", "sabrina", "safta", "salomea",
    "sanda", "saveta", "savina", "sanziana", "semenica", "severina",
    "sidonia", "silvia", "silviana", "simina", "simona", "smaranda", "sofia",
    "sonia", "sorana", "sorina", "speranta", "stana", "stanca", "stela",
    "steliana", "steluta", "suzana", "stefana", "stefania", "tamara", "tania",
    "tatiana", "teodora", "teodosia", "teona", "tiberia", "tinca", "tincuta",
    "tudorita", "tudosia", "valentina", "valeria", "vanesa", "varvara",
    "vasilica", "venera", "vera", "veronica", "veta", "victoria", "violeta",
    "viorela", "viorica", "virginia", "viviana", "voichita", "xenia",
    "zaharia", "zamfira", "zaraza", "zenobia", "zenovia", "zina", "zoe",
    "achim", "adam", "adelin", "adonis", "adrian", "adi", "agnos", "albert",
    "alex", "alexandru", "alexe", "aleodor", "alin", "alistar", "amza",
    "anatolie", "andrei", "angel", "anghel", "antim", "anton", "antonie",
    "antoniu", "arian", "aristide", "arsenie", "augustin", "aurel",
    "aurelian", "aurica", "avram", "axinte", "barbu", "bartolomeu", "basarab",
    "banel", "bebe", "beniamin", "bernard", "bogdan", "bradut", "bucur",
    "caius", "camil", "cantemir", "carol", "casian", "cazimir", "calin",
    "catalin", "cedrin", "cezar", "ciprian", "claudiu", "codin", "codrin",
    "codrut", "cornel", "corneliu", "corvin", "constantin", "cosmin",
    "costache", "costel", "costin", "crin", "cristea", "cristian",
    "cristobal", "cristofor", "dacian", "damian", "dan", "daniel", "darius",
    "david", "decebal", "denis", "dinu", "dominic", "dorel", "dorian",
    "dorin", "dorinel", "doru", "dragos", "ducu", "dumitru", "edgar",
    "edmond", "eduard", "eftimie", "emil", "emilian", "emanoil", "emanuel",
    "emanuil", "eremia", "eric", "ernest", "eugen", "eusebiu", "eustatiu",
    "fabian", "felix", "filip", "fiodor", "flaviu", "florea", "florentin",
    "florian", "florin", "francisc", "frederic", "gabi", "gabriel", "gelu",
    "george", "georgel", "georgian", "ghenadie", "gheorghe", "gheorghita",
    "ghita", "gica", "gicu", "giorgian", "gratian", "gregorian", "grigore",
    "haralamb", "haralambie", "horatiu", "horea", "horia", "iacob", "iancu",
    "ianis", "ieremia", "ilarie", "ilarion", "ilie", "inocentiu", "ioan",
    "ion", "ionel", "ionica", "ionut", "iosif", "irinel", "iulian", "iuliu",
    "iurie", "iustin", "iustinian", "ivan", "jan", "jean", "jenel",
    "ladislau", "lascar", "laurentiu", "laurian", "lazar", "leonard",
    "leontin", "lica", "liviu", "lorin", "luca", "lucentiu", "lucian",
    "lucretiu", "ludovic", "manole", "marcel", "marcu", "marian", "marin",
    "marius", "martin", "matei", "maxim", "maximilian", "madalin", "mihai",
    "mihail", "mihnea", "mircea", "miron", "mitica", "mitrut", "mugur",
    "mugurel", "nae", "narcis", "nechifor", "nelu", "nichifor", "nicoara",
    "nicodim", "nicolae", "nicolaie", "nicu", "nicuta", "niculita", "nicusor",
    "norbert", "octav", "octavian", "octaviu", "olimpian", "olimpiu",
    "oliviu", "ovidiu", "pamfil", "panait", "panagachie", "paul", "pavel",
    "patru", "petre", "petrica", "petrisor", "petru", "petrut", "pompiliu",
    "radu", "rares", "raducu", "razvan", "relu", "remus", "robert", "romeo",
    "romulus", "sabin", "sandu", "sava", "sebastian", "sergiu", "sever",
    "severin", "silvian", "silviu", "simi", "simion", "sinica", "sorin",
    "stan", "stancu", "stelian", "serban", "stefan", "teodor", "teofil",
    "teohari", "theodor", "tiberiu", "titus", "todor", "toma", "traian",
    "tudor", "valentin", "valeriu", "valter", "vasile", "vasilica",
    "veniamin", "vicentiu", "victor", "vincentiu", "viorel", "visarion",
    "vlad", "vladimir", "vlaicu", "voicu", "zamfir", "zeno"};

    const char *titlu[] = {
    "salvare", "save", "savegame", "game", "joc", "match", "meci",
    "provocare", "challenge", "partida", "confruntare", "salvat",
    "jocsalvat", "the_game", "jocul", "momentdeincordare", "tobecontinued",
    "va_urma", "nume", "nume_joc", "the_savegame", "the_match", "meciul",
    "the_challenge", "un_joc", "a_game", "le_game", "le_match"};

    inreg temp;
    alltm temp2;
    FILE *f;
    int k, i, n, poz, poz2, j, pas, nalt;
    int move;
    int col1, col2;
    int mutari[VMAX], alter[20];
    char *aux;
    time_t timp = time(NULL);

    srand(time(NULL));
    printf("Generating files:\nsaved.dat ............. ");
    fflush(stdin);
    if ((f = fopen("saved.dat", "wb")) != NULL)
    {
        n = rand() % (NMAX - NMIN + 1) + NMIN;
        for (k = 1; k <= n; k++)
        {
            poz = rand() % 647;
            strcpy(temp.nume1, nume[poz]);
            temp.nume1[strlen(nume[poz])] = 0;
            poz = rand() % 647;
            strcpy(temp.nume2, nume[poz]);
            temp.nume2[strlen(nume[poz])] = 0;
            temp.gid = i;
            do {
                temp.col1 = rand() % 7;
            } while (temp.col1 == 3);
            do {
                temp.col2 = rand() % 7;
            } while (temp.col2 == 3 || temp.col2 == temp.col1);
            temp.type = rand() % 2;
            temp.temp1 = rand() % (SMAX + 1);
            temp.temp2 = rand() % (SMAX + 1);
            temp.ore = rand() % (HMAX + 1);
            temp.min = rand() % (MMAX + 1);
            temp.sec = rand() % 60;
            temp.played = rand() % 2;
            temp.tur = 0;
        do {
                for (i = 1; i <= 6; i++)
                    for (j = 1; j <= 7; j++)
                        temp.A[i][j] = 0;
                pas = 0;
                do {
                    pas++;
                    nalt = 0;
                    for (i = 1; i <= 7; i++)
                        if (!temp.A[1][i])
                            alter[++nalt] = i;
                    /*if (temp.type)
                        for (i = 1; i <= 7; i++)
                            if (temp.A[6][i] == temp.tur + 1)
                                alter[++nalt] = i + 7;*/
                    move = alter[rand() % nalt + 1];
                    mutari[pas] = move;
                    proces(&temp, move);
                    temp.tur = 1 - temp.tur;
                } while (!game_over(temp) && pas < VMAX);
            } while (pas < 21);
        pas = rand() % (pas - 20) + 20;
            for (i = 1; i <= 6; i++)
                for (j = 1; j <= 7; j++)
                    temp.A[i][j] = 0;
            temp.tur = 0;
            for (i = 1; i <= pas; i++)
            {
                proces(&temp, mutari[i]);
                temp.tur = 1 - temp.tur;
            }
            aux = (ctime(&timp));
            strncpy(temp.data, aux, 24);
            temp.data[24] = 0;
            poz = rand() % 28;
            strcpy(temp.titlu, titlu[poz]);
            temp.titlu[strlen(titlu[poz])] = 0;
            if (rand() % 2)
            {
                temp.titlu[strlen(titlu[poz])] = rand() % 10 + 48;
                temp.titlu[strlen(titlu[poz]) + 1] = 0;
            }
            fwrite(&temp, sizeof(temp), 1, f);
        }
        fclose(f);
    printf("[OK]\n");
    }
    else
        printf("[Error]\n");
    printf("alltime.dat ........... ");
    if ((f = fopen("alltime.dat", "wb")) != NULL)
    {
        for (i = 0; i < 647; i++)
        {
            temp2.scor = 0;
            while (rand() % 100 < 99)
                temp2.scor++;
            strcpy(temp2.nume, nume[i]);
            temp2.nume[strlen(nume[i])] = 0;
            fwrite(&temp2, sizeof(temp2), 1, f);
        }
        printf("[OK]\n");
    }
    else
        printf("[Error]\n");
    printf("last_conf.ini ......... ");
    if ((f = fopen("last_conf.ini", "w")) != NULL)
    {
        do {
            col1 = rand() % 7;
        } while (col1 == 3);
        do {
            col2 = rand() % 7;
        } while (col2 == 3 || col2 == col1);
        poz = rand() % 647;
        do {
            poz2 = rand() % 647;
        } while (poz2 == poz);
        fprintf(f, "%d %s\n%d %s\n%d", col1, nume[poz], col2, nume[poz2], rand() % 2);
        printf("[OK]\n");
    }
    else
        printf("[Error]\n");
    return 0;
}

