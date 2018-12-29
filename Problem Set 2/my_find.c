/* Problem Set 2; Problem 3
Recursive Filesystem Lister
Sara Huang
my_find.c */

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		return list_dir(".");
	}
	else {
		int ret = 0;
		for (int i = 1; i < argc; i += 1) {
			if (list_dir(argv[i] != 0)) {
				ret = 1;
			}
		}
		return ret;
	}
}

int list_dir(const char *dir_name) {
	struct dirent *current_directory;
	struct stat my_stat;
	struct tm lt;
	struct passwd *pwd;
	DIR *directory = opendir(dir_name);
	if (directory == NULL) {
		fprintf(stderr,"list_dir: %s: %s \n",dir_name,strerror(errno));
		return 0;
	}
	printf("Directory: %s\n",dir_name);
	printf("\n");
	while ((current_directory = readdir(directory))) {
		stat(current_directory->d_name, &my_stat);
		char buf[1024];
		errno = 0;
		snprintf(buf,sizeof buf, "%s/%s",dir_name,current_directory->d_name);
		if ((stat(buf,&my_stat)) == 0) {
			pwd = getpwuid(my_stat.st_uid);
		}
		else {
			printf("%s: %s\n",buf,strerror(errno));
			exit(EXIT_FAILURE);
		}
		time_t t = my_stat.st_mtime;
		localtime_r(&t,&lt);
		char timebuf[80];
		strftime(timebuf,sizeof(timebuf),"%c",&lt);
		if (pwd != 0) {
			printf("%s \t %ld \t %s \t %s",pwd->pw_name,(long)my_stat.st_size,timebuf,current_directory->d_name);
			printf("\n");
		}
		else {
			printf("%d \t %ld \t %s \t %s",my_stat.st_uid,(long)my_stat.st_size,timebuf,current_directory->d_name);
			printf("\n");
		}
	}
	closedir(directory);
	return 0;
}
