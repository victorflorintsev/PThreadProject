// C program to demonstrate working of Semaphores
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;

sem_t mutex;

enum tunnelstatus {WB, CLOSED, BB};

class line {
public:
    line() {}
    int timeIn;
    bool WBound;
    int duration;
    int prevTime;
};

struct Compareline {
    bool operator()(line*& n1, line*& n2)
    {
        return n1->timeIn > n2->timeIn;
    }
};

bool isWBound(string basic_string);

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
    line* in = (line*) arg;
    sleep(in->prevTime);
    cout << "sleeping for " << in->timeIn << "seconds." << endl;
    sleep(in->timeIn);
    cout << "stopped sleeping." << endl;
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
    int n = 0;
    int test = 0;
    string x = "";
    string input = "";
    inFile >> x;
    int MAX_TUNNEL = stoi(x);
    input.append(x);
    input.append("\n");


    try {
        line *cur;
        int curtime = 0;
        while (inFile >> x) {
            cur = new line();
            cur->timeIn = stoi(x);
            input.append(x);
            input.append(" ");
            inFile >> x;
            cur->WBound = isWBound(x);
            input.append(x);
            input.append(" ");
            inFile >> x;
            cur->duration = stoi(x);
            input.append(x);
            input.append("\n");

            cur->prevTime = curtime;
            curtime = curtime + cur->timeIn;

            pthread_t carid;
            pthread_create(&carid,NULL,car,(void *)cur);

            n++;
        }
    } catch (exception e) {
        cout << "something went wrong reading the file";
    }
    pthread_exit(0);
    inFile.close();

    cout << input;

//    sem_init(&mutex, 0, 1);
//    pthread_t t1,t2;
//    pthread_create(&t1,NULL,thread,NULL);
//    sleep(2);
//    pthread_create(&t2,NULL,thread,NULL);
//    pthread_join(t1,NULL);
//    pthread_join(t2,NULL);
//    sem_destroy(&mutex);
    return 0;
}

bool isWBound(string str) {
    if (str == "WB") return true;
    return false;
}