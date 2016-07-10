/*
 * position-data-sample.h
 *
 *  Created on: 9 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTICS_MODEL_POSITION_DATA_SAMPLE_H_
#define SRC_KCL_HAPTICS_MODEL_POSITION_DATA_SAMPLE_H_

#include <string>
#include <vector>

namespace ns3 {

/**
 * \brief Can hold one position data sample (x,y,z)
 */
class PositionDataSample {
public:
	/**
	 * \brief Provided a string of the following format: "x, y, z" where x, y and z are floarting point
	 * numbers this constructor parses that line to populate one instance of PositionDataSample
	 *
	 * The values x, y and z can be singed or unsigned and differ in the number of digits before and after
	 * the floating point, however the values must be separated by a comma and a space.
	 *
	 * \param positionDataSample e.g. string sample = "-0.18180, -0.20503, -0.98147"
	 */
	PositionDataSample(std::string positionDataSample);
	virtual ~PositionDataSample();

	/**
	 * \return A copy of the position data stored as a vector of double values
	 */
	std::vector<double> getPositionDataVector();

private:
	std::vector<double> m_posData;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTICS_MODEL_POSITION_DATA_SAMPLE_H_ */
