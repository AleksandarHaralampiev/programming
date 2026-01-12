#include <string.h>

int balanced(const char *str) {
    if (str == NULL) {
        return 0;
    }
    
    char stack[1000];
    int top = -1;
    
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        
        if (c == '(' || c == '[' || c == '{') {
            stack[++top] = c;
        } else if (c == ')' || c == ']' || c == '}') {
            if (top == -1) {
                return 0;
            }
            
            char open = stack[top--];
            
            if ((c == ')' && open != '(') ||
                (c == ']' && open != '[') ||
                (c == '}' && open != '{')) {
                return 0;
            }
        }
    }
    
    return top == -1;
}