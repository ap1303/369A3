#include "trafficLightModular.h"

void enterTrafficLightWrapper(pthread_mutex_t* enter_update_light, pthread_cond_t* enter_condition, Car* car, TrafficLight* light) {
  lock(enter_update_light);

  while (((car->position == EAST || car->position == WEST) && getLightState(light) != EAST_WEST) ||
  ((car->position == NORTH || car->position == SOUTH) && getLightState(light) != NORTH_SOUTH)) {
     printf("%d blocked for entry\n", car->index);
     pthread_cond_wait(enter_condition, enter_update_light);
  }

  unlock(enter_update_light);

}

void checkForAction(pthread_mutex_t* act_mutex, pthread_cond_t* act_condition, pthread_cond_t* enter_condition, Car* car, TrafficLight* light) {
  lock(act_mutex);

	while(car->action == 2 && getStraightCount(light, (int) getOppositePosition(car->position)) != 0) {
		printf("locking %d for action\n", car->index);
		pthread_cond_wait(act_condition, act_mutex);
	}

	unlock(act_mutex);

}

void exitTrafficLight(pthread_mutex_t* exit_lock_light, pthread_cond_t* cleared_for_exit_light, EntryLane* lane, Car* car, int* exit_count_light, int* entered_light) {
  lock(exit_lock_light);

	while((*exit_count_light == 0 && car->index != entered_light[0]) || (*exit_count_light != 0 && entered_light[*exit_count_light] != car->index)) {
		pthread_cond_wait(cleared_for_exit_light, exit_lock_light);
	}

	unlock(exit_lock_light);

	exitIntersection(car, lane);

	lock(exit_lock_light);

  *exit_count_light += 1;
	pthread_cond_broadcast(cleared_for_exit_light);

	unlock(exit_lock_light);
}
