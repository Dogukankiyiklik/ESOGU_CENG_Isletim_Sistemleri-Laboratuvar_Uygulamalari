#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

void alarm_handler(int sig) {}

int main() {
    int pfd[2];

    if (pipe(pfd) == -1) {
        perror("pipe hatasi");
        exit(1);
    }

    pid_t child_id = fork();

    if (child_id < 0) {
        perror("fork hatasi");
        close(pfd[0]);
        close(pfd[1]);
        exit(1);
    }

    if (child_id == 0) {
        close(pfd[0]);
        char ogr_no[13];

        printf("Ogrenci numarasini giriniz: ");
        scanf("%12s", ogr_no);

        write(pfd[1], ogr_no, strlen(ogr_no) + 1);
        close(pfd[1]);
        sleep(4);
        exit(0);
    } else {
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        close(pfd[1]);

        char buf[13];
        read(pfd[0], buf, sizeof(buf));
        close(pfd[0]);

        if (strlen(buf) == 12) {
            int fakulte, bolum, giris_yili;
            sscanf(buf, "%2d%2d%4d", &fakulte, &bolum, &giris_yili);

            if (giris_yili >= 2016) {
                printf("Kosul 1: Cocuk bekleniyor...\n");
                waitpid(child_id, NULL, 0);
                printf("Cocuk surec tamamlandi\n");

            } else if (fakulte == 15 && bolum == 21) {
                printf("Kosul 2: 4 saniyelik alarm kuruluyor...\n");
                signal(SIGALRM, alarm_handler);
                alarm(4);
                pause();
                kill(child_id, SIGTERM);
                waitpid(child_id, NULL, 0);
                printf("Cocuk sonlandirildi\n");

            } else if (fakulte != 15) {
                printf("Kosul 3: 10 saniyelik alarm kuruluyor...\n");
                signal(SIGALRM, alarm_handler);
                alarm(10);
                pause();
                printf("Bekleme tamamlandi\n");

            } else {
                printf("Hatali ogrenci numarasi girisi.\n");
            }

        } else {
            printf("Gecersiz numara uzunlugu.\n");
        }

        printf("Ebeveyn surec tamamlandi\n");
    }

    return 0;
}
