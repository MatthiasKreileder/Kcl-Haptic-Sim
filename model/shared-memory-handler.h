

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_SHARED_MEMORY_HANDLER_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_SHARED_MEMORY_HANDLER_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <cstdlib>
#include <zconf.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>

#define SHMSZ     27

namespace ns3 {

class SharedMemoryHandler {
public:
	SharedMemoryHandler();
	virtual ~SharedMemoryHandler();
    void KclSafeRead(std::string& msg);
    void KclSafeWrite(std::string msg);
private:
    void initMem(sem_t* sem, char* start);
    char* m_startOfSharedMemoryPage;
    sem_t* m_semaphore;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_SHARED_MEMORY_HANDLER_H_ */
