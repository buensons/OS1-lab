// Write a program that accepts the following positional parameters pairs:

// path
//     path to the folder in the file system
// bytes
//     limiting folder content size

// The program appends the name of the folder given by "path" to the file out.txt 
// only if the total sum of contained objects sizes is above the limit given by "bytes".
// The same procedure apply to all the folders given in parameters. 
// Program sums sizes of all object types (folders,directories, links, etc.) 
// in the folder, it does not count the sizes of sub-folders. 
//If the folder can not be checked (does not exist or no access) proper message
//  must be printed on stderr but the program should continue to check other folders.


// Example.:

// $./prog /etc 5000 /run 2000 /root 100
// No access to folder "root"
// $./cat out.txt
// /etc

// In the above example size of all objects in /run is below 2000 bytes.

// Graded stages (3 points each, please report the stage as soon as you finish it).

// X Program lists the names of objects in the working folder. To show: run the program without params
// X Program lists the sizes (without names) of objects in the working folder, counts the sum of sizes and prints it on the screen. To show: run the program without params
// X Extend the code to accept folders as parameters (sizes are ignored) and repeat the printout for every folder. To show: run the program with params: /etc 1000 /run 200
// X Program prints only the names of folders if the content size is above the limit. To show: run the program with params: /etc 1000 /run 200, adjust the sizes so one folder will be printed and other not
// X Move output to the file out.txt, add message on access problems. To show: run the program with params as in the example below the task.


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_SIZE 101
#define ERROR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

extern int errno;

void usage(char * pname) {
    printf("Usage: %s PATH BYTES ...\n", pname);
    exit(EXIT_FAILURE);
}

int scan_dir(int size) {
    DIR * dirp;
    struct dirent * rdir;
    struct stat filestat;
    int sum = 0;
    if(NULL == (dirp = opendir("."))) ERROR("opendir");
    do {
        errno = 0;
        if((rdir = readdir(dirp)) != NULL) {
            if(lstat(rdir->d_name, &filestat)) ERROR("lstat");
            sum += filestat.st_size;
        }
    } while(rdir != NULL);
    if(errno != 0) ERROR("readdir");
    if(closedir(dirp)) ERROR("closedir");
    if(sum > size)
        return 0;
    else return -1;

}
int main(int argc, char ** argv) {
    char path[MAX_SIZE];
    int s;
    FILE * file;
    if((file = fopen("file.txt", "a")) == NULL) ERROR("fopen");
    if(getcwd(path, MAX_SIZE) == NULL) ERROR("getcwd");
    for(int i = 1; i < argc; i++) {
        if(chdir(argv[i])) {
            fprintf(stderr, "No access to folder %s", argv[i++]);
            continue;
        }
        i++;
        if(i >= argc) usage(argv[0]);
        s = atoi(argv[i]);
        if(scan_dir(s) == 0) {
            printf("%s\n", argv[i-1]);
            fprintf(file, "%s\n", argv[i-1]);
        }
        if(chdir(path)) ERROR("chdir");
    }
    if(fclose(file)) ERROR("fclose");
    return EXIT_SUCCESS;
}
