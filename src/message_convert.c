#include "message.h"

void	convert_entete_msg(char *buffer, entete_msg *msg)
{
	if (!buffer || !msg)
	{
		dprintf(2, "buffer or msg is NULL in convert_entete_msg\n");
		return ;
	}
	u_int16_t tmp = 0;
	tmp |= msg->codeReq;
	tmp |= msg->id << 13;
	tmp |= msg->eq << 15;
	tmp = htons(tmp);
	memmove(&buffer[0], &tmp, 2);
}

void	convert_action_msg(char *buffer, action_msg *msg)
{
	if (!buffer || !msg)
	{
		dprintf(2, "buffer or msg is NULL in convert_action_msg\n");
		return ;
	}
	convert_entete_msg(buffer, msg->entete);
	u_int16_t tmp = 0;
	tmp |= msg->num;
	tmp |= msg->action << 13;
	tmp = htons(tmp);
	memmove(&buffer[2], &tmp, 2);
}

void	convert_start_serve_msg(char *buffer, start_serve_msg *msg)
{
	if (!buffer || !msg)
	{
		dprintf(2, "buffer or msg is NULL in convert_start_serve_msg\n");
		return ;
	}
	convert_entete_msg(buffer, msg->entete);
	u_int16_t tmp = htons(msg->port);
	memmove(&buffer[2], &tmp, 2);
	tmp = htons(msg->portmdiff);
	memmove(&buffer[4], &tmp, 2);
	for (int i = 0; i < 16; i++)
	{
		tmp = htons(msg->adrmdiff[i]);
		memmove(&buffer[6 + (i * 2)], &tmp, 2);
	}
}

void	convert_tchat_msg(char *buffer, tchat_msg *msg)
{
	if (!buffer || !msg)
	{
		dprintf(2, "buffer or msg is NULL in convert_start_serve_msg\n");
		return ;
	}
	convert_entete_msg(buffer, msg->entete);
	memmove(&buffer[2], &msg->len, sizeof(uint8_t));
	memmove(&buffer[3], msg->data, msg->len);
}

void    convert_deroulement_partie(char *buffer, deroulement_serve_partie *msg) 
{
    if (!buffer || !msg)
    {
        dprintf(2, "buffer or msg is NULL in convert_start_serve_msg\n");
        return ;
    }
    convert_entete_msg(buffer, msg->entete);
    u_int16_t tmp = htons(msg->num);
    memmove(&buffer[2], &tmp, 2);
    memmove(&buffer[4], &msg->hauteur, sizeof(uint8_t));
    memmove(&buffer[5], &msg->largeur, sizeof(uint8_t));

    memmove(&buffer[6], msg->map, msg->hauteur * msg->largeur);
}

void    convert_freq_cases(char *buffer, freq_serve_cases *msg)
{
    if (!buffer || !msg)
    {
        dprintf(2, "buffer or msg is NULL in convert_start_serve_msg\n");
        return ;
    }
    convert_entete_msg(buffer, msg->entete);
    u_int16_t tmp = htons(msg->num);
    memmove(&buffer[2], &tmp, 2);
    memmove(&buffer[4], &msg->nb, sizeof(uint8_t));
}
