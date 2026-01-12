#include <string.h>

void reverse(char s[]) {
    int len = strlen(s);
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

int itoa(int n, char s[]) {
    int i = 0;
    int sign = n;
    
    if (n < 0) {
        n = -n;
    }
    
    do {
        s[i++] = n % 10 + '0';
        n /= 10;
    } while (n > 0);
    
    if (sign < 0) {
        s[i++] = '-';
    }
    
    s[i] = '\0';
    reverse(s);
    
    return 0;
}

int atoi(int *n, char s[]) {
    if (s == NULL || n == NULL) {
        return -1;
    }
    
    int i = 0;
    int sign = 1;
    int result = 0;
    
    if (s[i] == '-') {
        sign = -1;
        i++;
    } else if (s[i] == '+') {
        i++;
    }
    
    if (s[i] < '0' || s[i] > '9') {
        return -1;
    }
    
    while (s[i] >= '0' && s[i] <= '9') {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    
    if (s[i] != '\0') {
        return -1;
    }
    
    *n = sign * result;
    return 0;
}