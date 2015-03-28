#include <curses.h>

#ifndef WIDTH
#define WIDTH  70
#define HEIGHT 27
#define NO_KEY_PRESSED (-1)

//valorile lui status
#define MAIN_MENU 1
#define OPTIONS   2
#define PLAYING   3
#define LOAD      4
#define HIGH      5
#define SETTINGS  6

//valorile lui substatus
#define NORMAL        0
#define PAUSE         1
#define GAME_OVER     2
#define RETURN_DIALOG 3
#define QUIT_DIALOG   4
#define ENDING        5
#define SAVING        6

typedef struct
{
	int gid, col1, col2, type, temp1, temp2, ore, min, sec, tur, A[10][10], played;
	char nume1[20], nume2[20], data[25], titlu[20];
} inreg;

inreg Atribute;   // toate datele despre jocul curent
int Changed;  // daca s-au facut modificati si nu s-au salvat
int Term;     // daca jocul trebuie terminat
int Pause;    /* pentru Substatus = SAVING / RETURN_DIALOG / QUIT_DIALOG
                                                  | 0 -> PAUSE
		 daca Substatus = SAVING, pause = | 1 -> RETURN_DIALOG
						  | 2 -> QUIT_DIALOG */
int Substatus;    // util doar cand status = 0
int Scor1, Scor2; // all-time
int Falling;
int Speed, Speed_temp;
char Def1[20], Def2[20], Deftit[20], Def1_temp[20], Def2_temp[20], Deftit_temp[20];

int printable(int);
void getNext(int *, int *);
void paly_again(int, int, clock_t *);
void letter(int, int, int, int *, int *, clock_t, clock_t *, int);
void save_game(int, int);
int get_key_pressed();
int game_over(int, int);
const char *del(int, const char *);
char *begin(int, const char *);
void stai(int);
void salvare();
void init_pairs();
void afisare(int, int, int, int, const char *);
void clscreen(WINDOW *, int, int, int, int);
void redim(WINDOW *, int *, int *, int *, int *);
void write_name(int, int, int, char *, int, int, clock_t);
void start_image(int, int, int, int);
void main_menu(int, int, int, int);
void options_menu(int, int, int, int, clock_t, clock_t *);
void piesa(int, int, int);
int set_pos(int, int);
void update_time(int, int, clock_t *);
void playing_mode(int, int, int, int);
void cadere(WINDOW *, int *, int *, int *, int *, int *, clock_t, clock_t *, int, int *);
void popout(WINDOW *, int *, int *, int *, int *, int *, clock_t, clock_t *, int, int *);
void load_game(int, int, int, int *, int *, int);
void high_scores(int, int, int, int, int);
void settings(int, int, int, int, clock_t, clock_t *);
void destroy(int, int, int, int, int, int, clock_t, clock_t *);
void create(int, int, int, int, clock_t, clock_t *);

#endif
