#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#define KEY 2142

int main() {
    int sem_desc = semget(KEY, 1, 0);
    if (sem_desc < 0) {
        printf("Initialize program first with: $ ./control -c\n");
        exit(0);
    }
    printf("Waiting for access...\n");
    struct sembuf sb;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    sb.sem_num = 0;
    semop(sem_desc, &sb, 1);

    int mem_desc = shmget(KEY, sizeof(int), 0);
    int* mem_loc = shmat(mem_desc, 0, 0);
    int fd = open("story.txt", O_APPEND | O_RDWR);
    lseek(fd, -1 * *mem_loc, SEEK_END);
    char* story_end = (char*) calloc(*mem_loc + 1, sizeof(char));
    read(fd, story_end, *mem_loc);
    printf("Last contribution:\n");
    printf("%s\n", story_end);
    free(story_end);

    char new_contribution[256];
    printf("Enter next portion:\n");
    fgets(new_contribution, sizeof(new_contribution), stdin);
    lseek(fd, 0, SEEK_END);
    write(fd, new_contribution, strlen(new_contribution));
    *mem_loc = strlen(new_contribution);
    shmdt(mem_loc);
    
    sb.sem_op = 1;
    semop(sem_desc, &sb, 1);
    return 0;
}
