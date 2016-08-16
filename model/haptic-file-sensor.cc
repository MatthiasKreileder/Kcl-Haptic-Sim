
#include "haptic-file-sensor.h"
#include "../helper/position-to-velocity-conversion.h"
#include <fstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticFileSensor");

HapticFileSensor::HapticFileSensor(std::string fileName, HapticFileSensor::SensorFileType type, double interSampleSeconds){
	NS_LOG_FUNCTION(this << "Filename: " << fileName << " Type: " << type);

	m_posData = std::deque<SensorDataSample>();
	m_velocityData = std::deque<SensorDataSample>();
	m_forceData = std::deque<SensorDataSample>();

	if (type == FileType::POSITION){
		ReadSensorDataSamples(fileName,m_posData);
		m_velocityData = PositionToVelocityConversion::ConvertPositionToVelocity(m_posData,interSampleSeconds);
	}
	else if(type == FileType::VELOCITY){
		ReadSensorDataSamples(fileName,m_velocityData);
	}
	else{
		NS_ASSERT_MSG(type == FileType::FORCEFEEDBACK,"Un-known file type, options are: POSITION, VELOCITY or FORCEFEEDBACK");
		ReadSensorDataSamples(fileName,m_forceData);
	}

	NS_LOG_DEBUG("Position Container elements: " << m_posData.size());
	NS_LOG_DEBUG("Velocity Container elements: " << m_velocityData.size());
	NS_LOG_DEBUG("Force Container elements: " << m_forceData.size());

}

HapticFileSensor::HapticFileSensor(std::string forceSamplesFileName){
	NS_LOG_FUNCTION(this << " Filename: " << forceSamplesFileName);

	m_posData = std::deque<SensorDataSample>();
	m_velocityData = std::deque<SensorDataSample>();
	m_forceData = std::deque<SensorDataSample>();

	ReadSensorDataSamples(forceSamplesFileName,m_forceData);
}

HapticFileSensor::HapticFileSensor(std::string positionSamplesFileName, std::string velocitySamplesFileName){
	NS_LOG_FUNCTION(this << " positionSamplesFileName: " << positionSamplesFileName << " velocitySamplesFileName: " << velocitySamplesFileName);

	m_posData = std::deque<SensorDataSample>();
	m_velocityData = std::deque<SensorDataSample>();
	m_forceData = std::deque<SensorDataSample>();

	ReadSensorDataSamples(positionSamplesFileName,m_posData);
	ReadSensorDataSamples(velocitySamplesFileName,m_velocityData);
}

void HapticFileSensor::ReadSensorDataSamples(std::string fileName, std::deque<SensorDataSample>& sdsContainer){
	NS_LOG_FUNCTION("Filename: " << fileName);
	/*
	 * At the moment it looks like the force feedback can be parsed
	 * just like the position data. See issue #6
	 */
	std::ifstream forceFile (fileName);

	if (forceFile.is_open()){
		std::string line;
		while( getline(forceFile,line)){
			NS_LOG_DEBUG(line);

			SensorDataSample pds (line);
			sdsContainer.push_back(pds);
		}
		forceFile.close();
	}
	else {
		NS_LOG_ERROR("Unable to open file");
	}
}

std::deque<SensorDataSample>& HapticFileSensor::GetData(SensorFileType type){
	NS_LOG_FUNCTION(type);
	if(type == FileType::POSITION){
		NS_LOG_DEBUG("Returning position deque");
		return m_posData;
	}
	else if(type == FileType::VELOCITY){
		NS_LOG_DEBUG("Returning velocity deque");
		return m_velocityData;
	}
	else{
		NS_ASSERT_MSG(type == FileType::FORCEFEEDBACK,"Un-known file type, options are: POSITION, VELOCITY or FORCEFEEDBACK");
		NS_LOG_DEBUG("Returning force deque");
		return m_forceData;
	}
}

bool HapticFileSensor::GetNextSensorDataSample(SensorDataSample& sds,SensorFileType type){
	NS_LOG_FUNCTION(type);
	if (type == FileType::POSITION){
		return GetNextSensorDataSamplePriv(sds,m_posData);
	}
	else if(type == FileType::VELOCITY){
		return GetNextSensorDataSamplePriv(sds,m_velocityData);
	}
	else{
		return GetNextSensorDataSamplePriv(sds,m_forceData);
	}
}

bool HapticFileSensor::GetNextSensorDataSamplePriv(SensorDataSample& sds, std::deque<SensorDataSample>& sdsContainer){
	NS_LOG_FUNCTION(this);
	if(!sdsContainer.empty()){
		sds = sdsContainer.front();
		sdsContainer.pop_front();
		return true;
	}
	else{
		return false;
	}
}

}
