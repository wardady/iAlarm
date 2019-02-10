#include <stdio.h>

#include "queue.h"
#include "rtc.h"

extern uint8_t aTxBuffer[18];

//insert new alarm into queue and rearrange it if needed
int insert(queue *alarm_queue, uint8_t min, uint8_t hour, uint8_t day, uint8_t rep)
{
    // queue is full or new alarm has no repeats
    if ((alarm_queue->size == 250) || (rep == 0))
        return -1;

    // if queue is empty
    min %= 60; hour %= 24; day %= 7;
	if (alarm_queue->size == 0) {
		alarm inserted = {min, hour, day, rep};
		alarm_queue->list[0] = inserted;
		alarm_queue->size++;
		return 0;
	}

    // get current time to sort alarms
    get_buffer();
    uint32_t current_time = toDEC(aTxBuffer[1]) + toDEC(aTxBuffer[2]) * 60 + toDEC(aTxBuffer[3]) * 60 * 24;

    // find place to insert new alarm
    uint32_t to_compare = (uint32_t) (min + hour * 60 + day * 60 * 24);
    for (int32_t i = alarm_queue->size - 1; i > -1; i--) {
        uint32_t processing_alarm = (uint32_t) (alarm_queue->list[i].min + alarm_queue->list[i].hour * 60 + alarm_queue->list[i].dotw * 60 * 24);

        if ((to_compare > processing_alarm && current_time <= to_compare && processing_alarm > current_time) ||
            (current_time > to_compare && processing_alarm > current_time) ||
            (current_time > to_compare && processing_alarm < current_time && to_compare > processing_alarm)) {
            alarm temp1 = alarm_queue->list[i + 1], temp2;

            for (int32_t j = i + 2; j <= alarm_queue->size; ++j) {
                temp2 = alarm_queue->list[j];
                alarm_queue->list[j] = temp1;
                temp1 = temp2;
            }

            // insert alarm at the end
            alarm inserted = {min, hour, day, rep};
            alarm_queue->list[i + 1] = inserted;
            alarm_queue->size++;
            return 0;

        // queue already has such an alarm
        } else if (to_compare == processing_alarm)
            return -2;
    }

    // move all alarms on 1 position right
    alarm temp1 = alarm_queue->list[0], temp2;
    for (uint16_t j = 1; j < alarm_queue->size + 1; ++j) {
        temp2 = alarm_queue->list[j];
        alarm_queue->list[j] = temp1;
        temp1 = temp2;
    }
    // insert alarm
    alarm inserted = {min, hour, day, rep};
    alarm_queue->list[0] = inserted;
    alarm_queue->size++;
	return 0;
}

// return next to set alarm
alarm next(queue *alarm_queue)
{
    // return zero alarm if there are no alarms in the queue
    // (if there is one it is already set)
    if (alarm_queue->size <= 1) {
        alarm_queue->size = 0;
        alarm a = {0, 0, 0, 0};
        return a;
    }
    alarm temp = alarm_queue->list[0];
    for (uint16_t i = 0; i < alarm_queue->size; ++i) {
        alarm_queue->list[i] = alarm_queue->list[i + 1];
    }
    // if alarm has more repeats insert in in the queue again with one less repeat
    if (temp.repeat) {
        insert(alarm_queue, temp.min, temp.hour, temp.dotw, temp.repeat-1);
    } else alarm_queue->size--;

    alarm next = alarm_queue->list[0];
    return next;
}

// clear queue
void clear_queue(queue *alarm_queue)
{
    alarm_queue->size = 0;
}
