#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER; // kiyafeti_yazdir ve giyilen_oge_sayisi için

// Semaforlar
sem_t sem_gomlek_giyildi_saat_icin;
sem_t sem_gomlek_giyildi_pantolon_icin;
sem_t sem_kemer_takildi;
sem_t sem_saat_takildi;
sem_t sem_mont_giyildi;
sem_t sem_sapka_takildi;
sem_t sem_cuzdan_alindi;
sem_t sem_ready_for_gloves;

// Giyilen kıyafet sayısının bilgisi
int giyilen_oge_sayisi = 0;
const int toplam_oge_sayisi = 14;

// Ekrana yazdirma fonksiyonu
void kiyafeti_yazdir(const char* bolge, const char* esya_adi) {
    pthread_mutex_lock(&output_mutex); // Paylaşılan sayaç ve printf için kilitle
    giyilen_oge_sayisi++;

    if (giyilen_oge_sayisi == 1) {
        printf("--> Gene mi toplanti!...\n");
    }

    printf("%d. (%s) %s\n", giyilen_oge_sayisi, bolge, esya_adi);

    if ((toplam_oge_sayisi - giyilen_oge_sayisi) < (toplam_oge_sayisi / 2) && (toplam_oge_sayisi - giyilen_oge_sayisi) > 0) {
        printf("--> Kalan oge sayisi: %d\n", toplam_oge_sayisi - giyilen_oge_sayisi);
    }

    if (giyilen_oge_sayisi == (toplam_oge_sayisi - 1) ) { // Eldiven hariç tüm eşyalar giyildiğinde
        if (strcmp(esya_adi, "Eldiven") != 0) {
             sem_post(&sem_ready_for_gloves);
        }
    }

    if (giyilen_oge_sayisi == toplam_oge_sayisi) {
        printf("--> Hazirim.\n");
    }
    pthread_mutex_unlock(&output_mutex); // Kilidi aç
}

// Kafa bolgesi thread'i (Gozluk, Sapka)
void* kafa_isleri(void* arg) {
    kiyafeti_yazdir("kafa", "Gozluk");

    // Sapka, monttan sonra giyilir.
    sem_wait(&sem_mont_giyildi); // Mont'un giyilmesini bekle
    kiyafeti_yazdir("kafa", "Sapka");
    sem_post(&sem_sapka_takildi); // Sapka takıldı, Cüzdan'a haber ver

    return NULL;
}

// Govde bolgesi thread'i (Gomlek, Kravat, Ceket, Mont, Kaskol)
void* govde_isleri(void* arg) {
    // Gomlek pantolondan SONRA giyilemez.
    kiyafeti_yazdir("govde", "Gomlek");
    sem_post(&sem_gomlek_giyildi_saat_icin);       // Gomlek giyildi, Saat'e haber ver
    sem_post(&sem_gomlek_giyildi_pantolon_icin); // Gomlek giyildi, Pantolon'a haber ver

    // Kravat kemerden sonra.
    sem_wait(&sem_kemer_takildi); // Kemer'in takılmasını bekle
    kiyafeti_yazdir("govde", "Kravat");

    kiyafeti_yazdir("govde", "Ceket");

    // Mont, saatten sonra takilir.
    sem_wait(&sem_saat_takildi); // Saat'in takılmasını bekle
    kiyafeti_yazdir("govde", "Mont");
    sem_post(&sem_mont_giyildi); // Mont giyildi, Şapka'ya haber ver

    kiyafeti_yazdir("govde", "Kaskol");
    return NULL;
}

// El bolgesi thread'i (Saat, Eldiven)
void* el_isleri(void* arg) {
    // Saat, gomlekten sonra.
    sem_wait(&sem_gomlek_giyildi_saat_icin); // Gomlek'in giyilmesini bekle
    kiyafeti_yazdir("el", "Saat");
    sem_post(&sem_saat_takildi); // Saat takıldı, Mont'a haber ver

    // Eldiven en son giyilir. Diger 13 esyanin giyilmesini bekle.
    sem_wait(&sem_ready_for_gloves); // Diğer tüm eşyaların hazır olmasını bekle
    kiyafeti_yazdir("el", "Eldiven");
    return NULL;
}

// Bacak bolgesi thread'i (Pantolon, Kemer, Cuzdan)
void* bacak_isleri(void* arg) {
    // Pantolon, gömlek giyildikten sonra giyilebilir
    sem_wait(&sem_gomlek_giyildi_pantolon_icin); // Gömleğin giyilmesini bekle
    kiyafeti_yazdir("bacak", "Pantolon");

    kiyafeti_yazdir("bacak", "Kemer");
    sem_post(&sem_kemer_takildi); // Kemer takıldı, Kravat'a haber ver

    // Cuzdan, sapkadan sonra.
    sem_wait(&sem_sapka_takildi); // Şapka'nın takılmasını bekle
    kiyafeti_yazdir("bacak", "Cuzdan");
    sem_post(&sem_cuzdan_alindi); // Cuzdan alındı, Ayakkabı'ya haber ver

    return NULL;
}

// Ayak bolgesi thread'i (Corap, Ayakkabi)
void* ayak_isleri(void* arg) {
    kiyafeti_yazdir("ayak", "Corap");

    // Ayakkabi cuzdan alinmadan giyilmez.
    sem_wait(&sem_cuzdan_alindi); // Cuzdan'ın alınmasını bekle
    kiyafeti_yazdir("ayak", "Ayakkabi");
    return NULL;
}

int main() {
    pthread_t kafa_t, govde_t, el_t, bacak_t, ayak_t;

    // Semaforları başlatma
    // Başlangıç değerleri 0, çünkü bir olay gerçekleştiğinde artırılacaklar.
    sem_init(&sem_gomlek_giyildi_saat_icin, 0, 0);
    sem_init(&sem_gomlek_giyildi_pantolon_icin, 0, 0);
    sem_init(&sem_kemer_takildi, 0, 0);
    sem_init(&sem_saat_takildi, 0, 0);
    sem_init(&sem_mont_giyildi, 0, 0);
    sem_init(&sem_sapka_takildi, 0, 0);
    sem_init(&sem_cuzdan_alindi, 0, 0);
    sem_init(&sem_ready_for_gloves, 0, 0);

    // Thread'leri başlatma
    pthread_create(&govde_t, NULL, govde_isleri, NULL);
    pthread_create(&kafa_t, NULL, kafa_isleri, NULL);
    pthread_create(&bacak_t, NULL, bacak_isleri, NULL);
    pthread_create(&ayak_t, NULL, ayak_isleri, NULL);
    pthread_create(&el_t, NULL, el_isleri, NULL);

    // Bütün thread'lerin bitişinin beklenmesi
    pthread_join(kafa_t, NULL);
    pthread_join(govde_t, NULL);
    pthread_join(bacak_t, NULL);
    pthread_join(ayak_t, NULL);
    pthread_join(el_t, NULL);

    // Kullandığımız mutex ve semaforları yok etme
    pthread_mutex_destroy(&output_mutex);
    sem_destroy(&sem_gomlek_giyildi_saat_icin);
    sem_destroy(&sem_gomlek_giyildi_pantolon_icin);
    sem_destroy(&sem_kemer_takildi);
    sem_destroy(&sem_saat_takildi);
    sem_destroy(&sem_mont_giyildi);
    sem_destroy(&sem_sapka_takildi);
    sem_destroy(&sem_cuzdan_alindi);
    sem_destroy(&sem_ready_for_gloves);

    return 0;
}
