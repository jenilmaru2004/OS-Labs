#include <stdio.h>
#include <sys/types.h> // includes pid_t struct typedef
#include <sys/wait.h> // includes wait()
#include <unistd.h> // includes fork() and getpid()

int main() {

  //  (p1 -> p2)
  pid_t p2, p3, p4, p5;

  p2 = fork();
  printf("%d\n", getpid());

  // if process is a child process (pid_t of 0 indicates child process)
  if (p2 == 0) {
    //  (p2 -> p3)
    p3 = fork();
    if (p3 == 0) {
      printf("%d\n", getpid());
    } else {
      wait(NULL);
    }

    //  (p3 -> p4)  (p2 -> p5)
    p4 = fork();
    if (p4 == 0) {
      printf("%d\n", getpid());
    } else {
      wait(NULL);
    }

    // (p4 -> p6)  (p3 -> p7)  (p5 -> p8) (p2 -> p9) 
    p5 = fork();
    if (p5 == 0) {
      printf("%d\n", getpid()); 
    } else { 
      wait(NULL);
    }

  } else {
    wait(NULL);
  }
}
