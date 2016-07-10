
#include <iostream>
#include <string>
#include "ns3/core-module.h"

namespace ns3 {

/** \brief Reads recorded sensor data from a file - can handle position and force data
 *
 */
class HapticFileSensor
{
public:
	/**
	 * \brief Reads the file and stores the data internally
	 * \param fileName The name of the file which contains the recorded data
	 * \param type options are: "position" or "force-feedback"
	 */
	HapticFileSensor(std::string fileName, std::string type);
};

}
