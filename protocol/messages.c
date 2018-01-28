#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "messages.h"
#include "protocol_constants.h"
#include "serializer.h"

int send_msg(int socket, msg_t msg)
{
  int bytes;
  char * buffer = (char *)malloc(msg.bytes + sizeof(int)*2);
  char * ptr;

  ptr = serialize_int(buffer, msg.type);
  ptr = serialize_int(ptr, msg.bytes);
  memcpy(ptr, msg.buffer, msg.bytes);
  bytes = write(socket, &buffer, ptr-buffer);
  free((void *)buffer);

  return bytes;
}

int receive_msg(int socket, msg_t * msg)
{
  int bytes;
  char buffer[MAX_BUF_SIZE];
  char * ptr;

  bytes = read(socket, buffer, MAX_BUF_SIZE);
  ptr = deserialize_int(buffer, &msg->type);
  ptr = deserialize_int(ptr, &msg->bytes);
  msg->buffer = malloc(msg->bytes);
  memcpy(msg->buffer, ptr, msg->bytes);

  return bytes;
}
