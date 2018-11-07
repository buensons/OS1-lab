#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define ERROR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void scan_dir(const char * dname) {
    int largest_size = 0;
    char largest_file[50] = "";
    DIR * dirp;
    struct dirent * rdir;
    struct stat filestat;
    FILE * f = NULL;
    if(strcmp(getenv("OUTPUT"), "yes") == 0) {
        if(NULL == (f = fopen("sizes.txt", "w+"))) ERROR("fopen");
        fprintf(f, "Directory: %s\n", dname);
    } else {
        printf("\nDirectory: %s\n", dname);
    }
    if(NULL == (dirp = opendir("."))) ERROR("opendir");
    do {
        errno = 0;
        if(NULL != (rdir = readdir(dirp))) {
            if(lstat(rdir->d_name, &filestat)) ERROR("lstat");
            if(S_ISREG(filestat.st_mode)) {
                if(filestat.st_size > largest_size) {
                    largest_size = filestat.st_size;
                    strcpy(largest_file, rdir->d_name);
                }
                if(f == NULL) {
                    printf("%s: %ld\n", rdir->d_name, filestat.st_size);
                }
                else {
                    fprintf(f, "%s: %ld\n", rdir->d_name, filestat.st_size);
                }
            }
        }
    } while(rdir != NULL);
    if(errno != 0) ERROR("readdir");
    if(closedir(dirp)) ERROR("closedir");
    if(f == NULL) {
        if(strlen(largest_file) == 0) {
            printf("No files in this folder\n");
        } else {
            printf("The Largest file: %s | size: %d\n", largest_file, largest_size);
        }
    } else {
        if(strlen(largest_file) == 0) {
            fprintf(f, "No files in this folder\n");
        } else {
            fprintf(f, "The Largest file: %s | size: %d\n", largest_file, largest_size);
        }
        if(fclose(f)) ERROR("fclose");
        if(unlink("sizes.txt")&&errno!=ENOENT) ERROR("unlink");
    }
}

int walk(const char * name, const struct stat * fstat, int flag, struct FTW * f) {
    switch(flag) {
        case FTW_D: {
            char path[50];
            getcwd(path, 50);
            if(chdir(name)) ERROR("chdir");
            scan_dir(name);
            if(chdir(path)) ERROR("chdir");
            break;
        }
    }
    return 0;
}
int main(int argc, char ** argv) {
    char * dir;
    if((dir = getenv("DIR")) == NULL) ERROR("getenv");
    if(nftw(dir, walk, 20, FTW_PHYS) != 0) ERROR("nftw");
    return EXIT_SUCCESS;
}