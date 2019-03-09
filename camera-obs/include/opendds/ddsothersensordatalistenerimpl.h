#ifndef DDSOTHERSENSORDATALISTENERIMPL_H
#define DDSOTHERSENSORDATALISTENERIMPL_H


#include <ace/Global_Macros.h>
#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/LocalObject.h>
#include <dds/DCPS/Definitions.h>
#include <SkyworthMsgsC.h>

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>
#include "SkyworthMsgsTypeSupportC.h"
#include "SkyworthMsgsTypeSupportImpl.h"
#include <iostream>

using namespace std;

class DdsOtherSensorDataListenerImpl: public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener> {
  public:
    virtual void on_requested_deadline_missed(DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus& status);
    virtual void on_requested_incompatible_qos(DDS::DataReader_ptr reader, const DDS::RequestedIncompatibleQosStatus& status);
    virtual void on_sample_rejected(DDS::DataReader_ptr reader, const DDS::SampleRejectedStatus& status);
    virtual void on_liveliness_changed(DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus& status);
    virtual void on_data_available(DDS::DataReader_ptr reader);
    virtual void on_subscription_matched(DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus& status);
    virtual void on_sample_lost(DDS::DataReader_ptr reader, const DDS::SampleLostStatus& status);
    void setReceivedTopicFunction(std::function<void (const SkyworthMsgs::ObsData&)> topicFunc);

  private:
    std::function<void (const SkyworthMsgs::ObsData&)> mTopicFunc;
};

#endif // DDSOTHERSENSORDATALISTENERIMPL_H
