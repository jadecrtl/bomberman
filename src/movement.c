#include "serveur.h"

// Récupère la position d'un joueur à partir de son id
pos *get_pos(parti_info *info, int id)
{
	return (&info->pos_joueur[id]);
}

// Vérifie si le mouvement est valide pour une position donnée
int is_move_valid_pos(pos *pos_j, map_info *map, int action)
{
	char c = -1;
	if (pos_j->l - 1 >= 0 && action == MOVE_NORTH)
		c = get_grid(map->map, pos_j->c, pos_j->l - 1, map->w);
	else if (pos_j->l + 1 < map->h && action == MOVE_SOUTH)
		c = get_grid(map->map, pos_j->c, pos_j->l + 1, map->w);
	else if (pos_j->c + 1 < map->w && action == MOVE_EAST)
		c = get_grid(map->map, pos_j->c + 1, pos_j->l, map->w);
	else if (pos_j->c - 1 >= 0 && action == MOVE_WEST)
		c = get_grid(map->map, pos_j->c - 1, pos_j->l, map->w);
	else if (action == DROP_BOMBE)
		return (1); // Placer une bombe est toujours valide
	return c != MUR_DESTRUCTIBLE && c != MUR_INDESTRUCTIBLE && c != BOMBE && c != JOUEUR1 && c != JOUEUR2 && c != JOUEUR3 && c != JOUEUR4 && c != -1;
}

// Effectue le mouvement ou l'action du joueur et met à jour la carte
void do_move_pos_with_action(parti_info *info, pos *pos_j, map_info *map, int action, int id)
{
	char c = get_grid(map->map, pos_j->c, pos_j->l, map->w);
	if (c != '3' && c != '4')
	{
		set_grid(map->map, pos_j->c, pos_j->l, '0', map->w); // Efface l'ancienne position du joueur
		add_to_freq_case(info, pos_j->c, pos_j->l, '0');
	}
	else
	{
		set_grid(map->map, pos_j->c, pos_j->l, c, map->w); // Laisse la bombe ou autre objet
		add_to_freq_case(info, pos_j->c, pos_j->l, c);
	}

	if (action == MOVE_NORTH)
	{
		set_grid(map->map, pos_j->c, pos_j->l - 1, id + '5', map->w);
		add_to_freq_case(info, pos_j->c, pos_j->l - 1, id + '5');
		pos_j->l += -1;
	}
	else if (action == MOVE_SOUTH)
	{
		set_grid(map->map, pos_j->c, pos_j->l + 1, id + '5', map->w);
		add_to_freq_case(info, pos_j->c, pos_j->l + 1, id + '5');
		pos_j->l += 1;
	}
	else if (action == MOVE_EAST)
	{
		set_grid(map->map, pos_j->c + 1, pos_j->l, id + '5', map->w);
		add_to_freq_case(info, pos_j->c + 1, pos_j->l, id + '5');
		pos_j->c += 1;
	}
	else if (action == MOVE_WEST)
	{
		set_grid(map->map, pos_j->c - 1, pos_j->l, id + '5', map->w);
		add_to_freq_case(info, pos_j->c - 1, pos_j->l, id + '5');
		pos_j->c += -1;
	}
	else if (action == DROP_BOMBE)
	{
		set_grid(map->map, pos_j->c, pos_j->l, '3', map->w);
		add_to_freq_case(info, pos_j->c, pos_j->l, '3');
		bomb *tmp = init_bomb(pos_j->c, pos_j->l); // Initialise une nouvelle bombe
		if (!tmp)
			return;
		add_bomb_to_list(info, tmp); // Ajoute la bombe à la liste des bombes
		tmp->time = get_actual_time(); // Enregistre le temps de placement de la bombe
	}
}
