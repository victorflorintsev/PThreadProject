// C program to demonstrate working of Semaphores
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

using namespace std;

sem_t mutex;

void* thread(void* arg)
{
    //wait
    sem_wait(&mutex);
    printf("\nEntered..\n");

    //critical section
    sleep(4);

    //signal
    printf("\nJust Exiting...\n");
    sem_post(&mutex);
}

void* car(void* arg) {

}


int main()
{
    string filepath = "inputa.txt";
    ifstream inFile;
    inFile.open(filepath);
    if (!inFile) {
        cerr << "Unable to open file";
        exit(1);
    }
    string x = "";
    string input = "";
    while (inFile >> x) {
        input.append(x);
        input.append("\n");
    }
    inFile.close();

    cout << input;

    sem_init(&mutex, 0, 1);
    pthread_t t1,t2;
    pthread_create(&t1,NULL,thread,NULL);
    sleep(2);
    pthread_create(&t2,NULL,thread,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    sem_destroy(&mutex);
    return 0;
}