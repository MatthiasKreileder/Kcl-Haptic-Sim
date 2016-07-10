/*
 * position-data-sample.h
 *
 *  Created on: 9 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTICS_MODEL_POSITION_DATA_SAMPLE_H_
#define SRC_KCL_HAPTICS_MODEL_POSITION_DATA_SAMPLE_H_

#include <string>

namespace ns3 {

class PositionDataSample {
public:
	PositionDataSample(std::string positionDataSample);
	virtual ~PositionDataSample();
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTICS_MODEL_POSITION_DATA_SAMPLE_H_ */
