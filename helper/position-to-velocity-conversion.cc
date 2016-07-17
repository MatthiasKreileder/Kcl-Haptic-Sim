/*
 * position-to-velocity-conversion.cpp
 *
 *  Created on: 17 Jul 2016
 *      Author: matthias
 */

#include "position-to-velocity-conversion.h"

namespace ns3 {

std::deque<SensorDataSample>
PositionToVelocityConversion::ConvertPositionToVelocity(std::deque<SensorDataSample>& positionDeque, const double interSampleSeconds){

	if(positionDeque.size() == 0){
		std::deque<SensorDataSample> empty;
		return empty;
	}


	//positionDeque.push_front(SensorDataSample("0.0, 0.0, 0.0"));

	/*
	 * 	  x[t] - x[t-1]
	 * ------------------	= v[t]
	 * interSamplePosition
	 */
	std::deque<SensorDataSample>::iterator toPosition = positionDeque.begin();
	toPosition++;

	std::deque<SensorDataSample>::iterator fromPosition = positionDeque.begin();

	std::deque<SensorDataSample> velocityDeque;
	velocityDeque.push_back(SensorDataSample("0.0, 0.0, 0.0"));
	while(toPosition != positionDeque.end()){
		double v_x = (toPosition->getSensorDataVector()[0] - fromPosition->getSensorDataVector()[0]) / interSampleSeconds;
		double v_y = (toPosition->getSensorDataVector()[1] - fromPosition->getSensorDataVector()[1]) / interSampleSeconds;
		double v_z = (toPosition->getSensorDataVector()[2] - fromPosition->getSensorDataVector()[2]) / interSampleSeconds;
		velocityDeque.push_back(SensorDataSample(v_x,v_y,v_z));

		fromPosition++;
		toPosition++;
	}

	return velocityDeque;
}

PositionToVelocityConversion::PositionToVelocityConversion() {


}

PositionToVelocityConversion::~PositionToVelocityConversion() {

}

} /* namespace ns3 */
