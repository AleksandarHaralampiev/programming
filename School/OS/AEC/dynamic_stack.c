#include <stdbool.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *next;
};

struct stack {
    struct node *top;
};

void stack_init(struct stack *s) {
    s->top = NULL;
}

bool stack_is_empty(struct stack *s) {
    return s->top == NULL;
}

void stack_push(struct stack *s, int value) {
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL) {
        return;
    }
    
    new_node->data = value;
    new_node->next = s->top;
    s->top = new_node;
}

int stack_pop(struct stack *s) {
    if (stack_is_empty(s)) {
        return -1;
    }
    
    struct node *temp = s->top;
    int value = temp->data;
    s->top = temp->next;
    free(temp);
    
    return value;
}

int stack_peek(struct stack *s) {
    if (stack_is_empty(s)) {
        return -1;
    }
    return s->top->data;
}

void stack_clear(struct stack *s) {
    while (!stack_is_empty(s)) {
        stack_pop(s);
    }
}