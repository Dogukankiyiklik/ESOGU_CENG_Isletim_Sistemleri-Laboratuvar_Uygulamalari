#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct multiplePrintData {
    pthread_t id;
    int threadid;
    char *printText;
    double waitDuration;
};

void* printCharMultipleTimes(void *arg) {
    struct multiplePrintData *data = (struct multiplePrintData*) arg;

    while (1) {
        printf("%s", data->printText);
        fflush(stdout);
        usleep((int)(data->waitDuration * 1000));
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    int toplamSure = atoi(argv[1]);
    int threadSayisi = atoi(argv[2]);

    struct multiplePrintData threadData[threadSayisi];

    for (int i = 0; i < threadSayisi; i++) {
        threadData[i].threadid = i;
        threadData[i].printText = argv[3 + i * 2];
        threadData[i].waitDuration = atof(argv[4 + i * 2]);

        pthread_create(&threadData[i].id, NULL, printCharMultipleTimes, &threadData[i]);
    }

    usleep(toplamSure * 1000);

    for (int i = 0; i < threadSayisi; i++) {
        pthread_cancel(threadData[i].id);
    }

    return 0;
}
