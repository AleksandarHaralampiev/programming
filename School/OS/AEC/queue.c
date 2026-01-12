#include <stdbool.h>
#include <stdlib.h>

struct queue_item {
    int data;
    struct queue_item *next;
};

struct queue {
    struct queue_item *head;
    struct queue_item *tail;
};

void queue_init(struct queue *q) {
    q->head = NULL;
    q->tail = NULL;
}

bool queue_is_empty(struct queue *q) {
    return q->head == NULL;
}

void queue_enqueue(struct queue *q, int value) {
    struct queue_item *new_item = malloc(sizeof(struct queue_item));
    if (new_item == NULL) {
        return;
    }
    
    new_item->data = value;
    new_item->next = NULL;
    
    if (queue_is_empty(q)) {
        q->head = new_item;
        q->tail = new_item;
    } else {
        q->tail->next = new_item;
        q->tail = new_item;
    }
}

int queue_dequeue(struct queue *q) {
    if (queue_is_empty(q)) {
        return -1;
    }
    
    struct queue_item *temp = q->head;
    int value = temp->data;
    
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    free(temp);
    return value;
}

int queue_peek(struct queue *q) {
    if (queue_is_empty(q)) {
        return -1;
    }
    return q->head->data;
}

void queue_clear(struct queue *q) {
    while (!queue_is_empty(q)) {
        queue_dequeue(q);
    }
}