#ifndef SERVEUR_H
#define SERVEUR_H

#define PORT_UDP 14545		// Port UDP pour les communications avec les joueurs
#define PORT_TCP 12345
#define TIME_TO_EXPLODE 3
#define TIME_TO_SEND_MAP 1
#define NBR_JOUEUR_MAX 4

#include "message.h"

// Structure representant une bombe
typedef struct bomb {
	long	time;
	int		x;
	int		y;
	struct bomb *next;
}	bomb;

// Structure representant une liste de bombes
typedef struct bomb_list {
	struct bomb	*head;
}	bomb_list;

// Structure contenant les informations d'une partie
typedef struct s_parti_info {
	int		nbr_joueur;
	int		sock_joueur[4];
	int		mode_equipe;
	char 	*map;
	map_info *maps;
	int		sock_tcp;
	int		sock_udp;
	int		sock_mdiff;
	int		port_mdiff;
	char	*adr_mdiff;
	pos		pos_joueur[4];
	struct pollfd	fds[6];
	int				fds_size;
	char	**freq_case;		// Tableau des cases modifiees pour la multidiffusion frequente
	int		case_changes;
	bomb_list	*bomb_list;		// Liste des bombes actives
	bomb_list	*bomb_clean;
}	parti_info;

// Structure contenant les informations de synchronisation des temps dans une partie
typedef struct s_time_part {
    long tempsDernierDeroulement;
    long tempsDernierFreqCases;
    long tempsActuel;
}   time_part;

// Fonctions de validation et de gestion des mouvements des joueurs
int		is_move_valid_pos(pos *pos_j, map_info *map, int action);
void	do_move_pos_with_action(parti_info *info, pos *pos_j, map_info *map, int action, int id);
pos		*get_pos(parti_info *info, int id);

// Fonctions de gestion des bombes
void	add_bomb_to_list(parti_info *info, bomb *bomb);
void	explode_or_clean_bomb(parti_info *info, bomb *to_explode, char c);
void	delete_bomb_from_bomb_list(bomb_list *bomb_list, bomb *to_destroy);
void	check_bomb(parti_info *info);

// Fonctions de gestion des cases modifiees pour la multidiffusion frequente
void	add_to_freq_case(parti_info *info, int x, int y, char c);
int		send_start_serve_msg(parti_info *info, int sock_client, int codereq, int team_choice);
int		send_end_game(parti_info *info);
int		send_tchat_msg(parti_info *info, char *buffer);
void	send_deroulement_partie(parti_info *info, char *map);
void	send_freq_cases(parti_info *info);

// Fonctions d'initialisation et de gestion des sockets du serveur
void	handle_sigint(int sig);
int		init_tcp_serv_sock();
int		init_udp_serv_sock();
int		init_mdiff_socket(parti_info *info, int port_mdiff);
void	init_pos_joueur(parti_info *info);
int		init_map_info_in_parti_info(parti_info *info);
void	init_freq_case(parti_info *info);
bomb	*init_bomb(int x, int y);
parti_info *init_parti_info(int sock_serv);

// Fonction de libération des ressources du serveur
void	free_server(parti_info *info);

#endif