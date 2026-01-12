#include <stdio.h>
#include <stddef.h>

// 2. Макрос за броя елементи в масив
#define dim(a) (sizeof(a) / sizeof((a)[0]))

// 3. Функция за проверка дали числото е четно
int even(int n) {
    return (n & 1) == 0;
}

// 4. Функция за проверка дали числото е отрицателно
int neg(int n) {
    return n < 0;
}

// 5. Функция за изчисляване на 2^p
long long p2(int p) {
    return 1LL << p;
}

// 6. Функция за броене на вдигнати битове (popcount)
int popcnt(unsigned int n) {
    int count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

// 7. Функция за проверка дали числото е степен на 2
int is_p2(unsigned int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Функция за намиране на логаритъма по основа 2
int log2_int(unsigned int n) {
    int log = 0;
    if (n == 0) return -1; // Грешка: log2(0) не е дефиниран
    while (n > 1) {
        n >>= 1;
        log++;
    }
    return log;
}

// 8. Функция за закръгляне нагоре до степен на 2
unsigned int ceil_p2(unsigned int n, int p2) {
    unsigned int power = 1U << p2;
    unsigned int mask = power - 1;
    return (n + mask) & ~mask;
}

// Функция за закръгляне надолу до степен на 2
unsigned int floor_p2(unsigned int n, int p2) {
    unsigned int power = 1U << p2;
    unsigned int mask = power - 1;
    return n & ~mask;
}

// 9. Макроси за работа със структури
// offset_of - отместване на член в структура
#define offset_of(t, m) ((size_t)&(((t*)0)->m))

// member_size - размер на член в структура
#define member_size(t, m) (sizeof(((t*)0)->m))

// container_of - намиране на съдържащата структура по указател към член
#define container_of(p, t, m) ((t*)((char*)(p) - offset_of(t, m)))

// Примерна структура за демонстрация
struct Example {
    int a;
    char b;
    double c;
    int d;
};

// Демонстрационна функция
int main() {
    // Тест 2: dim макрос
    int arr[] = {1, 2, 3, 4, 5};
    printf("Брой елементи в масива: %zu\n", dim(arr));
    
    // Тест 3: even функция
    printf("\n4 е четно: %d\n", even(4));
    printf("7 е четно: %d\n", even(7));
    
    // Тест 4: neg функция
    printf("\n-5 е отрицателно: %d\n", neg(-5));
    printf("10 е отрицателно: %d\n", neg(10));
    
    // Тест 5: p2 функция
    printf("\n2^3 = %lld\n", p2(3));
    printf("2^10 = %lld\n", p2(10));
    
    // Тест 6: popcnt функция
    printf("\nБрой битове в 15 (1111): %d\n", popcnt(15));
    printf("Брой битове в 7 (0111): %d\n", popcnt(7));
    
    // Тест 7: is_p2 и log2_int функции
    printf("\n8 е степен на 2: %d\n", is_p2(8));
    printf("10 е степен на 2: %d\n", is_p2(10));
    printf("log2(8) = %d\n", log2_int(8));
    printf("log2(16) = %d\n", log2_int(16));
    
    // Тест 8: ceil_p2 и floor_p2 функции
    printf("\nceil_p2(13, 2) = %u (закръгляне до 2^2=4)\n", ceil_p2(13, 2));
    printf("floor_p2(13, 2) = %u (закръгляне до 2^2=4)\n", floor_p2(13, 2));
    printf("ceil_p2(17, 4) = %u (закръгляне до 2^4=16)\n", ceil_p2(17, 4));
    printf("floor_p2(17, 4) = %u (закръгляне до 2^4=16)\n", floor_p2(17, 4));
    
    // Тест 9: offset_of, member_size, container_of
    struct Example ex = {10, 'A', 3.14, 42};
    printf("\nОтместване на 'c' в Example: %zu\n", offset_of(struct Example, c));
    printf("Размер на 'c' в Example: %zu\n", member_size(struct Example, c));
    
    // container_of демонстрация
    double *ptr_to_c = &ex.c;
    struct Example *recovered = container_of(ptr_to_c, struct Example, c);
    printf("Оригинален адрес: %p\n", (void*)&ex);
    printf("Възстановен адрес: %p\n", (void*)recovered);
    printf("Стойност на 'a' от възстановената структура: %d\n", recovered->a);
    
    return 0;
}