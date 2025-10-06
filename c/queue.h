#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

typedef struct position {
    int i, j;
} position;
#define pos_eq(p1, p2) (p1.i == p2.i && p1.j == p2.j)
#define pos_neq(p1, p2) (p1.i != p2.i || p1.j != p2.j)

typedef struct pq {
    position *buffer;
    position* front;
    position* back;
    // [front, back);
} pq;

#include <stdint.h>
#include <stdbool.h>
void init_pq(pq*, int);
void free_pq(pq*);
bool is_pq_empty(pq*);

void pq_push(pq*, position);
position pq_pop(pq*);
position pq_get_back(pq*);
position pq_get_front(pq*);
#endif // QUEUE_H_INCLUDED
