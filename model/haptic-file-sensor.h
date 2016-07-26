
#include <iostream>
#include <string>
#include <deque>

#include "ns3/core-module.h"

#include "ns3/sensor-data-sample.h"

namespace ns3 {

/** \brief Reads recorded sensor data from a file - can handle position, velocity and force data
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
		VELOCITY,
		FORCEFEEDBACK
	};

	typedef enum FileType SensorFileType;

	/**
	 * \brief Reads the file and stores the data internally
	 * \param fileName The name of the file which contains the recorded data
	 * \param type indicates what type of data the file contains
	 */
	HapticFileSensor(std::string fileName, HapticFileSensor::SensorFileType type, double interSampleSeconds);

	/**
	 *
	 * \param type Gets the all SensorDataSample objects for the specified type stored in a deque<T> container
	 *
	 * \returns A reference to the sensor data
	 */
	std::deque<SensorDataSample>& GetData(SensorFileType type);

	/**
	 * Stores the next (in terms of the sequence they occured in the provided file)
	 * SensorDataSample object in sds.
	 *
	 * \param sds The object where the next SensorDataSample in the internal deque will be stored
	 *
	 * \return true in case of success, false otherwise (e.g. deque empty)
	 */
	bool GetNextSensorDataSample(SensorDataSample& sds,SensorFileType type);

private:

	bool GetNextSensorDataSamplePriv(SensorDataSample& sds, std::deque<SensorDataSample>& sdsContainer);

	void ReadSensorDataSamples(std::string fileName, std::deque<SensorDataSample>& sdsContainer);

	std::deque<SensorDataSample> m_posData;

	std::deque<SensorDataSample> m_velocityData;

	std::deque<SensorDataSample> m_forceData;

	/**
	 * The time in seconds between two data samples
	 */
	//double m_interSampleSeconds;
};

}
