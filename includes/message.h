#ifndef MESSAGE_H
#define MESSAGE_H

#define BUF_SIZE 1024
#define SCOPE_MULTICAST "eth0@if12"

#include "gui.h"

//Structure entete pour tous les messages
typedef struct entete_msg {
	int	codeReq;
	int	id;
	int	eq;
}	entete_msg;

//Structure pour les messages d'action
typedef struct c_action_msg {
	entete_msg	*entete;
	int			num;
	int			action;
}	action_msg;

// Structure pour les messages de demarrage du serveur
typedef struct s_start_msg {
	entete_msg	*entete;
	int			port;
	int			portmdiff;
	char		adrmdiff[16];
}	start_serve_msg;

// Structure pour les messages de tchat
typedef struct s_tchat_msg {
	entete_msg	*entete;
	int			len;
	char		*data;
}	tchat_msg;

// Structure pour les messages de deroulement de la partie (grille complete)
typedef struct s_deroulement_partie {
    entete_msg *entete;
    int        num;
    int        hauteur;
    int        largeur;
    char        *map;
}   deroulement_serve_partie;

// Structure pour les messages de cases modifiees (frequence de multidiffusion)
typedef struct s_freq_cases {
    entete_msg *entete;
    int        num;
    int        nb;
    char       **data;
}   freq_serve_cases;

// creation des messages et les mets dans des structures
entete_msg	*create_entete_msg(int codereq, int id, int eq);
action_msg	*create_action_msg(int codereq, int id, int eq, int num, int action);
start_serve_msg	*create_start_serve_msg(int codereq, int id, int eq, int port, int portmdiff, char adrmiff[16]);
tchat_msg	*create_tchat_msg(int codereq, int id, int eq, int len, char *data);
deroulement_serve_partie *create_deroulement_partie(int codereq, int id, int eq, int num, int hauteur, int largeur, char *map);
freq_serve_cases *create_freq_cases(int codereq, int id, int eq, int num, int nb, char **data);

// convertion des structures cree precedemment pour les envoyez
void	convert_entete_msg(char *buffer, entete_msg *msg);
void	convert_action_msg(char *buffer, action_msg *action_msg);
void	convert_start_serve_msg(char *buffer, start_serve_msg *msg);
void	convert_tchat_msg(char *buffer, tchat_msg *msg);
void    convert_deroulement_partie(char *buffer, deroulement_serve_partie *msg);
void    convert_freq_cases(char *buffer, freq_serve_cases *msg);

#endif
