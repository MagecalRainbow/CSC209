#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];
  int status;

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  // TODO

  int fds[2];

  if ((pipe(fds)) == -1) {
      perror("pipe");
      exit(1);
  }

  int r = fork();
  if (r < 0) {
      perror("fork");
      exit(1);
  } else if (r == 0) {
      dup2(fds[0], STDIN_FILENO);
      if (close(fds[0]) == -1) {
          perror("close reading and from inside child");
          exit(1);
      }
      if (close(fds[1]) == -1) {
          perror("close pipe after writing");
          exit(1);
      }
      execl("./validate", "validate", NULL);
      perror("execl");
      exit(1);
  } else {
      if (close(fds[0]) == -1) {
          perror("close reading of pipe in parent");
          exit(1);
      }
      if ((write(fds[1], user_id, MAX_PASSWORD)) == -1) {
          perror("write");
          exit(1);
      }
      if (write(fds[1], password, MAX_PASSWORD) == -1) {
          perror("write");
          exit(1);
      }
      if (close(fds[1]) == -1) {
          perror("close writing end of pipe in parent");
          exit(1);
      }
      if (wait(&status) == -1) {
          perror("wait");
          exit(1);
      }
      if (WIFEXITED(status)) {
          if (WEXITSTATUS(status) == 0) {
              printf(SUCCESS);
          } else if (WEXITSTATUS(status) == 2) {
              printf(INVALID);
          } else if (WEXITSTATUS(status) == 3) {
              printf(NO_USER);
          }
      }
  }

  return 0;
}
