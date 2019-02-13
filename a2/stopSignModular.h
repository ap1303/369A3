#include "stopSign.h"
#include "car.h"
#include "intersection.h"
#include "syncDestroy.h"
#include "common.h"

EntryLane* enterStopSign(Car* car, StopSign* sign, pthread_mutex_t* enter_update, int* entered, int* enter_count);

void act(Car* car, pthread_mutex_t* quad_locks, StopSign* sign);

void exitStopSign(Car* car, EntryLane* lane, pthread_mutex_t* exit_lock, pthread_cond_t* cleared_for_exit, int* exit_count, int* entered);
