#include "serveur.h"
#include "client.h"

void free_server(parti_info *info)
{
	free(info->adr_mdiff);
	for (int i = 0; i < 100; i++)
		free(info->freq_case[i]);
	free(info->freq_case);
	if (info->maps)
	{
		free(info->maps->map);
		free(info->maps);
	}
	bomb *tmp;
	if (info->bomb_list)
	{
		tmp = info->bomb_list->head;
		while (tmp)
		{
			bomb *next = tmp->next;
			free(tmp);
			tmp = next;
		}
		free(info->bomb_list);
	}
	if (info->bomb_clean)
	{
		tmp = info->bomb_clean->head;
		while (tmp)
		{
			bomb *next = tmp->next;
			free(tmp);
			tmp = next;
		}
		free(info->bomb_clean);
	}
	free(info);
}
