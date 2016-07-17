
#include "haptic-file-sensor.h"

#include <fstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticFileSensor");

HapticFileSensor::HapticFileSensor(std::string fileName, HapticFileSensor::SensorFileType type){
	NS_LOG_FUNCTION(this << "Filename: " << fileName << " Type: " << type);

	m_fileType = type;

	if (type == FileType::POSITION){
		ReadPositionData(fileName);
	}
	else{
		ReadForceFeedbackData(fileName);
	}
}

void HapticFileSensor::ReadPositionData(std::string fileName){
	NS_LOG_FUNCTION("Filename: " << fileName);

	std::ifstream posFile (fileName);

	if (posFile.is_open()){
		std::string line;
		while( getline(posFile,line)){
			NS_LOG_DEBUG(line);

			SensorDataSample pds (line);
			m_posData.push_back(pds);
		}
		posFile.close();
	}
	else {
		NS_LOG_ERROR("Unable to open file");
	}
}

void HapticFileSensor::ReadForceFeedbackData(std::string fileName){
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
			m_forceData.push_back(pds);
		}
		forceFile.close();
	}
	else {
		NS_LOG_ERROR("Unable to open file");
	}
}

std::deque<SensorDataSample>& HapticFileSensor::GetData(){

	if(m_fileType == FileType::POSITION){
		return m_posData;
	}

	else{
		NS_ASSERT_MSG(m_fileType == FileType::FORCEFEEDBACK,"Un-known file type, options are: POSITION or FORCEFEEDBACK");
		return m_forceData;
	}
}

bool HapticFileSensor::GetNextSensorDataSample(SensorDataSample& sds){
	if(m_fileType == FileType::POSITION){

		if(!m_posData.empty()){
			sds = m_posData.front();
			m_posData.pop_front();
			return true;
		}
		else{
			return false;
		}
	}

	else{
		NS_ASSERT_MSG(m_fileType == FileType::FORCEFEEDBACK,"Un-known file type, options are: POSITION or FORCEFEEDBACK");
		if(!m_forceData.empty()){
			sds = m_forceData.front();
			m_forceData.pop_front();
			return true;
		}
		else{
			return false;
		}
	}
}

}
