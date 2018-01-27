/*Todas reciben el buffer desde donde deben leer o escribir*/

char * serialize_int(char * buffer, int value)
{
  buffer[0] = value >> 24;
  buffer[1] = value >> 16;
  buffer[2] = value >> 8;
  buffer[3] = value;
  return buffer + 4;
}

char * serialize_char(char * buffer, char value)
{
  buffer[0] = value;
  return buffer + 1;
}

char * deserialize_int(char * buffer, int * value)
{
  *value = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
  return buffer + 4;
}

char * deserialize_char(char * buffer, char * value)
{
  *value = buffer[0];
  return buffer + 1;
}
