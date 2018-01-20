#ifndef DEAMONSERVERPRUEBA_H
#define DEAMONSERVERPRUEBA_H

#define ERROR_SERVER -1

int open_socket();
void bind_to_port(int listener_socket, int port);
void sigchld_handler(int sig);

#endif
