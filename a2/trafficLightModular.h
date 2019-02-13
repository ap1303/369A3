#include "trafficLight.h"
#include "car.h"
#include "intersection.h"
#include "syncDestroy.h"
#include "common.h"

void enterTrafficLightWrapper(pthread_mutex_t* enter_update_light, pthread_cond_t* enter_condition, Car* car, TrafficLight* light);

void checkForAction(pthread_mutex_t* act_mutex, pthread_cond_t* act_condition, pthread_cond_t* enter_condition, Car* car, TrafficLight* light);

void exitTrafficLight(pthread_mutex_t* exit_lock_light, pthread_cond_t* cleared_for_exit_light, EntryLane* lane, Car* car, int* exit_count_light, int* entered_light);
