#ifndef DDSOTHERSENSORDATA_H
#define DDSOTHERSENSORDATA_H

#include <iostream>
#include <string>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>
#include <dds/DCPS/WaitSet.h>
#include "dds/DCPS/StaticIncludes.h"
#include "SkyworthMsgsTypeSupportImpl.h"
#include <ace/Log_Msg.h>
#include "ddsothersensordatalistenerimpl.h"

using namespace std;

class DdsOtherSensorData {
  public:
    DdsOtherSensorData(int argc, char* argv[]);
    ~DdsOtherSensorData();

  public:
    void initPublisher();
    void initSubscriber();
    void sendData();
    void listening();
    void setReceivedTopicFunction(std::function<void (const SkyworthMsgs::ObsData&)> topicFunc);

  private:
    void createParticipant(int argc, char* argv[]);
    void registerTopic();
    void createPublisher();
    void createDataWriter();
    void createSubscriber();
    void createDataReader();


  private:
    DDS::DomainId_t domainID;
    string messageType, messageTopic;

    DDS::DomainParticipantFactory_var factory;
    DDS::DomainParticipant_var participant;
    DDS::Topic_var topic;

    DDS::Publisher_var publisher;
    SkyworthMsgs::ObsDataTypeSupport_var typeSupport;
    DDS::DataWriter_var writer;
    SkyworthMsgs::ObsDataDataWriter_var dataWriter;

    DDS::Subscriber_var subscriber;
    DDS::DataReader_var reader;
    DDS::DataReaderListener_var listener;
    DdsOtherSensorDataListenerImpl *listenerImpl;
};

#endif // DDSOTHERSENSORDATA_H
