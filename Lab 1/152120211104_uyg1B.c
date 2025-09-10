#include <stdio.h>

int prime(int number) {
    if (number <= 1) 
    {
        return 0;
    } 
    
    else 
    {
        for (int i = 2; i < number; i++) {
            if (number % i == 0) {
                return 0;
            }
        }
    }

    return 1;
}

unsigned long long factorial(int n) {
    unsigned long long fact = 1;
    for (int i = 1; i <= n; i++) 
    {
        fact *= i;
    }

    return fact;
}

int main() {
    int n;
    printf("Bir sayı giriniz: ");
    scanf("%d", &n);

    int first = 1;

    printf("Çıktı: ");
    for (int i = 2; i <= n; i++) 
    {
        if (prime(i)) 
        {
            if (first) 
            {
                printf("%d! = %llu", i, factorial(i));
                first = 0; 
            } 
            
            else 
            {
                printf(", %d! = %llu", i, factorial(i));
            }
        }
    }
    printf("\n"); 

    return 0;
}
