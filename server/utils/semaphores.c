#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "utils.h"

static sem_t * child_sem;
static sem_t * database_sem;

int init_semaphores()
{
  child_sem = sem_open("flights_child_sem", O_CREAT | O_EXCL, 0644, MAX_CHILD_PROCESSES);

  database_sem = sem_open("flights_database_sem", O_CREAT | O_EXCL, 0644, 1);

  return (child_sem == SEM_FAILED || database_sem == SEM_FAILED) * -1;
}

int wait_semaphore(int sem)
{
  int code = -1;

  switch (sem) {
    case CHILD_SEM:
      code = sem_wait(child_sem);
      break;
    case DATABASE_SEM:
      code = sem_wait(database_sem);
      break;
  }
  return code;
}

int post_semaphore(int sem)
{
  int code = -1;

  switch (sem) {
    case CHILD_SEM:
      code = sem_post(child_sem);
      break;
    case DATABASE_SEM:
      code = sem_post(database_sem);
      break;
  }
  return code;
}

int close_semaphores()
{
  int code = 0;

  code += sem_unlink("flights_child_sem");
  code += sem_unlink("flights_database_sem");
  code += sem_close(child_sem);
  code += sem_close(database_sem);

  return code;
}
