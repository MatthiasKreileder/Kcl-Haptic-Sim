


#include "ns3/core-module.h"
#include <string>
#include "ns3/test.h"
#include <vector>
#include "ns3/haptic-data-reduction-algorithm.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("HapticDataReductionAlgorithmTest");

class SuccessivSmallChanges : public ns3::TestCase {
public:
	SuccessivSmallChanges ();
	virtual void DoRun (void);
};

SuccessivSmallChanges::SuccessivSmallChanges()
	: ns3::TestCase ("SuccessivSmallChangesTestCase")
{
}

void
SuccessivSmallChanges::DoRun(){

    HapticDataReductionAlgorithm hda(0.1);

    std::vector<double> firstSample;
    firstSample.push_back(10);

    NS_TEST_ASSERT_MSG_EQ(true,hda.needsToBeTransmitted(firstSample),"First sample needs to transmitted");

    firstSample[0] = firstSample[0]*1.08;

    NS_TEST_ASSERT_MSG_EQ(false,hda.needsToBeTransmitted(firstSample),"Minor changes don't need to be transmitted");

    firstSample[0] = firstSample[0]*1.08;

    NS_TEST_ASSERT_MSG_EQ(true,hda.needsToBeTransmitted(firstSample),"Sample changed significantly => needs to be transmitted");
}

class MultiDimensionTestCase : public ns3::TestCase {
public:
	MultiDimensionTestCase ();
	virtual void DoRun (void);
};

MultiDimensionTestCase::MultiDimensionTestCase()
	: ns3::TestCase ("MultiDimensionTestCase")
{
}

void
MultiDimensionTestCase::DoRun()
{
    HapticDataReductionAlgorithm hda(0.1);

    std::vector<double> firstSample;
    firstSample.push_back(10);
    firstSample.push_back(20);
    firstSample.push_back(30);
    firstSample.push_back(40);

    NS_TEST_ASSERT_MSG_EQ(true,hda.needsToBeTransmitted(firstSample),"The first sample always needs to be transmitted");

    std::vector<double> secondSample;
    secondSample.push_back(11);
    secondSample.push_back(21);
    secondSample.push_back(31);
    secondSample.push_back(41);


    NS_TEST_ASSERT_MSG_EQ(false,hda.needsToBeTransmitted(secondSample),"Sample does not need to be transmitted (minor change in magnitude)");
}

static class HapticDataReductionAlgorithmTestSuite : public TestSuite
{
public:
	HapticDataReductionAlgorithmTestSuite ()
    : TestSuite ("HapticDataReductionAlgorithm", UNIT)
  {
    AddTestCase (new MultiDimensionTestCase (), TestCase::QUICK);
    AddTestCase (new SuccessivSmallChanges (), TestCase::QUICK);
  }
} g_hapticDataReductionAlgorithmTestSuite;


}
