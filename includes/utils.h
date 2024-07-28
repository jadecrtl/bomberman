#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#define PORT_MDIFF 12012 // Port par defaut pour la multidiffusion

int		sum_of_bit(int time);
char	**allocate_data(int nb);
void	add_in_data(int line, int col, char c, int pos, char *data[3]);
char	*create_ipv6_mdiff(void);
void	convert_ipv6_mdiff_to_buf(char *buffer, char *ipv6adr);
char	*create_ipv6_mdiff_client(char adrmdiff[16]);
int		extract_code_req(char *buf);
int		extract_eq(char *buf);
int		extract_id(char *buf);
int		extract_action(char *buf);
int		get_grid(char *map, int x, int y, int map_w);
void	set_grid(char *map, int x, int y, char c, int map_w);
long	get_actual_time();

#endif
