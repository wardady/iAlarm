#include <stdio.h>

#include "queue.h"
#include "rtc.h"

extern uint8_t aTxBuffer[18];

int insert(Queue *alarm_queue, uint8_t min, uint8_t hour, uint8_t day, uint8_t rep)
{
    if ((alarm_queue->size == 250) || (rep == 0))
        return -1;

    min %= 60; hour %= 24; day %= 7;
	if (alarm_queue->size == 0) {
		Alarm inserted = {min, hour, day, rep};
		alarm_queue->queue[0] = inserted;
		alarm_queue->size++;
		return 0;
	}

    get_buffer();
    uint32_t current_time = toDEC(aTxBuffer[1]) + toDEC(aTxBuffer[2]) * 60 + toDEC(aTxBuffer[3]) * 60 * 24;

    uint32_t to_compare = (uint32_t) (min + hour * 60 + day * 60 * 24);
    for (int32_t i = alarm_queue->size - 1; i > -1; i--) {
        uint32_t processing_alarm = (uint32_t) (alarm_queue->queue[i].min + alarm_queue->queue[i].hour * 60 + alarm_queue->queue[i].dotw * 60 * 24);

        if ((to_compare > processing_alarm && current_time <= to_compare && processing_alarm > current_time) ||
            (current_time > to_compare && processing_alarm > current_time) ||
            (current_time > to_compare && processing_alarm < current_time && to_compare > processing_alarm)) {
            Alarm temp1 = alarm_queue->queue[i + 1], temp2;

            for (int32_t j = i + 2; j <= alarm_queue->size; ++j) {
                temp2 = alarm_queue->queue[j];
                alarm_queue->queue[j] = temp1;
                temp1 = temp2;
            }

            Alarm inserted = {min, hour, day, rep};
            alarm_queue->queue[i + 1] = inserted;
            alarm_queue->size++;
            return 0;
        } else if (to_compare == processing_alarm)
            return -2;
    }

    Alarm temp1 = alarm_queue->queue[0], temp2;
    for (uint16_t j = 1; j < alarm_queue->size + 1; ++j) {
        temp2 = alarm_queue->queue[j];
        alarm_queue->queue[j] = temp1;
        temp1 = temp2;
    }
    Alarm inserted = {min, hour, day, rep};
    alarm_queue->queue[0] = inserted;
    alarm_queue->size++;
	return 0;
}

Alarm next(Queue *alarm_queue)
{
    if (alarm_queue->size <= 1) {
        alarm_queue->size = 0;
        Alarm a = {0, 0, 0, 0};
        return a;
    }
    Alarm temp = alarm_queue->queue[0];
    for (uint16_t i = 0; i < alarm_queue->size; ++i) {
        alarm_queue->queue[i] = alarm_queue->queue[i + 1];
    }
    if (temp.repeat) {
        insert(alarm_queue, temp.min, temp.hour, temp.dotw, temp.repeat-1);
    } else alarm_queue->size--;

    Alarm next = alarm_queue->queue[0];
    return next;
}

void clear_queue(Queue *alarm_queue)
{
    alarm_queue->size = 0;
}
