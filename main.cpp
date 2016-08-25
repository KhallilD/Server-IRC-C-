#include "Server.h"

int main(int argc, char ** argv) {
  if (argc < 2) {
    fprintf(stderr, "%s", "server + port\n");
    exit(-1);
  }

  t_env e;
  int	i;
  memset(e.fd_type, FD_FREE, 255);
  for (i = 0; i < 255; i++)
    {
      e.fct_read[i] = NULL;
    }
  e.port= atoi(argv[1]);

  Server Server;

  Server.run_server(&e);
}
