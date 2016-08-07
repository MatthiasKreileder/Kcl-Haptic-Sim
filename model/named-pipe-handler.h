
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
 * Handle reading and writing to the pipe: WILL ONLY ACCEPT MESSAGES WITH A MAXIMUM LENGT OF 100 bytes (because it is
 * intended so send small messages that just contains a few floating point numbers).
 */
class NamedPipeHandler{
public:

	/**
	 * \brief Sets up the inter process communication via two named pipes
	 *
	 * \param ns3ToChai3D The name of the named pipe that will be used by NS-3 to send messages to CHAI3D
	 * \param chai3DToNs3 The name of the named pipe that will be used by NS-3 to receive messages from CHAI3D
	 * \param isInit Set to 'true' if this process should created the named pipes
	 * 				 Set to 'false' if this process should just open existing named pipes but not create them - please note:
	 * 				 in the current code [August 2016] NS-3 is the parent process and CHAI3D is the child process and NS-3
	 * 				 is meant to create the named pipes and pass the names as program arguments to the child which will just
	 * 				 open (but not create) the pipes. Unless you have a good reason please invoke the NamedPipeHandler with
	 * 				 the \param isInit set to 'true' on the NS-3 side and set to 'false' on the CHAI3D side.
	 */
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
