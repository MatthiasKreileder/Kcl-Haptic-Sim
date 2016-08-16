/*
 * data-reduction-algorithm.h
 *
 *  Created on: 20 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_DATA_REDUCTION_ALGORITHM_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_DATA_REDUCTION_ALGORITHM_H_

#include "ns3/object.h"
#include "ns3/type-id.h"

#include "sensor-data-sample.h"

namespace ns3 {

/**
 * \brief Base class for data reduction algorithms. Serves the purpose to provide applications a uniform way to use different
 * data reduction algorithms
 */
class DataReductionAlgorithm : public Object {
public:
	  /**
	   * \brief Get the type ID.
	   * \return the object TypeId
	   */
	  static TypeId GetTypeId (void);

	DataReductionAlgorithm();
	virtual ~DataReductionAlgorithm();

	/*
	 * \brief Answers the question if the provided SensorDataSample object should be transmitted
	 * or not.
	 *
	 * \param sds The SensorDataSample object which needs to checked if it should be transmitted
	 *
	 * \return true when the sample needs to be transmitted, false otherwise
	 */
	virtual bool NeedsToBeTransmitted (SensorDataSample sds) = 0;

};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_DATA_REDUCTION_ALGORITHM_H_ */
