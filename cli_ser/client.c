/**
 * Simple File Transfer Client For 2016 Introduction to Computer Network.
 * Author: vicky-sunshine @ HSNL-TAs
 * 2016/10
 * **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  /* contains a number of basic derived types */
#include <sys/socket.h> /* provide functions and data structures of socket */
#include <arpa/inet.h>  /* convert internet address and dotted-decimal notation */
#include <netinet/in.h> /* provide constants and structures needed for internet domain addresses*/
#include <unistd.h>     /* `read()` and `write()` functions */
#include <dirent.h>     /* format of directory entries */
#include <sys/stat.h>   /* stat information about files attributes */

#define MAX_SIZE 2048

void connection_handler (int sockfd);
void file_download_handler(int sockfd, char filename[]);

int main (int argc, char *argv[]) {
  int cli_fd;                   // descriptor of client, used by `socket()`
  struct sockaddr_in svr_addr;  // address of server, used by `connect()`

  /* create the socket, use `socket()` */
  cli_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (cli_fd < 0) {
    perror("Create socket failed.");
    exit(1);
  }

  int cli_port = atoi(argv[2]);
  /**
    TODO 1:
    preparing sockaddr_in
  **/
  bzero(&svr_addr, sizeof(svr_addr));
  svr_addr.sin_family = AF_INET/* protocol stack */;
  svr_addr.sin_addr.s_addr = inet_addr(argv[1]);
  svr_addr.sin_port = htons(cli_port)/* bind port */;
  if (inet_pton(AF_INET, argv[1], &svr_addr.sin_addr) <= 0) {
     perror("Address converting fail with wrong address argument");
     return 0;
  }

  /****/

  /* connect to server with prepared sockaddr_in structure */
  if (connect(cli_fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) {
    perror("Connect failed");
    exit(1);
  }
  connection_handler(cli_fd);

  close(cli_fd);
  return 0;
}

void connection_handler (int sockfd) {
    char filename[MAX_SIZE];  // file want to download
    char path[MAX_SIZE];      // file path
    char buf[MAX_SIZE];       // store data from server

    puts("f");
    /* create download dir */
    sprintf(path, "./download");
    mkdir(path, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    printf("D");

    /* read hello msg from server */
    memset(buf, '\0', MAX_SIZE);
    read(sockfd, buf, MAX_SIZE);
    printf("%s", buf);

//todo2
    char input[MAX_SIZE];
    read(sockfd,input,MAX_SIZE);
    int count = atoi(input);
    while(count){
      count--;
      memset(input,'\0',MAX_SIZE);
      read(sockfd,input,MAX_SIZE);
      printf("%s\n",input);
    }
    /*
      TODO 2:
      read file list from server
      //server client 間如何達成協議，彼此知道要write/read幾次為關鍵！
    **/

    /****/

    printf("-----------\nEnter the filename: ");
    while (scanf(" %s", filename) > 0) {
        if (strcmp(filename, ".exit") == 0) {
            break;
        }
//todo3
        write(sockfd,filename,MAX_SIZE);
        /** TODO 3:
            send requested file name to server
        **/

        /****/

        /* download this file */
        file_download_handler(sockfd, filename);

        /* send next file request*/
        printf("-----------\nEnter the filename: ");
    }
    printf("[x] Socket closed\n");
}

void file_download_handler(int sockfd, char filename[]) {
  char buf[MAX_SIZE];   // store data from server
  char path[MAX_SIZE];  // file path

  int file_size = 0;    //  size of this file
  int read_byte = 0;    //  bytes this time recv
  int read_sum = 0;     //  bytes have been recv
  FILE *fp;

  /* receive start message from server */
  memset(buf, '\0', MAX_SIZE);
  read(sockfd, buf, MAX_SIZE);
  printf("%s", buf);

  /* receive file size */
  memset(buf, '\0', MAX_SIZE);
  read(sockfd, buf, MAX_SIZE);
  file_size = atoi(buf);

  /* file path */
  
  memset(path, '\0', MAX_SIZE);
  sprintf(path, "./download/%s", filename);

  read_sum = 0;
  fp = fopen(path, "wb");
  if (fp) {
      while (read_sum < file_size) {
        memset(buf, '\0', MAX_SIZE);
//todo4
        read_byte=read(sockfd,buf,MAX_SIZE);
        /**
          TODO 4:
          receive file data from server
        **/

        /****/

        /* write file to local disk*/
        fwrite(&buf, sizeof(char), read_byte, fp);
        read_sum += read_byte;
      }
      fclose(fp);
      /* receive download complete message */
      memset(buf, '\0', MAX_SIZE);
      read(sockfd, buf, MAX_SIZE);
      printf("%s", buf);

  } else {
    perror("Allocate memory fail\n");
    return;
  }
}
