
#include "ns3/haptic-file-sensor.h"
#include "ns3/position-data-sample.h"
#include "ns3/test.h"

#include <string>
#include <vector>

using namespace ns3;

class HapticFileSensorParseLineTestCase : public ns3::TestCase {
public:
	HapticFileSensorParseLineTestCase ();
	virtual void DoRun (void);
};

HapticFileSensorParseLineTestCase::HapticFileSensorParseLineTestCase ()
  : ns3::TestCase ("ParseLineConstructPositionDataSample")
{
}

void
HapticFileSensorParseLineTestCase::DoRun(){

	//
	//	Desired Output
	//
	std::vector<double> expectedResult;
	expectedResult.push_back(-0.1818099);
	expectedResult.push_back(0.20503);
	expectedResult.push_back(-0.98147);

	std::string positionDataAsString = "-0.1818099, 0.20503, -0.98147";
	PositionDataSample pds(positionDataAsString);

	for(int i = 0; i < 3; i++)
		NS_TEST_ASSERT_MSG_EQ_TOL(pds.getPositionDataVector()[i],expectedResult[i],0.01,"PositionDataSample constructor could not parse position string");

}


static class HapticFileSensorTestSuite : public TestSuite
{
public:
	HapticFileSensorTestSuite ()
    : TestSuite ("FileSensor", UNIT)
  {
    AddTestCase (new HapticFileSensorParseLineTestCase (), TestCase::QUICK);
  }
} g_hapticFileSensorTestSuite;
