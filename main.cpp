// C program to demonstrate working of Semaphores
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <condition_variable>

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

    int carNum;
    string WBoundString;
};

class tunnelinfo {
public:
    tunnelinfo() {}
    int maxAllowed;
    int numCars;
    int waitTime;
};

struct Compareline {
    bool operator()(line*& n1, line*& n2)
    {
        return n1->timeIn > n2->timeIn;
    }
};

bool isWBound(string basic_string);

int numExited = 0;

bool allCarsPassed(int total) {
    return numExited == total;
}

//void* thread(void* arg)
//{
//    //wait
//    sem_wait(&mutex);
//    printf("\nEntered..\n");
//
//    //critical section
//    sleep(4);
//
//    //signal
//    printf("\nJust Exiting...\n");
//    sem_post(&mutex);
//}

pthread_mutex_t carLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mCanWB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mCanBB = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t canWB = PTHREAD_COND_INITIALIZER;
pthread_cond_t canBB = PTHREAD_COND_INITIALIZER;
bool boolCanWB = false;
bool boolCanBB = false;
sem_t maxInTunnel;

void* car(void* arg) {
    line* in = (line*) arg;
    sleep(in->prevTime);
    sleep(in->timeIn);
    cout << "Car #" << in->carNum << " going to " << in->WBoundString << " arrives at the tunnel." << endl;
    pthread_mutex_lock(&carLock);
    if (in->WBound) {
        while (!boolCanWB) {
            pthread_cond_wait(&canWB, &carLock);
            sem_wait(&maxInTunnel);        }
    } else {
        while (!boolCanBB) {
            pthread_cond_wait(&canBB, &carLock);
            sem_wait(&maxInTunnel);
        }
    }
    pthread_mutex_unlock(&carLock);

    cout << "Car #" << in->carNum << " going to " << in->WBoundString << " enters the tunnel." << endl;
    sleep(in->duration);
    sem_post(&maxInTunnel);
    cout << "Car #" << in->carNum << " going to " << in->WBoundString << " exits the tunnel." << endl;
    numExited++;
}

bool goTunnel = true;

void* tunnel(void* arg) {
    tunnelinfo *info = (tunnelinfo*) arg;
    while (goTunnel) {
        boolCanWB = true;
        pthread_cond_broadcast(&canWB);
        if (allCarsPassed(info->numCars)) break;
        cout << "The tunnel is now open to Whittier-bound traffic." << endl;
        sleep(info->waitTime);
        boolCanWB = false;
        if (allCarsPassed(info->numCars)) break;
        cout << "The tunnel is now closed to ALL traffic." << endl;
        sleep(info->waitTime);
        boolCanBB = true;
        pthread_cond_broadcast(&canBB);
        if (allCarsPassed(info->numCars)) break;
        cout << "The tunnel is now open to Bear Valley-bound traffic." << endl;
        sleep(info->waitTime);
        boolCanBB = false;
        if (allCarsPassed(info->numCars)) break;
        cout << "The tunnel is now closed to ALL traffic." << endl;
        sleep(info->waitTime);
    }
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
    sem_init(&maxInTunnel, 0 ,MAX_TUNNEL);


    try {
        line *cur;
        int curtime = 0;
        while (inFile >> x) {
            cur = new line();
            cur-> carNum = n+1;
            cur->timeIn = stoi(x);
            input.append(x);
            input.append(" ");
            inFile >> x;
            cur->WBound = isWBound(x);
            cur->WBoundString = x;
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

            n++; // at the end, the n contains the number of threads
        }
    } catch (exception e) {
        cout << "something went wrong reading the file";
    }

    tunnelinfo *info = new tunnelinfo();
    info->waitTime = 5;
    info->maxAllowed = MAX_TUNNEL;
    info->numCars = n;
    pthread_t tid;
    pthread_create(&tid,NULL,tunnel,(void *)info);
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