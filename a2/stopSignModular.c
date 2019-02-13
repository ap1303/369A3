#include "stopSignModular.h"

EntryLane* enterStopSign(Car* car, StopSign* sign, pthread_mutex_t* enter_update, int* entered, int* enter_count)  {
  pthread_mutex_lock(enter_update);

  EntryLane* lane = getLane(car, sign);

  enterLane(car, lane);

  entered[*enter_count] = car->index;
  *enter_count += 1;

  pthread_mutex_unlock(enter_update);

  return lane;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void act(Car* car, pthread_mutex_t* quad_locks, StopSign* sign) {
  int required[QUADRANT_COUNT];
  int count = getStopSignRequiredQuadrants(car, required);

  qsort(required, count, sizeof(int), cmpfunc);

  for(int i = 0; i < count; i++) {
    int quad = required[i];

    pthread_mutex_lock(quad_locks + quad);
  }

  goThroughStopSign(car, sign);

	for(int i = 0; i < count; i++) {
		int quad = required[i];

		pthread_mutex_unlock(&(quad_locks[quad]));
	}
}

void exitStopSign(Car* car, EntryLane* lane, pthread_mutex_t* exit_lock, pthread_cond_t* cleared_for_exit, int* exit_count, int* entered) {
  pthread_mutex_lock(exit_lock);

  while((*exit_count == 0 && car->index != entered[0]) || (*exit_count != 0 && entered[*exit_count] != car->index)) {
    pthread_cond_wait(cleared_for_exit, exit_lock);
  }

  pthread_mutex_unlock(exit_lock);

  exitIntersection(car, lane);

  pthread_mutex_lock(exit_lock);

  *exit_count += 1;
  pthread_cond_broadcast(cleared_for_exit);

  pthread_mutex_unlock(exit_lock);
}
