#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> /*Contains the stat structure definition*/
#include <time.h>

struct stat fileAttributes;

int main(int argc, char* argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
    return 1;
  }

  stat(argv[1], &fileAttributes);

  printf("\n\nFile attributes for the File: %s\n", argv[1]);
  printf("-------------------------------------------------\n");
  printf("Inode: %ld\n", (long) fileAttributes.st_ino);
  printf("Size (in bytes): %ld\n", (long) fileAttributes.st_size);
  printf("Blocks: %ld\n", (long) fileAttributes.st_blocks);
  printf("File Permissions: ");
  printf((fileAttributes.st_mode & S_IFDIR) ? "d" : "-");
  printf((fileAttributes.st_mode & S_IRUSR) ? "r" : "-");
  printf((fileAttributes.st_mode & S_IWUSR) ? "w" : "-");
  printf((fileAttributes.st_mode & S_IXUSR) ? "x" : "-");
  printf((fileAttributes.st_mode & S_IRGRP) ? "r" : "-");
  printf((fileAttributes.st_mode & S_IWGRP) ? "w" : "-");
  printf((fileAttributes.st_mode & S_IXGRP) ? "x" : "-");
  printf((fileAttributes.st_mode & S_IROTH) ? "r" : "-");
  printf((fileAttributes.st_mode & S_IWOTH) ? "w" : "-");
  printf((fileAttributes.st_mode & S_IXOTH) ? "x" : "-");
  printf("\n");
  printf("Uid: %d\n", fileAttributes.st_uid);
  printf("Time of last access: %s", ctime(&fileAttributes.st_atime));
  printf("Time of last data modification: %s", ctime(&fileAttributes.st_mtime));
  printf("Last Status Change time: %s\n", ctime(&fileAttributes.st_ctime));
  return 0;
}
