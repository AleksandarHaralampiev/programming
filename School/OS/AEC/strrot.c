#include <stdio.h>
#include <string.h>

void strrot(char str[]) {
    if (str == NULL) {
        return;
    }
    
    int len = strlen(str);
    
    // Ако низът е празен или има само 1 символ, не правим нищо
    if (len <= 1) {
        return;
    }
    
    // Разменяме символите от двата края към средата
    int left = 0;
    int right = len - 1;
    
    while (left < right) {
        // Размяна на символите
        char temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        
        // Преместваме указателите
        left++;
        right--;
    }
}