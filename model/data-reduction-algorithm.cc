

#include "data-reduction-algorithm.h"

namespace ns3 {

TypeId
DataReductionAlgorithm::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DataReductionAlgorithm")
    .SetParent<Object> ()
    //.SetGroupName("Network")
//    .AddAttribute ("StartTime", "Time at which the application will start",
//                   TimeValue (Seconds (0.0)),
//                   MakeTimeAccessor (&Application::m_startTime),
//                   MakeTimeChecker ())
//    .AddAttribute ("StopTime", "Time at which the application will stop",
//                   TimeValue (TimeStep (0)),
//                   MakeTimeAccessor (&Application::m_stopTime),
//                   MakeTimeChecker ())
  ;
  return tid;
}

DataReductionAlgorithm::DataReductionAlgorithm() {
	// TODO Auto-generated constructor stub

}

DataReductionAlgorithm::~DataReductionAlgorithm() {
	// TODO Auto-generated destructor stub
}

} /* namespace ns3 */
