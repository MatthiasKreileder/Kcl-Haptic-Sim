/*
 * position-to-velocity-conversion.cpp
 *
 *  Created on: 17 Jul 2016
 *      Author: matthias
 */

#include "position-to-velocity-conversion.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PositionToVelocityConversion");

std::deque<SensorDataSample>
PositionToVelocityConversion::ConvertPositionToVelocity(std::deque<SensorDataSample>& positionDeque, const double interSampleSeconds){
	NS_LOG_FUNCTION("interSampleSeconds: " << interSampleSeconds);

	if(positionDeque.size() == 0){
		std::deque<SensorDataSample> empty;
		return empty;
	}


	//positionDeque.push_front(SensorDataSample("0.0, 0.0, 0.0"));

	/*
	 * 	Formula used to calculate the velocity at time t
	 * 	We assume an intial velocity of (0,0,0) i.e. the recording started when
	 * 	the haptic device was not moving.
	 *
	 * 	  x[t] - x[t-1]
	 * 	------------------	= v[t]
	 * 	interSamplePosition
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
