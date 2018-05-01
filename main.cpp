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

void updateArrive(bool WBbound);

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
pthread_cond_t canWB = PTHREAD_COND_INITIALIZER;
pthread_cond_t canBB = PTHREAD_COND_INITIALIZER;
bool boolCanWB = false;
bool boolCanBB = false;
sem_t maxInTunnel;
int numHadToWait = 0;
void hadToWait() {
    numHadToWait++;
}

void* car(void* arg) {
    line* in = (line*) arg;
    sleep(in->prevTime);
    sleep(in->timeIn);
    bool didGrab = false;
    cout << "Car #" << in->carNum << " going to " << in->WBoundString << " arrives at the tunnel." << endl;
    updateArrive(in->WBound);
    pthread_mutex_lock(&carLock);
    if (in->WBound) {
        while (!boolCanWB) {
            pthread_cond_wait(&canWB, &carLock);
        }

    } else {
        while (!boolCanBB) {
            pthread_cond_wait(&canBB, &carLock);
        }
    }
    pthread_mutex_unlock(&carLock);
    int test;
    sem_getvalue(&maxInTunnel,&test);
    if (test <= 0) hadToWait();
    sem_wait(&maxInTunnel);

    cout << "Car #" << in->carNum << " going to " << in->WBoundString << " enters the tunnel." << endl;
    sleep(in->duration);
    sem_post(&maxInTunnel);
    cout << "Car #" << in->carNum << " going to " << in->WBoundString << " exits the tunnel." << endl;
    numExited++;
}
int numWBArrived = 0;
int numBBArrived = 0;
void updateArrive(bool WBbound) {
    if (WBbound) {
        numWBArrived++;
    } else {
        numBBArrived++;
    }
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
    cout << numBBArrived << " car(s) going to Bear Valley arrived at the tunnel" << endl;
    cout << numWBArrived << " car(s) going to Whittier arrived at the tunnel" << endl;

    cout << numHadToWait << " car(s) had to wait because the tunnel was full" << endl;
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