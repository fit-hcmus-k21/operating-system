#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


struct ThreadInfo {
    char* fileName;
    sem_t* semaphore;
};


// function implement feature write to file
void* writeToFile(void *arg) {
    struct ThreadInfo* info = (struct ThreadInfo*) arg;
    char* fileName = info->fileName;
    sem_t* semaphore = info->semaphore;

    char data[] = "21120505 - Bùi Thị Thanh Ngân";
    FILE* file = fopen(fileName, "w");

    // check semaphore to make sure only one thread write to file at a time
    sem_wait(semaphore);

    if (file != NULL) {
        fprintf(file, "%s", data);
        fclose(file);
        // printf("Thread 1 wrote to file: %s\n", data);
    } else {
        printf("Error opening file for writing\n");
    }

    // increase semaphore value after write to file
    sem_post(semaphore);

    // exit thread after finish 
    pthread_exit(NULL);
}

// function implement feature read from file
void* readFromFile(void *arg) {
    struct ThreadInfo* info = (struct ThreadInfo*) arg;
    char* fileName = info->fileName;
    sem_t* semaphore = info->semaphore;
    char buffer[256];

    // check semaphore to make sure only one thread read from file at a time
    sem_wait(semaphore);

    FILE* file = fopen(fileName, "r");
    if (file != NULL) {
        fgets(buffer, sizeof(buffer), file);
        fclose(file);
        printf("%s\n", buffer);
    } else {
        printf("Error opening file for reading\n");
    }

    // increase semaphore value after read from file
    sem_post(semaphore);

    pthread_exit(NULL);
}

int main() {

    // declare Semaphore
    sem_t semaphore;

    // initialize semaphore with value 1
    sem_init(&semaphore, 0, 1);

    pthread_t thread1, thread2;

    char* fileName = malloc(sizeof(char) * 256);
    strcpy(fileName, "text.txt");

    // create struct info for thread
    struct ThreadInfo threadInfo;
    threadInfo.fileName = fileName;
    threadInfo.semaphore = &semaphore;

    // create first thread to write to file
    if (pthread_create(&thread1, NULL, writeToFile, (void*) &threadInfo)) {
        fprintf(stderr, "Have error when creating first thread - write to file\n");
        return 22;
    }

    // wait until thread 1 finish
    pthread_join(thread1, NULL);


    // create second thread to read from file
    if (pthread_create(&thread2, NULL, readFromFile, (void*) &threadInfo)) {
        fprintf(stderr, "Have error when creating second thread - read from file\n");
        return 22;
    }

    // wait until thread 2 finish
    pthread_join(thread2, NULL);

    // destroy semaphore
    sem_destroy(&semaphore);

    // free memory
    free(fileName);

    return 225;
}
