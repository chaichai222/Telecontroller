#include "cmsis_os.h"
#include "CMSIS_OS_SUPPLEMENT.h"
/**
* @brief Create and Initialize a Semaphore object used for managing resources
* @param semaphore_def semaphore definition referenced with \ref osSemaphore.
* @param count         number of available resources.
* @retval  semaphore ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osSemaphoreCreate shall be consistent in every CMSIS-RTOS.
*/
osSemaphoreId osUserDefinedSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count)
{
  (void) semaphore_def;
  osSemaphoreId sema;
  
  if (count == 1) {
    vSemaphoreCreateBinary(sema);
    return sema;
  }

#if (configUSE_COUNTING_SEMAPHORES == 1 )	
  return xSemaphoreCreateCounting(count, count);
#else
  return NULL;
#endif
}
