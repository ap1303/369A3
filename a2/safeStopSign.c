/**
* CSC369 Assignment 2
*
* This is the source/implementation file for your safe stop sign
* submission code.
*/
#include "safeStopSign.h"
#include "syncDestroy.h"
#include "stopSignModular.h"

pthread_mutex_t quad_locks[4];
pthread_mutex_t exit_lock;
pthread_cond_t cleared_for_exit;
pthread_mutex_t enter_update;
pthread_mutex_t carsInsideLock;
int entered[1000];
int enter_count;
int exit_count;

void initSafeStopSign(SafeStopSign* sign, int count) {
	initStopSign(&sign->base, count);

	for(int w = 0; w < 4; w++) {
		initMutex(&(quad_locks[w]));
	}

  initMutex(&exit_lock);

	for(int z = 0; z < 1000; z++) {
		entered[z] = 0;
	}

	initConditionVariable(&cleared_for_exit);

	initMutex(&enter_update);

  enter_count = 0;
	exit_count = 0;
}

void destroySafeStopSign(SafeStopSign* sign) {
	destroyStopSign(&sign->base);

	for(int w = 0; w < 4; w++) {
		destroyMutex(&(quad_locks[w]));
	}

	destroyMutex(&exit_lock);

	destroyMutex(&enter_update);

	destroyConditionVariable(&cleared_for_exit);
}

void runStopSignCar(Car* car, SafeStopSign* sign) {

	EntryLane* lane = enterStopSign(car, &sign->base, &enter_update, entered, &enter_count);

	act(car, quad_locks, &sign->base);

	exitStopSign(car, lane, &exit_lock, &cleared_for_exit, &exit_count, entered);
}
