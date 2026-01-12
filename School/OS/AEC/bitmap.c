#include <stdint.h>
#include <stdbool.h>

struct set {
    uint64_t data[1 << 7];
};

void set_insert(struct set *s, unsigned bit) {
    unsigned index = bit / 64;
    unsigned offset = bit % 64;
    s->data[index] |= (1ULL << offset);
}

void set_remove(struct set *s, unsigned bit) {
    unsigned index = bit / 64;
    unsigned offset = bit % 64;
    s->data[index] &= ~(1ULL << offset);
}

bool set_check(struct set *s, unsigned bit) {
    unsigned index = bit / 64;
    unsigned offset = bit % 64;
    return (s->data[index] & (1ULL << offset)) != 0;
}

void set_intersection(struct set *s1, struct set *s2, struct set *out) {
    for (int i = 0; i < (1 << 7); i++) {
        out->data[i] = s1->data[i] & s2->data[i];
    }
}

void set_union(struct set *s1, struct set *s2, struct set *out) {
    for (int i = 0; i < (1 << 7); i++) {
        out->data[i] = s1->data[i] | s2->data[i];
    }
}

void set_difference(struct set *s1, struct set *s2, struct set *out) {
    for (int i = 0; i < (1 << 7); i++) {
        out->data[i] = s1->data[i] & ~s2->data[i];
    }
}