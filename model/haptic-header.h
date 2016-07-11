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

	static TypeId GetTypeId (void);

	/**
	 * \brief Constructor
	 */
	HapticHeader();
	virtual ~HapticHeader();

	/**
	 * \brief Set the string that will be sent over the network
	 *
	 * In order that another haptic application can make sense out of the provided string one
	 * must stick to the same message format.
	 *
	 * \param msg The string containing the force-feeback or position data stored as a string, e.g.
	 * "12.5, -98.0004, 100.0"
	 */
	void SetHapticMessage(std::string msg);

	/**
	 * \returns The member which stores the haptic message
	 */
	std::string GetHapticMessage () const;

	/**
	 * \brief Inherted from base class.
	 */
	virtual
	TypeId GetInstanceTypeId (void) const;

	/**
	 * \brief The size of the serialized header
	 */
	virtual
	uint32_t GetSerializedSize (void) const;

	/**
	 * \brief Serializes the header and write the obtained bytes
	 * into the memory location provided by the argument
	 *
	 */
	virtual
	void Serialize (Buffer::Iterator start) const;

	/**
	 * \brief Read the bytes pointed to by start and populates a HapticHeader instance
	 */
	virtual
	uint32_t Deserialize (Buffer::Iterator start);

	virtual
	void Print (std::ostream &os) const;



private:
	std::string m_hapticMessage;

	uint32_t GetStringLength () const;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_HEADER_H_ */
