
#include <iostream>
#include <string>
#include <queue>

#include "ns3/core-module.h"

#include "ns3/sensor-data-sample.h"

namespace ns3 {

/** \brief Reads recorded sensor data from a file - can handle position and force data
 *
 */
class HapticFileSensor
{
public:

	/**
	 * Indicates what type of data the file contains
	 */
	enum FileType {
		POSITION,
		FORCEFEEBACK
	};

	typedef enum FileType SensorFileType;

	/**
	 * \brief Reads the file and stores the data internally
	 * \param fileName The name of the file which contains the recorded data
	 * \param type options are: "position" or "force-feedback"
	 */
	HapticFileSensor(std::string fileName, HapticFileSensor::SensorFileType type);

	/**
	 * \returns A reference to the sensor data
	 */
	std::queue<SensorDataSample>& GetData();

private:
	void ReadPositionData(std::string fileName);

	std::queue<SensorDataSample> m_posData;
};

}
