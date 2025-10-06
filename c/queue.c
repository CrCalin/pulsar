#ifndef QUEUE_C_INCLUDED
#define QUEUE_C_INCLUDED
#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
void init_pq(pq* q, int max_buffer_size) {
    q->buffer = malloc(max_buffer_size * sizeof(position));
    q->front = q->buffer;
    q->back = q->buffer;
}
void free_pq(pq* q) {
    free(q->buffer);
}

#include <stdbool.h>
// the implementation is fairily unsafe, it assumes you know what you're doing when calling the functions
bool is_pq_empty(pq* q) {
    return (q->back - q->front) <= 0;
}

void pq_push(pq* q, position p) {
    *(q->back) = p;
    q->back++;
}
position pq_pop(pq* q) {
    position temp = *(q->front);
    q->front++;
    return temp;
}
position pq_get_back(pq* q) {
    return *(q->back - 1); // remember, [front, back)
}
position pq_get_front(pq* q) {
    return *(q->front);
}


#endif // QUEUE_C_INCLUDED
