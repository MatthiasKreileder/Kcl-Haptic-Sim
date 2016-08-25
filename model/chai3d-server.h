/* * chai3d-server.h
 *
 *  Created on: 25 Jul 2016
 *      Author: matthias
 */
//
// Code and idea bases on https://www.nsnam.org/doxygen/udp-echo-server_8cc_source.html
//
#ifndef SRC_KCL_HAPTIC_SIM_MODEL_CHAI3D_SERVER_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_CHAI3D_SERVER_H_

#include "ns3/application.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/socket.h"
#include "ns3/address.h"
#include "ns3/address-utils.h"

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

//#include "boost/thread.hpp"
//#include <boost/asio.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/bind.hpp>

#include "named-pipe-handler.h"
#include "shared-memory-handler.h"

namespace ns3 {

//static boost::mutex namedPipes_lock;

class Chai3dServer : public Application {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  Chai3dServer();
  virtual ~Chai3dServer();

  /**
   * \brief Set's up communication with the program m_chai3dWrapperProg
   *
   * Creates a child process and established a pipe between the parent and the child. This pipe is used to
   * read and write the haptic information back and forth between ns-3 and the program which interacts with
   * Chai3d.
   */
  void Setup ();

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);

  /**
   * \brief Shuts down the application
   *
   * It removes the two named pipes that are created during the installation process of this App on the node.
   * If the simulator does not execute this function (e.g. the user aborts with 'Ctrl + C') the simulator before this function
   * gets called you need to manually remove those files from your file system.
   */
  virtual void StopApplication (void);
  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  std::string m_chai3dWrapperProg; //!< The binary (+path name) of the CHAI3D program ns-3 should talk to

  std::string m_namedPipesFolder; //!< The folder where the two names pipes (files) will be created to enable the IPC with CHAI3D.

  std::string m_fullNameNamedPipeNs3ToChaid3D;
  std::string m_fullNameNamedPipeChai3dToNs3;

  uint16_t m_port; //!< Port on which we listen for incoming packets.

  Ptr<Socket> m_socket; //!< IPv4 Socket
  Address m_local; //!< local multicast address

  Address m_phantomAgentAddress; //!< Remote peer address
  uint16_t m_phantomAgentPort; //!< Remote peer port

  NamedPipeHandler* m_nph;
  SharedMemoryHandler* m_sharedMemHandler;

};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_CHAI3D_SERVER_H_ */
