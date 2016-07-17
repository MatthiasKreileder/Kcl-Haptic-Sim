
#include <iostream>
#include <string>
#include <deque>

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
		FORCEFEEDBACK
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
	std::deque<SensorDataSample>& GetData();

	/**
	 * Stores the next (in terms of the sequence they occured in the provided file)
	 * SensorDataSample object in sds.
	 *
	 * \param sds The object where the next SensorDataSample in the internal deque will be stored
	 *
	 * \return true in case of success, false otherwise (e.g. deque empty)
	 */
	bool GetNextSensorDataSample(SensorDataSample& sds);

private:
	void ReadPositionData(std::string fileName);

	void ReadForceFeedbackData(std::string fileName);

	std::deque<SensorDataSample> m_posData;

	std::deque<SensorDataSample> m_forceData;

	enum FileType m_fileType;
};

}
