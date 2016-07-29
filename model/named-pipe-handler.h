
#ifndef SRC_KCL_HAPTIC_SIM_MODEL_NAMED_PIPE_HANDLER_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_NAMED_PIPE_HANDLER_H_

#include <iostream>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace ns3{

/*
 * \brief Handle the communication via the named pipe passed to its constructor
 *
 * Creates and opens the named pipe
 * Handle reading and writing to the pipe: WILL ONLY ACCEPT MESSAGES WITH A MAXIMUM LENGT OF 100 bytes (because it is intended so send small
 */
class NamedPipeHandler{
public:
    NamedPipeHandler(std::string& ns3ToChai3D, std::string& chai3DToNs3, bool isInit);

    /*
     * \brief write a string of up to 100 bytes to the named pipe
     *
     * Does nothing if the string contains more than 100 bytes
     *
     * \param s the string that will be written to the pipe
     */
    void SafeWrite(std::string s);

    /*
     * \brief Will read a message of up to 100 bytes from the pipe
     *
     * Blocks until one message arrives.
     *
     * \param s puts the string that was read from the named pipe into s
     */
    void SafeRead(std::string& s);

private:
    int m_rx_pipe;  // Pipe to receive from peer
    int m_tx_pipe;  // Pipe to send to peer
};

} // ns3

#endif //SRC_KCL_HAPTIC_SIM_MODEL_NAMED_PIPE_HANDLER_H_
