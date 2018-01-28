#ifndef MESSAGES_H
#define MESSAGES_H

#include "types.h"

int send_msg(int socket, msg_t msg);
int receive_msg(int socket, msg_t * msg);

#endif
