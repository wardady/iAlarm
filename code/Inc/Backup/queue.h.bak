//
// Created by hermann on 14.01.19.
//

#ifndef QUEUE_QUEUE_H
#define QUEUE_QUEUE_H
#include <stdint.h>

typedef struct {
    uint8_t min;
    uint8_t hour;
    uint8_t dotw;
    uint8_t repeat;
} Alarm;

typedef struct {
    Alarm queue[250];
    uint8_t size;
} Queue;

int insert(Queue *alarm_queue, uint8_t min, uint8_t hour, uint8_t day, uint8_t rep);
Alarm next(Queue *alarm_queue);
void clear_queue(Queue *alarm_queue);

#endif //QUEUE_QUEUE_H
