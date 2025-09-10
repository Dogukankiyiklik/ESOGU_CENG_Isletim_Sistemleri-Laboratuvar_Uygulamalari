#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

// Hata yakalama fonksiyonu (SEGMENTATION FAULT için)
void handler(int sig, siginfo_t *info, void *context) {
    printf("\n>>> Segmentation fault oluştu! Hatalı adres: %p\n", info->si_addr);
    exit(1); // Programı bitir
}

// Belirtilen aralıktan rastgele adres döndürür
unsigned char *rastgele_adres(unsigned long bas, unsigned long bit) {
    if (bit <= bas) return (unsigned char*)bas;
    unsigned long fark = bit - bas;
    return (unsigned char*)(bas + rand() % fark);
}

int main() {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) {
        perror("maps dosyası açılamadı");
        return 1;
    }

    // Sinyal yakalama ayarı (SIGSEGV için)
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGSEGV, &act, NULL);

    srand(time(NULL)); // rand için seed

    char satir[256], izinler[5];
    unsigned long bas, bit;

    printf("PID: %d\n", getpid());
    printf("Hafıza segmentleri ve izin testi:\n");

    while (fgets(satir, sizeof(satir), fp)) {
        if (sscanf(satir, "%lx-%lx %4s", &bas, &bit, izinler) != 3) continue;

        printf("\nSegment: %lx - %lx | İzinler: %s\n", bas, bit, izinler);

        unsigned char *adres = rastgele_adres(bas, bit);
        unsigned char okunan;

        if (izinler[0] == 'r' && izinler[1] == 'w') {
            printf("  -> Tip: Okunabilir ve yazılabilir\n");
            okunan = *adres;
            printf("  Okuma tamam. Değer (hex): %02X\n", okunan);
            *adres = 0xAB;
            printf("  Yazma tamamlandı. Yeni değer: %02X\n", *adres);
        }
        else if (izinler[0] == 'r' && (izinler[1] == '-' || izinler[1] == 'x')) {
            printf("  -> Tip: Sadece okunabilir\n");
            okunan = *adres;
            printf("  Okuma tamam. Değer (hex): %02X\n", okunan);
            printf("  Yazma deneniyor...\n");
            *adres = 0xFF; // Burada segmentation fault olabilir
            printf("  Yazma başarılı (bu beklenmeyen durumdur)\n");
        }
    }

    fclose(fp);

    // Bilerek hata üretelim (ödev gereği)
    printf("\nBilerek hata üretiliyor (NULL pointer'a yazma)...\n");
    unsigned char *hatali = NULL;
    *hatali = 0x55; // Bu satır SIGSEGV oluşturmalı

    return 0;
}
