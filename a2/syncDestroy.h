#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

void destroyMutex(pthread_mutex_t* mutex);

void destroyConditionVariable(pthread_cond_t* cond);
