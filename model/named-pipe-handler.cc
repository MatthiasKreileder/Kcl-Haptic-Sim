
#include <cstdlib>
#include "named-pipe-handler.h"
#include "ns3/log.h"
namespace ns3{

NS_LOG_COMPONENT_DEFINE ("NamedPipeHandler");

NamedPipeHandler::NamedPipeHandler(std::string& ns3ToChai3D, std::string& chai3DToNs3, bool isInit){

    NS_LOG_DEBUG("Invoking NamedPipeHandler constructor: " << ns3ToChai3D << "\n" << chai3DToNs3 << "\n" << "isInit: " << isInit);

    if(isInit){
        /*
        * Open the pipe to send
        */
        m_tx_pipe = open(ns3ToChai3D.c_str(), O_WRONLY);
        if(m_tx_pipe == -1){
        	NS_LOG_DEBUG("Opening named pipe failed: Does the file maybe already exist? Please remove it before starting the simulation");
            exit(0);
        }

        NS_LOG_DEBUG("[NS-3] " <<"Opened a pipe " << ns3ToChai3D);

        /*
        * open pipe to receive
        */
        m_rx_pipe = open(chai3DToNs3.c_str(), O_RDONLY);
        if(m_rx_pipe == -1){
        	NS_LOG_DEBUG("Opening named pipe failed: Does the file maybe already exist? Please remove it before starting the simulation");
            exit(-1);
        }

        NS_LOG_DEBUG("Opened a pipe " << chai3DToNs3);
    }
    else{
        /*
         * open pipe to receive
         */

         m_rx_pipe = open(ns3ToChai3D.c_str(), O_RDONLY);
         if (m_rx_pipe == -1) {
            NS_LOG_DEBUG("[NS-3] " << "Opening named pipe failed: " << ns3ToChai3D);
         }
         NS_LOG_DEBUG("[NS-3] " <<"Openend pipe: " << ns3ToChai3D);

         m_tx_pipe = open(chai3DToNs3.c_str(), O_WRONLY);
         if(m_rx_pipe == -1){
            NS_LOG_DEBUG("Opening named pipe failed");
            exit(-1);
         }

         NS_LOG_DEBUG("Opened a pipe " << chai3DToNs3);

    }


}

void NamedPipeHandler::SafeWrite(std::string s){
NS_LOG_FUNCTION(this << "Message: " << s);
    size_t messageBytes = s.length();
    if (messageBytes > 100){
        return;
    }

    /*
     * Write the string character by character into writeBuffer
     */
    char writeBuffer[101];
    writeBuffer[100] = '\0';
    for(size_t i = 0; i < messageBytes; i++){
        writeBuffer[i] = s[i];
    }

    /*
     * Add Padding
     */
    for(int i = messageBytes; i < 100; i++){
        writeBuffer[i] = '#';
    }

    size_t tot = 0;

    while(tot < sizeof(writeBuffer)){
        int num = write(m_tx_pipe,writeBuffer + tot,sizeof(writeBuffer)- tot);

        if (num < 0){
            exit(1);
        }
        tot += num;
    }

}

void NamedPipeHandler::SafeRead(std::string& s){
NS_LOG_FUNCTION(this);

    char readBuffer[101];

    size_t tot = 0;
    while(tot < sizeof(readBuffer)){
        int num = read(m_rx_pipe,readBuffer + tot,sizeof(readBuffer)- tot);

        if (num < 0){
            exit(1);
        }
        tot += num;
    }

    std::string rawString = std::string(readBuffer);

    std::string::size_type msgEndIndex = rawString.find('#');
    if(msgEndIndex == std::string::npos){
        exit(1);
    }

    s = rawString.substr(0,msgEndIndex);
    NS_LOG_DEBUG(this << "Received message: " << s);
}

}
