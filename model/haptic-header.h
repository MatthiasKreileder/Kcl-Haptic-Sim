/*
 * haptic-header.h
 *
 *  Created on: 11 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_HEADER_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_HEADER_H_

#include "ns3/core-module.h"
#include "ns3/header.h"

#include <string>

namespace ns3 {

/**
 * \brief Holds the data for a haptic communication message
 *
 * The message can be a position data sample or a force-feeback sample
 */
class HapticHeader : public Header {
public:
	HapticHeader();
	virtual ~HapticHeader();

	void setPayload(std::string payload);

	static TypeId
	GetTypeId (void);

	virtual
	TypeId GetInstanceTypeId (void) const;

	virtual
	uint32_t GetSerializedSize (void) const;

	virtual
	void Serialize (Buffer::Iterator start) const;

	virtual
	uint32_t Deserialize (Buffer::Iterator start);

	virtual
	void Print (std::ostream &os) const;

	std::string GetHapticMessage () const;

private:
	std::string m_hapticMessage;

	uint32_t GetStringLength () const;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_HEADER_H_ */
