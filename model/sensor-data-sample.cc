

#include "sensor-data-sample.h"

#include <sstream>
#include <iomanip>

#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("SensorDataSample");

SensorDataSample::SensorDataSample(std::string sensorDataSample) {

	m_sensorDataSample = sensorDataSample;

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

SensorDataSample::SensorDataSample(){
	m_sensorData = std::vector<double>();
	m_sensorDataSample = "";
}

SensorDataSample::SensorDataSample(double x, double y, double z){
	NS_LOG_FUNCTION(this);
	std::vector<double> sensorDataVector;
	sensorDataVector.push_back(x);
	sensorDataVector.push_back(y);
	sensorDataVector.push_back(z);
	m_sensorData = sensorDataVector;

	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << std::setprecision(5);
	ss << x + 0.00001;
	ss << ", ";
	ss << std::setprecision(5);
	ss << y+ 0.00001;
	ss << ", ";
	ss << std::setprecision(5);
	ss << z+ 0.00001;
	m_sensorDataSample = ss.str();
	NS_LOG_DEBUG(m_sensorDataSample);
}

void
SensorDataSample::operator =(SensorDataSample& sds){
	m_sensorData = sds.getSensorDataVector();
	m_sensorDataSample = sds.getSensorDataString();
}

SensorDataSample::~SensorDataSample() {
	// TODO Auto-generated destructor stub
}

std::vector<double>
SensorDataSample::getSensorDataVector(){
	return m_sensorData;
}

std::string
SensorDataSample::getSensorDataString (){
	return m_sensorDataSample;
}

} /* namespace ns3 */
