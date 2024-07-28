#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
// #include "grille.h"

typedef struct client_info {
	int		id;
	int		eq;
	int		sock_tcp;			
	int		sock_udp;			
	int		sock_mdiff;			
	int		port_udp;			
	int		port_mdiff;			
	char	adrmdiff[16];		// Adresse de multidiffusion (IPv6) sous forme de chaine de caracteres
	char	*adrmdiff_str;		// Pointeur vers une chaine de caracteres pour l'adresse de multidiffusion
	struct pollfd fds[2];		// Tableau de structures pollfd pour la gestion des événements sur les sockets
	int		fds_size;
	gui		*gui;				// Pointeur vers la structure GUI pour l'interface utilisateur
}	client_info;

int		init_client_socket();
client_info *join_game(int sock, int mode_equipe, int team_choice);

#endif