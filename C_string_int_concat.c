int main(int argc, char ** argv)
{
    char pid_string[6 + sizeof(pid_t)];
    sprintf(pid_string, "PID: %d ", getpid());
    printf("%s\n", pid_string);
    printf("size: %ld\n", sizeof(pid_string));
    printf("length: %ld\n", strlen(pid_string));
    return 0;
}
