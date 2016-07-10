/*
 * position.data-sample.cc
 *
 *  Created on: 9 Jul 2016
 *      Author: matthias
 */

#include "position-data-sample.h"

namespace ns3 {

PositionDataSample::PositionDataSample(std::string positionDataSample) {

	//
	//	We assume three dimensional, comma separated position data here:
	//	"-0.18180, -0.20503, -0.98147"

	//
	// x coordinate
	//
	std::size_t firstCommaPos = positionDataSample.find(",");
	std::string xString = positionDataSample.substr(0,firstCommaPos - 1);
	double x = stod(xString);
	m_posData.push_back(x);

	//
	// y coordinate
	//
	// "-0.20503, -0.98147"
	std::string truncatedPosData = positionDataSample.substr(firstCommaPos + 1,positionDataSample.size() - firstCommaPos - 1);
	std::size_t secondCommaPos = truncatedPosData.find(",");
	std::string yString = truncatedPosData.substr(0,secondCommaPos -1);
	double y = stod(yString);
	m_posData.push_back(y);

	//
	// z coordinate
	//
	// "-0.98147"
	std::string zString = truncatedPosData.substr(secondCommaPos + 1, truncatedPosData.size() - secondCommaPos -1);
	double z = stod(zString);
	m_posData.push_back(z);
}

PositionDataSample::~PositionDataSample() {
	// TODO Auto-generated destructor stub
}

std::vector<double>
PositionDataSample::getPositionDataVector(){
	return m_posData;
}

} /* namespace ns3 */
