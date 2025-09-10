#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// Log yazmak icin basit fonksiyon
void logla(char *mesaj) {
    int f = open("logs/log_all.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (f < 0) {
        printf("Log dosyasi acilamadi!\n");
        exit(1);
    }
    
    char buf[100];
    sprintf(buf, "PID:%d PPID:%d - %s\n", getpid(), getppid(), mesaj);
    write(f, buf, strlen(buf));
    close(f);
}

// Komutu calistir ve sonucu logla
void komut_calistir(char *komut, char *aciklama) {
    // Gecici dosya olustur
    char temp[50] = "/tmp/cikti_XXXXXX";
    int fd = mkstemp(temp);
    
    // Komutu calistir
    char cmd[200];
    sprintf(cmd, "%s > %s", komut, temp);
    system(cmd);
    
    // Sonucu oku
    char sonuc[2000] = {0};
    lseek(fd, 0, SEEK_SET);
    read(fd, sonuc, 1999);
    close(fd);
    unlink(temp);
    
    // Loga yaz
    int f = open("logs/log_all.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (f < 0) {
        printf("Log dosyasi acilamadi!\n");
        exit(1);
    }
    
    char log_metin[2500];
    sprintf(log_metin, "PID:%d PPID:%d - %s:\n%s\n", 
            getpid(), getppid(), aciklama, sonuc);
    
    write(f, log_metin, strlen(log_metin));
    close(f);
}

int main() {
    printf("Ebeveyn basladi [%d]\n", getpid());
    
    // logs klasoru olustur
    mkdir("logs", 0755);
    
    // Tarih bilgisini logla
    komut_calistir("date", "Tarih");
    logla("Ebeveyn basladi");
    
    // Cocuk1 olustur
    pid_t c1 = fork();
    
    if (c1 == 0) { 
        // Cocuk1 kodlari
        printf("Cocuk1 [%d]\n", getpid());
        logla("Cocuk1 basladi");
        
        // Torun1 olustur
        if (fork() == 0) {
            printf("Torun1 [%d]\n", getpid());
            logla("Torun1 basladi");
            komut_calistir("pwd && ls -la", "Calisma dizini");
            sleep(1);
            logla("Torun1 bitti");
            exit(0);
        }
        
        // Torun2 olustur
        if (fork() == 0) {
            printf("Torun2 [%d]\n", getpid());
            logla("Torun2 basladi");
            char cmd[50];
            sprintf(cmd, "pstree -sg %d", getppid());
            komut_calistir(cmd, "Proses agaci");
            sleep(2);
            logla("Torun2 bitti");
            exit(0);
        }
        
        // Torun3 olustur
        if (fork() == 0) {
            printf("Torun3 [%d]\n", getpid());
            logla("Torun3 basladi");
            komut_calistir("who -a", "Oturum bilgisi");
            sleep(3);
            logla("Torun3 bitti");
            exit(0);
        }
        
        // En az bir torun bekle
        wait(NULL);
        logla("Bir torun beklendi");
        logla("Cocuk1 bitti");
        exit(0);
    }
    
    // Cocuk2 olustur
    if (fork() == 0) {
        printf("Cocuk2 [%d]\n", getpid());
        logla("Cocuk2 basladi");
        sleep(1);
        logla("Cocuk2 bitti");
        exit(0);
    }
    
    // Ebeveyn 2sn bekliyor
    sleep(2);
    
    // Cocuklari bekle
    wait(NULL);
    wait(NULL);
    
    logla("Ebeveyn bitti");
    printf("Ebeveyn bitti [%d]\n", getpid());
    
    return 0;
}
