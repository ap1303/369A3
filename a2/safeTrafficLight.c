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
pthread_mutex_t carsInsideLock;
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

	initMutex(&carsInsideLock);

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

	destroyMutex(&carsInsideLock);
}

void runTrafficLightCar(Car* car, SafeTrafficLight* light) {

	enterTrafficLightWrapper(&enter_update_light, &enter_condition, car, &light->base);

	printf("%d released from initial blocked entry\n", car->index);

	checkForAction(&act_mutex, &act_condition, &enter_condition, car, &light->base);

	printf("%d released from initial blocked action\n", car->index);

	lock(&carsInsideLock);

	int blockedInside = 0;

	printf("%d holds the update lock now\n", car->index);

	printf("checking entry inside update section for %d\n", car->index);

  while (((car->position == EAST || car->position == WEST) && getLightState(&light->base) != EAST_WEST) ||
  ((car->position == NORTH || car->position == SOUTH) && getLightState(&light->base) != NORTH_SOUTH)) {
		blockedInside = 1;
		unlock(&carsInsideLock);
		printf("%d blocked for entry inside update section, %d\n", car->index, car->position);
		pthread_cond_wait(&enter_condition, &enter_update_light);
	}

  printf("%d not blocked for entry inside update section, blockedInside is: %d\n", car->index, blockedInside);

  if(blockedInside == 1) {
	  lock(&carsInsideLock);
		printf("lock reacquired after blocking inside for %d\n", car->index);
	}

	EntryLane* lane = getLaneLight(car, &light->base);
  enterLane(car, lane);

  entered_light[enter_count_light] = car->index;
  enter_count_light += 1;

  enterTrafficLight(car, &light->base);

	printf("after %d entered, light state turned to: %d\n", car->index, getLightState(&light->base));

	actTrafficLight(car, &light->base, NULL, NULL, NULL);

	printf("after %d acted, light state turned to: %d\n", car->index, getLightState(&light->base));

  if (car->action == 0) {
		pthread_cond_broadcast(&act_condition);
		printf("release action cond variable on %d\n", car->index);
	}

	if (getLightState(&light->base) != RED) {
		pthread_cond_broadcast(&enter_condition);
		printf("release enter cond variable on %d\n", car->index);
	}

  printf("%d is about to release the update lock now\n", car->index);

  unlock(&carsInsideLock);

	exitTrafficLight(&exit_lock_light, &cleared_for_exit_light, lane, car, &exit_count_light, entered_light);








}
