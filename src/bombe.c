#include "serveur.h"

// Ajoute une case modifiee a la liste des cases a envoyer frequemment
void	add_to_freq_case(parti_info *info, int x, int y, char c)
{
	info->freq_case[info->case_changes][0] = x;
	info->freq_case[info->case_changes][1] = y;
	info->freq_case[info->case_changes][2] = c;
	info->case_changes++;
}

// Ajoute une bombe a la liste des bombes
void	add_bomb_to_list(parti_info *info, bomb *bomb)
{
	if (!info->bomb_list)
	{
		info->bomb_list = calloc(1, sizeof(bomb_list));
		if (!info->bomb_list)	
			return (perror("alloc in add_bomb_to_list"));
		info->bomb_list->head = bomb;
	}
	else if (!info->bomb_list->head)
	{ // Si la liste est vide
		info->bomb_list->head = bomb; // Ajoute la bombe en tete de liste
	}
	else
	{ // Sinon, ajoute la bombe a la fin de la liste
		struct bomb *a = info->bomb_list->head; 
		while (a->next)
			a = a->next;
		a->next = bomb;
	}
}

void	add_bombe_to_clean_list(parti_info *info, bomb *bomb)
{
	bomb->next = NULL;
	if (!info->bomb_clean)
	{
		info->bomb_clean = calloc(1, sizeof(bomb_list));
		if (!info->bomb_clean)	
			return (perror("alloc in add_bombe_to_clean_list"));
		info->bomb_clean->head = bomb;
	}
	else if (!info->bomb_clean->head)
	{ // Si la liste est vide
		info->bomb_clean->head = bomb; // Ajoute la bombe en tete de liste
	}
	else
	{ // Sinon, ajoute la bombe a la fin de la liste
		struct bomb *a = info->bomb_clean->head; 
		while (a->next)
			a = a->next;
		a->next = bomb;
	}
	bomb->time = get_actual_time();
}

void	check_player_died(parti_info *info)
{
	char *map = info->maps->map;
	int map_w = info->maps->w;
	int	map_h = info->maps->h;
	for (int i = 0; i < info->nbr_joueur; i++)
	{
		if (info->sock_joueur[i] == -1) // joueur mort;
			continue;
		pos	actual = info->pos_joueur[i];
		int x = actual.c;
		int y = actual.l;
		if ((x - 1 >= 0 && get_grid(map, x - 1, y, map_w) == EXPLOSION)
			|| (y - 1 >= 0 && get_grid(map, x, y - 1, map_w) == EXPLOSION)
			|| (x + 1 < map_w && get_grid(map, x + 1, y, map_w) == EXPLOSION)
			|| (y + 1 < map_h && get_grid(map, x, y + 1, map_w) == EXPLOSION)
			|| (get_grid(map, x, y, map_w) == EXPLOSION))
			{
				set_grid(map, x, y, '0', map_w);
				add_to_freq_case(info, x, y, '0');
				entete_msg *tmp = create_entete_msg(17, 4, 4);
				char buf[BUF_SIZE] = {0};
				convert_entete_msg(buf, tmp);
				if (send(info->sock_joueur[i], buf, 2, 0) < 0)
					return (perror("send in check_player_die"), free(tmp));
				free(tmp);
				close(info->sock_joueur[i]);
				info->sock_joueur[i] = -1;
			}
	}
}

// fait explosez la bombe + modification de la map + ajoutez dans freq_Case
void	explode_or_clean_bomb(parti_info *info, bomb *to_explode, char c)
{
	int		x = to_explode->x;
	int		y = to_explode->y;
	char	*map = info->maps->map;
	int		map_w = info->maps->w;

	add_to_freq_case(info, x, y, c); // Marque la case de la bombe comme explosee
	set_grid(map, x, y, c, map_w); // Met a jour la carte
	if (x - 1 >= 0 && get_grid(map, x - 1, y, map_w) != MUR_INDESTRUCTIBLE)
	{
		set_grid(map, x - 1, y, c, map_w);
		add_to_freq_case(info, x - 1, y, c);
	}
	if (y - 1 >= 0 && get_grid(map, x, y - 1, map_w) != MUR_INDESTRUCTIBLE)
	{
		set_grid(map, x, y - 1, c, map_w);
		add_to_freq_case(info, x, y - 1, c);
	}
	if (x + 1 < map_w && get_grid(map, x + 1, y, map_w) != MUR_INDESTRUCTIBLE)
	{
		set_grid(map, x + 1, y, c, map_w);
		add_to_freq_case(info, x + 1, y, c);
	}
	if (y + 1 < info->maps->h && get_grid(map, x, y + 1, map_w) != MUR_INDESTRUCTIBLE)
	{
		set_grid(map, x, y + 1, c, map_w);
		add_to_freq_case(info, x, y + 1, c);
	}
	if (c == '4')
		check_player_died(info);
}

// Supprime une bombe de la liste des bombes
void	delete_bomb_from_bomb_list(bomb_list *bomb_list, bomb *to_destroy)
{
	bomb *tmp = bomb_list->head;

	if (tmp == to_destroy)
	{
		bomb_list->head = to_destroy->next;
		return ;
	}

	while (tmp->next)
	{
		if (tmp->next == to_destroy)
		{
			tmp->next = to_destroy->next;
			return;
		}
		tmp = tmp->next;
	}
}

// Verifie si des bombes doivent exploser et les fait exploser si necessaire
void	check_bomb(parti_info *info)
{
	if (!info->bomb_list)
		return;
	bomb *tmp = info->bomb_list->head;
	long actual_time;
	while (tmp)
	{
		actual_time = get_actual_time();
		if (actual_time - tmp->time >= TIME_TO_EXPLODE * 1000000)
		{
			explode_or_clean_bomb(info, tmp, '4');
			delete_bomb_from_bomb_list(info->bomb_list, tmp);
			add_bombe_to_clean_list(info, tmp);
			tmp = info->bomb_list->head;
			continue;
		}
		tmp = tmp->next;
	}

	if (!info->bomb_clean)
		return ;
	tmp = info->bomb_clean->head;
	while (tmp)
	{
		actual_time = get_actual_time();
		if (actual_time - tmp->time >= 1000000)
		{
			explode_or_clean_bomb(info, tmp, '0');
			delete_bomb_from_bomb_list(info->bomb_clean, tmp);
			free(tmp);
			tmp = info->bomb_clean->head;
			continue;
		}
		tmp = tmp->next;
	}
}
