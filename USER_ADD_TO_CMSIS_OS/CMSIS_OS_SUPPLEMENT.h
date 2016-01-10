#ifndef CMSIS_OS_SUPPLEMENT_H
#define CMSIS_OS_SUPPLEMENT_H
#include "cmsis_os.h"

osSemaphoreId osUserDefinedSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count);

#endif
