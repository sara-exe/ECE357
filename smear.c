/* Sara Huang
 * Problem 3, Problem Set 5
 * smear program
 */

#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc, char *argv[]) {
	if (argc <= 3) {
		fprintf(stderr,"Error: not enough input arguments to smear; needs at least 1 TARGET string, 1 REPLACEMENT string, and 1 infile specified\n");
	}
	else {
		if (strlen(argv[1]) != strlen(argv[2])) {
			fprintf(stderr,"Error: TARGET and REPLACEMENT strings are not the same number of characters\n");
		}
		++argv;
		++argv;
		++argv;	
		for (int i = 3; i < argc; i++) {
			int fd;	
			fd = open(*argv,O_RDONLY);
			++argv;
			if (fd < 0) {
				fprintf(stderr,"Error: cannot open infile %s for reading\n",*argv);
			}
			else {
				off_t size = sizeof(**argv);
				char *addr = argv[3];
				char *dest;
				int fdout;
				int fdin = open(argv[3],O_RDONLY);
				if (fdin < 0) {
					fprintf(stderr,"Error: cannot open infile %s for reading\n",*argv);
				}
				else {
				addr = mmap(NULL,size,PROT_READ,MAP_SHARED,*argv[3],0);
					if (addr == MAP_FAILED) {
						fprintf(stderr,"Error: mmap error\n");
					}
					else {
						dest = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fdout,0);
						if (dest == MAP_FAILED) {
							fprintf(stderr,"Error: mmap error\n");	
						}
					}
				}
			}
		}
	}
}
