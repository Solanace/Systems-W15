#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#define KEY 2142

void setup_all() {
    int sem_desc, mem_desc;
    // printf("Creating semaphore\n");
    sem_desc = semget(KEY, 1, 0644 | IPC_CREAT | IPC_EXCL);
    if (sem_desc < 0) {
        printf("Program has already been initialized\n");
    }
    else {
        union semun {
            int val;
            struct semid_ds *buf;
            unsigned short *array;
            struct seminfo *__buf;
        } arg;
        arg.val = 1;
        if (semctl(sem_desc, 0, SETVAL, arg) < 0) {
            printf("Error encountered while setting value of semaphore\n");
            printf("%s\n", strerror(errno));
        }
        printf("Semaphore created with a semid of %d\n", sem_desc);
        close(open("story.txt", O_CREAT | O_EXCL | O_TRUNC, 0644));
        mem_desc = shmget(KEY, sizeof(int), IPC_CREAT | IPC_EXCL | 0644);
        int* mem_loc = shmat(mem_desc, 0, 0);
        *mem_loc = 0;
        shmdt(mem_loc);
    }
}    

void read_full_story() {
    int sem_desc = semget(KEY, 1, 0);
    if (sem_desc < 0) {
        printf("Initialize program first with: $ ./control -c\n");
        exit(0);
    }
    printf("Printing full story:\n");
    char *args[3];
    args[0] = "cat";
    args[1] = "story.txt";
    args[2] = 0;
    execvp(args[0], args);
}

void remove_all() {
    int sem_desc, mem_desc;
    // printf("Removing semaphore\n");
    sem_desc = semget(KEY, 1, 0);
    int sem_val = semctl(sem_desc, 0, IPC_RMID);
    if (sem_val < 0) {
        printf("Initialize program first with: $ ./control -c\n");
        exit(0);
    }
    printf("Semaphore removed: %d\n", sem_desc);
    mem_desc = shmget(KEY, sizeof(int), 0);
    int shm_val = shmctl(mem_desc, IPC_RMID, 0);
    printf("Shared memory segment removed\n");
    printf("story.txt removed\n");
    char *args[3];
    args[0] = "rm";
    args[1] = "story.txt";
    args[2] = 0;
    execvp(args[0], args);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: $ ./control -c | -r | -v\n");
    }
    else if (!strcmp(argv[1], "-c")) {
        setup_all();
    }
    else if (!strcmp(argv[1], "-v")) {
        read_full_story();
    }
    else if (!strcmp(argv[1], "-r")) {
        remove_all();
    }
    return 0;
}
