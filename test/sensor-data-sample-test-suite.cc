/*
 * sensor-data-sample-test-suite.cc
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */

#include "ns3/test.h"
#include "ns3/sensor-data-sample.h"

#include <string>
#include <vector>

namespace ns3 {

class AssignmentOperator : public ns3::TestCase {
public:
	AssignmentOperator ();
	virtual void DoRun (void);
};

AssignmentOperator::AssignmentOperator()
	: ns3::TestCase ("AssignmentOperator")
{
}

void
AssignmentOperator::DoRun()
{

	SensorDataSample sds("-0.18180, -0.20503, -0.98147");

	SensorDataSample sdsCopy;

	sdsCopy = sds;

	int compareResult = sds.getSensorDataString().compare(sdsCopy.getSensorDataString());
	NS_TEST_ASSERT_MSG_EQ(compareResult,0,"Assignment Operator failed");

	std::vector<double> sensorVec = sds.getSensorDataVector();
	std::vector<double> sensorVecCopy = sds.getSensorDataVector();

	for(std::vector<double>::size_type i = 0; i < sensorVec.size(); i++)
		NS_TEST_ASSERT_MSG_EQ_TOL(sensorVec[i],sensorVecCopy[i],0.01,"Assignment Operator failed");

	// std::string::compare returns 0 if the two strings compare equally
	//int compareResult = dataAsString.compare(rxHeader.GetHapticMessage());
	//NS_TEST_ASSERT_MSG_EQ(compareResult,0,"Strings do not compare equally");

}

static class SensorDataSampleTestSuite : public TestSuite
{
public:
	SensorDataSampleTestSuite ()
    : TestSuite ("SensorDataSample", UNIT)
  {
    AddTestCase (new AssignmentOperator (), TestCase::QUICK);
  }
} g_assignmentOperatorTestSuite;


}



