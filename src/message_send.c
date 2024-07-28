#include "serveur.h"

int	send_start_serve_msg(parti_info *info, int sock_client, int codereq, int team_choice)
{
	char adrmdiff[16] = {0};
	convert_ipv6_mdiff_to_buf(adrmdiff, info->adr_mdiff);
	int team = (codereq == 2) ? team_choice : 0;
	start_serve_msg *msg = create_start_serve_msg(codereq == 2 ? 10 : 9, info->nbr_joueur, team, PORT_UDP, PORT_MDIFF, adrmdiff);
	info->nbr_joueur++;
	if (!msg)
		return (1);
	char buffer[BUF_SIZE] = {0};
	convert_start_serve_msg(buffer, msg);
	int r = 0;
	if ((r = send(sock_client, buffer, BUF_SIZE, 0)) < 0)
		return (perror("send send_start_serve_msg"), free(msg), 1);
	free(msg->entete);
	free(msg);
	return (1);
}

void send_deroulement_partie(parti_info *info, char *map)
{
	if (!map)
		return;

	char buffer[BUF_SIZE] = {0};
	deroulement_serve_partie *msg = create_deroulement_partie(11, 0, 0, 0, HAUTEUR_GRILLE, LARGEUR_GRILLE, map);
	if (!msg)
		return (perror("Failed to create deroulement_partie message"));

	convert_deroulement_partie(buffer, msg);
	free(msg->entete);
	free(msg->map);
	free(msg);

	struct sockaddr_in6 dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, info->adr_mdiff, &dest_addr.sin6_addr);
	dest_addr.sin6_port = htons(info->port_mdiff);
	int ifindex = if_nametoindex(SCOPE_MULTICAST);
	dest_addr.sin6_scope_id = ifindex;

	if (sendto(info->sock_mdiff, buffer, BUF_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
	{
		perror("sendto failed in send_deroulement_partie");
	}
}

void send_freq_cases(parti_info *info)
{
	freq_serve_cases *msg = create_freq_cases(12, 0, 0, 0, info->case_changes, info->freq_case);
	if (!msg)
	{
		perror("Failed to create freq_cases message");
		for (int i = 0; i < info->case_changes; i++)
			free(info->freq_case[i]);
		return (free(info->freq_case));
	}

	char buffer[BUF_SIZE] = {0};
	convert_freq_cases(buffer, msg);

	struct sockaddr_in6 dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, info->adr_mdiff, &dest_addr.sin6_addr);
	dest_addr.sin6_port = htons(info->port_mdiff);
	int ifindex = if_nametoindex(SCOPE_MULTICAST);
	dest_addr.sin6_scope_id = ifindex;

	if (sendto(info->sock_mdiff, buffer, 5, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
		return (perror("sendto failed in send_freq_cases"));
	for (int i = 0; i < info->case_changes; i++)
	{
		if (sendto(info->sock_mdiff, info->freq_case[i], 3, 0, 
			(struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
			return (perror("sendto failed in send_freq_cases"));
	}

	free(msg->entete);
	free(msg);
	for (int i = 0; i < info->case_changes; i++)
		memset(info->freq_case[i], 0, 3);
	info->case_changes = 0;
}

int send_end_game(parti_info *info)
{
	int codereq = 15;
	if (info->mode_equipe)
		codereq = 16;
	int id = 0; // celui du gagnant;
	int eq = 0; // celui de l'equipe gagnante;
	entete_msg *msg = create_entete_msg(codereq, id, eq);
	if (!msg)
		return (0);
	char buffer[BUF_SIZE] = {0};
	convert_entete_msg(buffer, msg);
	for (int i = 0; i < 4; i++)
	{
		if (send(info->sock_joueur[i], buffer, BUF_SIZE, 0) < 0)
			perror("send error"); // potentiellement a modifiez si les gens quitte la parti et ferme les sockets;
	}
	return (1);
}

int send_tchat_msg(parti_info *info, char *buffer)
{
	uint16_t tmp = 0;
	memmove(&tmp, &buffer[0], 2);
	tmp = ntohs(tmp);
	int id = tmp >> 13 & sum_of_bit(2);
	int eq = tmp >> 15 & 1;
	uint8_t len = 0;
	memmove(&len, &buffer[2], sizeof(uint8_t));
	tchat_msg *msg = create_tchat_msg(13, id, eq, len, &buffer[3]);
	if (!msg)
		return (0);
	char buf_send[BUF_SIZE] = {0};
	convert_tchat_msg(buf_send, msg);
	free(msg->data);
	free(msg->entete);
	free(msg);

	int sock = socket(PF_INET6, SOCK_DGRAM, 0);
	if (!sock)
		return (1);
	struct sockaddr_in6 gradr;
	memset(&gradr, 0, sizeof(gradr));
	gradr.sin6_family = AF_INET6;
	if (!inet_pton(AF_INET6, info->adr_mdiff, &gradr.sin6_addr))
		return (perror("inet_pton"), 0);
	gradr.sin6_port = htons(info->port_mdiff);
	int ifindex = if_nametoindex(SCOPE_MULTICAST);
	gradr.sin6_scope_id = ifindex;

	if (sendto(sock, buf_send, len + 3, 0, (struct sockaddr *)&gradr, (socklen_t)sizeof(gradr)) < 0)
		return (perror("sendto in send_tchat_msg"), 0);
	return (1);
}
