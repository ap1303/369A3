/**
* CSC369 Assignment 2
*
* This is the source/implementation file for your safe traffic light
* submission code.
*/
#include "safeTrafficLight.h"
#include "trafficLightModular.h"

pthread_mutex_t enter_update_light;
pthread_cond_t enter_condition;
pthread_mutex_t act_mutex;
pthread_cond_t act_condition;
pthread_mutex_t exit_lock_light;
pthread_cond_t cleared_for_exit_light;
int entered_light[1000];
int enter_count_light;
int exit_count_light;

void initSafeTrafficLight(SafeTrafficLight* light, int horizontal, int vertical) {
	initTrafficLight(&light->base, horizontal, vertical);

	initMutex(&exit_lock_light);
	initConditionVariable(&cleared_for_exit_light);

	initMutex(&act_mutex);
	initConditionVariable(&act_condition);

	initMutex(&enter_update_light);
	initConditionVariable(&enter_condition);

	for(int z = 0; z < 1000; z++) {
		entered_light[z] = 0;
	}

	enter_count_light = 0;
	exit_count_light = 0;
}

void destroySafeTrafficLight(SafeTrafficLight* light) {
	destroyTrafficLight(&light->base);

	destroyMutex(&exit_lock_light);
	destroyConditionVariable(&cleared_for_exit_light);

	destroyMutex(&act_mutex);
	destroyConditionVariable(&act_condition);

	destroyMutex(&enter_update_light);
	destroyConditionVariable(&enter_condition);
}

void runTrafficLightCar(Car* car, SafeTrafficLight* light) {

	EntryLane* lane = enterTrafficLightWrapper(&enter_update_light, &enter_condition, car, &light->base, entered_light, &enter_count_light);

	actTrafficLightWrapper(&act_mutex, &act_condition, car, &light->base);

	lock(&enter_update_light);

	actTrafficLight(car, &light->base, NULL, NULL, NULL);

  int newLightState = getLightState(&light->base);

  if (newLightState != RED) {
		//printf("release enter cond variable on %d inside act, light state turned to: %d\n", car->index, newLightState);
		pthread_cond_broadcast(&enter_condition);
	}

	if (car->action == 0) {
		pthread_cond_broadcast(&act_condition);
		//printf("release action cond variable on %d\n", car->index);
	}

  unlock(&enter_update_light);

  exitTrafficLight(&exit_lock_light, &cleared_for_exit_light, lane, car, &exit_count_light, entered_light);

}
