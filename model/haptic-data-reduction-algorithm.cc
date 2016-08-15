/*
 * haptic-data-reduction-algorithm.cpp
 *
 *  Created on: 15 Aug 2016
 *      Author: matthias
 */

#include "haptic-data-reduction-algorithm.h"
//#include "../include/HapticDataReductionAlgorithm.h"
#include <stdexcept>
#include <cmath>

namespace ns3 {


HapticDataReductionAlgorithm::HapticDataReductionAlgorithm(double deadBand) {

    if(deadBand < 0 || deadBand > 1)
        throw std::invalid_argument("Provided deadband value must be in the range of [0,1]");

    m_deadBand = deadBand;
    m_isFirstSample = true;
}

bool HapticDataReductionAlgorithm::needsToBeTransmitted(const std::vector<double>& currentSensorSample){

    /*
     * Is it the first sample => needs to be transmitted
     */
    if(m_isFirstSample){
        m_lastSample = currentSensorSample;
        m_isFirstSample = false;
        return true;
    }

    /*
     * Check dimensions
     */
    if(currentSensorSample.size() != m_lastSample.size())
        throw std::invalid_argument("The sensor samples fed to the HaptiDataReductionAlgorithm differ in their dimensions => Can not determine result");

    /*
     * Let's do the math
     */
    return needsToBeTransmittedPriv(currentSensorSample);


}

bool HapticDataReductionAlgorithm::needsToBeTransmittedPriv(const std::vector<double>& currentSensorSample){

    /*
     * length of the sensor vector that was transmitted last time
     */
    double lengthLastTransmittedSample = 0;
    for(std::vector<double>::const_iterator cIt = m_lastSample.begin(); cIt != m_lastSample.end(); cIt++){

        double tmp = *cIt;
        lengthLastTransmittedSample += (tmp*tmp);
    }
    lengthLastTransmittedSample = sqrt(lengthLastTransmittedSample);

    /*
     * Diff vector length
     */
    double diffVectorLegth = 0;
    for(size_t i = 0; i < currentSensorSample.size(); i++){

        double tmp = currentSensorSample[i] - m_lastSample[i];
        tmp = tmp*tmp;
        diffVectorLegth += tmp;
    }
    diffVectorLegth = sqrt(diffVectorLegth);

    if(diffVectorLegth >= m_deadBand*lengthLastTransmittedSample){
		m_lastSample = currentSensorSample;
        return true;
    }
    else{
        return false;
    }

}

HapticDataReductionAlgorithm::~HapticDataReductionAlgorithm() {
	// TODO Auto-generated destructor stub
}

} /* namespace ns3 */
