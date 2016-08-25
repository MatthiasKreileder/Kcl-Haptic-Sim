

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_NO_DATA_REDUCTION_ALGORITHM_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_NO_DATA_REDUCTION_ALGORITHM_H_

#include "data-reduction-algorithm.h"

namespace ns3 {

class NoDataReductionAlgorithm : public DataReductionAlgorithm {
public:
	static TypeId GetTypeId (void);

	NoDataReductionAlgorithm();
	virtual ~NoDataReductionAlgorithm();

	/*
	 * \brief Answers the question if the provided SensorDataSample object should be transmitted
	 * or not. This algorithm will always answer with yes, since this gives us the transmission of
	 * the raw sensor data.
	 *
	 * \param sds The SensorDataSample object which needs to checked if it should be transmitted
	 *
	 * \return true when the sample needs to be transmitted, false otherwise
	 */
	virtual bool NeedsToBeTransmitted (SensorDataSample sds);
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_NO_DATA_REDUCTION_ALGORITHM_H_ */
