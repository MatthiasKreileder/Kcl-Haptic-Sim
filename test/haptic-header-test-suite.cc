/*
 * haptic-header-test-suite.cc
 *
 *  Created on: 11 Jul 2016
 *      Author: matthias
 */


#include "ns3/test.h"
#include "ns3/haptic-header.h"
#include "ns3/packet.h"

#include <string>

namespace ns3 {

class HapticHeaderEncodingDecodingTestCase : public ns3::TestCase {
public:
	HapticHeaderEncodingDecodingTestCase ();
	virtual void DoRun (void);
};

HapticHeaderEncodingDecodingTestCase::HapticHeaderEncodingDecodingTestCase()
	: ns3::TestCase ("ParseSensorDataFromFile")
{
}

void
HapticHeaderEncodingDecodingTestCase::DoRun()
{

	//
	//	The HapticHeader does not interpret the provided string. It simply
	//	makes sure to adapt to the size of the string and correct interaction
	//	when used together with a Packet.
	//
	//	To make this more obvious we use a non-sense string here
	//
	std::string dataAsString = "-0.1818099, 0.20503, -0.98147 ewfeoviknsvolin";
	HapticHeader hapticHeader;
	hapticHeader.SetHapticMessage(dataAsString);

	// add header
	Ptr<Packet> packet = Create<Packet> ();

	// Fill out udpHeader fields appropriately
	packet->AddHeader (hapticHeader);

	//
	//	There is no actual network topology or transmission to keep it simple
	//	and avoid randomness inside tests
	//

	// remove header
	HapticHeader rxHeader;
	packet->RemoveHeader (rxHeader);

	// std::string::compare returns 0 if the two strings compare equally
	int compareResult = dataAsString.compare(rxHeader.GetHapticMessage());
	NS_TEST_ASSERT_MSG_EQ(compareResult,0,"Strings do not compare equally");

}

static class HapticHeaderTestSuite : public TestSuite
{
public:
	HapticHeaderTestSuite ()
    : TestSuite ("HapticHeader", UNIT)
  {
    AddTestCase (new HapticHeaderEncodingDecodingTestCase (), TestCase::QUICK);
  }
} g_hapticHeaderTestSuite;


}
