
#include "ns3/haptic-file-sensor.h"
#include "ns3/sensor-data-sample.h"
#include "ns3/test.h"

#include <string>
#include <vector>

using namespace ns3;

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
	//
	//	Please note: This test depends on a valid input file with 21 lines
	//
	HapticFileSensor::SensorFileType type = HapticFileSensor::FORCEFEEDBACK;
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_force.txt",type);
	size_t numElements = hfs.GetData().size();

	// Let's on purpose try to extract one SensorDataSample too much

	int retrievedSensorDataSamples = 0;
	for(size_t i = 0; i < numElements + 1; i++){
		SensorDataSample sds;
		if(hfs.GetNextSensorDataSample(sds))
			retrievedSensorDataSamples++;
	}

	NS_TEST_ASSERT_MSG_EQ(retrievedSensorDataSamples,21,"Retrieved unexpected number of SensorDataSemple objects");
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
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_force.txt",type);
	size_t numElements = hfs.GetData().size();

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
	HapticFileSensor hfs ("src/Kcl-Haptic-Sim/test/test_pos.txt",type);
	size_t numElements = hfs.GetData().size();

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
  }
} g_hapticFileSensorTestSuite;
