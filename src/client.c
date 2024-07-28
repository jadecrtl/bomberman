#include "client.h"

int num_msg = 0;
int	dead = 0;

// Cree une nouvelle structure client_info a partir des donnees reçues du serveur
client_info	*create_new_client(char *buffer, int tcp_socket)
{
	if (!buffer)
		return (NULL);

	client_info *res = calloc(1, sizeof(client_info));
	if (!res)
		return (dprintf(2, "Alloc error in join_game\n"), NULL);

	u_int16_t tmp;
	memmove(&tmp, &buffer[0], 2);
	tmp = ntohs(tmp);
	res->id = tmp >> 13 & sum_of_bit(2);
	res->eq = tmp >> 15 & 1;

	memmove(&tmp, &buffer[2], 2);
	res->port_udp = ntohs(tmp);

	memmove(&tmp, &buffer[4], 2);
	res->port_mdiff = ntohs(tmp);

	memset(res->adrmdiff, 0, 16);
	for (int i = 0; i < 16; i++)
	{
		memmove(&tmp, &buffer[6 + (i * 2)], 2);
		tmp = ntohs(tmp);
		res->adrmdiff[i] = tmp;
	}
	res->adrmdiff_str = create_ipv6_mdiff_client(res->adrmdiff);
	res->sock_tcp = tcp_socket;
	res->fds_size = 0;

	// Initialise la structure pollfd pour surveiller les événements sur le socket TCP
	struct pollfd fd;
	memset(&fd, 0, sizeof(struct pollfd));
	fd.fd = tcp_socket;
	fd.events = POLLIN;
	res->fds[res->fds_size] = fd;
	res->fds_size++;
	return (res);
}

// Rejoint une partie en envoyant une requete au serveur
client_info	*join_game(int sock, int mode_equipe, int team_choice)
{
	int codereq = 1; // mode solo
	if (mode_equipe)
		codereq = 2;
	entete_msg *msg = create_entete_msg(codereq, 0, team_choice);
	if (!msg)
		return (dprintf(2, "Alloc error in join_game\n"), NULL);
	char buffer[BUF_SIZE] = {0};
	convert_entete_msg(buffer, msg);

	free(msg);
	if (send(sock, buffer, 2, 0) < 0)
		return (perror("send in join_game"), NULL);
	memset(buffer, 0, BUF_SIZE);
	if (recv(sock, buffer, BUF_SIZE - 1, 0) < 0)
		return (perror("recv in join_game"), NULL);
	return (create_new_client(buffer, sock));
}

// Envoie un message indiquant que le client est pret a jouer
void	ready_to_play(client_info *info, int mode_equipe)
{
	int codereq = 3;
	if (mode_equipe)
		codereq = 4;
	int id = info->id;
	int eq = info->eq;
	entete_msg *msg = create_entete_msg(codereq, id, eq);
	if (!msg)
		return ((void)dprintf(2, "Alloc error in ready_to_play\n"));
	char buffer[BUF_SIZE] = {0};
	convert_entete_msg(buffer, msg);
	free(msg);
	if (send(info->sock_tcp, buffer, 2, 0) < 0)
		return ((void)perror("send in ready_to_play\n"));
}

// S'abonne a la multidiffusion pour recevoir les messages du serveur
int	subscribe_mdiff(client_info *info)
{
	int sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (!sock)
		return (perror("socket in subscribe_mdiff"), 0);

	struct sockaddr_in6 adr;
	memset(&adr, 0, sizeof(adr));
	adr.sin6_family = AF_INET6;
	adr.sin6_addr = in6addr_any;
	adr.sin6_port = htons(info->port_mdiff);

	// Configure le socket pour reutiliser l'adresse
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		return (perror("setsockopt in subscribe_mdiff"), close(sock), 0);

	if (bind(sock, (struct sockaddr *)&adr, sizeof(adr)) < 0)
		return (perror("bind in subscribe_mdiff"), close(sock), 0);
	struct ipv6_mreq group;
	inet_pton(AF_INET6, info->adrmdiff_str , &group.ipv6mr_multiaddr.s6_addr);
	group.ipv6mr_interface = if_nametoindex(SCOPE_MULTICAST);
	// Rejoint le groupe de multidiffusion
	if (setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof(group)) < 0)
		return (perror("subscribe in sub_mdiff"), close(sock), 0);
	info->sock_mdiff = sock;
	struct pollfd fd;
	memset(&fd, 0, sizeof(struct pollfd));
	fd.fd = sock;
	fd.events = POLLIN;
	info->fds[info->fds_size] = fd; // Ajoute le socket de multidiffusion aux descripteurs surveilles
	info->fds_size++;
	return (1);
}

// Remplace les cases de la carte avec les nouvelles donnees reçues du serveur
void	replace_case_in_map(char *buf, client_info *info, int sock)
{
	int num_tmp = 0;
	memmove(&num_tmp, &buf[2], sizeof(uint16_t));
	num_msg = ntohs(num_tmp);
	int nb = 0;
	memmove(&nb, &buf[4], sizeof(uint8_t));
	char case_buf[3] = {0};
	for (int i = 0; i < nb; i++)
	{
		if (read(sock, case_buf, 3) < 0) // Lit les donnees des cases
			return ;
		set_grid(info->gui->game->map->map, case_buf[0], case_buf[1], case_buf[2], info->gui->game->map->w);
		memset(case_buf, 0, 3);
	}
}

// Gere les messages reçus du serveur pendant la partie
void game_part(int sock, client_info *info) {
	char buf[BUF_SIZE] = {0};
	int r = recv(sock, buf, BUF_SIZE, 0);
	if (r < 0)
		return (perror("recv in game_part"));
	int codereq = extract_code_req(buf);
	uint8_t height = 0;
	uint8_t widht = 0;
	switch (codereq) {
	case 11: // Message contenant la carte initiale
		memmove(&height, &buf[4], sizeof(uint8_t));
		memmove(&widht, &buf[5], sizeof(uint8_t));
		if (!info->gui) {
			info->gui = init_main_gui(&buf[6], info->id + '0', widht, height);
			return ;
		}
		memmove(&num_msg, &buf[2], sizeof(uint16_t));
		memmove(info->gui->game->map->map, &buf[6], height * widht);
		break;
	case 12 : // Message contenant les cases modifiees
		replace_case_in_map(buf, info, sock);
		break;
	case 13 : // Message de tchat
		add_msg_to_tchat_panel(info->gui, &buf[3], extract_id(buf));
		break;
	default:
		fprintf(stderr, "Unknown message type received: %d\n", codereq);
		break;
	}
}

// Envoie un message de tchat au serveur
void	send_tchat_to_serv(client_info *info)
{
	char buf[BUF_SIZE] = {0};
	tchat_msg *tmp = create_tchat_msg(7, info->id, info->eq, info->gui->tchat->cursor, info->gui->tchat->data);

	convert_tchat_msg(buf, tmp);
	if (send(info->sock_tcp, buf, BUF_SIZE, 0) < 0)
		return (perror("send codereq 7"), free(tmp->entete), free(tmp));
	free(tmp->entete);
	free(tmp);
	cleaning_tchat_msg(info->gui);
}

// Envoie une action (deplacement, depot de bombe) au serveur
void	send_action_to_serv(client_info *info, int action)
{
	char buf[BUF_SIZE] = {0};
	int codereq = (info->eq != 1) ? 6 : 5; // 6: team mode, 5: solo mode
	action_msg *tmp = create_action_msg(codereq, info->id, info->eq, num_msg, action);
	if (!tmp)
		return (perror("alloc in send_action_to_serv"));
	convert_action_msg(buf, tmp);
	int sock = socket(PF_INET6, SOCK_DGRAM, 0);
	struct sockaddr_in6 adr;
	memset(&adr, 0, sizeof(adr));
	adr.sin6_family = AF_INET6;
	adr.sin6_port = htons(info->port_udp);
	adr.sin6_addr = in6addr_any;
	socklen_t len = sizeof(adr);
	if (sendto(sock, buf, 4, 0, (struct sockaddr *)&adr, len) < 0)
		return (perror("send codereq 5"), free(tmp));
	free(tmp->entete);
	free(tmp);
}

void	quitting_part(client_info *info)
{
	char buf[BUF_SIZE] = {0};
    int r = recv(info->sock_tcp, buf, BUF_SIZE, 0);
    if (r < 0)
		return (perror("recv in game_part"));
	int codereq = extract_code_req(buf);
	switch (codereq) {
	case 15:
		dead = 2;
		break;
	case 17:
		dead = 1;
		break;
	default:
		fprintf(stderr, "Unknown message type received: %d\n", codereq);
		break;
	}
}

// Fonction principale du client
int main(void)
{
	int sock_client = init_client_socket();
	if (!sock_client)
		return (1);
	// 0 sans equipe, 1 avec equipe
	int mode_equipe = 0;
	int team_choice = -1;

	printf("Choose game mode: 0 for solo, 1 for team: ");
	scanf("%d", &mode_equipe);

	if (mode_equipe == 1)
	{
		printf("Choose your team: 0 or 1: ");
		scanf("%d", &team_choice);
	}

	client_info *info_client = join_game(sock_client, mode_equipe, team_choice); // Rejoint une partie en mode solo ou en equipe
	if (!info_client)
		return (close(sock_client), 1);
	if (!subscribe_mdiff(info_client))
		return (close(sock_client), 1);
	ready_to_play(info_client, mode_equipe);
	while (!dead)
	{
		// Attend les evenements sur les sockets
		if (poll(&info_client->fds[0], info_client->fds_size, 0) < 0)
			return (perror("poll in main client"), close(sock_client), 1);
		for (int i = 0; i < info_client->fds_size; i++)
		{
			if (info_client->fds[i].revents & POLLIN)
			{
				if (info_client->fds[i].fd == info_client->sock_mdiff)
					game_part(info_client->fds[i].fd, info_client); // Gere les messages reçus pendant la partie
				if (info_client->fds[i].fd == info_client->sock_tcp)
					quitting_part(info_client);
			}
		}
		if (info_client->gui)
		{
			int ret = update_gui(info_client->gui); // Met a jour l'interface graphique
			switch (ret)
			{
			case MSG_TO_SEND:
				send_tchat_to_serv(info_client);
				break;
			case MOVE_NORTH: case MOVE_EAST: case MOVE_SOUTH : case MOVE_WEST :
				send_action_to_serv(info_client, ret - 3);
				break;
			case DROP_BOMBE :
				send_action_to_serv(info_client, 4);
				break;
			case 0 :
				// Libere les ressources et quitte
				free_all(info_client->gui);
				free(info_client->adrmdiff_str);
				free(info_client);
				return (0);
			default:
				break;
			}
		}
	}
	// Libere les ressources et ferme le socket
	free(info_client->adrmdiff_str);
	free_all(info_client->gui);
	free(info_client);
	close(sock_client);	
	if (dead == 1)
		printf("you lose \n");
	else
		printf("you win\n");
	return (0);
}
