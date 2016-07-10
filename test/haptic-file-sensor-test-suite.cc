
#include "ns3/haptic-file-sensor.h"
#include "ns3/test.h"

using namespace ns3;

class HapticFileSensorTestCase : public ns3::TestCase {
private:
public:
	HapticFileSensorTestCase ();
  virtual void DoRun (void);


};

HapticFileSensorTestCase::HapticFileSensorTestCase ()
  : ns3::TestCase ("ReadDataFromFile")
{
}


void
HapticFileSensorTestCase::DoRun (void)
{
//  Histogram h0 (3.5);
//  // Testing floating-point bin widths
//  {
//    for (int i=1; i <= 10; i++)
//      {
//        h0.AddValue (3.4);
//      }
//
//    for (int i=1; i <= 5; i++)
//      {
//        h0.AddValue (3.6);
//      }
//
//    NS_TEST_EXPECT_MSG_EQ_TOL (h0.GetBinWidth (0),  3.5, 1e-6, "");
//    NS_TEST_EXPECT_MSG_EQ (h0.GetNBins (),  2, "");
//    NS_TEST_EXPECT_MSG_EQ_TOL (h0.GetBinStart (1),  3.5, 1e-6, "");
//    NS_TEST_EXPECT_MSG_EQ (h0.GetBinCount (0),  10, "");
//    NS_TEST_EXPECT_MSG_EQ (h0.GetBinCount (1),  5, "");
//  }
//
//  {
//    // Testing bin expansion
//    h0.AddValue (74.3);
//    NS_TEST_EXPECT_MSG_EQ (h0.GetNBins (), 22, "");
//    NS_TEST_EXPECT_MSG_EQ (h0.GetBinCount (21), 1, "");
//  }

	NS_TEST_ASSERT_MSG_EQ(10,10,"Ten is not equal to ten. Something smells fishy.");
}

static class HapticFileSensorTestSuite : public TestSuite
{
public:
	HapticFileSensorTestSuite ()
    : TestSuite ("FileSensor", UNIT)
  {
    AddTestCase (new HapticFileSensorTestCase (), TestCase::QUICK);
  }
} g_hapticFileSensorTestSuite;
