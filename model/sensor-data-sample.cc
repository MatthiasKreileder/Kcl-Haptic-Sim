/*
 * sensor-data-sample.cc
 *
 *  Created on: 9 Jul 2016
 *      Author: matthias
 */

#include "sensor-data-sample.h"

namespace ns3 {

SensorDataSample::SensorDataSample(std::string sensorDataSample) {

	//
	//	We assume three dimensional, comma separated sensor data here:
	//	"-0.18180, -0.20503, -0.98147"

	//
	// x coordinate
	//
	std::size_t firstCommaPos = sensorDataSample.find(",");
	std::string xString = sensorDataSample.substr(0,firstCommaPos - 1);
	double x = stod(xString);
	m_sensorData.push_back(x);

	//
	// y coordinate
	//
	// "-0.20503, -0.98147"
	std::string truncatedPosData = sensorDataSample.substr(firstCommaPos + 1,sensorDataSample.size() - firstCommaPos - 1);
	std::size_t secondCommaPos = truncatedPosData.find(",");
	std::string yString = truncatedPosData.substr(0,secondCommaPos -1);
	double y = stod(yString);
	m_sensorData.push_back(y);

	//
	// z coordinate
	//
	// "-0.98147"
	std::string zString = truncatedPosData.substr(secondCommaPos + 1, truncatedPosData.size() - secondCommaPos -1);
	double z = stod(zString);
	m_sensorData.push_back(z);
}

SensorDataSample::~SensorDataSample() {
	// TODO Auto-generated destructor stub
}

std::vector<double>
SensorDataSample::getSensorDataVector(){
	return m_sensorData;
}

} /* namespace ns3 */
