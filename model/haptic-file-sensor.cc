
#include "haptic-file-sensor.h"
#include "../helper/position-to-velocity-conversion.h"
#include <fstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticFileSensor");

HapticFileSensor::HapticFileSensor(std::string fileName, HapticFileSensor::SensorFileType type){
	NS_LOG_FUNCTION(this << "Filename: " << fileName << " Type: " << type);

	m_posData = std::deque<SensorDataSample>();
	m_velocityData = std::deque<SensorDataSample>();
	m_forceData = std::deque<SensorDataSample>();

	if (type == FileType::POSITION){
		ReadSensorDataSamples(fileName,m_posData);
		m_velocityData = PositionToVelocityConversion::ConvertPositionToVelocity(m_posData,m_interSampleSeconds);
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

//void HapticFileSensor::ReadPositionData(std::string fileName){
//	NS_LOG_FUNCTION("Filename: " << fileName);
//
//	std::ifstream posFile (fileName);
//
//	if (posFile.is_open()){
//		std::string line;
//		while( getline(posFile,line)){
//			NS_LOG_DEBUG(line);
//
//			SensorDataSample pds (line);
//			m_posData.push_back(pds);
//		}
//		posFile.close();
//	}
//	else {
//		NS_LOG_ERROR("Unable to open file");
//	}
//}
//
//void HapticFileSensor::ReadForceFeedbackData(std::string fileName){
//	NS_LOG_FUNCTION("Filename: " << fileName);
//	/*
//	 * At the moment it looks like the force feedback can be parsed
//	 * just like the position data. See issue #6
//	 */
//	std::ifstream forceFile (fileName);
//
//	if (forceFile.is_open()){
//		std::string line;
//		while( getline(forceFile,line)){
//			NS_LOG_DEBUG(line);
//
//			SensorDataSample pds (line);
//			m_forceData.push_back(pds);
//		}
//		forceFile.close();
//	}
//	else {
//		NS_LOG_ERROR("Unable to open file");
//	}
//}

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
		return m_posData;
	}
	else if(type == FileType::VELOCITY){
		return m_velocityData;
	}
	else{
		NS_ASSERT_MSG(type == FileType::FORCEFEEDBACK,"Un-known file type, options are: POSITION, VELOCITY or FORCEFEEDBACK");
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
