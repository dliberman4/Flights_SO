#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "messages.h"
#include "protocol_constants.h"
#include "serializer.h"
#include <stdio.h>

int send_msg(int socket, msg_t msg)
{
  int bytes;
  unsigned char * buffer = (unsigned char *)malloc(msg.bytes + sizeof(int)*2);
  unsigned char * ptr;

  ptr = serialize_int(buffer, msg.type);
  ptr = serialize_int(ptr, msg.bytes);
  memcpy(ptr, msg.buffer, msg.bytes);
  ptr += msg.bytes;
  printf("send_msg: type = %d; bytes = %d; buffer = %d\n", msg.type, msg.bytes, *((int *)msg.buffer));
  bytes = write(socket, buffer, ptr-buffer);
  free((void *)buffer);

  return bytes;
}

int receive_msg(int socket, msg_t * msg)
{
  int bytes;
  unsigned char buffer[MAX_BUF_SIZE];
  unsigned char * ptr;

  bytes = read(socket, buffer, MAX_BUF_SIZE);
  ptr = deserialize_int(buffer, &msg->type);
  printf("type = %d\n", msg->type);
  ptr = deserialize_int(ptr, &msg->bytes);
  msg->buffer = (unsigned char *)malloc(msg->bytes);
  memcpy(msg->buffer, ptr, msg->bytes);

  return bytes;
}
