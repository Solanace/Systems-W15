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

int main(int argc, char **argv) {
	int sem_desc, mem_desc;
	if (argc > 1) {
		if (!strcmp(argv[1], "-c")) {
			// printf("Creating semaphore\n");
			sem_desc = semget(KEY, 1, 0644 | IPC_CREAT | IPC_EXCL);
			if (sem_desc < 0) {
				printf("Semaphore already exists!\n");
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
				mem_desc = shmget(KEY, sizeof(char) * 256, IPC_CREAT | IPC_EXCL | 0644);
			}
		}
		else if (!strcmp(argv[1], "-v")) {
			char *args[3];
			args[0] = "cat";
			args[1] = "story.txt";
			args[2] = 0;
			execvp(args[0], args);
		}
		else if (!strcmp(argv[1], "-r")) {
			// printf("Removing semaphore\n");
			sem_desc = semget(KEY, 1, 0);
			int sem_val = semctl(sem_desc, 0, IPC_RMID);
			if (sem_val < 0) {
				printf("Error encountered while removing semaphore\n");
				printf("%s\n", strerror(errno));
			}
			else printf("Semaphore removed: %d\n", sem_desc);
			
			mem_desc = shmget(KEY, sizeof(char) * 256, 0);
			int shm_val = shmctl(mem_desc, IPC_RMID, 0);
			if (shm_val < 0) {
				printf("%s\n", strerror(errno));
			}
			char *args[3];
			args[0] = "rm";
			args[1] = "story.txt";
			args[2] = 0;
			execvp(args[0], args);
			
		}
	}
	return 0;
}
