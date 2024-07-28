#ifndef GUI_H
#define GUI_H

#include "utils.h"
#include "ncurses.h"

#define LARGEUR_GRILLE 30
#define HAUTEUR_GRILLE 30

// Definition des types de cases en utilisant des caracteres
#define VIDE '0'
#define MUR_INDESTRUCTIBLE '1'
#define MUR_DESTRUCTIBLE '2'
#define BOMBE '3'
#define EXPLOSION '4'
#define JOUEUR1 '5'
#define JOUEUR2 '6'
#define JOUEUR3 '7'
#define JOUEUR4 '8'

// Definition des actions possibles
#define MSG_TO_SEND 2
#define MOVE_NORTH 3
#define MOVE_EAST 4
#define MOVE_SOUTH 5
#define MOVE_WEST 6
#define DROP_BOMBE 7

#define TEXT_SIZE 255

// Enumeration des differentes actions possibles dans le jeu
typedef enum ACTION
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	SEND_MSG,
	BOMB,
	QUIT
} ACTION;

// Structure contenant les informations de la carte
typedef struct map_info
{
	char	*map;
	int		w;
	int		h;
}	map_info;

// Structure representant la position d'un joueur
typedef struct player_pos
{
	int l;
	int c;
} pos;

// Structure representant l'interface de jeu
typedef struct game_interface	
{
	WINDOW		*win;
	map_info	*map;
	int			start_x;
	int			start_y;
	int			lines;
	int			columns;
	char		id;
} game_gui;

// Structure representant l'interface de tchat
typedef struct tchat_interface
{
	WINDOW	*win;
	int		lines;
	int		columns;
	char	data[TEXT_SIZE];
	int		cursor;
	char	**message;
	int		*id;
	int		nbr_msg;
}	tchat_gui;

// Structure principale regroupant les interfaces graphique, jeu et tchat
typedef struct graphique_interface
{
	game_gui	*game;
	tchat_gui	*tchat;
	pos			*player;
	
}	gui;

bool	is_move_valid(char *map, int new_x, int new_y, int map_w);
bool	perform_action(gui *main_gui, ACTION a);
void	init_ncurses();

void	put_title(WINDOW *win, const char *name, int columns);
void	printing_map_in_game(game_gui *gui);

map_info	*init_map_info(const char *map, int map_w, int map_h);
game_gui	*init_game_gui(const char *map, int lines, int columns, int map_w, int map_h);

void	put_info_tchat(tchat_gui *gui, const char *error);

tchat_gui	*init_tchat_gui(int lines, int columns);

ACTION	control(gui *main_gui);
void	refresh_tchat(tchat_gui *gui);
void	refresh_game(game_gui *game);
void	set_pos(gui *gui, char to_find);

gui		*init_main_gui(char *map, char id, int map_w, int map_h);

int		update_gui(gui *gui);

void	free_all(gui *main_gui);
void	cleaning_tchat_msg(gui *main_gui);
void add_msg_to_tchat_panel(gui *main_gui, char *msg, int id);

#endif