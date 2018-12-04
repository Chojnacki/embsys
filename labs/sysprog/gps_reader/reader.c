#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <util.h>

//-----------------------------------------------------------------------------
int main(int argc, char *argv [])
{
    char * port1 = NULL;
    char * port2 = NULL;

    // parse comand line
    if (argc != 5)
    {
        fprintf(stderr, "Invalid usage: reader -p port1_name -q port2_name\n");
        exit(EXIT_FAILURE);
    }

    char * options = "p:q:";
    int option;
    while((option = getopt(argc, argv, options)) != -1)
    {
        switch(option)
        {
            case 'p':
                port1 = optarg;
                break;
                
            case 'q':
                port2 = optarg;
                break;

            case '?':
                fprintf(stderr, "Invalid option %c\n", optopt);
                exit(EXIT_FAILURE);
        }
    }

    // open serial port1
    int fd1 = open(port1, O_RDWR | O_NOCTTY);
    if (fd1 == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    tcflush(fd1, TCIOFLUSH);

    // open serial port2
    int fd2 = open(port2, O_RDWR | O_NOCTTY);
    if (fd2 == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    tcflush(fd2, TCIOFLUSH);

    // read port
    char buff[50];
    fd_set fdset1;
    fd_set fdset2;

    while(1)
    {
        bzero(buff, sizeof(buff));

        FD_ZERO(&fdset1);
        FD_SET(fd1, &fdset1);

        select(fd1+1, &fdset1, NULL, NULL, NULL);

        if (FD_ISSET(fd1, &fdset1))
        {
            int bytes = read (fd1, buff, sizeof(buff));

            if (bytes > 0)
            {
                printf("%s\n", buff);
                fflush(stdout);
            }
        }
        bzero(buff, sizeof(buff));

        FD_ZERO(&fdset2);
        FD_SET(fd2, &fdset2);

        select(fd2+1, &fdset2, NULL, NULL, NULL);

        if (FD_ISSET(fd2, &fdset2))
        {
            int bytes = read (fd2, buff, sizeof(buff));

            if (bytes > 0)
            {
                printf("%s\n", buff);
                fflush(stdout);
            }
        }
    }

    // close serial ports
    close(fd1);
    close(fd2);

    exit(EXIT_SUCCESS);
}
