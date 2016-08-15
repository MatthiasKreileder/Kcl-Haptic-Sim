/*
 * haptic-data-reduction-algorithm.h
 *
 *  Created on: 15 Aug 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_DATA_REDUCTION_ALGORITHM_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_DATA_REDUCTION_ALGORITHM_H_

#include <vector>

namespace ns3 {

class HapticDataReductionAlgorithm {
public:
    /*
     * \brief creates a HapticDataReductionAlgorithm object and sets the deadband value to \param deadBand
     *
     * \param deadBand The value which defines the deadband (acceptable range: [0,1]). 0 means every sample will be transmitted (no data reduction)
     *
     * When an object of this class is queried if a sensor sample needs to be transmitted via its needsToBeTransmitted() method
     * the provided parameter \param deadBand defines how big the difference between the new sample and the last transmitted sample
     * needs to be (percentage wise).
     *
     * E.g. let's assume one-dimensional samples and the last sample that was transmitted had the value x_last and the new sample
     * has the value x_current then this method returns true if: abs(x_current - x_last) >= (deadBand * x_last)
     */
    HapticDataReductionAlgorithm(double deadBand);

    /*
     * \brief Checks if a new sensor sample \param currentSensorSample needs to be transmitted
     *
     * \param currentSensorSample the new sample
     *
     * \returns true if the provided sample \param currentSensorSample needs to be transmitted
     */
    bool needsToBeTransmitted(const std::vector<double>& currentSensorSample);


	virtual ~HapticDataReductionAlgorithm();

private:

    bool needsToBeTransmittedPriv(const std::vector<double>& currentSensorSample);

    // The deadband value used by this algorithm
    double m_deadBand;

    // The first sample always needs to be transmitted
    bool m_isFirstSample;

    // the last sample that was transmitted
    std::vector<double> m_lastSample;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_DATA_REDUCTION_ALGORITHM_H_ */
