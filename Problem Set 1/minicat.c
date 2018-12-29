/* Use of system calls in a simple concatenation program
Problem 3: minicat
Sara Huang
*/ 

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#define STDIN (0)
#define STDOUT (1)
#define STDERR (-1)

void filecopy(int,int,int);
int main (int argc,char *argv[]) {
    int fd, fdo = STDOUT; 
    int bufsiz = 512; //arbitrary initializing buffer size
    if (argc == 1) {
        filecopy(STDIN,STDOUT,bufsiz);
    }
    else {
        while (--argc > 0) {
            ++argv;
            if (strcmp(*argv,"-b") == 0) { //if has buffer arg
                ++argv;
                --argc;
                bufsiz = atoi(*argv);
            }
            else if (strcmp(*argv,"-o") == 0) { // if has outfile arg
                ++argv;
                --argc;
                if ((fdo = open(*argv,O_RDWR|O_CREAT|O_TRUNC,0666)) == -1) {
                    fprintf(stderr,"Error: cannot open file %s for writing\n",*argv);
                }
            }
            else {
                if ((fd = open(*argv,O_RDONLY)) == -1) {
                    fprintf(stderr,"Error: cannot open file %s for reading\n",*argv);
                }
                else {
                    filecopy(fd,fdo,bufsiz);
					if (close(fd) == -1) {
						fprintf(stderr, "Error: cannot close file %s\n", fd);
					}
					else {
						close(fd);
					}
                }
            }      
        }
    }       
    return 0;
}

/* filecopy: copy file ifd to ofd */
void filecopy(int ifd,int ofd,int bsiz) {
    int n;
    char *buf = malloc(bsiz); 
    while ((n = read(ifd,buf,bsiz)) > 0) {
        if (write(ofd,buf,n) != n) {
            fprintf(stderr,"Error: Cannot write\n");
        }
    }
}
