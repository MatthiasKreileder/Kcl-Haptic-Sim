/*
 * shared-memory-handler.cc
 *
 *  Created on: 18 Aug 2016
 *      Author: matthias
 */
// idea and code based on: 
// http://web.archive.org/web/20150524005553/http://advancedlinuxprogramming.com/alp-folder/alp-ch05-ipc.pdf
// and
// http://stackoverflow.com/questions/8359322/how-to-share-semaphores-between-processes-using-shared-memory
// and
// http://www.csc.villanova.edu/~mdamian/threads/posixsem.html

#include "shared-memory-handler.h"

namespace ns3 {

SharedMemoryHandler::SharedMemoryHandler() {
	sem_unlink("theCrazySemaphore");

	    sem_t *sem = sem_open("theCrazySemaphore", O_CREAT, 0664, 1);

	    char c;
	    int shmid;
	    key_t key;
	    char *shm, *s;

	    /*
	     * We'll name our shared memory segment
	     * "5678".
	     */
	    key = 5678;

	    /*
	     * Create the segment.
	     */
	    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
	        perror("shmget");
	        exit(1);
	    }

	    /*
	     * Now we attach the segment to our data space.
	     */
	    if ((shm = (char*) shmat(shmid, NULL, 0)) == (char *) -1) {
	        perror("shmat");
	        exit(1);
	    }


	    /*
	     * Init memory
	     */
	    initMem(sem, shm);

	    m_semaphore = sem;
	    m_startOfSharedMemoryPage = shm;
}

SharedMemoryHandler::~SharedMemoryHandler() {
	// TODO Auto-generated destructor stub
}

void SharedMemoryHandler::KclSafeRead(std::string& msg){
    char* s = m_startOfSharedMemoryPage + 50;

    char buf[51];
    buf[50] = '\0';
    sem_wait(m_semaphore);

    int bufferIndex = 0;
    for(int i = 50; i < 100; i++){
        buf[bufferIndex] = *s;
        s++;
        bufferIndex++;
    }

    sem_post(m_semaphore);

    std::string suchAString(buf);
    msg = suchAString;
}
void SharedMemoryHandler::KclSafeWrite(std::string msg){

    size_t msg_len = msg.size();

    if(msg_len > 50)
        return;

    /*
     * Make a copy!!! of the start loaction
     * We will use it to iterate over the shared memory
     */
    char* it = m_startOfSharedMemoryPage;

    sem_wait(m_semaphore);

    for(int i = 0; i < 50; i++){
        *it = '#';
        it++;
    }

    /*
     * Now it's time to write the actual message
     */
    it = m_startOfSharedMemoryPage;
    for(size_t i = 0; i < msg_len; i++){
        *it = msg[i];
        it++;
    }

    sem_post(m_semaphore);
}

void SharedMemoryHandler::initMem(sem_t* sem, char* start){
    char* s = start;

    sem_wait(sem);

    for(int i = 0; i < 100; i++){
        *s = '#';
        s++;
    }

    sem_post(sem);
}

} /* namespace ns3 */
