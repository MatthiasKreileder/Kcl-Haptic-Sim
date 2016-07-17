/*
 * position-to-velocity-conversion-test-suite.cc
 *
 *  Created on: 17 Jul 2016
 *      Author: matthias
 */

#include "ns3/test.h"
#include "../helper/position-to-velocity-conversion.h"
#include "ns3/sensor-data-sample.h"

#include <string>
#include <deque>
#include <vector>
#include <cmath>

namespace ns3 {

class PositionToVelocityTestCase : public ns3::TestCase {
public:
	PositionToVelocityTestCase ();
	virtual void DoRun (void);
private:
	bool isEqual(SensorDataSample& sds1, SensorDataSample& sds2);
};

PositionToVelocityTestCase::PositionToVelocityTestCase()
	: ns3::TestCase ("PositionToVelocityTestCase")
{
}

bool PositionToVelocityTestCase::isEqual(SensorDataSample& sds1, SensorDataSample& sds2){

	std::vector<double> vec1 = sds1.getSensorDataVector();
	std::vector<double> vec2 = sds2.getSensorDataVector();

	for(std::vector<double>::size_type i = 0; i < vec1.size(); i++){
		if(std::abs(vec1[i] - vec2[i]) > 0.001)
			return false;
	}

	// we made it here => passed all checks
	return true;
}

void
PositionToVelocityTestCase::DoRun()
{

	/*
	 * Let's pretend we have three recorded samples
	 * which were measured at a frequency of 10 Hz, i.e. inter sample time is 0.1 seconds
	 */
	double interSampleSeconds = 0.1;
	SensorDataSample sds1 = SensorDataSample("10.0, 10.0, 10.0");
	SensorDataSample sds2 = SensorDataSample("0.0, 0.0, 0.0");
	SensorDataSample sds3 = SensorDataSample("20.0, 20.0, 20.0");

	/*
	 * Those are the samples we want to get
	 */
	SensorDataSample v1 = SensorDataSample(0,0,0);
	SensorDataSample v2 = SensorDataSample(-100,-100,-100);
	SensorDataSample v3 = SensorDataSample(200.0, 200.0, 200.0);

	std::deque<SensorDataSample> pos_q;
	pos_q.push_back(sds1);
	pos_q.push_back(sds2);
	pos_q.push_back(sds3);

	std::deque<SensorDataSample> velocity_q = PositionToVelocityConversion::ConvertPositionToVelocity(pos_q,interSampleSeconds);

	/*
	 * Checking
	 */
	NS_TEST_ASSERT_MSG_EQ(velocity_q.size(),3,"Wrong number of elements in velocity deque");

	// Sample 1
	bool wentFine = true;
	if(!isEqual(v1,velocity_q.front())){
		wentFine = false;
	}
	velocity_q.pop_front();

	// Sample 2
	if(!isEqual(v2,velocity_q.front())){
		wentFine = false;
	}
	velocity_q.pop_front();

	// Sample 3
	if(!isEqual(v3,velocity_q.front())){
		wentFine = false;
	}
	velocity_q.pop_front();

	int result;
	if(wentFine){
		result = 1;
	}
	else{
		result = 0;
	}

	NS_TEST_ASSERT_MSG_EQ(result,1,"Conversion failed");


}

static class PositionToVelocityTestSuite : public TestSuite
{
public:
	PositionToVelocityTestSuite ()
    : TestSuite ("PositionToVelocityConversion", UNIT)
  {
    AddTestCase (new PositionToVelocityTestCase (), TestCase::QUICK);
  }
} g_PositionToVelocityTestSuite;


}



