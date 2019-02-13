#include "trafficLightModular.h"

EntryLane* enterTrafficLightWrapper(pthread_mutex_t* enter_update_light, pthread_cond_t* enter_condition, Car* car, TrafficLight* light, int* entered_light, int* enter_count_light) {
  lock(enter_update_light);

  while (((car->position == EAST || car->position == WEST) && getLightState(light) != EAST_WEST) ||
  ((car->position == NORTH || car->position == SOUTH) && getLightState(light) != NORTH_SOUTH)) {
     pthread_cond_wait(enter_condition, enter_update_light);
  }

  EntryLane* lane = getLaneLight(car, light);
  enterLane(car, lane);

  entered_light[*enter_count_light] = car->index;
  *enter_count_light += 1;

  printf("%d just entered. Up until now, entered cars: %d with light state: %d with car position %d, car action %d and thus lane %d\n", car->index, *enter_count_light, getLightState(light), car->position, car->action, getLaneIndexLight(car));

  enterTrafficLight(car, light);

  unlock(enter_update_light);

  return lane;
}

void actTrafficLightWrapper(pthread_mutex_t* act_mutex, pthread_cond_t* act_condition, Car* car, TrafficLight* light) {
  lock(act_mutex);

	while(car->action == 2 && getStraightCount(light, (int) getOppositePosition(car->position)) != 0) {
		//printf("locking %d for action because opposite (%d) straight count is: %d\n", car->index, getOppositePosition(car->position), (light->base).straightCounts[getOppositePosition(car->position)]);
		pthread_cond_wait(act_condition, act_mutex);
	}

	actTrafficLight(car, light, NULL, NULL, NULL);

	unlock(act_mutex);

	lock(act_mutex);

  if (car->action == 0) {
		pthread_cond_broadcast(act_condition);
		//printf("release action cond variable on %d\n", car->index);
	}

  unlock(act_mutex);
}

void exitTrafficLight(pthread_mutex_t* exit_lock_light, pthread_cond_t* cleared_for_exit_light, EntryLane* lane, Car* car, int* exit_count_light, int* entered_light) {
  lock(exit_lock_light);

	while((*exit_count_light == 0 && car->index != entered_light[0]) || (*exit_count_light != 0 && entered_light[*exit_count_light] != car->index)) {
		pthread_cond_wait(cleared_for_exit_light, exit_lock_light);
	}

	unlock(exit_lock_light);

	//printf("%d exiting\n", car->index);

	exitIntersection(car, lane);

	lock(exit_lock_light);

  *exit_count_light += 1;
	pthread_cond_broadcast(cleared_for_exit_light);

	unlock(exit_lock_light);
}
