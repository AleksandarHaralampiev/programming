#include <stdlib.h>
#include <string.h>

struct student {
    char name[50];
    int math;
    int programming;
    int history;
    int age;
    char specialty[50];
};

int compare_by_math(const void *a, const void *b) {
    const struct student *s1 = a;
    const struct student *s2 = b;
    return s1->math - s2->math;
}

int compare_by_math_then_programming(const void *a, const void *b) {
    const struct student *s1 = a;
    const struct student *s2 = b;
    
    if (s1->math != s2->math) {
        return s1->math - s2->math;
    }
    return s1->programming - s2->programming;
}

int compare_by_age_then_name_desc(const void *a, const void *b) {
    const struct student *s1 = a;
    const struct student *s2 = b;
    
    if (s1->age != s2->age) {
        return s2->age - s1->age;
    }
    return strcmp(s2->name, s1->name);
}

void sort_students_by_math(struct student arr[], int n) {
    qsort(arr, n, sizeof(struct student), compare_by_math);
}

void sort_students_by_math_and_programming(struct student arr[], int n) {
    qsort(arr, n, sizeof(struct student), compare_by_math_then_programming);
}

void sort_students_by_age_and_name_desc(struct student arr[], int n) {
    qsort(arr, n, sizeof(struct student), compare_by_age_then_name_desc);
}

void radix_sort_bit(unsigned *arr, int n, int bit, int start, int end) {
    if (start >= end || bit < 0) {
        return;
    }
    
    int left = start;
    int right = end;
    
    while (left < right) {
        while (left < right && ((arr[left] >> bit) & 1) == 0) {
            left++;
        }
        while (left < right && ((arr[right] >> bit) & 1) == 1) {
            right--;
        }
        if (left < right) {
            unsigned temp = arr[left];
            arr[left] = arr[right];
            arr[right] = temp;
        }
    }
    
    int pivot = ((arr[left] >> bit) & 1) ? left : left + 1;
    
    radix_sort_bit(arr, n, bit - 1, start, pivot - 1);
    radix_sort_bit(arr, n, bit - 1, pivot, end);
}

void radix_sort(unsigned *arr, int n) {
    if (n <= 1) {
        return;
    }
    radix_sort_bit(arr, n, 31, 0, n - 1);
}