#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include "../log/log.h"
#include "../constants.h"
#include "utils.h"

int init_socket()
{
  int listener_socket;

  if((listener_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    print_error_msg("Al crear el socket");
    return ERROR;
  }

  print_ok_msg("listener_socket creado");

  if(bind_to_port(listener_socket, SERVER_PORT) < 0) {
    return ERROR;
  }

  if(listen(listener_socket, LISTEN_QUEUE_SIZE) < 0) {
    print_error_msg("En el listen");
    return ERROR;
  }

  return listener_socket;
}

int bind_to_port(int listener_socket, int port)
{
  char * reuse;
  struct sockaddr_in address;

  bzero(&address, sizeof(address));

  address.sin_family = PF_INET;
  address.sin_port = htons(port);

  if(INADDR_ANY)
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  /*evitar problemas de reutilizacion del puerto*/
  reuse = (char *) 1;
  setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, (char *) reuse, sizeof(int));

  if(bind(listener_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
    print_error_msg("En el bind");
    return ERROR;
  }
  print_ok_msg("bind");
  return 0;
}

int accept_client(int listener_socket)
{
  int accepted_socket;
  socklen_t address_size;
  struct sockaddr_storage client;

  address_size = sizeof(client);
  accepted_socket = accept(listener_socket, (struct sockaddr *) &client, &address_size);
  print_ok_msg("Nuevo cliente");
  if(accepted_socket < 0) {
    print_error_msg("Al aceptar");
    return ERROR;
  }
  return accepted_socket;
}

void sigchld_handler(int sig)
{
  while (waitpid(-1, 0, WNOHANG) > 0)
  ;
  return;
}

void close_handler(int sig)
{
  if(terminate_semaphores() < 0){
    print_error_msg("aqui:Al cerrar los semáforos");
    exit(1);
  }
  printf("============ Finalizó el server. ============\n\n");
  exit(0);
}

void become_daemon()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  int log_file;

  switch (fork()) {
    case -1:
      print_error_msg("En el fork del listener");
      exit(1);
      break;
    default:
      exit(0);
      break;
    case 0:
      /* redirect stdout to log.txt */
      log_file = open("log.txt", O_WRONLY|O_CREAT, 0666);
      close(1);
      dup(log_file);
      lseek(log_file, 0, SEEK_END);
      colors_off();
      printf("==== Server started: %d-%02d-%02d %02d:%02d:%02d ====\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      break;
  }
}
