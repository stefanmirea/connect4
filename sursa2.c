#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "antet.h"

void stai(int mil) // functie pentru asteptare
{
  clock_t t2, t1 = clock();

  t2 = t1;
  while (((float)(t2 - t1)) / CLOCKS_PER_SEC * 1000 < mil)
    t2 = clock();
}

char *invers(char *s) // inversare sir
{
  int i;
  char aux;
  for (i = 0; i < strlen(s) / 2; i++)
  {
    aux = s[strlen(s) - i - 1];
    s[strlen(s) - i - 1] = s[i];
    s[i] = aux;
  }
  return s;
}

void save_game(int r, int c)
{
  inreg temp;
  int v[1000];
  int n = 0;
  int i, j;
  int found;
  FILE *f;
  time_t timp = time(NULL);
  char *aux = ctime(&timp);
  int pos;
  int ok;

  if (Falling)
    Atribute.tur = 1 - Atribute.tur;
  strncpy(Atribute.data, aux, 24);
  Atribute.data[24] = 0;
  if ((f = fopen("saved.dat", "a")) != NULL)
  {
    ok = 1;
    fclose(f);
  }
  else
    ok = 0;
  if (!ok || (f = fopen("saved.dat", "rb+")) == NULL)
  {
    if ((f = fopen("saved.dat", "w")) != NULL)
      ok = 1;
    else
      ok = 0;
    if (ok)
    {
      fclose(f);
      if ((f = fopen("saved.dat", "rb+")) == NULL)
        ok = 0;
    }
    if (!ok)
    {
      color_set(21, 0);
      afisare(r, c, 1, 1, "Error while saving.        ");
      if (Falling)
        Atribute.tur = 1 - Atribute.tur;
      return;
    }
  }
  if (!Atribute.gid)
  {
    while (fread(&temp, sizeof(temp), 1, f) == 1)
      v[++n] = temp.gid;
    fclose(f);
    f = fopen("saved.dat", "ab");
    for (i = 1; i <= 32767; i++)  // se cauta un Game ID disponibil
    {
      found = 0;
      for (j = 1; j <= n; j++)
        if (v[j] == i)
        {
          found = 1;
          break;
        }
      if (!found)
        break;
    }
    Atribute.gid = i;
    fwrite(&Atribute, sizeof(Atribute), 1, f);
  }
  else
  {
    found = 0;
    while (fread(&temp, sizeof(temp), 1, f) == 1)
      if (temp.gid == Atribute.gid)
      {
        found = 1;
        pos = ftell(f) - sizeof(inreg);
        break;
      }
    if (!found)
    {
      fclose(f);
      f = fopen("saved.dat", "ab");
      fwrite(&Atribute, sizeof(Atribute), 1, f);
    }
    else
    {
      fseek(f, pos, 0);
      fwrite(&Atribute, sizeof(inreg), 1, f);
    }
  }
  color_set(24, 0);
  Changed = 0;
  afisare(r, c, 1, 1, "Successfully saved.        ");
  fclose(f);
  if (Falling)
    Atribute.tur = 1 - Atribute.tur;
}

char *sir(int x) // transforma oglinditul unui numar intr-un sir
{
  char *ret = (char *)malloc(3);
  int len = 0;
  do {
    ret[len] = x % 10 + 48;
    len++;
    ret = (char *)realloc(ret, len + 2);
    x /= 10;
  } while (x);
  ret[len] = 0;
  return ret;
}

void salvare() // salveaza ultima configurare
{
  FILE *f;

  if (!strlen(Atribute.nume1))
  {
    strcpy(Atribute.nume1, Def1);
    Atribute.nume1[strlen(Def1)] = 0;
  }
  if (!strlen(Atribute.nume2))
  {
    strcpy(Atribute.nume2, Def2);
    Atribute.nume2[strlen(Def2)] = 0;
  }
  if ((f = fopen("last_conf.ini", "w")) != NULL)
  {
    fprintf(f, "%d %s\n%d %s\n%d", Atribute.col1, Atribute.nume1, Atribute.col2, Atribute.nume2, Atribute.type);
    fclose(f);
  }
}

void init_pairs()
{
  int i;

  for (i = 0; i < 8; i++)
  {
    init_pair(i, 0, i);
    init_pair(20 + i, i, 7);
  }
  init_pair(8, 4, 7);   // optiune normala in meniul principal
  init_pair(9, 1, 7);   // optiune selectata
  init_pair(10, 4, 7);  // optiune normala in meniul Optiuni
  init_pair(11, 7, 6);  // campurile de nume
  init_pair(12, 7, 4);  // butonul START neselectat
  init_pair(13, 7, 1);  // butonul START selectat
}

void clscreen(WINDOW *wnd, int nrows, int ncols, int r, int c)
{                             // goleste spatiul 27 x 70 in care e afisat jocul
  int i, j;

  color_set(7, 0);
  for (i = 1; i <= HEIGHT; i++)
    for (j = 1; j <= WIDTH; j++)
      mvaddch(r + i - 1, c + j - 1, ' ');
  refresh();
}

void redim(WINDOW *wnd, int *nrows, int *ncols, int *r, int *c)
{                  // goleste spatiul din afara dreptunghiului 27 x 70 cu jocul
  int i, j;

  getmaxyx(wnd, *nrows, *ncols);
  *r = (*nrows - HEIGHT) / 2;
  *c = (*ncols - WIDTH) / 2;
  color_set(7, 0);
  for (i = 0; i < *nrows; i++)
    for (j = 0; j < *ncols; j++)
      if (i < *r || i > *r + HEIGHT - 1 || j < *c || j > *c + WIDTH - 1)
        mvaddch(i, j, ' ');
  refresh();
}

void start_image(int nrows, int ncols, int r, int c)
{
  int i, j, a, b;
  char *s;
  FILE *f;

  if ((f = fopen("start_image", "r")) != NULL)
  {
    s = (char *)malloc(WIDTH + 3);
    for (i = 1; i <= HEIGHT; i++)
    {
      a = r + i - 1;
      fgets(s, WIDTH + 3, f);
      for (j = 0; j < WIDTH; j++)
      {
        b = c + j;
        if (a >= 0 && a <= nrows && b >= 0 && b <= ncols)
        {
          color_set(s[j] - 48, 0);
          mvaddch(a, b, ' ');
        }
      }
    }
    free(s);
    fclose(f);
  }
  else
  {
    color_set(7, 0);
    for (i = 1; i <= HEIGHT; i++)
      for (j = 1; j <= WIDTH; j++)
        mvaddch(r + i - 1, c + j - 1, ' ');
  }
  refresh();
}

const char *del(int nr, const char *s)
{            // returneaza un sir fara primele nr caractere ale sirului initial
  if (nr > strlen(s))
    return NULL;
  return (s + nr);
}

char *begin(int nr, const char *s)         // returneaza prefixul de lungime nr
{
  if (nr <= 0)
    return NULL;
  if (nr >= strlen(s))
    return strdup(s);
  char *nou = (char *)malloc(nr + 1);
  strncpy(nou, s, nr);
  nou[nr] = 0;
  return nou;
}

void afisare(int r, int c, int ln, int col, const char *s)
{                     // afiseaza un text fara a iesi in afara spatiului de joc
  if (ln < 1 || ln > HEIGHT)
    return;
  if (col >= 1 && col + strlen(s) - 1 <= WIDTH)
    mvaddstr(r + ln - 1, c + col - 1, s);
  else
    if (col < 1)
      mvaddstr(r + ln - 1, c, del(1 - col, s));
    else
    {
      char *aux = begin(WIDTH - col + 1, s);
      mvaddstr(r + ln - 1, c + col - 1, aux);
      free(aux);
    }
}

void main_menu(int r, int c, int sel, int left)
{                                                  // afiseaza meniul principal
  const char *s[] = {"New Game  ", "Load Saved Game  ", "Highscores  ", "Settings  ", "Quit  "};
  int i;

  for (i = 0; i < 5; i++)
  {
    if (i == sel - 1)
    {
      color_set(9, 0);
      if (6 + left > 0)
        afisare(r, c, 9 + i, 5 + left, "  > ");
    }
    else
    {
      color_set(8, 0);
      if (6 + left > 0)
        afisare(r, c, 9 + i, 5 + left, "    ");
    }
    afisare(r, c, 9 + i, 9 + left, s[i]);
  }
  refresh();
}

void write_name(int r, int c, int anim, char *nume, int nr, int sel, clock_t cursor)
{                          // afiseaza numele jucatorilor in ecranul de optiuni
  int i;

  color_set(11, 0);
  afisare(r, c, 10 + nr * 4, 39 + anim, nume);
  for (i = 1; i <= 18 - strlen(nume) && 38 + strlen(nume) + anim + i <= WIDTH; i++)
    mvaddch(r + 9 + nr * 4, c + 37 + strlen(nume) + anim + i, ' ');
  if (sel && strlen(nume) < 18)
  {
    if (((int)(((float)(clock() - cursor)) / CLOCKS_PER_SEC * 1000)) % 1000 < 500)
      afisare(r, c, 10 + nr * 4, 39 + anim + strlen(nume), "_");
    else
      afisare(r, c, 10 + nr * 4, 39 + anim + strlen(nume), " ");
  }
  color_set(7, 0);
  afisare(r, c, 10 + nr * 4, 38 + strlen(nume) + anim + i, "  ");
}

void options_menu(int r, int c, int sel, int anim, clock_t cursor, clock_t *last_update)
{                                                 // afiseaza meniul de optiuni
  const char *s[] = {"Player 1 name", "Player 1 color", "Player 2 name", "Player 2 color", "Game type"};
  int i, left;

  if (!sel)
    color_set(9, 0);
  else
    color_set(10, 0);
  afisare(r, c, 2, -anim / 8 - 1, "  < Back  ");
  for (i = 0; i < 5; i++)
  {
    if (i % 2 == 0)
      left = anim;
    else
      left = -anim;
    if (i == sel - 1)  // afisare marcaj selectie ('> ')
    {
      color_set(9, 0);
      afisare(r, c, i * 2 + 10, 28 - strlen(s[i]) + left, "  > ");
    }
    else
    {
      color_set(10, 0);
      afisare(r, c, i * 2 + 10, 28 - strlen(s[i]) + left, "    ");
    }
    if (i % 2 == 0)  // afisare optiune
      afisare(r, c, i * 2 + 10, 32 - strlen(s[i]) + left, s[i]);
    else
      afisare(r, c, i * 2 + 10, 32 - strlen(s[i]) + left, s[i]);
    afisare(r, c, i * 2 + 10, 32 + left, "   :   ");
  }

  write_name(r, c, anim, Atribute.nume1, 0, (sel == 1 ? 1 : 0), cursor);
  write_name(r, c, anim, Atribute.nume2, 1, (sel == 3 ? 1 : 0), cursor);

  if (sel == 2)
    color_set(9, 0);
  else
    color_set(10, 0);
  afisare(r, c, 12, 36 - anim, "   <    >   ");

  if (sel == 4)
    color_set(9, 0);
  else
    color_set(10, 0);
  afisare(r, c, 16, 36 - anim, "   <    >   ");

  // caracterele '<' si '>' din jurul optiunii Classic / Pop Out
  if (sel == 5)
    color_set(9, 0);
  else
    color_set(10, 0);
  afisare(r, c, 18, 36 + anim, "   <         >   ");

  // tipul de joc
  if (!Atribute.type)
    afisare(r, c, 18, 41 + anim, "Classic");
  else
    afisare(r, c, 18, 41 + anim, "Pop out");

  // culoarea 1
  color_set(Atribute.col1, 0);
  afisare(r, c, 12, 41 - anim, "  ");

  // culoarea 2
  color_set(Atribute.col2, 0);
  afisare(r, c, 16, 41 - anim, "  ");

  // butonul START
  color_set(7, 0);
  afisare(r, c, 20, 31 - anim, "         ");
  if (sel == 6)
    color_set(13, 0);
  else
    color_set(12, 0);
  afisare(r, c, 20, 32 - anim, " START ");

  refresh();
  *last_update = clock();
}

void mark(int r, int c, int ln, int col)
{                                     // marcheaza una din casutele consecutive
  if (Atribute.col1 && Atribute.col2)
    color_set(0, 0);
  else if (Atribute.col1 != 6 && Atribute.col2 != 6)
    color_set(6, 0);
  else
    color_set(4, 0);
  afisare(r, c, ln * 3 + 3, col * 6 - 5, "        ");
  afisare(r, c, ln * 3 + 4, col * 6 - 5, "  ");
  afisare(r, c, ln * 3 + 4, col * 6 + 1, "  ");
  afisare(r, c, ln * 3 + 5, col * 6 - 5, "  ");
  afisare(r, c, ln * 3 + 5, col * 6 + 1, "  ");
  afisare(r, c, ln * 3 + 6, col * 6 - 5, "        ");
}

void play_again(int r, int c, clock_t *game_update)
{                  // afiseaza piesele cazand inainte de inceperea unui joc nou
  int fall[25][10];
  int i, j;
  int x, y;
  int pas = 0;
  int modif;
  clock_t t1, t2;

  Substatus = ENDING;
  playing_mode(r, c, 44, 0);
  for (j = 1; j <= 7; j++)
    fall[0][j] = 0;
  for (i = 19; i <= 21; i++)
    for (j = 1; j <= 7; j++)
      fall[i][j] = 0;
  for (i = 1; i <= 6; i++)
    for (j = 1; j <= 7; j++)
      fall[i * 3 - 2][j] = fall[i * 3 - 1][j] = fall[i * 3][j] = Atribute.A[i][j];
  x = 6;
  y = 0;
  do {
    modif = 0;
    if (pas % 4 == 0)
    {
      getNext(&x, &y);
      if (x >= 1)
      {
        fall[x * 3 - 2][y] += 2;
        fall[x * 3 - 1][y] += 2;
        fall[x * 3][y] += 2;
      }
    }
    for (i = 22; i > 1; i--)
      for (j = 1; j <= 7; j++)
        if (fall[i - 1][j] > 2)
        {
          fall[i][j] = fall[i - 1][j];
          fall[i - 1][j] = 0;
          modif = 1;
        }
    for (i = 1; i <= 21; i++)
    {
      for (j = 1; j <= 7; j++)
        if (i == 21 || i % 3 != 0)
        {
          if (!fall[i][j])
            color_set(7, 0);
          else if (fall[i][j] % 2)
            color_set(Atribute.col1, 0);
          else
            color_set(Atribute.col2, 0);
          if (i < 18)
            afisare(r, c, i + 6, j * 6 - 3, "    ");
          else
            afisare(r, c, i + 6, j * 6 - 4, "      ");
        }
    }
    pas++;
    refresh();
    t1 = t2 = clock();
    while (((float)(t2 - t1)) / CLOCKS_PER_SEC * 1000 < 12)
    {
      update_time(r, c, game_update);
      t2 = clock();
    }
  } while (modif);
  for (i = 1; i <= 6; i++)
    for (j = 1; j <= 7; j++)
      Atribute.A[i][j] = 0;
  Atribute.played++;
  Atribute.tur = (Atribute.played % 2 ? 1 : 0);
  Substatus = NORMAL;
  playing_mode(r, c, 44, 1);
}

int game_over(int r, int c)
{                        /* | 0, daca jocul nu s-a terminat
                   intoarce | numarul jucatorului care a castigat, daca e cazul
                            | 3, pentru remiza */
  int i, j, k;
  int a, b;
  int last;
  int full, draw;
  int ret = 0;

  // pe orizontala
  for (i = 1; i <= 6; i++)
  {
    last = 0;
    for (j = 1; j <= 7; j++)
    {
      if (j == 1 || Atribute.A[i][j] == Atribute.A[i][j - 1])
        last++;
      else
        last = 1;
      if (last >= 4 && Atribute.A[i][j])
      {
        if (Substatus == NORMAL)
          for (k = j - 3; k <= j; k++)
            mark(r, c, i, k);
        if (ret && Atribute.A[i][j] != ret)
          ret = 3;
        else
          ret = Atribute.A[i][j];
      }
    }
  }

  // pe verticala
  if (ret != 3)
    for (j = 1; j <= 7; j++)
    {
      last = 0;
      for (i = 1; i <= 6; i++)
      {
        if (i == 1 || Atribute.A[i][j] == Atribute.A[i - 1][j])
          last++;
        else
          last = 1;
        if (last >= 4 && Atribute.A[i][j])
        {
          if (Substatus == NORMAL)
            for (k = i - 3; k <= i; k++)
              mark(r, c, k, j);
          if (ret && Atribute.A[i][j] != ret)
            ret = 3;
          else
            ret = Atribute.A[i][j];
        }
      }
    }

  // oblic NV-SE
  if (ret != 3)
  {
    i = 6;
    j = 1;
    while (j <= 7)
    {
      a = i + 1;
      b = j + 1;
      last = 1;
      while (a <= 6 && b <= 7)
      {
        if (Atribute.A[a][b] == Atribute.A[a - 1][b - 1])
          last++;
        else
          last = 1;
        if (last >= 4 && Atribute.A[a][b])
        {
          if (Substatus == NORMAL)
            for (k = a - 3; k <= a; k++)
              mark(r, c, k, b - a + k);
          if (ret && Atribute.A[a][b] != ret)
            ret = 3;
          else
            ret = Atribute.A[a][b];
        }
        a++;
        b++;
      }
      if (i > 1)
        i--;
      else
        j++;
    }
  }

  // oblic NE-SV
  if (ret != 3)
  {
    i = 1;
    j = 1;
    while (i <= 6)
    {
      a = i + 1;
      b = j - 1;
      last = 1;
      while (a <= 6 && b >= 1)
      {
        if (Atribute.A[a][b] == Atribute.A[a - 1][b + 1])
          last++;
        else
          last = 1;
        if (last >= 4 && Atribute.A[a][b])
        {
          if (Substatus == NORMAL)
            for (k = a - 3; k <= a; k++)
              mark(r, c, k, a - k + b);
          if (ret && Atribute.A[a][b] != ret)
            ret = 3;
          else
            ret = Atribute.A[a][b];
        }
        a++;
        b--;
      }
      if (j < 7)
        j++;
      else
        i++;
    }
  }

  // remiza
  if (!ret)
  {
    full = 1;
    for (i = 1; i <= 6; i++)
    {
      for (j = 1; j <= 7; j++)
        if (!Atribute.A[i][j])
        {
          full = 0;
          break;
        }
      if (!full)
        break;
    }
    if (full)
    {
      if (!Atribute.type)
        ret = 3;
      else
      {
        draw = 1;
        for (j = 1; j <= 7; j++)
          if (Atribute.A[6][j] == Atribute.tur + 1)
          {
            draw = 0;
            break;
          }
        if (draw)
          ret = 3;
      }
    }
  }
  if (ret && Substatus == NORMAL)
  {
    color_set(21, 0);
    if (ret == 1)
    {
      afisare(r, c, 3, (WIDTH - strlen(Atribute.nume1) - 6) / 2, Atribute.nume1);
      afisare(r, c, 3, (WIDTH - strlen(Atribute.nume1) - 6) / 2 + strlen(Atribute.nume1), " won !");
    }
    else if (ret == 2)
    {
      afisare(r, c, 3, (WIDTH - strlen(Atribute.nume2) - 6) / 2, Atribute.nume2);
      afisare(r, c, 3, (WIDTH - strlen(Atribute.nume2) - 6) / 2 + strlen(Atribute.nume2), " won !");
    }
    else
      afisare(r, c, 3, 28, "It's a draw !");
    refresh();
  }
  return ret;
}

void numescor(int r, int c, int line, int color, int anim, char *nume, int scor)
{                    // afiseaza numele si scorul jucatorilor in timpul jocului
  char *aux;
  color_set(20 + color, 0);
  afisare(r, c, line, 68 - anim / 2, " ");
  attroff(A_BOLD);
  afisare(r, c, line, 69 - anim / 2, nume);
  afisare(r, c, line, 69 - anim / 2 + strlen(nume), ": ");
  aux = sir(scor);
  afisare(r, c, line, 69 - anim / 2 + strlen(nume) + 2, invers(aux));
  afisare(r, c, line, 69 - anim / 2 + strlen(nume) + 2 + strlen(aux), " ");
  free(aux);
  attron(A_BOLD);
}

void piesa(int r, int c, int pos)
{                 // Afiseaza piesa care urmeaza a fi aruncata (sau "^Pop out")
  int i, j;
  color_set(7, 0);
  for (i = 3; i <= 5; i++)
    for (j = 1; j <= 44; j++)
      mvaddch(r + i - 1, c + j - 1, ' ');
  for (i = 25; i <= 26; i++)
    for (j = 1; j <= 44; j++)
      mvaddch(r + i - 1, c + j - 1, ' ');
  if (pos >= 1 && pos <= 7)
  {
    color_set((Atribute.tur ? Atribute.col2 : Atribute.col1), 0);
    for (i = 3; i <= 5; i++)
      afisare(r, c, i, pos * 6 - 4, "      ");
  }
  else if (pos > 7)
  {
    color_set((Atribute.tur ? Atribute.col2 : Atribute.col1) + 20, 0);
    attroff(A_BOLD);
    afisare(r, c, 25, (pos - 7) * 6 - 3, "^Pop");
    afisare(r, c, 26, (pos - 7) * 6 - 2, "Out");
    attron(A_BOLD);
  }
  refresh();
}

int set_pos(int r, int c)
{                       // returneaza prima posibila pozitie a piesei urmatoare
  int i;
  int pos = 0;

  for (i = 1; i <= 7; i++)
    if (!Atribute.A[1][i])
    {
      pos = i;
      break;
    }
  if (!pos && Atribute.type)
    for (i = 1; i <= 7; i++)
      if (Atribute.A[6][i] == Atribute.tur + 1)
      {
        pos = i + 7;
        break;
      }
  if (!pos)
    return 0;
  piesa(r, c, pos);
  return pos;
}

void playing_mode(int r, int c, int anim, int showBoard)
{                                                    // afiseaza ecranul de joc
  int i, j;
  int len; // lungimea numarului de ore
  time_t data_ora = time(NULL);
  char *ore, *aux;

  if (showBoard != 2) // nu trebuie actualizat doar panoul
  {
    for (i = 1; i <= 6; i++)
      for (j = 7 - (anim - 1) / 6; j <= 7; j++)
      {
        color_set(3, 0);
        afisare(r, c, 6 + (i - 1) * 3, (j - 1) * 6 + anim - 41, "      ");
        afisare(r, c, 7 + (i - 1) * 3, (j - 1) * 6 + anim - 37, "  ");
        afisare(r, c, 8 + (i - 1) * 3, (j - 1) * 6 + anim - 37, "  ");
        if (showBoard)
        {
          if (Atribute.A[i][j] == 1)
            color_set(Atribute.col1, 0);
          else if (Atribute.A[i][j] == 2)
            color_set(Atribute.col2, 0);
          else
            color_set(7, 0);
        }
        else
          color_set(7, 0);
        afisare(r, c, 7 + (i - 1) * 3, (j - 1) * 6 + anim - 41, "    ");
        afisare(r, c, 8 + (i - 1) * 3, (j - 1) * 6 + anim - 41, "    ");
      }
    color_set(3, 0);
    for (i = 1; i <= anim; i++)
      afisare(r, c, 24, i, " ");
    color_set(7, 0);
    for (i = 6; i <= 24; i++)
      afisare(r, c, i, anim + 1, "  ");
    if (Substatus == GAME_OVER)
      game_over(r, c);
    color_set(8, 0);
    afisare(r, c, 7, 68 - anim / 2, "                          ");
    afisare(r, c, 7, 69 - anim / 2, ctime(&data_ora));
  }
  color_set(8, 0);
  afisare(r, c, 6, 68 - anim / 2, " Current date and time: ");
  afisare(r, c, 9, 68 - anim / 2, " Current score: ");

  numescor(r, c, 10, Atribute.col1, anim, Atribute.nume1, Atribute.temp1);
  numescor(r, c, 11, Atribute.col2, anim, Atribute.nume2, Atribute.temp2);

  color_set(8, 0);
  afisare(r, c, 13, 68 - anim / 2, " All-time score: ");
  numescor(r, c, 14, Atribute.col1, anim, Atribute.nume1, Scor1);
  numescor(r, c, 15, Atribute.col2, anim, Atribute.nume2, Scor2);

  color_set(8, 0);
  afisare(r, c, 17, 68 - anim / 2, " Game type: ");
  if (!Atribute.type)
    afisare(r, c, 17, 80 - anim / 2, "Classic ");
  else
    afisare(r, c, 17, 80 - anim / 2, "Pop out ");

  afisare(r, c, 19, 68 - anim / 2, " Total time: ");
  ore = sir(Atribute.ore);
  len = strlen(ore);
  if (len == 1)
  {
    len = 2;
    afisare(r, c, 19, 81 - anim / 2, "0");
    afisare(r, c, 19, 82 - anim / 2, ore);
  }
  else
    afisare(r, c, 19, 81 - anim / 2, invers(ore));
  afisare(r, c, 19, 81 - anim / 2 + len, ":");
  aux = sir(Atribute.min);
  if (strlen(aux) == 1)
  {
    afisare(r, c, 19, 82 - anim / 2 + len, "0");
    afisare(r, c, 19, 83 - anim / 2 + len, aux);
  }
  else
    afisare(r, c, 19, 82 - anim / 2 + len, invers(aux));
  free(aux);
  afisare(r, c, 19, 84 - anim / 2 + len, ":");
  aux = sir(Atribute.sec);
  if (strlen(aux) == 1)
  {
    afisare(r, c, 19, 85 - anim / 2 + len, "0");
    afisare(r, c, 19, 86 - anim / 2 + len, aux);
  }
  else
    afisare(r, c, 19, 85 - anim / 2 + len, invers(aux));
  afisare(r, c, 19, 87 - anim / 2 + len, " ");
  free(ore);
  free(aux);

  if (Substatus == NORMAL || Substatus == PAUSE || Substatus == GAME_OVER)
  {
    switch (Substatus)
    {
      case NORMAL:
      {
        afisare(r, c, 27, 31, "        ");
        afisare(r, c, 21, 68 - anim / 2, " P = Pause            ");
      }
      break;
      case PAUSE:
      {
        color_set(21, 0);
        afisare(r, c, 27, 31, " PAUSED ");
        color_set(8, 0);
        afisare(r, c, 21, 68 - anim / 2, " P = Resume           ");
      }
      break;
      case GAME_OVER:
      {
        afisare(r, c, 21, 68 - anim / 2, "                         ");
        afisare(r, c, 22, 68 - anim / 2, " P = Play Again          ");
      }
      break;
    }
    if (Substatus != GAME_OVER)
      afisare(r, c, 22, 68 - anim / 2, " S = Save                ");
    afisare(r, c, 23, 68 - anim / 2, " R = Return to Main Menu ");
    afisare(r, c, 24, 68 - anim / 2, " Q = Quit                ");
  }
  else if (Substatus == ENDING || Substatus == SAVING)
  {
    if (Substatus == SAVING && Pause == 1)
    {
      color_set(21, 0);
      afisare(r, c, 27, 31, " PAUSED ");
    }
    for (i = 1; i <= 4; i++)
      afisare(r, c, 20 + i, 68 - anim / 2, "                         ");
  }
  else
  {
    color_set(21, 0);
    afisare(r, c, 27, 31, "        ");
    afisare(r, c, 21, 68 - anim / 2, " Do you want to save? ");
    color_set(8, 0);
    afisare(r, c, 22, 68 - anim / 2, " Y = Yes                 ");
    afisare(r, c, 23, 68 - anim / 2, " N = No                  ");
    afisare(r, c, 24, 68 - anim / 2, " C = Cancel              ");
  }
  if (Substatus == SAVING)
  {
    color_set(21, 0);
    afisare(r, c, 1, 1, "Save as: ");
    color_set(8, 0);
    for (i = 1; i <= 18; i++)
      if (i - 1 < strlen(Atribute.titlu))
        mvaddch(r, c + 8 + i, Atribute.titlu[i - 1]);
      else
        afisare(r, c, 1, 9 + i, " ");
    aux = ctime(&data_ora);
    if (aux[18] % 2)
      mvaddch(r, c + 9 + strlen(Atribute.titlu), '_');
    else
      mvaddch(r, c + 9 + strlen(Atribute.titlu), ' ');
  }
  refresh();
}

int printable(int p)
{
  return ((p >= 'a' && p <= 'z') || (p >= '0' && p <= '9') || p == '_');
}

void letter(int p, int r, int c, int *status, int *pos, clock_t start, clock_t *last_update, int main_sel)
{                 // trateaza apasarea unei taste in timpul jocului propriu-zis
  struct
  {
    char nume[20];
    int scor;
  } temp;
  inreg joc;
  FILE *f, *g;
  int found1, found2;
  int len;
  int i;

  if (Substatus == SAVING && printable(p))
  {
    len = strlen(Atribute.titlu);
    if (len < 18)
    {
      Atribute.titlu[len] = p;
      Atribute.titlu[len + 1] = 0;
      playing_mode(r, c, 44, 2);
    }
  }
  switch (p)
  {
    case KEY_BACKSPACE:
      if (Substatus == SAVING)
      {
        len = strlen(Atribute.titlu);
        if (len)
          Atribute.titlu[len - 1] = 0;
        playing_mode(r, c, 44, 2);
      }
    break;
    case '\n':
    case KEY_ENTER:
      if (Substatus == SAVING)
      {
        if (!strlen(Atribute.titlu))
        {
          strcpy(Atribute.titlu, Deftit);
          Atribute.titlu[strlen(Deftit)] = 0;
        }
        save_game(r, c);
        if (Pause == 0 || Pause == 1)
        {
          Substatus = (Pause ? PAUSE : NORMAL);
          playing_mode(r, c, 44, 2);
        }
        if (Pause == 2 || Pause == 3)
        {
          color_set(7, 0);
          afisare(r, c, 1, 1, "                           ");
          *status = 0;
          *pos = 0;
          piesa(r, c, 0);
          Substatus = ENDING;
          destroy(PLAYING, r, c, 0, 0, 0, start, last_update);
          if (Pause == 2)
          {
            create(MAIN_MENU, r, c, main_sel, start, last_update);
            *status = MAIN_MENU;
            Substatus = NORMAL;
          }
          else
            Term = 1;
        }
      }
    break;
    case 'p':
      switch (Substatus)
      {
        case NORMAL:
        {
          Substatus = PAUSE;
          playing_mode(r, c, 44, 2);
        }
        break;
        case PAUSE:
        {
          Substatus = NORMAL;
          i = game_over(r, c);
          if (i)
          {
            Substatus = GAME_OVER;
            if (i == 1)
              Atribute.temp1++;
            else if (i == 2)
              Atribute.temp2++;
          }
          playing_mode(r, c, 44, 2);
        }
        break;
        case GAME_OVER:
        {
          Substatus = NORMAL;
          playing_mode(r, c, 44, 2);
          play_again(r, c, last_update);
          *pos = set_pos(r, c);
        }
        break;
      }
    break;
    case 's':
      if (Substatus == NORMAL || Substatus == PAUSE)
      {
        if (!Atribute.gid)
        {
          if (Substatus == NORMAL)
            Pause = 0;
          else
            Pause = 1;
          Substatus = SAVING;
          playing_mode(r, c, 44, 2);
        }
        else
          save_game(r, c);
      }
    break;
    case 'r':
    case 'q':
    {
      if (Substatus == GAME_OVER)
      {
        found1 = found2 = 0;
        if ((f = fopen("alltime.dat", "rb+")) != NULL)
        {
          while (fread(&temp, sizeof(temp), 1, f))
          {
            if (!strcmp(temp.nume, Atribute.nume1))
            {
              found1 = 1;
              temp.scor += Atribute.temp1;
              fseek(f, ftell(f) - sizeof(temp), 0);
              fwrite(&temp, sizeof(temp), 1, f);
            }
            if (!strcmp(temp.nume, Atribute.nume2))
            {
              found2 = 1;
              temp.scor += Atribute.temp2;
              fseek(f, ftell(f) - sizeof(temp), 0);
              fwrite(&temp, sizeof(temp), 1, f);
            }
          }
          fclose(f);
        }
        if (!found1 || !found2)
        {
          if ((f = fopen("alltime.dat", "ab")) != NULL)
          {
            if (!found1)
            {
              strcpy(temp.nume, Atribute.nume1);
              temp.nume[strlen(Atribute.nume1)] = 0;
              temp.scor = Atribute.temp1;
              fwrite(&temp, sizeof(temp), 1, f);
            }
            if (!found2)
            {
              strcpy(temp.nume, Atribute.nume2);
              temp.nume[strlen(Atribute.nume2)] = 0;
              temp.scor = Atribute.temp2;
              fwrite(&temp, sizeof(temp), 1, f);
            }
            fclose(f);
          }
        }
        if (Atribute.gid)
          if ((f = fopen("saved.dat", "rb")) && (g = fopen("saved~", "wb")))
          {
            while (fread(&joc, sizeof(joc), 1, f) == 1)
              if (joc.gid != Atribute.gid)
                fwrite(&joc, sizeof(joc), 1, g);
            fclose(f);
            fclose(g);
            remove("saved.dat");
            rename("saved~", "saved.dat");
          }
        color_set(7, 0);
        afisare(r, c, 1, 1, "                           ");
        *status = 0;
        *pos = 0;
        piesa(r, c, 0);
        Substatus = ENDING;
        destroy(PLAYING, r, c, 0, 0, 0, start, last_update);
        if (p == 'r')
        {
          create(MAIN_MENU, r, c, main_sel, start, last_update);
          *status = MAIN_MENU;
          Substatus = NORMAL;
        }
        else
          Term = 1;
      }
      else if (Substatus == NORMAL || Substatus == PAUSE)
      {
        if (Changed)
        {
          if (Substatus == NORMAL)
            Pause = 0;
          else
            Pause = 1;
          if (p == 'r')
            Substatus = RETURN_DIALOG;
          else
            Substatus = QUIT_DIALOG;
          playing_mode(r, c, 44, 2);
        }
        else
        {
          color_set(7, 0);
          afisare(r, c, 1, 1, "                           ");
          *status = 0;
          *pos = 0;
          piesa(r, c, 0);
          Substatus = ENDING;
          destroy(PLAYING, r, c, 0, 0, 0, start, last_update);
          if (p == 'r')
          {
            create(MAIN_MENU, r, c, main_sel, start, last_update);
            *status = MAIN_MENU;
            Substatus = NORMAL;
          }
          else
            Term = 1;
        }
      }
    }
    break;
    case 'y':
    case 'n':
    {
      if (Substatus == RETURN_DIALOG || Substatus == QUIT_DIALOG)
      {
        if (p == 'y' && !Atribute.gid)
        {
          if (Substatus == RETURN_DIALOG)
            Pause = 2;
          else
            Pause = 3; // Substatus = QUIT_DIALOG
          Substatus = SAVING;
          playing_mode(r, c, 44, 2);
        }
        else
        {
          if (p == 'y')
          {
            save_game(r, c);
            color_set(7, 0);
            afisare(r, c, 1, 1, "                           ");
          }
          *status = *pos = 0;
          piesa(r, c, 0);
          destroy(PLAYING, r, c, 0, 0, 0, start, last_update);
          if (Substatus == RETURN_DIALOG)
          {
            Substatus = ENDING;
            create(MAIN_MENU, r, c, main_sel, start, last_update);
            *status = MAIN_MENU;
            Substatus = NORMAL;
          }
          else
            Term = 1;
        }
      }
    }
    break;
    case 'c':
      if (Substatus == RETURN_DIALOG || Substatus == QUIT_DIALOG)
      {
        if (Pause)
          Substatus = PAUSE;
        else
        {
          Substatus = NORMAL;
          i = game_over(r, c);
          if (i)
          {
            Substatus = GAME_OVER;
            if (i == 1)
              Atribute.temp1++;
            else if (i == 2)
              Atribute.temp2++;
          }
        }
        playing_mode(r, c, 44, 2);
      }
    break;
  }
}

void update_time(int r, int c, clock_t *game_update)
{                          // updateaza data, ora si timpul total, daca e cazul
  time_t data_ora = time(NULL);

  if (((float)(clock() - *game_update)) / CLOCKS_PER_SEC > 1)
  {
    if (Substatus == NORMAL || Substatus == GAME_OVER || Substatus == ENDING)
    {
      Atribute.sec++;
      if (Atribute.sec == 60)
      {
        Atribute.sec = 0;
        Atribute.min++;
        if (Atribute.min == 60)
        {
          Atribute.min = 0;
          Atribute.ore++;
        }
      }
    }
    playing_mode(r, c, 44, 2);
    color_set(8, 0);
    afisare(r, c, 7, 46, "                          ");
    afisare(r, c, 7, 47, ctime(&data_ora));
    refresh();
    *game_update = clock();
  }
}

void cadere(WINDOW *wnd, int *nrows, int *ncols, int *r, int *c, int *pos, clock_t start, clock_t *game_update, int main_sel, int *status)
{                            // animatia de cadere a unei piese aruncate de sus
  int i;
  int limit; // liniia unde trebuie sa ajunga
  int line = 0;
  int lastr, lastc, interrupt = 0;
  clock_t t1, t2, dif;
  int p;

  for (i = 1; i <= 6; i++)
    if (Atribute.A[i][*pos])
      break;
  Falling = 1;
  limit = i * 3;
  do {
    line++;
    if (line % 3 != 1)
    {
      color_set((Atribute.tur ? Atribute.col2 : Atribute.col1), 0);
      afisare(*r, *c, line + 5, *pos * 6 - 3, "    ");
    }
    color_set(7, 0);
    if (line - 3 <= 0)
      afisare(*r, *c, line + 2, *pos * 6 - 4, "      ");
    else if (line % 3 != 1)
      afisare(*r, *c, line + 2, *pos * 6 - 3, "    ");
    t1 = clock();
    t2 = t1;
    // Speed / 3 = timpul parcurgerii unei linii
    while (((float)(t2 - t1)) / CLOCKS_PER_SEC * 1000 < Speed / 3)
    {
      dif = t2 - t1;
      update_time(*r, *c, game_update);
      p = get_key_pressed();
      if (p != NO_KEY_PRESSED)
      {
        p = tolower(p);
        if (printable(p) || p == KEY_BACKSPACE || p == KEY_ENTER || p == '\n')
        {
          letter(p, *r, *c, status, pos, start, game_update, main_sel);
          if (*status != PLAYING)
            interrupt = 1;
        }
        else
        {
          lastr = *r;
          lastc = *c;
          redim(wnd, nrows, ncols, r, c);
          if (lastr != *r || lastc != *c)
          {
            interrupt = 1;
            clscreen(wnd, *nrows, *ncols, *r, *c);
            playing_mode(*r, *c, 44, 1);
            i = game_over(*r, *c);
            if (i)
              *pos = 0;
            piesa(*r, *c, *pos);
          }
        }
      }
      if (interrupt)
        break;
      t2 = clock();
      if (Substatus == PAUSE || Substatus == RETURN_DIALOG || Substatus == QUIT_DIALOG || Substatus == SAVING)
        t1 = t2 - dif;
    }
    refresh();
    if (interrupt)
      break;
  } while (line != limit);
  Falling = 0;
}

void popout(WINDOW *wnd, int *nrows, int *ncols, int *r, int *c, int *pos, clock_t start, clock_t *game_update, int main_sel, int *status)
{               // disparitia unei piese eliminate si caderea celor de deasupra
  clock_t t1, t2, dif;
  int i, j;
  int p;
  int lastr, lastc;
  int interrupt = 0;

  *pos -= 7;
  color_set(7, 0);
  afisare(*r, *c, 22, *pos * 6 - 3, "    ");
  afisare(*r, *c, 23, *pos * 6 - 3, "    ");
  refresh();
  Falling = 1;
  for (j = 0; j <= 1; j++)
  {
    t1 = clock();
    t2 = t1;
    while (((float)(t2 - t1)) / CLOCKS_PER_SEC * 1000 < Speed / 3)
    {
      dif = t2 - t1;
      update_time(*r, *c, game_update);
      p = get_key_pressed();
      if (p != NO_KEY_PRESSED)
      {
        p = tolower(p);
        if (printable(p) || p == KEY_ENTER || p == '\n' || p == KEY_BACKSPACE)
        {
          letter(p, *r, *c, status, pos, start, game_update, main_sel);
          if (*status != PLAYING)
            interrupt = 1;
        }
        switch (p)
        {
          default:
          {
            lastr = *r;
            lastc = *c;
            redim(wnd, nrows, ncols, r, c);
            if (lastr != *r || lastc != *c)
            {
              interrupt = 1;
              clscreen(wnd, *nrows, *ncols, *r, *c);
              playing_mode(*r, *c, 44, 1);
            }
          }
          break;
        }
      }
      if (interrupt)
        break;
      t2 = clock();
      if (Substatus == PAUSE || Substatus == RETURN_DIALOG || Substatus == QUIT_DIALOG || Substatus == SAVING)
        t1 = t2 - dif;
    }
    for (i = 6; i >= 1; i--)
    {
      if (!Atribute.A[i][*pos])
        color_set(7, 0);
      else
        color_set((Atribute.A[i][*pos] == 1 ? Atribute.col1 : Atribute.col2), 0);
      afisare(*r, *c, i * 3 + 4 + j, *pos * 6 - 3, "    ");
    }
    refresh();
    if (interrupt)
      break;
  }
  Falling = 0;
}

void load_game(int r, int c, int load_sel_oriz, int *load_sel_vert, int *first, int anim)
{                                           // afiseaza ecranul Load Saved Game
  FILE *f;
  inreg temp;
  int k, i, j, size = 0;
  int line;
  int shown = 0;
  int len, pos;
  int ok;

  if ((f = fopen("saved.dat", "rb")) != NULL)
    ok = 1;
  else
    ok = 0;
  if (ok)
  {
    if (fread(&temp, sizeof(temp), 1, f) == 1)
      fseek(f, 0, SEEK_SET);
    else
    {
      ok = 0;
      fclose(f);
    }
  }
  if (!ok)
  {
    color_set(7, 0);
    for (i = 1; i <= HEIGHT; i++)
      for (j = 1; j <= WIDTH; j++)
        mvaddch(r + i - 1, c + j - 1, ' ');
    color_set(9, 0);
    afisare(r, c, 2, anim - 23, " < Back ");
    color_set(21, 0);
    afisare(r, c, 37 - anim, 14, "No saved games found. Press ENTER to return.");
    refresh();
    return;
  }
  if (*first <= 1)
    *first = 1;

  if (!load_sel_oriz)
    color_set(9, 0);
  else
    color_set(8, 0);
  afisare(r, c, 2, anim - 23, " < Back ");
  for (i = 4; i <= 25; i++)
    for (j = 4; j <= 65; j++)
      if (i + 22 - anim < HEIGHT)
        mvaddch(r + i + 22 - anim, c + j - 1, ' ');
  for (k = 1; k <= *first + 2; k++)
    if (fread(&temp, sizeof(temp), 1, f))
    {
      size++;
      if (k < *first)
        continue;
      shown++;
      for (i = 1; i <= 6; i++)
        for (j = 1; j <= 7; j++)
        {
          switch (temp.A[i][j])
          {
            case 0:
              color_set(3, 0);
            break;
            case 1:
              color_set(temp.col1, 0);
            break;
            case 2:
              color_set(temp.col2, 0);
            break;
          }
          afisare(r, c, (k - *first) * 7 + 27 + i - anim, 2 + j * 2, "  ");
        }
      if (load_sel_oriz == 1 && *load_sel_vert == k - *first + 1)
        color_set(9, 0);
      else
        color_set(8, 0);
      line = (k - *first) * 7 + 5;
      if (line + 22 - anim < HEIGHT)
        mvprintw(r + line + 22 - anim, c + 19, "%s (%s)", temp.titlu, temp.data);
      attroff(A_BOLD);
      if (line + 24 - anim < HEIGHT)
        mvprintw(r + line + 24 - anim, c + 19, "Game type: %s", (temp.type ? "Pop Out" : "Classic"));
      color_set(temp.col1 + 20, 0);
      if (line + 25 - anim < HEIGHT)
      {
        mvprintw(r + line + 25 - anim, c + 19, "%s", temp.nume1);
        color_set(((load_sel_oriz == 1 && *load_sel_vert == k - *first + 1) ? 9 : 8), 0);
        printw(" %d", temp.temp1);
      }
      color_set(temp.col2 + 20, 0);
      if (line + 26 - anim < HEIGHT)
      {
        mvprintw(r + line + 26 - anim, c + 19, "%s", temp.nume2);
        color_set(((load_sel_oriz == 1 && *load_sel_vert == k - *first + 1) ? 9 : 8), 0);
        printw(" %d", temp.temp2);
      }
      if (line + 27 - anim < HEIGHT)
        mvprintw(r + line + 27 - anim, c + 19, "Total time: %02d:%02d:%02d", temp.ore, temp.min, temp.sec);
      attron(A_BOLD);
    }
    else
      break;
  while (fread(&temp, sizeof(temp), 1, f) == 1)
    size++;
  fclose(f);
  if (!shown)
  {
    if (size >= 3)
      *first = size - 2;
    else
      *first = 1;
    load_game(r, c, load_sel_oriz, load_sel_vert, first, anim);
    refresh();
    return;
  }
  else if (shown < 3 && size >= 3)
  {
    *first = size - 2;
    load_game(r, c, load_sel_oriz, load_sel_vert, first, anim);
    refresh();
    return;
  }
  else if (*load_sel_vert > shown)
  {
    *load_sel_vert = shown;
    load_game(r, c, load_sel_oriz, load_sel_vert, first, anim);
    refresh();
    return;
  }
  color_set(8, 0);
  afisare(r, c, HEIGHT + 22 - anim, 1, "                                                          ");
  afisare(r, c, HEIGHT + 23 - anim, 1, "ENTER = Load    D = Delete    Up = Previous    Down = Next");

  // scrollbar
  if (size > 3 && anim == 23)
  {
    afisare(r, c, 28 - anim, WIDTH, "^");
    afisare(r, c, 47 - anim, WIDTH, "v");
    len = 54 / size;
    if (!len)
      len = 1;
    pos = (int)(1 + (float)((18 - len) * (*first - 1)) / (size - 3));
    if (!pos)
      pos = 1;
    for (i = 1; i < pos; i++)
      afisare(r, c, 28 + i - anim, WIDTH, "|");
    color_set(1, 0);
    if (*first == size - 2)
      for (i = pos; i <= 18; i++)
        afisare(r, c, 28 + i - anim, WIDTH, " ");
    else
    {
      for (i = 1; i <= len; i++)
        afisare(r, c, 27 + i + pos - anim, WIDTH, " ");
      color_set(8, 0);
      for (i = 1; i <= 19 - len - pos; i++)
        afisare(r, c, 27 + i + pos + len - anim, WIDTH, "|");
    }
  }
  else
    for (i = 4; i <= 25; i++)
      afisare(r, c, i + 23 - anim, WIDTH, " ");
  refresh();
}

void high_scores(int r, int c, int sel, int anim, int delscores)
{                                               // afiseaza ecranul High Scores
  int i, j;
  int k;
  int t;
  struct
  {
    char nume[20];
    int scor;
  } v[1000], temp, aux;
  int left;
  int n = 0;
  FILE *f;
  char *s;

  if (!delscores)
    left = anim - 58;
  else
    left = 0;
  color_set(8, 0);
  afisare(r, c, 2, 29 + left, " HIGHSCORES ");
  color_set(11, 0);
  afisare(r, c, 4, 13 + left, " No ");
  afisare(r, c, 4, 18 + left, " Name");
  for (i = 1; i <= 28; i++)
    afisare(r, c, 4, 22 + i + left, " ");
  afisare(r, c, 4, 52 + left, " Score ");
  color_set(7, 0);
  afisare(r, c, 4, 12 + left, " ");
  afisare(r, c, 4, 17 + left, " ");
  afisare(r, c, 4, 51 + left, " ");
  afisare(r, c, 4, 59 + left, " ");

  if ((f = fopen("alltime.dat", "rb")) != NULL)
  {
    while (fread(&temp, sizeof(temp), 1, f) == 1)
      if (temp.scor)
        v[++n] = temp;
    fclose(f);
  }
  t = n - 1;
  do {
    k = 1;
    for (i = 1; i <= t; i++)
      if (v[i].scor < v[i + 1].scor)
      {
        aux = v[i];
        v[i] = v[i + 1];
        v[i + 1] = aux;
        k = 0;
      }
    t--;
  } while (!k);
  attroff(A_BOLD);
  for (i = 1; i <= (n > 20 ? 20 : n); i++)
  {
    if (i % 2 == 0)
      color_set(11, 0);
    else
      color_set(26, 0);
    if (!delscores)
    {
      if (i % 4 <= 1)
        left = anim - 58;
      else
        left = 58 - anim;
    }
    else
    {
      if (i % 4 == 1 || i % 4 == 2)
        left = anim - 58;
      else
        left = 58 - anim;
    }
    s = sir(i);
    afisare(r, c, i + 4, left + 13, "    ");
    afisare(r, c, i + 4, left + 16 - strlen(s), invers(s));
    free(s);
    for (j = 18; j <= 50; j++)
      afisare(r, c, i + 4, j + left, " ");
    afisare(r, c, i + 4, left + 19, v[i].nume);
    afisare(r, c, i + 4, left + 52, "       ");
    s = sir(v[i].scor);
    afisare(r, c, i + 4, left + 58 - strlen(s), invers(s));
    free(s);
    color_set(8, 0);
    afisare(r, c, i + 4, 12 + left, " ");
    afisare(r, c, i + 4, 17 + left, " ");
    afisare(r, c, i + 4, 51 + left, " ");
    afisare(r, c, i + 4, 59 + left, " ");
  }
  if (!delscores)
    left = anim - 58;
  else
    left = 0;
  attron(A_BOLD);
  if (!sel)
    color_set(9, 0);
  else
    color_set(8, 0);
  afisare(r, c, 26, 12 + left, " < Back ");
  if (sel)
    color_set(9, 0);
  else
    color_set(8, 0);
  afisare(r, c, 26, 48 + left, " Clear data ");
  refresh();
}

void settings(int r, int c, int sel, int anim, clock_t cursor, clock_t *last_update)
{                                                 // afiseaza ecranul de setari
  int i, j;
  int left = anim - 43;
  FILE *f;
  char *s;
  int col;

  if (!sel)
    color_set(9, 0);
  else
    color_set(8, 0);
  afisare(r, c, 2, anim - 43, " < Back ");

  if ((f = fopen("tool_image", "r")) != NULL)
  {
    s = (char *)malloc(36);
    for (i = 1; i <= 22; i++)
    {
      fgets(s, 36, f);
      color_set(7, 0);
      afisare(r, c, 5 + i, 37 - left, " ");
      for (j = 0; j < 33; j++)
      {
        if (5 + i < 10 || 5 + i > 18 || 38 + j - left > 44 + left)
          color_set(s[j] - 48, 0);
        else
          color_set(7, 0);
        afisare(r, c, 5 + i, 38 + j - left, " ");
      }
    }
    free(s);
    fclose(f);
  }
  color_set(8, 0);
  afisare(r, c, 7, 18 + left, " SETTINGS ");
  if (sel == 1)
  {
    color_set(9, 0);
    afisare(r, c, 11, 1 + left, "> ");
  }
  else
  {
    color_set(8, 0);
    afisare(r, c, 11, 1 + left, "  ");
  }
  afisare(r, c, 11, 3 + left, "Viteza de cadere: un patratel la       ms ");
  if (sel == 2)
  {
    color_set(9, 0);
    afisare(r, c, 13, 1 + left, "> ");
  }
  else
  {
    color_set(8, 0);
    afisare(r, c, 13, 1 + left, "  ");
  }
  afisare(r, c, 13, 3 + left, "Player 1 default name:                    ");
  if (sel == 3)
  {
    color_set(9, 0);
    afisare(r, c, 15, 1 + left, "> ");
  }
  else
  {
    color_set(8, 0);
    afisare(r, c, 15, 1 + left, "  ");
  }
  afisare(r, c, 15, 3 + left, "Player 2 default name:                    ");
  if (sel == 4)
  {
    color_set(9, 0);
    afisare(r, c, 17, 1 + left, "> ");
  }
  else
  {
    color_set(8, 0);
    afisare(r, c, 17, 1 + left, "  ");
  }
  afisare(r, c, 17, 3 + left, "Default game title:                       ");
  afisare(r, c, 20, 19 + left, "        ");
  afisare(r, c, 22, 16, "              ");
  if (sel == 5)
    color_set(13, 0);
  else
    color_set(12, 0);
  afisare(r, c, 20, 20 + left, " Save ");
  color_set(11, 0);
  s = sir(Speed_temp);
  afisare(r, c, 11, 36 + left, invers(s));
  for (i = 0; i <= 4 - strlen(s); i++)
    afisare(r, c, 11, 36 + left + strlen(s) + i, " ");
  for (i = 1; i <= 18; i++)
  {
    if (25 + left + i <= 0)
      continue;
    if (i <= strlen(Def1_temp))
      mvaddch(r + 12, c + 24 + left + i, Def1_temp[i - 1]);
    else
      mvaddch(r + 12, c + 24 + left + i, ' ');
    if (i <= strlen(Def2_temp))
      mvaddch(r + 14, c + 24 + left + i, Def2_temp[i - 1]);
    else
      mvaddch(r + 14, c + 24 + left + i, ' ');
    if (i <= strlen(Deftit_temp))
      mvaddch(r + 16, c + 24 + left + i, Deftit_temp[i - 1]);
    else
      mvaddch(r + 16, c + 24 + left + i, ' ');
  }
  if (sel >= 1 && sel <= 4)
  {
    switch (sel)
    {
      case 1:
        col = 36 + strlen(s);
      break;
      case 2:
        col = 26 + strlen(Def1_temp);
      break;
      case 3:
        col = 26 + strlen(Def2_temp);
      break;
      case 4:
        col = 26 + strlen(Deftit_temp);
      break;
    }
    if (((int)(((float)(clock() - cursor)) / CLOCKS_PER_SEC * 1000)) % 1000 < 500)
      afisare(r, c, 9 + sel * 2, col + left, "_");
    else
      afisare(r, c, 9 + sel * 2, col + left, " ");
  }
  *last_update = clock();
  free(s);
  refresh();
}

void destroy(int status, int r, int c, int sel, int sel2, int first, clock_t cursor, clock_t *last_update)
{                                                     // distrugerea unui ecran
  int i, j, k, a[30][73];
  char *s;
  FILE *f;

  switch (status)
  {
    case MAIN_MENU:
    {
      if ((f = fopen("start_image", "r")) != NULL)
      {
        s = (char *)malloc(WIDTH + 3);
        for (i = 1; i <= HEIGHT; i++)
        {
          fgets(s, WIDTH + 3, f);
          for (j = 0; j < WIDTH; j++)
            a[i][j + 1] = s[j] - 48;
        }
        free(s);
        fclose(f);
      }
      else
        for (i = 1; i <= HEIGHT; i++)
          for (j = 1; j <= WIDTH; j++)
            a[i][j] = 7;
      for (k = 1; k <= 14; k++)
      {
        for (i = 1; i <= 6 - k; i++)
          for (j = 1; j <= 50; j++)
          {
            a[i][j] = a[i + 1][j];
            color_set(a[i][j], 0);
            mvaddch(r + i - 1, c + j - 1, ' ');
          }
        for (i = 1; i <= 15 - k; i++)
          for (j = 52; j <= WIDTH; j++)
          {
            a[i][j] = a[i + 1][j];
            color_set(a[i][j], 0);
            mvaddch(r + i - 1, c + j - 1, ' ');
          }
        for (i = HEIGHT; i >= 13 + k; i--)
          for (j = 1; j <= (i > 15 ? WIDTH : 62); j++)
          {
            a[i][j] = a[i - 1][j];
            color_set(a[i][j], 0);
            mvaddch(r + i - 1, c + j - 1, ' ');
          }
        main_menu(r, c, sel, -k * 2);
        refresh();
        stai(20);
      }
    }
    break;
    case OPTIONS:
      for (k = 1; k <= 54; k++)
      {
        options_menu(r, c, sel, k, cursor, last_update);
        stai(5);
      }
    break;
    case PLAYING:
    {
      for (k = 44; k >= 1; k--)
      {
        playing_mode(r, c, k, 1);
        stai(5);
      }
      color_set(7, 0);
      for (i = 1; i <= HEIGHT; i++)
        for (j = 1; j <= WIDTH; j += (j == 1 ? (WIDTH - 2) : 1))
          mvaddch(r + i - 1, c + j - 1, ' ');
      refresh();
      stai(5);
    }
    break;
    case LOAD:
      for (k = 22; k >= 0; k--)
      {
        load_game(r, c, sel, &sel2, &first, k);
        stai(13);
      }
    break;
    case HIGH:
      for (k = 57; k >= 0; k--)
      {
        high_scores(r, c, sel, k, 0);
        stai(5);
      }
    break;
    case SETTINGS:
      for (k = 42; k >= 0; k--)
      {
        settings(r, c, sel, k, cursor, last_update);
        stai(5);
      }
    break;
  }
}

void getNext(int *ln, int *col)
{                         // urmatorul element in cadrul parcurgerii in spirala
  while (*ln >= 1)
  {
    if (*ln % 2)
    {
      if (*col == 1)
        (*ln)--;
      else
        (*col)--;
    }
    else
    {
      if (*col == 7)
        (*ln)--;
      else
        (*col)++;
    }
    if (Atribute.A[*ln][*col])
      return;
  }
}

void create(int status, int r, int c, int sel, clock_t start, clock_t *last_update)
{                                                         // crearea unui ecran
  int k, i, j, a[30][73];
  int x, y;
  int pas;
  int fall[30][10];
  int modif;
  char *s;
  FILE *f;
  time_t data_ora = time(NULL);
  int first = 1;

  switch (status)
  {
    case MAIN_MENU:
    {
      if ((f = fopen("start_image", "r")) != NULL)
      {
        s = (char *)malloc(WIDTH + 3);
        for (i = 1; i <= HEIGHT; i++)
        {
          fgets(s, WIDTH + 3, f);
          for (j = 0; j < WIDTH; j++)
            a[i][j + 1] = s[j] - 48;
        }
        free(s);
        fclose(f);
      }
      else
        for (i = 1; i <= HEIGHT; i++)
          for (j = 1; j <= WIDTH; j++)
            a[i][j] = 7;
      for (k = 14; k >= 1; k--)
      {
        for (i = k; i <= 14; i++)
          for (j = (i == 14 ? 32 : 1); j <= WIDTH; j++)
          {
            color_set(a[i][j], 0);
            mvaddch(r + i - k, c + j - 1, ' ');
          }
        for (i = 14; i <= HEIGHT - k + 1; i++)
          for (j = 1; j <= (i == 14 ? 62 : WIDTH); j++)
          {
            color_set(a[i][j], 0);
            mvaddch(r + i + k - 2, c + j - 1, ' ');
          }
        main_menu(r, c, sel, -k * 2);
        stai(20);
      }
      main_menu(r, c, sel, 0);
    }
    break;
    case OPTIONS:
    {
      for (k = 52; k >= 0; k--)
      {
        options_menu(r, c, sel, k, start, last_update);
        stai(5);
      }
    }
    break;
    case PLAYING:
    {
      for (k = 1; k <= 44; k++)
      {
        playing_mode(r, c, k, 0);
        stai(5);
      }
      for (i = 0; i <= 26; i++)
        for (j = 1; j <= 7; j++)
          fall[i][j] = 0;
      x = 6;
      y = 0;
      pas = 0;
      do {
        modif = 0;
        if (pas % 4 == 0)
        {
          getNext(&x, &y);
          if (x >= 1)
            for (i = 1; i <= 3; i++)
              fall[i][y] = Atribute.A[x][y];
        }
        for (j = 1; j <= 7; j++)
          for (i = 26; i >= 4; i--)
          {
            if (!fall[i][j] && fall[i - 1][j])
            {
              modif = 1;
              for (k = i; k >= 1; k--)
                fall[k][j] = fall[k - 1][j];
              break;
            }
          }
        for (i = 4; i <= 26; i++)
          for (j = 1; j <= 7; j++)
            if (i >= 4 && i != 9 && i != 12 && i != 15 && i != 18 && i != 21 && i != 24)
            {
              if (fall[i][j] == 1)
                color_set(Atribute.col1, 0);
              else if (fall[i][j] == 2)
                color_set(Atribute.col2, 0);
              else
                color_set(7, 0);
              afisare(r, c, i - 3, j * 6 - 3, "    ");
              if (i <= 8)
              {
                afisare(r, c, i - 3, j * 6 - 4, " ");
                afisare(r, c, i - 3, j * 6 + 1, " ");
              }
            }
        pas++;
        refresh();
        stai(12);
        if ((float)(clock() - *last_update) / CLOCKS_PER_SEC > 1)
        {
          color_set(8, 0);
          data_ora = time(NULL);
          afisare(r, c, 7, 47, ctime(&data_ora));
          *last_update = clock();
        }
      } while (modif);
    }
    break;
    case LOAD:
      for (k = 1; k <= 23; k++)
      {
        load_game(r, c, 1, &sel, &first, k);
        stai(13);
      }
    break;
    case HIGH:
      for (k = 1; k <= 58; k++)
      {
        high_scores(r, c, sel, k, 0);
        stai(5);
      }
    break;
    case SETTINGS:
      for (k = 1; k <= 43; k++)
      {
        settings(r, c, sel, k, start, last_update);
        stai(5);
      }
    break;
  }
}

