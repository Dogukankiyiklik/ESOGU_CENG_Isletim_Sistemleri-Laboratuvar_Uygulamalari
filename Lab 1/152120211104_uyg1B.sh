#!/bin/bash

echo "Script'i executable yapmak için laba hazırlık pdf'indeki chmod komutunu kullanıyoruz."
chmod +x 152120211104_uyg1A.sh

echo "-p If the parent directories don't exist, this command creates them."
mkdir -p backup

echo "Çalışma klasörünün içeriği (boyutlu ve alfabetik):"
ls -lh | sort

echo "Çalışma klasörünün içeriği (sade):"
ls

echo ".c ve .sh dosyalarını backup’a taşı"
mv *.c *.sh backup/

echo "Çalışma klasöründeki tüm dosyaları sil (klasörler hariç)"
find . -maxdepth 1 -type f -delete

echo "3.6 Backup’taki dosyaları geri kopyala"
cp backup/* .

echo "3.7 Backup klasörünü sil"
rm -r backup

echo "3.8 C/C++ kodunu derle"
gcc 152120211104_uyg1A.c -o 152120211104_uyg1A


echo "Program ilk çalıştırma (girdi: 7):"
./152120211104_uyg1A << EOF > 152120211104_uyg1A_output.txt
7
EOF

echo "Çıktı dosyasına yazıldı. Satır ve kelime sayıları:"
wc -l -w 152120211104_uyg1A_output.txt

echo "Program ikinci çalıştırma (girdi: 3):"
./152120211104_uyg1A << EOF > 152120211104_uyg1A_output.txt
3
EOF

echo "Çıktı dosyasına yazıldı. Satır ve kelime sayıları:"
wc -l -w 152120211104_uyg1A_output.txt

echo "Çıktı dosyasının içeriği:"
cat 152120211104_uyg1A_output.txt
