
#include "ns3/haptic-file-sensor.h"
#include "ns3/sensor-data-sample.h"
#include "ns3/test.h"
#include "ns3/log.h"

#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HapticFileSensorTest");

class CheckSensorDataSampleContainers : public ns3::TestCase {
public:
	CheckSensorDataSampleContainers ();
	virtual void DoRun (void);
};

CheckSensorDataSampleContainers::CheckSensorDataSampleContainers ()
	: ns3::TestCase("CheckSensorDataSampleContainers")
{
}

void CheckSensorDataSampleContainers::DoRun(){
	NS_LOG_FUNCTION_NOARGS();
	//
	//	Please note: This test depends on a valid input file with 21 lines
	//
	HapticFileSensor::SensorFileType type = HapticFileSensor::POSITION;
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_pos.txt",type, 1);
	size_t numPosElements = hfs.GetData(HapticFileSensor::POSITION).size();
	size_t numVelElements = hfs.GetData(HapticFileSensor::VELOCITY).size();
	size_t numForceElements = hfs.GetData(HapticFileSensor::FORCEFEEDBACK).size();

	NS_TEST_ASSERT_MSG_EQ(numPosElements,19,"Wrong number of position data samples");
	NS_TEST_ASSERT_MSG_EQ(numVelElements,19,"Wrong number of velocity data samples");
	NS_TEST_ASSERT_MSG_EQ(numForceElements,0,"Wrong number of force data samples");
}

class GetNextSensorDataSampleTestCase : public ns3::TestCase {
public:
	GetNextSensorDataSampleTestCase ();
	virtual void DoRun (void);
};

GetNextSensorDataSampleTestCase::GetNextSensorDataSampleTestCase()
	: ns3::TestCase("GetNextSensorDataSampleTestCase")
{
}

void GetNextSensorDataSampleTestCase::DoRun(){
	NS_LOG_FUNCTION_NOARGS();
	//
	//	Please note: This test depends on a valid input file with 21 lines
	//
	HapticFileSensor::SensorFileType type = HapticFileSensor::FORCEFEEDBACK;
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_force.txt",type,1);
	size_t numElements = hfs.GetData(HapticFileSensor::FORCEFEEDBACK).size();

	NS_LOG_DEBUG("HapicFileSensor read " << numElements);

	// Let's on purpose try to extract one SensorDataSample too much

	int retrievedSensorDataSamples = 0;
	for(size_t i = 0; i < numElements + 1; i++){
		SensorDataSample sds;
		NS_LOG_DEBUG("About to retrieve new sample");
		if(hfs.GetNextSensorDataSample(sds,HapticFileSensor::FORCEFEEDBACK)){
			retrievedSensorDataSamples++;
			NS_LOG_DEBUG("retrievedSample: " << retrievedSensorDataSamples);
		}
	}
	NS_LOG_DEBUG("Assert: " << retrievedSensorDataSamples);
	NS_TEST_ASSERT_MSG_EQ(retrievedSensorDataSamples,21,"Retrieved unexpected number of SensorDataSemple objects");
	NS_LOG_DEBUG("Finished");
}

class HapticFileSensorParseForceFeedbackFileTestCase : public ns3::TestCase {
public:
	HapticFileSensorParseForceFeedbackFileTestCase ();
	virtual void DoRun (void);
};

HapticFileSensorParseForceFeedbackFileTestCase::HapticFileSensorParseForceFeedbackFileTestCase()
	: ns3::TestCase("HapticFileSensorParseForceFeedbackFileTestCase")
{
}

void HapticFileSensorParseForceFeedbackFileTestCase::DoRun(){
	//
	//	Please note: This test depends on a valid input file with 21 lines
	//
	HapticFileSensor::SensorFileType type = HapticFileSensor::FORCEFEEDBACK;
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_force.txt",type,1);
	size_t numElements = hfs.GetData(HapticFileSensor::FORCEFEEDBACK).size();

	NS_TEST_ASSERT_MSG_EQ(numElements,21,"Parsing position data from file failed");
}

class HapticFileSensorParseFileTestCase : public ns3::TestCase {
public:
	HapticFileSensorParseFileTestCase ();
	virtual void DoRun (void);
};

HapticFileSensorParseFileTestCase::HapticFileSensorParseFileTestCase()
	: ns3::TestCase ("ParsePositionSensorDataFromFile")
{
}

void
HapticFileSensorParseFileTestCase::DoRun()
{
	//
	//	Please note: This test depends on a valid input file with 19 lines
	//
	HapticFileSensor::SensorFileType type = HapticFileSensor::POSITION;
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_pos.txt",type,1);
	size_t numElements = hfs.GetData(HapticFileSensor::POSITION).size();

	NS_TEST_ASSERT_MSG_EQ(numElements,19,"Parsing position data from file failed");


}

class HapticFileSensorParseLineTestCase : public ns3::TestCase {
public:
	HapticFileSensorParseLineTestCase ();
	virtual void DoRun (void);
};

HapticFileSensorParseLineTestCase::HapticFileSensorParseLineTestCase ()
  : ns3::TestCase ("ParseLineConstructSensorDataSample")
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

	std::string sensorDataAsString = "-0.1818099, 0.20503, -0.98147";
	SensorDataSample pds(sensorDataAsString);

	for(int i = 0; i < 3; i++)
		NS_TEST_ASSERT_MSG_EQ_TOL(pds.getSensorDataVector()[i],expectedResult[i],0.01,"SensorDataSample constructor could not parse sensor string");

}


static class HapticFileSensorTestSuite : public TestSuite
{
public:
	HapticFileSensorTestSuite ()
    : TestSuite ("HapticFileSensor", UNIT)
  {
    AddTestCase (new HapticFileSensorParseLineTestCase (), TestCase::QUICK);
    AddTestCase (new HapticFileSensorParseFileTestCase (), TestCase::QUICK);
    AddTestCase (new HapticFileSensorParseForceFeedbackFileTestCase (), TestCase::QUICK);
    AddTestCase (new GetNextSensorDataSampleTestCase (), TestCase::QUICK);
    AddTestCase (new CheckSensorDataSampleContainers (), TestCase::QUICK);
  }
} g_hapticFileSensorTestSuite;
