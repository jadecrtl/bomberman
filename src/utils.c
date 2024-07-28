#include "utils.h"

// Calcule la somme des bits sur un certain nombre d'itérations
int sum_of_bit(int time)
{
	int res = 0;
	for (int i = 0, tmp = 1; i < time; i++, tmp *= 2)
		res += tmp;
	return res;
}

// Alloue de la mémoire pour un tableau de chaînes de caractères
char **allocate_data(int nb)
{
	char **data = calloc(nb, sizeof(char *));
	if (!data)
		return (perror("Alloc failed in allocate_data"), NULL);
	for (int i = 0; i < nb; i++)
	{
		data[i] = calloc(3, sizeof(char));
		if (!data[i])
		{
			for (int j = i; j >= 0; j--)
				free(data[j]);
			return (free(data), perror("Alloc failed in allocate_data"), NULL);
		}
	}
	return data;
}

// Ajoute des données dans une ligne spécifique du tableau
void add_in_data(int line, int col, char c, int pos, char *data[3])
{
	data[pos][0] = line;
	data[pos][1] = col;
	data[pos][2] = c;
}

// Crée une adresse IPv6 multicast aléatoire
char *create_ipv6_mdiff(void)
{
	char *adr = calloc(40, sizeof(char));
	const char *base_hexa = "0123456789abcdef";
	if (!adr)
		return (perror("Alloc in create_ipv6_mdiff"), NULL);
	memmove(&adr[0], "ff12:", 5);
	
	srand(time(NULL));
	for (int i = 5; i < 39; i++)
	{
		if (i % 5 == 4)
		{
			adr[i] = ':';
			continue;
		}
		adr[i] = base_hexa[rand() % 16];
	}
	return (adr);
}

// Convertit un caractère hexadécimal en entier
int hexa_to_int(char c)
{
	const char *base_hexa = "0123456789abcdef";
	for (int i = 0; i < 16; i++)
	{
		if (c == base_hexa[i])
			return i;
	}
	return (-1);
}

// Convertit une adresse IPv6 multicast en un buffer binaire
void convert_ipv6_mdiff_to_buf(char *buffer, char *ipv6adr)
{
	uint8_t tmp = 0;
	int i = 0;
	int j = 0;
	while (ipv6adr[i])
	{
		tmp = 0;
		if (ipv6adr[i] == ':')
		{
			i++;
			continue;
		}
		tmp = hexa_to_int(ipv6adr[i]) << 4;
		i++;
		tmp |= hexa_to_int(ipv6adr[i]);
		i++;
		buffer[j] = tmp;
		j++;
	}
}

// Crée une adresse IPv6 multicast pour un client à partir d'une adresse donnée
char *create_ipv6_mdiff_client(char adrmdiff[16])
{
	const char *base_hexa = "0123456789abcdef";
	char *res = calloc(40, sizeof(char));
	if (!res)
		return (perror("Alloc error create_ipv6_mdiff_client"), NULL);
	int check_sum = sum_of_bit(4);
	int j = 0;
	for (int i = 0; i < 16; i++)
	{
		uint8_t _tmp = 0;
		memmove(&_tmp, &adrmdiff[i], sizeof(uint8_t));
		int tmp = _tmp & check_sum;
		int tmp2 = _tmp >> 4;
		res[j] = base_hexa[tmp2];
		j++;
		res[j] = base_hexa[tmp];
		j++;
		if (j % 5 == 4 && j != 39)
		{
			res[j] = ':';
			j++;
		}
	}
	return (res);
}

// Extrait le code de la requête depuis un buffer
int extract_code_req(char *buf)
{
	uint16_t entete;
	memmove(&entete, &buf[0], 2);
	entete = ntohs(entete);
	return (entete & sum_of_bit(12));
}

// Extrait un bit de "qualité" depuis un buffer
int extract_eq(char *buf)
{
	uint16_t entete;
	memmove(&entete, &buf[0], 2);
	entete = ntohs(entete);
	return (entete >> 15 & 1);
}

// Extrait l'identifiant depuis un buffer
int extract_id(char *buf)
{
	uint16_t entete;
	memmove(&entete, &buf[0], 2);
	entete = ntohs(entete);
	return (entete >> 13 & sum_of_bit(2));
}

// Extrait l'action depuis un buffer
int extract_action(char *buf)
{
	u_int16_t tmp;
	memmove(&tmp, &buf[2], 2);
	tmp = ntohs(tmp);
	return (tmp >> 13);
}

// Récupère la valeur d'une cellule dans une grille
int get_grid(char *map, int x, int y, int map_w)
{
	return map[y * map_w + x];
}

// Modifie la valeur d'une cellule dans une grille
void set_grid(char *map, int x, int y, char c, int map_w)
{
	map[y * map_w + x] = c;
}

long	get_actual_time()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}
