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
	NS_LOG_FUNCTION(this);
}

HapticHeader::~HapticHeader() {
	NS_LOG_FUNCTION(this);
}

void HapticHeader::SetHapticMessage(std::string hapticMessage){
	NS_LOG_FUNCTION(hapticMessage);
	m_hapticMessage = hapticMessage;
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
  return GetTypeId ();
}

uint32_t
HapticHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);
  //
  //	The first four bytes indicate the lenght of the string that
  //	follows the first uint32_t
  //
  return 4 + GetStringLength();
}

void
HapticHeader::Serialize (Buffer::Iterator start) const
{
	NS_LOG_FUNCTION (this << &start);
	NS_LOG_DEBUG("String to serialize: " << m_hapticMessage);

	Buffer::Iterator bufIt = start;
	NS_LOG_DEBUG("Writing a string of size [bytes]: " << GetSerializedSize() - 4);
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
	NS_LOG_DEBUG("Reading a string of size [bytes]: " << stringLength);
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
	os <<  sizeof(uint32_t) << " " << m_hapticMessage << std::endl;
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
