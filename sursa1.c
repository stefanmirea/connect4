#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "antet.h"

int get_key_pressed()
{
  int ch = getch();
  if (ch != ERR)
    return ch;
  return NO_KEY_PRESSED;
}

int main()
{
  WINDOW* wnd = initscr();
  int nrows, ncols;
  int i, j;

  // optiunile selectate in diverse meniuri
  int main_sel = 1;
  int opt_sel = 1;
  int load_sel_oriz, load_sel_vert;
  int high_sel = 1;
  int set_sel;

  int p;
  int just_fallen = 0;
  int r, c; // linia si coloana coltului din stanga sus
  int status = MAIN_MENU;
  int first;
  int found;
  int len; // retine lungimea unui nume la realocare
  clock_t start = time(NULL); // timpul cand un camp de nume a fost selectat ultima data
  clock_t last_update = time(NULL); // ultimul update al ecranului de optiuni/setari
  clock_t game_update = time(NULL); // ultimul update al panoului de informatii
  FILE *f, *g;
  int pos;
  struct
  {
    char nume[20];
    int scor;
  } temp; // fisierul binar alltime.dat e alcatuit din astfel de structuri

  curs_set(0);
  clear();
  noecho();
  cbreak();
  keypad(wnd, TRUE);
  nodelay(wnd, TRUE);
  start_color();
  init_pairs();
  attron(A_BOLD);

  redim(wnd, &nrows, &ncols, &r, &c);
  clscreen(wnd, nrows, ncols, r, c);
  create(MAIN_MENU, r, c, main_sel, start, &last_update);

  if ((f = fopen("settings.ini", "r")) != NULL)
  {
    fscanf(f, "%d%s%s%s", &Speed, Def1, Def2, Deftit);
    fclose(f);
  }
  else
  {
    Speed = 35; // setari implicite
    strcpy(Def1, "player1");
    Def1[7] = 0;
    strcpy(Def2, "player2");
    Def2[7] = 0;
    strcpy(Deftit, "untitled");
    Deftit[8] = 0;
  }
  while (!Term)
  {
    p = get_key_pressed();
    if (p != NO_KEY_PRESSED)
    {
      p = tolower(p);
      switch (p)
      {
        case KEY_DOWN:
        {
          switch (status)
          {
            case MAIN_MENU:
            {
              main_sel++;
              if (main_sel > 5)
                main_sel = 1;
              main_menu(r, c, main_sel, 0);
            }
            break;
            case OPTIONS:
            {
              opt_sel++;
              if (opt_sel > 6)
                opt_sel = 0;
              if (opt_sel == 1 || opt_sel == 3)
                start = clock();
              options_menu(r, c, opt_sel, 0, start, &last_update);
            }
            break;
            case PLAYING:
            {
              if (Substatus == NORMAL && Atribute.type && pos >= 1 && pos <= 7)
                for (i = 1; i <= 7; i++)
                  if (Atribute.A[6][i] == Atribute.tur + 1)
                  {
                    pos = i + 7;
                    piesa(r, c, pos);
                    break;
                  }
            }
            break;
            case LOAD:
            {
              if (!load_sel_oriz)
                load_sel_oriz = 1;
              if (load_sel_vert == 3)
              {
                first++;
                load_game(r, c, 1, &load_sel_vert, &first, 23);
              }
              else
              {
                load_sel_vert++;
                load_game(r, c, 1, &load_sel_vert, &first, 23);
              }
            }
            break;
            case SETTINGS:
            {
              set_sel++;
              if (set_sel > 5)
                set_sel = 0;
              if (set_sel >= 1 && set_sel <= 4)
                start = clock();
              settings(r, c, set_sel, 43, start, &last_update);
            }
            break;
          }
        }
        break;
        case KEY_UP:
        {
          switch (status)
          {
            case MAIN_MENU:
            {
              main_sel--;
              if (main_sel < 1)
                main_sel = 5;
              main_menu(r, c, main_sel, 0);
            }
            break;
            case OPTIONS:
            {
              opt_sel--;
              if (opt_sel < 0)
                opt_sel = 6;
              if (opt_sel == 1 || opt_sel == 3)
                start = clock();
              options_menu(r, c, opt_sel, 0, start, &last_update);
            }
            break;
            case PLAYING:
            {
              if (Substatus == NORMAL && pos >= 7)
                for (i = 1; i <= 7; i++)
                  if (!Atribute.A[1][i])
                  {
                    pos = i;
                    piesa(r, c, pos);
                    break;
                  }
            }
            break;
            case LOAD:
            {
              if (!load_sel_oriz)
                load_sel_oriz = 1;
              if (load_sel_vert == 1)
              {
                first--;
                load_game(r, c, 1, &load_sel_vert, &first, 23);
              }
              else
              {
                load_sel_vert--;
                load_game(r, c, 1, &load_sel_vert, &first, 23);
              }
            }
            break;
            case SETTINGS:
            {
              set_sel--;
              if (set_sel < 0)
                set_sel = 5;
              if (set_sel >= 1 && set_sel <= 4)
                start = clock();
              settings(r, c, set_sel, 43, start, &last_update);
            }
            break;
          }
        }
        break;
        case KEY_LEFT:
        {
          switch (status)
          {
            case OPTIONS:
            {
              switch (opt_sel)
              {
                case 2:
                {
                  do {
                    Atribute.col1--;
                    if (Atribute.col1 < 0)
                      Atribute.col1 = 6;
                  } while (Atribute.col1 == Atribute.col2 || Atribute.col1 == 3);
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
                case 4:
                {
                  do {
                    Atribute.col2--;
                    if (Atribute.col2 < 0)
                      Atribute.col2 = 6;
                  } while (Atribute.col1 == Atribute.col2 || Atribute.col2 == 3);
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
                case 5:
                {
                  Atribute.type = 1 - Atribute.type;
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
              }
            }
            break;
            case PLAYING:
            {
              if (Substatus == NORMAL)
              {
                do {
                  pos--;
                  if (pos == 0)
                    pos = 7;
                  else if (pos == 7)
                    pos = 14;
                } while (Atribute.A[(pos <= 7 ? 1 : 6)][(pos <= 7 ? pos : (pos - 7))] != (pos <= 7 ? 0 : (Atribute.tur + 1)));
                piesa(r, c, pos);
              }
            }
            break;
            case LOAD:
              if (load_sel_oriz)
              {
                load_sel_oriz = 0;
                load_game(r, c, load_sel_oriz, &load_sel_vert, &first, 23);
              }
            break;
            case HIGH:
              if (high_sel)
              {
                high_sel = 0;
                high_scores(r, c, high_sel, 58, 0);
              }
            break;
          }
        }
        break;
        case KEY_RIGHT:
        {
          switch (status)
          {
            case OPTIONS:
            {
              switch (opt_sel)
              {
                case 2:
                {
                  do {
                    Atribute.col1++;
                    if (Atribute.col1 > 6)
                      Atribute.col1 = 0;
                  } while (Atribute.col1 == Atribute.col2 || Atribute.col1 == 3);
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
                case 4:
                {
                  do {
                    Atribute.col2++;
                    if (Atribute.col2 > 6)
                      Atribute.col2 = 0;
                  } while (Atribute.col1 == Atribute.col2 || Atribute.col2 == 3);
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
                case 5:
                {
                  Atribute.type = 1 - Atribute.type;
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
              }
            }
            break;
            case PLAYING:
            {
              if (Substatus == NORMAL)
              {
                do {
                  pos++;
                  if (pos == 8)
                    pos = 1;
                  else if (pos == 15)
                    pos = 8;
                } while (Atribute.A[(pos <= 7 ? 1 : 6)][(pos <= 7 ? pos : (pos - 7))] != (pos <= 7 ? 0 : (Atribute.tur + 1)));
                piesa(r, c, pos);
              }
            }
            break;
            case LOAD:
              if (!load_sel_oriz)
              {
                load_sel_oriz = 1;
                load_game(r, c, load_sel_oriz, &load_sel_vert, &first, 23);
              }
            break;
            case HIGH:
              if (!high_sel)
              {
                high_sel = 1;
                high_scores(r, c, high_sel, 58, 0);
              }
            break;
          }
        }
        break;
        case KEY_ENTER:
        case '\n':
        case ' ':
        {
          switch (status)
          {
            case MAIN_MENU:
            {
              switch (main_sel)
              {
                case 1: // New Game
                {
                  status = 0;
                  destroy(MAIN_MENU, r, c, main_sel, 0, 0, start, &last_update);
                  if ((f = fopen("last_conf.ini", "r")) != NULL)
                  {
                    fscanf(f, "%d %18s %d %18s %d", &Atribute.col1, Atribute.nume1, &Atribute.col2, Atribute.nume2, &Atribute.type);
                    fclose(f);
                  }
                  else
                  {
                    Atribute.col1 = 1; // culori implicite
                    Atribute.col2 = 4;
                    Atribute.nume1[0] = 0;
                    Atribute.nume2[0] = 0;
                    Atribute.type = 0;
                  }
                  opt_sel = 1;
                  start = clock();
                  create(OPTIONS, r, c, opt_sel, start, &last_update);
                  status = OPTIONS;
                }
                break;
                case 2: // Load Saved Game
                {
                  status = 0;
                  destroy(MAIN_MENU, r, c, main_sel, 0, 0, start, &last_update);
                  load_sel_oriz = load_sel_vert = 1;
                  first = 1;
                  create(LOAD, r, c, load_sel_vert, start, &last_update);
                  status = LOAD;
                }
                break;
                case 3: // Highscores
                {
                  status = 0;
                  destroy(MAIN_MENU, r, c, main_sel, 0, 0, start, &last_update);
                  high_sel = 0;
                  create(HIGH, r, c, high_sel, start, &last_update);
                  status = HIGH;
                }
                break;
                case 4: // Settings
                {
                  status = 0;
                  destroy(MAIN_MENU, r, c, main_sel, 0, 0, start, &last_update);
                  Speed_temp = Speed;
                  strcpy(Def1_temp, Def1);
                  Def1_temp[strlen(Def1)] = 0;
                  strcpy(Def2_temp, Def2);
                  Def2_temp[strlen(Def2)] = 0;
                  strcpy(Deftit_temp, Deftit);
                  Deftit_temp[strlen(Deftit)] = 0;
                  set_sel = 1;
                  start = clock();
                  create(SETTINGS, r, c, set_sel, start, &last_update);
                  status = SETTINGS;
                }
                break;
                case 5: // Quit
                  Term = 1;
                break;
              }
            }
            break;
            case OPTIONS:
            {
              switch (opt_sel)
              {
                case 0: // Back
                {
                  status = 0;
                  salvare();
                  destroy(OPTIONS, r, c, opt_sel, 0, 0, start, &last_update);
                  create(MAIN_MENU, r, c, main_sel, start, &last_update);
                  status = MAIN_MENU;
                }
                break;
                case 6: // Start
                {
                  status = 0;
                  salvare();
                  destroy(OPTIONS, r, c, opt_sel, 0, 0, start, &last_update);

                  Atribute.gid = 0;
                  for (i = 1; i <= 6; i++)
                    for (j = 1; j <= 7; j++)
                      Atribute.A[i][j] = 0;
                  Atribute.temp1 = Atribute.temp2 = 0;
                  Scor1 = Scor2 = 0;
                  if ((f = fopen("alltime.dat", "rb")) != NULL)
                  {
                    while (fread(&temp, sizeof(temp), 1, f) == 1)
                    {
                      if (!strcmp(temp.nume, Atribute.nume1))
                        Scor1 = temp.scor;
                      if (!strcmp(temp.nume, Atribute.nume2))
                        Scor2 = temp.scor;
                    }
                    fclose(f);
                  }

                  Atribute.ore = Atribute.min = Atribute.sec = 0;
                  Atribute.tur = 0;
                  Atribute.played = 0;
                  Atribute.titlu[0] = 0;

                  last_update = clock(); // pentru data si ora
                  create(PLAYING, r, c, 0, start, &last_update);
                  game_update = clock(); // pentru timpul trecut
                  status = PLAYING;
                  Changed = 0;
                  Substatus = NORMAL;
                  pos = set_pos(r, c);
                }
                break;
                default:
                {
                  opt_sel++;
                  if (opt_sel > 6)
                    opt_sel = 0;
                  if (opt_sel == 1 || opt_sel == 3)
                    start = clock();
                  options_menu(r, c, opt_sel, 0, start, &last_update);
                }
                break;
              }
            }
            break;
            case PLAYING:
            {
              if (Substatus == NORMAL)
              {
                Changed = 1;
                color_set(7, 0);
                afisare(r, c, 1, 1, "                    ");
                if (pos >= 1 && pos <= 7)
                {
                  for (i = 1; i <= 6; i++)
                    if (Atribute.A[i][pos])
                      break;
                  Atribute.A[i - 1][pos] = Atribute.tur + 1;
                  cadere(wnd, &nrows, &ncols, &r, &c, &pos, start, &game_update, main_sel, &status);
                  just_fallen = 1;
                }
                else
                {
                  piesa(r, c, 0);
                  for (i = 6; i >= 1; i--)
                    Atribute.A[i][pos - 7] = Atribute.A[i - 1][pos - 7];
                  popout(wnd, &nrows, &ncols, &r, &c, &pos, start, &game_update, main_sel, &status);
                }
                if (status == PLAYING)
                {
                  i = game_over(r, c);
                  if (i && Substatus == NORMAL)
                  {
                    Substatus = GAME_OVER;
                    if (i == 1)
                      Atribute.temp1++;
                    else if (i == 2)
                      Atribute.temp2++;
                    playing_mode(r, c, 44, 2);
                  }
                  if (!i && Substatus != GAME_OVER)
                  {
                    Atribute.tur = 1 - Atribute.tur;
                    pos = set_pos(r, c);
                  }
                }
              }
            }
            break;
            case LOAD:
            {
              if (!load_sel_oriz) // Back
              {
                status = 0;
                destroy(LOAD, r, c, load_sel_oriz, load_sel_vert, first, start, &last_update);
                create(MAIN_MENU, r, c, main_sel, start, &last_update);
                status = MAIN_MENU;
              }
              else
              {
                if ((f = fopen("saved.dat", "rb")) != NULL)
                {
                  found = 1;
                  for (i = 1; i <= first + load_sel_vert - 1; i++)
                    if (fread(&Atribute, sizeof(Atribute), 1, f) != 1)
                    {
                      found = 0;
                      break;
                    }
                  fclose(f);
                }
                else
                  found = 0;
                if (found) // exista salvarea specificata
                {
                  status = 0;
                  destroy(LOAD, r, c, load_sel_oriz, load_sel_vert, first, start, &last_update);
                  Scor1 = Scor2 = 0;
                  if ((f = fopen("alltime.dat", "rb")) != NULL)
                  {
                    while (fread(&temp, sizeof(temp), 1, f) == 1)
                    {
                      if (!strcmp(temp.nume, Atribute.nume1))
                        Scor1 = temp.scor;
                      if (!strcmp(temp.nume, Atribute.nume2))
                        Scor2 = temp.scor;
                    }
                    fclose(f);
                  }
                  last_update = clock(); // pentru data si ora
                  create(PLAYING, r, c, 0, start, &last_update);
                  game_update = clock(); // pentru timpul trecut
                  status = PLAYING;
                  Changed = 0;
                  Substatus = NORMAL;
                  i = game_over(r, c);
                  if (i)
                  {
                    Substatus = GAME_OVER;
                    if (i == 1)
                      Atribute.temp1++;
                    else if (i == 2)
                      Atribute.temp2++;
                    playing_mode(r, c, 44, 2);
                  }
                  else
                    pos = set_pos(r, c);
                }
                else
                {
                  status = 0;
                  destroy(LOAD, r, c, load_sel_oriz, load_sel_vert, first, start, &last_update);
                  create(MAIN_MENU, r, c, main_sel, start, &last_update);
                  status = MAIN_MENU;
                }
              }
            }
            break;
            case HIGH:
              if (!high_sel) // Back
              {
                status = 0;
                destroy(HIGH, r, c, high_sel, 0, 0, start, &last_update);
                create(MAIN_MENU, r, c, main_sel, start, &last_update);
                status = MAIN_MENU;
              }
              else // Clear data
              {
                for (i = 57; i >= 0; i--)
                {
                  high_scores(r, c, high_sel, i, 1);
                  stai(5);
                }
                remove("alltime.dat");
              }
            break;
            case SETTINGS:
              switch (set_sel)
              {
                case 0: // Back
                {
                  status = 0;
                  destroy(SETTINGS, r, c, set_sel, 0, 0, start, &last_update);
                  create(MAIN_MENU, r, c, main_sel, start, &last_update);
                  status = MAIN_MENU;
                }
                break;
                case 5: // Save
                {
                  if ((f = fopen("settings.ini", "w")) != NULL)
                  {
                    fprintf(f, "%d\n%s\n%s\n%s", Speed_temp, Def1_temp, Def2_temp, Deftit_temp);
                    fclose(f);
                    Speed = Speed_temp;
                    strcpy(Def1, Def1_temp);
                    Def1[strlen(Def1_temp)] = 0;
                    strcpy(Def2, Def2_temp);
                    Def2[strlen(Def2_temp)] = 0;
                    strcpy(Deftit, Deftit_temp);
                    Deftit[strlen(Deftit_temp)] = 0;
                    status = 0;
                    destroy(SETTINGS, r, c, set_sel, 0, 0, start, &last_update);
                    create(MAIN_MENU, r, c, main_sel, start, &last_update);
                    status = MAIN_MENU;
                  }
                  else
                  {
                    color_set(21, 0);
                    afisare(r, c, 22, 16, "Access denied.");
                  }
                }
                break;
                default:
                {
                  set_sel++;
                  start = clock();
                  settings(r, c, set_sel, 43, start, &last_update);
                }
                break;
              }
            break;
          }
        }
        break;
        case KEY_BACKSPACE:
        {
          switch (status)
          {
            case OPTIONS:
              switch (opt_sel)
              {
                case 1:
                {
                  len = strlen(Atribute.nume1);
                  if (len > 0)
                  {
                    Atribute.nume1[len - 1] = 0;
                    options_menu(r, c, opt_sel, 0, start, &last_update);
                  }
                }
                break;
                case 3:
                {
                  len = strlen(Atribute.nume2);
                  if (len > 0)
                  {
                    Atribute.nume2[len - 1] = 0;
                    options_menu(r, c, opt_sel, 0, start, &last_update);
                  }
                }
                break;
              }
            break;
            case SETTINGS:
              switch(set_sel)
              {
                case 1:
                {
                  Speed_temp /= 10;
                  settings(r, c, set_sel, 43, start, &last_update);
                }
                break;
                case 2:
                {
                  len = strlen(Def1_temp);
                  if (len > 0)
                  {
                    Def1_temp[len - 1] = 0;
                    settings(r, c, set_sel, 43, start, &last_update);
                  }
                }
                break;
                case 3:
                {
                  len = strlen(Def2_temp);
                  if (len > 0)
                  {
                    Def2_temp[len - 1] = 0;
                    settings(r, c, set_sel, 43, start, &last_update);
                  }
                }
                break;
                case 4:
                {
                  len = strlen(Deftit_temp);
                  if (len > 0)
                  {
                    Deftit_temp[len - 1] = 0;
                    settings(r, c, set_sel, 43, start, &last_update);
                  }
                }
                break;
              }
            break;
          }
        }
        break;
        default:
        {
          if (printable(p))
          {
            switch (status)
            {
              case OPTIONS:
              {
                switch (opt_sel)
                {
                  case 1:
                  {
                    len = strlen(Atribute.nume1);
                    if (len < 18)
                    {
                      Atribute.nume1[len] = p;
                      Atribute.nume1[len + 1] = 0;
                      options_menu(r, c, opt_sel, 0, start, &last_update);
                    }
                  }
                  break;
                  case 3:
                  {
                    len = strlen(Atribute.nume2);
                    if (len < 18)
                    {
                      Atribute.nume2[len] = p;
                      Atribute.nume2[len + 1] = 0;
                      options_menu(r, c, opt_sel, 0, start, &last_update);
                    }
                  }
                  break;
                }
              }
              break;
              case LOAD:
                if (p == 'd' && load_sel_oriz)
                  if ((f = fopen("saved.dat", "rb")) && (g = fopen("saved~", "wb")))
                  {
                    for (i = 1; i < first + load_sel_vert - 1; i++)
                    {
                      fread(&Atribute, sizeof(Atribute), 1, f);
                      fwrite(&Atribute, sizeof(Atribute), 1, g);
                    }
                    fread(&Atribute, sizeof(Atribute), 1, f);
                    while (fread(&Atribute, sizeof(Atribute), 1, f) == 1)
                      fwrite(&Atribute, sizeof(Atribute), 1, g);
                    fclose(f);
                    fclose(g);
                    remove("saved.dat");
                    rename("saved~", "saved.dat");
                    load_game(r, c, load_sel_oriz, &load_sel_vert, &first, 23);
                  }
              break;
              case SETTINGS:
                switch (set_sel)
                {
                  case 1:
                    if (p >= '0' && p <= '9' && Speed_temp < 1000)
                    {
                      Speed_temp = Speed_temp * 10 + p - 48;
                      settings(r, c, set_sel, 43, start, &last_update);
                    }
                  break;
                  case 2:
                  {
                    len = strlen(Def1_temp);
                    if (len < 18)
                    {
                      Def1_temp[len] = p;
                      Def1_temp[len + 1] = 0;
                      settings(r, c, set_sel, 43, start, &last_update);
                    }
                  }
                  break;
                  case 3:
                  {
                    len = strlen(Def2_temp);
                    if (len < 18)
                    {
                      Def2_temp[len] = p;
                      Def2_temp[len + 1] = 0;
                      settings(r, c, set_sel, 43, start, &last_update);
                    }
                  }
                  break;
                  case 4:
                  {
                    len = strlen(Deftit_temp);
                    if (len < 18)
                    {
                      Deftit_temp[len] = p;
                      Deftit_temp[len + 1] = 0;
                      settings(r, c, set_sel, 43, start, &last_update);
                    }
                  }
                }
              break;
            }
          }
          else
          {
            redim(wnd, &nrows, &ncols, &r, &c);
            switch (status)
            {
              case MAIN_MENU:
              {
                start_image(nrows, ncols, r, c);
                main_menu(r, c, main_sel, 0);
              }
              break;
              case OPTIONS:
              {
                clscreen(wnd, nrows, ncols, r, c);
                options_menu(r, c, opt_sel, 0, start, &last_update);
              }
              break;
              case PLAYING:
              {
                clscreen(wnd, nrows, ncols, r, c);
                playing_mode(r, c, 44, 1);
                if (Substatus == NORMAL || Substatus == PAUSE || Substatus == RETURN_DIALOG || Substatus == QUIT_DIALOG || Substatus == SAVING)
                  piesa(r, c, pos);
              }
              break;
              case LOAD:
              {
                clscreen(wnd, nrows, ncols, r, c);
                load_game(r, c, load_sel_oriz, &load_sel_vert, &first, 23);
              }
              break;
              case HIGH:
              {
                clscreen(wnd, nrows, ncols, r, c);
                high_scores(r, c, high_sel, 58, 0);
              }
              break;
              case SETTINGS:
              {
                clscreen(wnd, nrows, ncols, r, c);
                settings(r, c, set_sel, 43, start, &last_update);
              }
              break;
            }
          }
        }
        break;
      }
      if ((printable(p) || p == KEY_ENTER || p == '\n' || p == KEY_BACKSPACE) && status == PLAYING && !just_fallen)
        letter(p, r, c, &status, &pos, start, &game_update, main_sel);
      just_fallen = 0;
      refresh();
    }

    // actualizare cursor daca e cazul
    if (status == OPTIONS && (opt_sel == 1 || opt_sel == 3))
      if (((float)(clock() - last_update)) / CLOCKS_PER_SEC > 0.5)
        options_menu(r, c, opt_sel, 0, start, &last_update);
    if (status == SETTINGS && (set_sel >= 1 && set_sel <= 4))
      if (((float)(clock() - last_update)) / CLOCKS_PER_SEC > 0.5)
        settings(r, c, set_sel, 43, start, &last_update);

    // actualizare data, ora si timp total
    if (status == PLAYING)
      update_time(r, c, &game_update);
  }
  attroff(A_BOLD);
  endwin();
  return 0;
}

