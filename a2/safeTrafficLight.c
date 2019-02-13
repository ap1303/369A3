/**
* CSC369 Assignment 2
*
* This is the source/implementation file for your safe traffic light
* submission code.
*/
#include "safeTrafficLight.h"
#include "common.h"
#include "syncDestroy.h"

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

	pthread_mutex_lock(&enter_update_light);

	while (((car->position == EAST || car->position == WEST) && getLightState(&light->base) != EAST_WEST) ||
	((car->position == NORTH || car->position == SOUTH) && getLightState(&light->base) != NORTH_SOUTH)) {
     pthread_cond_wait(&enter_condition, &enter_update_light);
	}

	EntryLane* lane = getLaneLight(car, &light->base);
	enterLane(car, lane);

	entered_light[enter_count_light] = car->index;
	enter_count_light += 1;

	printf("%d just entered. Up until now, entered cars: %d with light state: %d with car position %d, car action %d and thus lane %d\n", car->index, enter_count_light, getLightState(&light->base), car->position, car->action, getLaneIndexLight(car));

	enterTrafficLight(car, &light->base);

	pthread_mutex_unlock(&enter_update_light);

	printf("after %d entered, light state turned to: %d\n", car->index, getLightState(&light->base));

	pthread_mutex_lock(&act_mutex);

	while(car->action == 2 && getStraightCount(&light->base, (int) getOppositePosition(car->position)) != 0) {
		//printf("locking %d for action because opposite (%d) straight count is: %d\n", car->index, getOppositePosition(car->position), (light->base).straightCounts[getOppositePosition(car->position)]);
		pthread_cond_wait(&act_condition, &act_mutex);
	}

	actTrafficLight(car, &light->base, NULL, NULL, NULL);

	pthread_mutex_unlock(&act_mutex);

	//actTrafficLight(car, &light->base, NULL, NULL, NULL);

  if (getLightState(&light->base) != RED) {
		//printf("release enter cond variable on %d\n", car->index);
		pthread_cond_broadcast(&enter_condition);
	}

	pthread_mutex_lock(&act_mutex);

  if (car->action == 0) {
		pthread_cond_broadcast(&act_condition);
		//printf("release action cond variable on %d\n", car->index);
	}

	pthread_mutex_unlock(&act_mutex);

	pthread_mutex_lock(&exit_lock_light);

	while((exit_count_light == 0 && car->index != entered_light[0]) || (exit_count_light != 0 && entered_light[exit_count_light] != car->index)) {
		pthread_cond_wait(&(cleared_for_exit_light), &exit_lock_light);
	}

	pthread_mutex_unlock(&exit_lock_light);

	//printf("%d exiting\n", car->index);

	exitIntersection(car, lane);

	pthread_mutex_lock(&exit_lock_light);

  exit_count_light += 1;
	pthread_cond_broadcast(&cleared_for_exit_light);

	pthread_mutex_unlock(&exit_lock_light);



}
