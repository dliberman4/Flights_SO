#ifndef SETUP_H
#define SETUP_H

int init_socket();
int bind_to_port(int listener_socket, int port);
int accept_client(int listener_socket);
void sigchld_handler(int sig);
void become_daemon();

#endif
