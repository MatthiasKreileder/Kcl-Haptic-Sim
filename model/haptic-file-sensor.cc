
#include "haptic-file-sensor.h"

#include <fstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticFileSensor");

HapticFileSensor::HapticFileSensor(std::string fileName, HapticFileSensor::SensorFileType type){
	NS_LOG_FUNCTION(this << "Filename: " << fileName << " Type: " << type);

	if (type == FileType::POSITION)
		ReadPositionData(fileName);
}

void HapticFileSensor::ReadPositionData(std::string fileName){
	NS_LOG_FUNCTION("Filename: " << fileName);

	std::ifstream posFile (fileName);

	if (posFile.is_open()){
		std::string line;
		while( getline(posFile,line)){
			NS_LOG_DEBUG(line);

			PositionDataSample pds (line);
			m_posData.push(pds);
		}
		posFile.close();
	}
	else {
		NS_LOG_ERROR("Unable to open file");
	}
}

std::queue<PositionDataSample>& HapticFileSensor::GetData(){
	return m_posData;
}

}
