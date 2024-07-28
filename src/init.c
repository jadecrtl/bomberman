#include "serveur.h"
#include "client.h"

// =============== SERVER =================

// Initialise le socket TCP pour le serveur
int init_tcp_serv_sock()
{
	int sock_serv = socket(AF_INET6, SOCK_STREAM, 0);
	if (!sock_serv)
		return (perror("sock_serv"), 0);

	struct sockaddr_in6 adr;
	memset(&adr, 0, sizeof(adr));
	adr.sin6_family = AF_INET6;
	adr.sin6_port = htons(PORT_TCP);
	adr.sin6_addr = in6addr_any;

	if (fcntl(sock_serv, F_SETFL, O_NONBLOCK) < 0)
		return (perror("fcntl in init_tcp_serv_sock"), 0);
	if (setsockopt(sock_serv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		return (perror("setsockopt init_tcp_serv_sock"), 0);
	int r = bind(sock_serv, (struct sockaddr *)&adr, sizeof(adr));
	if (r < 0)
		return (perror("bind sock_serv"), 0);
	r = listen(sock_serv, 0);
	if (r < 0)
		return (perror("listen sock_serv"), 0);
	return (sock_serv);
}

// Initialise le socket UDP pour le serveur
int init_udp_serv_sock()
{
	int sock_serv = socket(AF_INET6, SOCK_DGRAM, 0);
	if (!sock_serv)
		return (perror("sock_serv"), 0);

	struct sockaddr_in6 adr;
	memset(&adr, 0, sizeof(adr));
	adr.sin6_family = AF_INET6;
	adr.sin6_port = htons(PORT_UDP);
	adr.sin6_addr = in6addr_any;

	if (fcntl(sock_serv, F_SETFL, O_NONBLOCK) < 0)
		return (perror("fcntl in init_udp_serv_sock"), 0);
	if (setsockopt(sock_serv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		return (perror("setsockopt init_udp_serv_sock"), 0);
	int r = bind(sock_serv, (struct sockaddr *)&adr, sizeof(adr));
	if (r < 0)
		return (perror("bind sock_serv"), 0);

	return (sock_serv);
}

// Initialise le socket multicast
int init_mdiff_socket(parti_info *info, int port_mdiff)
{
	info->port_mdiff = port_mdiff;
	int sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (!sock)
		return (perror("sock in init_mdiff_socket"), 0);

	info->adr_mdiff = create_ipv6_mdiff();
	if (!info->adr_mdiff)
		return (0);
	info->sock_mdiff = sock;
	return (1);
}

// Initialise les positions des joueurs
void init_pos_joueur(parti_info *info)
{
	memset(info->pos_joueur, 0, sizeof(pos) * 4);
	info->pos_joueur[0].c = 1;
	info->pos_joueur[0].l = 1;
	info->pos_joueur[1].c = info->maps->w - 2;
	info->pos_joueur[1].l = 1;
	info->pos_joueur[2].c = 1;
	info->pos_joueur[2].l = info->maps->h - 2;
	info->pos_joueur[3].c = info->maps->w - 2;
	info->pos_joueur[3].l = info->maps->h - 2;
}

// Initialise les informations de la carte dans les informations de la partie
int init_map_info_in_parti_info(parti_info *info)
{
	map_info *tmp = calloc(1, sizeof(map_info));
	if (!tmp)
		return (0);
	tmp->map = strdup("211111111111111111111111111112150000002000000000000000000061122200002000000000000022222221100000002000000000000000000001100000002000000000000000000001100000002222220000000000011001100000000200000000000000011001100000000200000000000000000001100000000020000000000222222221122000000020000000000020000001122000000000000000000020011001100000000000000000000020011001100220000002000000000020000001100220000002222200000002000001100000000110200000000000200001100000000110200000000000020001100000000000200000000000002001100000000000200000000000000201100000000000200000000000000021100000000000200000000000000001100000000222222000000002200001100000000200000000000002200001100000000200000000000000000001100000000200000000000000022001100022222200000000000000022001100020000000000002222222222221100020110000000000000000000001100020110002222200000000000001170020000020000000002000000081111111111111111111111111111111");
	if (!tmp->map)
		return (0);
	tmp->w = LARGEUR_GRILLE;
	tmp->h = HAUTEUR_GRILLE;
	info->maps = tmp;
	return (1);
}

// Initialise la fréquence des cases
void init_freq_case(parti_info *info)
{
	info->freq_case = allocate_data(100);
	if (!info->freq_case)
		return (perror("alloc error in init_freq_case"));
	info->case_changes = 0;
}

// Initialise une bombe
bomb *init_bomb(int x, int y)
{
	bomb *res = calloc(1, sizeof(bomb));
	if (!res)
		return (perror("alloc error in init_bomb"), NULL);
	res->x = x;
	res->y = y;
	res->next = NULL;
	return (res);
}


parti_info *init_parti_info(int sock_serv)
{
	parti_info *res = calloc(1, sizeof(parti_info));
	if (!res)
		return (perror("alloc"), NULL);
	res->bomb_clean = NULL;
	res->bomb_list = NULL;
	init_freq_case(res);

	memset(res->fds, 0, sizeof(struct pollfd) * 5);
	res->fds_size = 0;
	res->sock_tcp = sock_serv;
	if (!init_mdiff_socket(res, PORT_MDIFF))
		return (NULL);
	res->map = NULL;

	res->sock_udp = init_udp_serv_sock();
	struct pollfd tmp;
	memset(&tmp, 0, sizeof(struct pollfd));
	tmp.fd = sock_serv;
	tmp.events = POLLIN;
	res->fds[res->fds_size] = tmp;
	res->fds_size++;

	memset(&tmp, 0, sizeof(struct pollfd));
	tmp.fd = res->sock_udp;
	tmp.events = POLLIN;
	res->fds[res->fds_size] = tmp;
	res->fds_size++;
	return (res);
}

// ====================================================

// ==================== CLIENT ========================

// Initialise le socket pour le client
int init_client_socket()
{
	int sock_client = socket(AF_INET6, SOCK_STREAM, 0);
	if (!sock_client)
		return (perror("sock_client"), 0);

	struct sockaddr_in6 adr;
	memset(&adr, 0, sizeof(adr));
	adr.sin6_family = AF_INET6;
	adr.sin6_port = htons(PORT_TCP);
	adr.sin6_addr = in6addr_any;

	int r = connect(sock_client, (struct sockaddr *)&adr, sizeof(adr));
	if (r < 0)
		return (perror("connect in init_client_socket"), 0);
	return (sock_client);
}

// ====================================================