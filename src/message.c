#include "message.h"

entete_msg	*create_entete_msg(int codereq, int id, int eq)
{
	entete_msg	*res = calloc(1, sizeof(entete_msg));
	if (!res)
		return (NULL);
	res->codeReq = codereq;
	res->id = id;
	res->eq = eq;
	return (res);
}

action_msg	*create_action_msg(int codereq, int id, int eq, int num, int action)
{
	entete_msg	*entete = create_entete_msg(codereq, id, eq);
	if (!entete)
		return(dprintf(2, "Alloc error in creation action msg\n"), NULL);
	action_msg	*res = calloc(1, sizeof(action_msg));
	if (!res)
		return (free(entete), dprintf(2, "Alloc error in creation action msg\n"), NULL);
	res->entete = entete;
	res->num = num;
	res->action = action;
	return (res);
}

start_serve_msg	*create_start_serve_msg(int codereq, int id, int eq, int port, int portmdiff, char adrmiff[16])
{
	entete_msg	*entete = create_entete_msg(codereq, id, eq);
	if (!entete)
		return (dprintf(2, "Alloc error in creation start serve msg\n"), NULL);
	start_serve_msg	*res = calloc(1, sizeof(start_serve_msg));
	if (!res)
		return(free(entete), dprintf(2, "Alloc error in creation start serve msg\n"), NULL);
	res->entete = entete;
	res->port = port;
	res->portmdiff = portmdiff;
	for (int i = 0; i < 16; i++)
		res->adrmdiff[i] = adrmiff[i];
	return (res);
}

tchat_msg	*create_tchat_msg(int codereq, int id, int eq, int len, char *data)
{
	entete_msg	*entete = create_entete_msg(codereq, id, eq);
	if (!entete)
		return (dprintf(2, "Alloc error in create_tchat_msg\n"), NULL);
	tchat_msg	*res = calloc(1, sizeof(tchat_msg));
	if (!res)
		return(free(entete), dprintf(2, "Alloc error in create_tchat_msg\n"), NULL);
	res->entete = entete;
	res->len = len;
	res->data = strdup(data);
	if (!res->data)
		return (free(entete), free(res), dprintf(2, "Alloc error in create_tchat_msg\n"), NULL);
	return (res);
}

deroulement_serve_partie *create_deroulement_partie(int codereq, int id, int eq, int num, int hauteur, int largeur, char *map)
{
    entete_msg *entete = create_entete_msg(codereq, id, eq);
    if (!entete)
        return (dprintf(2, "Alloc error in create_deroulement_partie\n"), NULL);
    deroulement_serve_partie *res = calloc(1, sizeof(deroulement_serve_partie));
    if (!res)
        return (free(entete), dprintf(2, "Alloc error in create_deroulement_partie\n"), NULL);
    res->entete = entete;
    res->num = num;
    res->hauteur = hauteur;
    res->largeur = largeur;
    res->map = strdup(map);
    if (!res->map)
        return (free(entete), free(res), dprintf(2, "Alloc error in create_deroulement_partie\n"), NULL);
    return (res);
}

freq_serve_cases *create_freq_cases(int codereq, int id, int eq, int num, int nb, char **data)
{
    entete_msg *entete = create_entete_msg(codereq, id, eq);
    if (!entete)
        return (dprintf(2, "Alloc error in create_freq_cases\n"), NULL);
    freq_serve_cases *res = calloc(1, sizeof(freq_serve_cases));
    if (!res)
        return (free(entete), dprintf(2, "Alloc error in create_freq_cases\n"), NULL);
    res->entete = entete;
    res->num = num;
    res->nb = nb;
    res->data = data;
    return (res);
}
