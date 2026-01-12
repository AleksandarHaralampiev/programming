#include <stdbool.h>

#define STACK_SIZE 1000

struct stack {
    int data[STACK_SIZE];
    int top;
};

void stack_init(struct stack *s) {
    s->top = -1;
}

bool stack_is_empty(struct stack *s) {
    return s->top == -1;
}

bool stack_is_full(struct stack *s) {
    return s->top == STACK_SIZE - 1;
}

void stack_push(struct stack *s, int value) {
    if (!stack_is_full(s)) {
        s->data[++s->top] = value;
    }
}

int stack_pop(struct stack *s) {
    if (!stack_is_empty(s)) {
        return s->data[s->top--];
    }
    return -1;
}

int stack_peek(struct stack *s) {
    if (!stack_is_empty(s)) {
        return s->data[s->top];
    }
    return -1;
}

int stack_size(struct stack *s) {
    return s->top + 1;
}