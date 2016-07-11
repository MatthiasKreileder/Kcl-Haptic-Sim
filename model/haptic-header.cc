/*
 * haptic-header.cpp
 *
 *  Created on: 11 Jul 2016
 *      Author: matthias
 */

#include "haptic-header.h"
#include "ns3/log.h"
#include "ns3/buffer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticHeader");

HapticHeader::HapticHeader() {
}

HapticHeader::~HapticHeader() {
	// TODO Auto-generated destructor stub
}

void HapticHeader::setPayload(std::string payload){
	m_hapticMessage = payload;
}

TypeId
HapticHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HapticHeader")
    .SetParent<Header> ()
    .SetGroupName("Applications")
    .AddConstructor<HapticHeader> ()
  ;
  return tid;
}
TypeId
HapticHeader::GetInstanceTypeId (void) const
{
  return HapticHeader::GetTypeId ();
}

uint32_t
HapticHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  return 4 + GetStringLength();
}

void
HapticHeader::Serialize (Buffer::Iterator start) const
{
	NS_LOG_FUNCTION (this << &start);
	NS_LOG_DEBUG("String to serialize: " << m_hapticMessage);

	Buffer::Iterator bufIt = start;
	bufIt.WriteHtonU32 (GetSerializedSize() - 4);

	for(std::string::size_type strIt = 0; strIt < m_hapticMessage.size(); strIt++){
		uint8_t token = m_hapticMessage[strIt];
		bufIt.WriteU8 (token);
	}
}

uint32_t
HapticHeader::Deserialize (Buffer::Iterator start)
{
	NS_LOG_FUNCTION (this << &start);
	Buffer::Iterator bufIt = start;

	uint32_t stringLength = bufIt.ReadNtohU32 ();
	char payloadBuffer[stringLength + 1];
	payloadBuffer[stringLength] = '\0';

	for(uint i = 0; i < stringLength; i++){
		payloadBuffer[i] = bufIt.ReadU8();
	}

	std::string s (payloadBuffer);
	NS_LOG_DEBUG("Deserialized string: " << s);
	m_hapticMessage = s;

	return GetSerializedSize ();
}

void HapticHeader::Print (std::ostream &os) const
{
	os << m_hapticMessage << std::endl;
}

std::string HapticHeader::GetHapticMessage () const
{
	return m_hapticMessage;
}

uint32_t HapticHeader::GetStringLength () const
{
	NS_LOG_FUNCTION(this);
	uint32_t sz = 0;
	//
	//	ToDo:This could maybe also be done without a loop by casting from size_t
	//	to uint32_t. Think about when this cast would fail.
	//
	for(std::string::size_type i = 0; i < m_hapticMessage.size(); i++)
		sz++;

	return sz;
}

} /* namespace ns3 */
