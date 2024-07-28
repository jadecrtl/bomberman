#include "serveur.h"

// Variable globale pour gérer les interruptions
volatile sig_atomic_t glo_sig = 1;

// Accepte un nouveau joueur et l'ajoute à la liste des fds
int accept_new_player(parti_info *info)
{
	int sock_client = accept(info->sock_tcp, NULL, NULL);
	if (!sock_client)
		return (perror("accept in accept_new_player"), 0);
	if (fcntl(sock_client, F_SETFL, O_NONBLOCK) < 0)
		return (perror(("fcntl in accept_new_player")), 0);
	info->sock_joueur[info->nbr_joueur] = sock_client;
	struct pollfd tmp;
	memset(&tmp, 0, sizeof(struct pollfd));
	tmp.fd = sock_client;
	tmp.events = POLLIN;
	info->fds[info->fds_size] = tmp;
	info->fds_size++;
	return (1);
}

// Envoie la carte et les mises à jour de la partie aux clients
void envoyer_map(parti_info *info, time_part *timing)
{
	timing->tempsActuel = get_actual_time(); // Temps en microsecondes

	// Envoyer l'entête déroulement de partie toutes les secondes
	if (timing->tempsActuel - timing->tempsDernierDeroulement >= TIME_TO_SEND_MAP * 1000000)
	{
		timing->tempsDernierDeroulement = timing->tempsActuel;
		send_deroulement_partie(info, info->maps->map);
	}

	// Envoyer l'entête fréquence des cases toutes les 50 millisecondes
	if (timing->tempsActuel - timing->tempsDernierFreqCases >= 50000 && info->case_changes != 0)
	{
		timing->tempsDernierFreqCases = timing->tempsActuel;
		send_freq_cases(info);
	}
}

// Reçoit des données d'un client et traite la requête
int receive_data_from_client(int sock_client, parti_info *info)
{
	char buffer[1024] = {0};
	int r = recv(sock_client, buffer, 1024 - 1, 0);
	if (r < 0)
		return (perror("recv in receive_data_from_client"), 0);
	int codereq = extract_code_req(buffer);
	int action = 0;
	pos *joueur_actual = 0;
	if (codereq == 0)
		return (1);
	switch (codereq)
	{
	case 1:
	case 2:
		if (!send_start_serve_msg(info, sock_client, codereq, buffer[1]))
			return (1);
		break;
	case 3:
	case 4:
		if (info->nbr_joueur == NBR_JOUEUR_MAX)
		{
			if (!init_map_info_in_parti_info(info))
				return (1);
			init_pos_joueur(info);
		}
		break;
	case 5:
	case 6:
		action = extract_action(buffer);
		joueur_actual = get_pos(info, extract_id(buffer));
		if (!is_move_valid_pos(joueur_actual, info->maps, action + 3))
			return (1);
		do_move_pos_with_action(info, joueur_actual, info->maps, action + 3, extract_id(buffer));	
		break;
	case 7:
		if (!send_tchat_msg(info, buffer))
			return (1);
		break;
	default:
		break;
	}
	return (1);
}

// Gère les signaux d'interruption (SIGINT et SIGQUIT)
void handle_sigint(int sig) 
{
	if (sig == SIGINT || sig == SIGQUIT)
		glo_sig = 0;
}

int	ended(parti_info *info)
{
	if (!info->maps)
		return (0);
	int nbr_player_alive = 0;
	int	pos = 0;
	for (int i = 0; i < info->nbr_joueur; i++)
	{
		if (info->sock_joueur[i] != -1)
		{
			nbr_player_alive++;
			pos = i;
		}
	}
	if (nbr_player_alive == 1)
	{
		entete_msg *end = create_entete_msg(15, pos, 0);
		char buf[BUF_SIZE] = {0};
		convert_entete_msg(buf, end);
		send(info->sock_joueur[pos], buf, 3, 0);
		free(end);
		return (1);
	}
	return (0);
}

// Fonction principale du serveur
int main(void)
{
	int sock_serv = init_tcp_serv_sock();
	if (!sock_serv)
		return (1);

	parti_info *info = init_parti_info(sock_serv);
	if (!info)
		return (1);
	time_part timing; // Instance de la structure de gestion des temps
	timing.tempsDernierDeroulement = 0;
	timing.tempsDernierFreqCases = 0;

	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigint);

	// Boucle de réception des messages
	while (glo_sig)
	{
		if (poll(&info->fds[0], info->fds_size, 0) < 0)
			return (perror("poll in main server"), 1);
		for (int i = 0; i < info->fds_size; i++)
		{
			if (info->fds[i].revents & POLLIN)
			{
				if (info->fds[i].fd == info->sock_tcp) // Vérifie si c'est une nouvelle connexion
					accept_new_player(info);
				else
					receive_data_from_client(info->fds[i].fd, info);
			}
		}
		check_bomb(info); // Vérifie les bombes
		if (ended(info))
			break;
		if (info->maps)
			envoyer_map(info, &timing); // Envoie la carte et les mises à jour
	}
	free_server(info); // Libère les ressources du serveur
	close(sock_serv); // Ferme le socket du serveur
	return (0);
}
