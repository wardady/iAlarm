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
} alarm;

typedef struct {
    alarm list[255];
    uint8_t size;
} queue;

int insert(queue *alarm_queue, uint8_t min, uint8_t hour, uint8_t day, uint8_t rep);
alarm next(queue *alarm_queue);
void clear_queue(queue *alarm_queue);

#endif //QUEUE_QUEUE_H
