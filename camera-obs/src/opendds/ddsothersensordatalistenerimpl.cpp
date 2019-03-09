#include "ddsothersensordatalistenerimpl.h"

void DdsOtherSensorDataListenerImpl::on_requested_deadline_missed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedDeadlineMissedStatus& /*status*/) {
    cout << "on_requested_deadline_missed" << endl;
}

void DdsOtherSensorDataListenerImpl::on_requested_incompatible_qos(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedIncompatibleQosStatus& /*status*/) {
    cout << "on_requested_incompatible_qos" << endl;
}

void DdsOtherSensorDataListenerImpl::on_sample_rejected(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleRejectedStatus& /*status*/) {
    cout << "on_sample_rejected" << endl;
}

void DdsOtherSensorDataListenerImpl::on_liveliness_changed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::LivelinessChangedStatus& /*status*/) {
    cout << "on_liveliness_changed" << endl;
}


void DdsOtherSensorDataListenerImpl::setReceivedTopicFunction(std::function<void (const SkyworthMsgs::ObsData &)> topicFunc) {
    mTopicFunc = topicFunc;
}

void DdsOtherSensorDataListenerImpl::on_data_available(DDS::DataReader_ptr reader) {
    SkyworthMsgs::ObsDataDataReader_var reader_i = SkyworthMsgs::ObsDataDataReader::_narrow(reader);

    if (!reader_i) {
        cerr << "ERROR : on_data_available() - _narrow failed!" << endl;
        ACE_OS::exit(-1);
    }

    SkyworthMsgs::ObsData data;
    DDS::SampleInfo info;

    DDS::ReturnCode_t status = reader_i->take_next_sample(data, info);
    if (status == DDS::RETCODE_OK) {
        cout << "SampleInfo.sample_rank = " << info.sample_rank << endl;
        cout << "SampleInfo.instance_state = " << info.instance_state << endl;
        if (info.valid_data) {
            mTopicFunc(data);
        }
    } else {
        cerr << "ERROR : on_data_available() - take_next_sample failed!" << endl;
    }
}

void DdsOtherSensorDataListenerImpl::on_subscription_matched(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SubscriptionMatchedStatus& /*status*/) {
    cout << "on_subscription_matched" << endl;
}

void DdsOtherSensorDataListenerImpl::on_sample_lost(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleLostStatus& /*status*/) {
    cout << "on_sample_lost" << endl;
}


