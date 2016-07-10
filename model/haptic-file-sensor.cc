
#include "haptic-file-sensor.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticFileSensor");

HapticFileSensor::HapticFileSensor(std::string fileName, std::string type){

	  std::string orbits ("-0.18180");
	  std::string::size_type sz;     // alias of size_t

	  double earth = stod (orbits);
	  //double moon = std::stod (orbits.substr(sz));
	  NS_LOG(LOG_LEVEL_INFO,earth);
}

}
