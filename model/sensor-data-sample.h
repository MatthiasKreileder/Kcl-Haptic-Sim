/*
 * sensor-data-sample.h
 *
 *  Created on: 9 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTICS_MODEL_SENSOR_DATA_SAMPLE_H_
#define SRC_KCL_HAPTICS_MODEL_SENSOR_DATA_SAMPLE_H_

#include <string>
#include <vector>

namespace ns3 {

/**
 * \brief Can hold one sensor data sample (x,y,z)
 */
class SensorDataSample {
public:
	/**
	 * \brief Provided a string of the following format: "x, y, z" where x, y and z are floarting point
	 * numbers this constructor parses that line to populate one instance of SensorDataSample
	 *
	 * The values x, y and z can be singed or unsigned and differ in the number of digits before and after
	 * the floating point, however the values must be separated by a comma and a space.
	 *
	 * \param SensorDataSample e.g. string sample = "-0.18180, -0.20503, -0.98147"
	 */
	SensorDataSample(std::string sensorDataSample);
	virtual ~SensorDataSample();

	/**
	 * \return A copy of the sensor data stored as a vector of double values
	 */
	std::vector<double> getSensorDataVector();

	/**
	 * \return A copy of the sensor data stored as a string
	 */
	std::string getSensorDataString ();

private:
	std::vector<double> m_sensorData;

	std::string m_sensorDataSample;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTICS_MODEL_SENSOR_DATA_SAMPLE_H_ */
