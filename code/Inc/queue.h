//
// Created by hermann on 14.01.19.
//

#ifndef QUEUE_QUEUE_H
#define QUEUE_QUEUE_H
#include <stdint.h>

// alarm
typedef struct {
    uint8_t min;
    uint8_t hour;
    uint8_t dotw;
    uint8_t repeat;
} alarm;

// alarm queue
typedef struct {
    alarm list[255];
    uint8_t size;
} queue;

// insert new alarm into queue and rearrange it if needed
int insert(queue *alarm_queue, uint8_t min, uint8_t hour, uint8_t day, uint8_t rep);
// return next to set alarm
alarm next(queue *alarm_queue);
// clear queue
void clear_queue(queue *alarm_queue);

#endif //QUEUE_QUEUE_H
