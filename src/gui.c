#include "gui.h"

// Initialisation de ncurses
void init_ncurses()
{
	initscr();							   // Démarre le mode curses
	raw();								   // Désactive le buffering de ligne
	intrflush(stdscr, FALSE);			   // Pas besoin de flush lors de la pression de la touche interrupt
	keypad(stdscr, TRUE);				   // Nécessaire pour obtenir les événements du clavier
	nodelay(stdscr, TRUE);				   // Rend getch non-bloquant
	noecho();							   // N'affiche pas les caractères lors de getch
	curs_set(0);						   // Rendre le curseur invisible
	start_color();						   // Active les couleurs
	init_pair(1, COLOR_RED, COLOR_BLACK);  // Définit un style de couleur (texte rouge, fond noir)
	init_pair(2, COLOR_WHITE, COLOR_RED);  // Définit un style de couleur (texte blanc, fond rouge)
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_BLACK, COLOR_WHITE); // wall color 
	init_pair(5, COLOR_WHITE, COLOR_BLUE); // player color 0
	init_pair(6, COLOR_WHITE, COLOR_GREEN); // player color 1
	init_pair(7, COLOR_WHITE, COLOR_YELLOW); // player color 2
	init_pair(8, COLOR_WHITE, COLOR_CYAN); // player color 3
}

// Affiche le titre dans une fenêtre ncurses
void put_title(WINDOW *win, const char *name, int columns)
{
	wattron(win, COLOR_PAIR(1));
	box(win, 0, 0);
	wattroff(win, COLOR_PAIR(1));

	int name_size = strlen(name);
	int mid = columns / 2 - name_size / 2;
	wattron(win, COLOR_PAIR(2));
	for (int i = mid, j = 0; j < name_size; i++, j++)
		mvwaddch(win, 0, i, name[j]);
	wattroff(win, COLOR_PAIR(2));
}

// Affiche la carte du jeu dans la fenêtre
void printing_map_in_game(game_gui *gui)
{
	gui->start_x = gui->lines / 2 - gui->map->h / 2;
	gui->start_y = gui->columns / 2 - gui->map->w / 2;

	int buf_ite = 0;
	int end_h = gui->map->h + gui->start_x;
	int end_w = gui->map->w + gui->start_y;

	for (int i = gui->start_x; i < end_h; i++)
	{
		for (int j = gui->start_y; j < end_w; j++)
		{
			if (gui->map->map[buf_ite] == '5')
			{
				wattron(gui->win, COLOR_PAIR(5));
				mvwaddch(gui->win, i, j, gui->map->map[buf_ite]);
				wattroff(gui->win, COLOR_PAIR(5));
			}
			else if (gui->map->map[buf_ite] == '4')
			{
				wattron(gui->win, COLOR_PAIR(2));
				mvwaddch(gui->win, i, j, '*');
				wattroff(gui->win, COLOR_PAIR(2));
			}
			else if (gui->map->map[buf_ite] == '3')
			{
				wattron(gui->win, COLOR_PAIR(2));
				mvwaddch(gui->win, i, j, 'B');
				wattroff(gui->win, COLOR_PAIR(2));
			}
			else if (gui->map->map[buf_ite] == '0')
				mvwaddch(gui->win, i, j, ' ');
			else if (gui->map->map[buf_ite] == '1')
			{
				wattron(gui->win, COLOR_PAIR(4));
				mvwaddch(gui->win, i, j, 'W');
				wattroff(gui->win, COLOR_PAIR(4));
			}
			else if (gui->map->map[buf_ite] == '2')
			{
				wattron(gui->win, COLOR_PAIR(4));
				mvwaddch(gui->win, i, j, 'D');
				wattroff(gui->win, COLOR_PAIR(4));
			}
			else if (gui->map->map[buf_ite] == '6')
			{
				wattron(gui->win, COLOR_PAIR(6));
				mvwaddch(gui->win, i, j, gui->map->map[buf_ite]);
				wattroff(gui->win, COLOR_PAIR(6));
			}
			else if (gui->map->map[buf_ite] == '7')
			{
				wattron(gui->win, COLOR_PAIR(7));
				mvwaddch(gui->win, i, j, gui->map->map[buf_ite]);
				wattroff(gui->win, COLOR_PAIR(7));
			}
			else if (gui->map->map[buf_ite] == '8')
			{
				wattron(gui->win, COLOR_PAIR(8));
				mvwaddch(gui->win, i, j, gui->map->map[buf_ite]);
				wattroff(gui->win, COLOR_PAIR(8));
			}
			else
				mvwaddch(gui->win, i, j, gui->map->map[buf_ite]);
			buf_ite++;
		}
	}
}

// Initialise les informations de la carte
map_info *init_map_info(const char *map, int map_w, int map_h)
{
	map_info *res = calloc(1, sizeof(map_info));
	if (!res)
		return (perror("init_map_info"), NULL);
	res->w = map_w;
	res->h = map_h;
	res->map = strdup(map);
	if (!res->map)
		return (perror("init_map_info"), free(res), NULL);
	return (res);
}

// Initialise l'interface de jeu
game_gui *init_game_gui(const char *map, int lines, int columns, int map_w, int map_h)
{
	game_gui *res = calloc(1, sizeof(game_gui));
	if (!res)
		return (perror("init_game_gui"), NULL);
	if (!(res->map = init_map_info(map, map_w, map_h)))
		return (free(res), perror("init_game_gui"), NULL);
	res->lines = lines;
	res->columns = columns - (columns / 3);
	WINDOW *win = newwin(lines, res->columns, 0, 0);
	res->win = win;
	put_title(win, "G-A-M-E", res->columns);
	printing_map_in_game(res);
	return (res);
}

// Affiche des informations dans la fenêtre de tchat
void put_info_tchat(tchat_gui *gui, const char *error)
{
	const char *msg = "Newest msg on top";
	int msg_size = strlen(msg);
	wattron(gui->win, COLOR_PAIR(3));
	for (int i = 1 , j = 0; j < msg_size; i++, j++)
		mvwaddch(gui->win, 1, i, msg[j]);
	mvwaddch(gui->win, 1, msg_size + 1, '|');
	for (int i = 1; i < gui->columns - 1; i++)
		mvwaddch(gui->win, 2, i, '-');
	wattroff(gui->win, COLOR_PAIR(3));
	wattron(gui->win, COLOR_PAIR(1));
	for (int i = msg_size + 2; i < (int)((gui->columns - msg_size + 1) / 2 + msg_size + 1 - (strlen(error) / 2)); i++)
		mvwaddch(gui->win, 1, i,' ');
	for (int i = (gui->columns - msg_size + 1) / 2 + msg_size + 1 - (strlen(error) / 2), j = 0; j < (int)strlen(error); j++, i++)
		mvwaddch(gui->win, 1, i, error[j]);
	for (int i = (gui->columns - msg_size + 1) / 2 + msg_size + 1 - (strlen(error) / 2) + strlen(error); i < gui->columns - 1 ;i++)
		mvwaddch(gui->win, 1, i,' ');
	wattroff(gui->win, COLOR_PAIR(1));
}

// Initialise l'interface de tchat
tchat_gui *init_tchat_gui(int lines, int columns)
{
	tchat_gui *res = calloc(1, sizeof(tchat_gui));
	if (!res)
		return (perror("init_tchat_gui"), NULL);
	res->lines = lines;
	res->columns = columns / 3;
	res->cursor = 0;
	memset(res->data, 0, TEXT_SIZE);
	WINDOW *win = newwin(lines, res->columns, 0, columns - res->columns);
	res->win = win;
	put_title(win, "T-C-H-A-T", res->columns);
	put_info_tchat(res, "/!\\ No error /!\\");
	for (int i = 1; i < res->columns - 1; i++)
		mvwaddch(win, lines - 3, i, '_');
	res->message = calloc(lines - 2, sizeof(char *));
	if (!res->message)
		return (free(res), NULL);
	res->id = calloc(lines - 2, sizeof(int));
	if (!res->id)
		return (free(res->message), free(res), NULL);
	res->nbr_msg = 0;
	return (res);
}

// Ajoute un message au panneau de tchat
void add_msg_to_tchat_panel(gui *main_gui, char *msg, int id)
{
	if (main_gui->tchat->nbr_msg + 1 == main_gui->tchat->lines - 5)
	{
		for (int i = 0; i < main_gui->tchat->nbr_msg; i++)
			free(main_gui->tchat->message[i]);
		main_gui->tchat->nbr_msg = 0;
	}
	main_gui->tchat->message[main_gui->tchat->nbr_msg] = strdup(msg);
	main_gui->tchat->id[main_gui->tchat->nbr_msg] = id;
	main_gui->tchat->nbr_msg++;
}

// Nettoie les messages du panneau de tchat
void cleaning_tchat_msg(gui *main_gui)
{
	memset(main_gui->tchat->data, 0, TEXT_SIZE);
	main_gui->tchat->cursor = 0;
	put_info_tchat(main_gui->tchat, "/!\\ No error /!\\");
}

// Gère les entrées clavier et les actions associées
ACTION control(gui *main_gui)
{
	int c;
	int prev_c = ERR;
	// Consomme toutes les pressions de touches consécutives similaires
	while ((c = getch()) != ERR)
	{
		if (prev_c != ERR && prev_c != c)
		{
			ungetch(c); // Remet 'c' dans la file d'attente
			break;
		}
		prev_c = c;
	}
	ACTION a = NONE;
	switch (prev_c)
	{
	case ERR:
		break;
	case KEY_LEFT:
		a = LEFT;
		break;
	case KEY_RIGHT:
		a = RIGHT;
		break;
	case KEY_UP:
		a = UP;
		break;
	case KEY_DOWN:
		a = DOWN;
		break;
	case '*' :
		a = BOMB;
		break;
	case '~': case 27: // Pour la touche escape mais pas instantané
		a = QUIT;
		break;
	case KEY_ENTER : case 10 : // Vide le buffer et l'ajoute aux messages
	
	// add_msg_to_tchat_panel(main_gui); // trigger une fonction qui envoie au serveur et la renvoie au client qui lui appelle add_msg_to_tchat_panel
		a = SEND_MSG;
		// cleaning_tchat_msg(main_gui);
		break;
	case KEY_BACKSPACE: case 127:
		if (main_gui->tchat->cursor > 0)
			main_gui->tchat->cursor--;
		put_info_tchat(main_gui->tchat, "/!\\ No error /!\\");
		break;
	default:
		if (prev_c >= ' ' && prev_c <= '~' && main_gui->tchat->cursor < main_gui->tchat->columns - 2)
			main_gui->tchat->data[(main_gui->tchat->cursor)++] = prev_c;
		if (main_gui->tchat->cursor == main_gui->tchat->columns - 2)
			put_info_tchat(main_gui->tchat, "/!\\ BUFFER FULL /!\\");
		break;
	}	
	return a;
}

// Rafraîchit la fenêtre de tchat
void refresh_tchat(tchat_gui *gui)
{
	for (int k = 0; k < gui->nbr_msg; k++)
	{
		wattron(gui->win, COLOR_PAIR(gui->id[k] + 5));
		mvwaddstr(gui->win, k + 3, 1, gui->message[k]);
		for (int j = strlen(gui->message[k]); j < gui->columns - 2 ; j++)
			mvwaddch(gui->win, k + 3, j + 1, ' ');
		wattroff(gui->win, COLOR_PAIR(gui->id[k] + 5));
	}
	int i = 1;
	wattron(gui->win, A_BOLD);
	for (int j = 0; j < gui->cursor && i < gui->columns - 1; i++, j++)
		mvwaddch(gui->win, gui->lines - 2, i, gui->data[j]);
	wattroff(gui->win, A_BOLD);
	for (; i < gui->columns - 1; i++)
		mvwaddch(gui->win, gui->lines - 2, i, ' ');
	wrefresh(gui->win);
}

// Rafraîchit la fenêtre de jeu
void refresh_game(game_gui *game)
{
	printing_map_in_game(game);
	wrefresh(game->win);
}

// Définit la position du joueur sur la carte
void set_pos(gui *gui, char to_find)
{
	char *map = gui->game->map->map;
	int map_c = gui->game->map->w;
	int nbr_nl = 0;

	int map_len = strlen(map);
	int pos_to_find = 0;
	for (; pos_to_find < map_len; pos_to_find++)
	{
		if (map[pos_to_find] == to_find)
			break;
	}
	pos_to_find -= nbr_nl;
	gui->player->l = pos_to_find / map_c;
	gui->player->c = pos_to_find % map_c;
}

// Libère toutes les ressources allouées
void free_all(gui *main_gui)
{
	curs_set(1); // Rendre le curseur visible à nouveau
	nodelay(stdscr, FALSE);
	intrflush(stdscr, TRUE);
	endwin();	 // Termine le mode curses
	werase(main_gui->game->win);
	wclear(main_gui->game->win);
	werase(main_gui->tchat->win);
	wclear(main_gui->tchat->win);
	delwin(main_gui->game->win);
	delwin(main_gui->tchat->win);
	free(main_gui->player);
	free(main_gui->game->map->map);
	free(main_gui->game->map);
	free(main_gui->tchat->id);
	for (int i = 0; i < main_gui->tchat->nbr_msg; i++)
		free(main_gui->tchat->message[i]);
	free(main_gui->tchat->message);
	free(main_gui->tchat);
	free(main_gui->game);
	free(main_gui);
}

// Initialise l'interface principale
gui *init_main_gui(char *map, char id, int map_w, int map_h)
{
	init_ncurses();
	int lines = 0;
	int columns = 0;
	getmaxyx(stdscr, lines, columns);
	gui *main_gui = calloc(1, sizeof(gui));
	if (!main_gui)
		return (NULL);
	game_gui *g_gui = init_game_gui(map, lines, columns, map_w, map_h);
	if (!g_gui)
		return (free(main_gui), NULL);
	g_gui->id = id;
	tchat_gui *t_gui = init_tchat_gui(lines, columns);
	if (!t_gui)
		return (free(g_gui), free(main_gui), NULL);
	pos *p = calloc(1, sizeof(pos));
	if (!p)
		return (free(g_gui), free(main_gui), free(t_gui), NULL);
	main_gui->game = g_gui;
	main_gui->tchat = t_gui;
	main_gui->player = p;

	set_pos(main_gui, id);
	return (main_gui);
}

// Met à jour l'interface graphique
int update_gui(gui *gui)
{
	if (!gui)
		return (0);
	ACTION a = control(gui);
	if (a == QUIT)
		return (0);
	if (a == SEND_MSG)
		return (2);
	if (a == UP)
		return (MOVE_NORTH);
	if (a == LEFT)
		return (MOVE_WEST);
	if (a == DOWN)
		return (MOVE_SOUTH);
	if (a == RIGHT)
		return (MOVE_EAST);
	if (a == BOMB)
		return (DROP_BOMBE);
	refresh_game(gui->game);
	refresh_tchat(gui->tchat);
	return (1);
}
