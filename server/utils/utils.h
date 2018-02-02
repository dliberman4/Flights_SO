#ifndef UTILS_H
#define UTILS_H

#define CHILD_SEM 0
#define DATABASE_SEM 1
#define MAX_CHILD_PROCESSES 2

int init_socket();
int bind_to_port(int listener_socket, int port);
int accept_client(int listener_socket);
void sigchld_handler(int sig);
void close_handler(int sig);
void become_daemon();

int init_semaphores();
int wait_semaphore(int sem);
int post_semaphore(int sem);
int close_semaphores();

#endif
