

#include "no-data-reduction-algorithm.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (NoDataReductionAlgorithm);

TypeId
NoDataReductionAlgorithm::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NoDataReductionAlgorithm")
    .SetParent<DataReductionAlgorithm> ()
    //.SetGroupName("Applications")
    .AddConstructor<NoDataReductionAlgorithm> ()
//    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
//                   UintegerValue (9),
//                   MakeUintegerAccessor (&UdpEchoServer::m_port),
//                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

NoDataReductionAlgorithm::NoDataReductionAlgorithm() {
	// TODO Auto-generated constructor stub

}

NoDataReductionAlgorithm::~NoDataReductionAlgorithm() {
	// TODO Auto-generated destructor stub
}

bool NoDataReductionAlgorithm::NeedsToBeTransmitted (SensorDataSample sds){
	return true;
}

} /* namespace ns3 */
