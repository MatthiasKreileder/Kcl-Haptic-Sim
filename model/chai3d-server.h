/*
 * chai3d-server.h
 *
 *  Created on: 25 Jul 2016
 *      Author: matthias
 */

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

namespace ns3 {

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
  virtual void StopApplication (void);
  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  std::string m_chai3dWrapperProg;
  uint16_t m_port; //!< Port on which we listen for incoming packets.
  FILE* m_ns3ToChai3dServerStream;	//!< Write end of the pipe which connects ns-3 and the Chai3d-Wrapper
  Ptr<Socket> m_socket; //!< IPv4 Socket
  Address m_local; //!< local multicast address

};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_CHAI3D_SERVER_H_ */
