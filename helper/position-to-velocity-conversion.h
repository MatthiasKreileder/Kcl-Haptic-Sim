/*
 * position-to-velocity-conversion.h
 *
 *  Created on: 17 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_HELPER_POSITION_TO_VELOCITY_CONVERSION_H_
#define SRC_KCL_HAPTIC_SIM_HELPER_POSITION_TO_VELOCITY_CONVERSION_H_

#include <deque>
#include "../model/sensor-data-sample.h"

namespace ns3 {

/**
 * Brief Converts a queue of SensorDataSample objects which represent
 * position vectors to velocity vectors.
 *
 * An untested precondition is that the SensorDataSample objects do in fact
 * represent position values.
 */
class PositionToVelocityConversion {
public:
	/**
	 * \brief Transforms the SensorDataSample objects inside positionDeque to elements which represent represent velocity
	 *
	 * \param positionDeque The container which stores the SensorDataSample objects. Those should represent position samples!
	 * \param interSampleSeconds The time interval between two samples. The velocity can only be computed if the position and the elapsed time between two samples is known.
	 *
	 * \returns A deque which contains SensorDataSample objects which represent velocity
	 */
	static std::deque<SensorDataSample> ConvertPositionToVelocity(std::deque<SensorDataSample>& positionDeque, const double interSampleSeconds);

private:
	/**
	 * The PositionToVelocityConversion class is intended to be a utility only => there is no need to create an object
	 * of it, because it is meant to be completely stateless.
	 *
	 * The constructor is private to prevent miss-use
	 */
	PositionToVelocityConversion();
	virtual ~PositionToVelocityConversion();
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_HELPER_POSITION_TO_VELOCITY_CONVERSION_H_ */
