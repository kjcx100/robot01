
#include <qmath.h>
#include "ddsothersensordata.h"

DdsOtherSensorData::DdsOtherSensorData(int argc, char* argv[]) {
    domainID = 2222;
    messageTopic = "ObsDataTopic";
    messageType = "ObsDataType";
    createParticipant(argc, argv);
    registerTopic();
}

DdsOtherSensorData::~DdsOtherSensorData() {
    try {
        if(!CORBA::is_nil(participant.in())) {
            participant->delete_contained_entities();
        }
        if(!CORBA::is_nil(factory.in())) {
            factory->delete_participant(participant);
        }
    } catch (CORBA::Exception& e) {
        cerr << "Exception caught in cleanup : " << e << endl;
    }

    TheServiceParticipant->shutdown();
}

void DdsOtherSensorData::createParticipant(int argc, char* argv[]) {
    // Initialize DomainParticipantFactory
    factory = TheParticipantFactoryWithArgs(argc, argv);

    // Create DomainParticipant
    participant = factory->create_participant(domainID,
                  PARTICIPANT_QOS_DEFAULT,
                  DDS::DomainParticipantListener::_nil(),
                  OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil(participant.in())) {
        cerr << "ERROR : Failed to create participant!" << endl;
    }
}

void DdsOtherSensorData::registerTopic() {
    // Register TypeSupport
    typeSupport = new SkyworthMsgs::ObsDataTypeSupportImpl;

    if (typeSupport->register_type(participant.in(), messageType.c_str()) != DDS::RETCODE_OK) {
        cerr << "ERROR : register type for " << messageType << " failed." << endl;
    }

    // Create Topic
    topic = participant->create_topic(messageTopic.c_str(),
                                      messageType.c_str(),
                                      TOPIC_QOS_DEFAULT,
                                      DDS::TopicListener::_nil(),
                                      OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (CORBA::is_nil(topic.in())) {
        cerr << "ERROR : register topic for " << messageTopic << " failed." << endl;
    }
}

void DdsOtherSensorData::initPublisher() {
    createPublisher();
    createDataWriter();
}

void DdsOtherSensorData::createPublisher() {
    // Create Publisher
    publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                DDS::PublisherListener::_nil(),
                OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (CORBA::is_nil(publisher.in())) {
        cerr << "ERROR : create publisher failed." << endl;
    }
}

void DdsOtherSensorData::createDataWriter() {
    // Create DataWriter
    writer = publisher->create_datawriter(topic.in(),
                                          DATAWRITER_QOS_DEFAULT,
                                          DDS::DataWriterListener::_nil(),
                                          OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (CORBA::is_nil(writer.in())) {
        cerr << "ERROR : create datawriter for " << messageTopic << " failed." << endl;
    }

    dataWriter = SkyworthMsgs::ObsDataDataWriter::_narrow(writer);

    if (CORBA::is_nil(dataWriter.in())) {
        cerr << "ERROR : datawriter could not be narrowed." << endl;
    }
}

void DdsOtherSensorData::sendData( uint16_t* indata) {
    SkyworthMsgs::ObsData data;
	float rx[360];
	float ry[360];
	float distance = 0;
    //data.pointList[0].x = 1;
    //data.pointList[1].x = 2;
    //data.pointList[2].x = 3;
    //data.pointList[3].x = 4;
    for(int i=0; i < 360; i++)
	{
		//每度一个点，从0度开始到359度
        distance = (indata[i]/100.0) + 0.3;//(data[i]/cos((3.14159*i)/180))/1000.0;
		rx[i] = distance * cos((3.14159*i)/180);
		ry[i] = distance * sin((3.14159*i)/180);
	
		data.pointList[i].x = rx[i];
		data.pointList[i].y = ry[i];
		data.pointList[i].z = 0;
		
	}
    DDS::ReturnCode_t status = dataWriter->write(data, DDS::HANDLE_NIL);
    if (status != DDS::RETCODE_OK) {
        cerr << "ERROR : write data returned " << status << endl;
    }
}


void DdsOtherSensorData::initSubscriber() {
    createSubscriber();
    createDataReader();
}

void DdsOtherSensorData::createSubscriber() {
    // Create Subscriber
    subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                 DDS::SubscriberListener::_nil(),
                 OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (CORBA::is_nil(subscriber.in())) {
        cerr << "ERROR : create subscriber failed." << endl;
    }
}

void DdsOtherSensorData::setReceivedTopicFunction(std::function<void (const SkyworthMsgs::ObsData &)> topicFunc) {
    listenerImpl->setReceivedTopicFunction(topicFunc);
}

void DdsOtherSensorData::createDataReader() {
    // Create DataReader
    listener = new DdsOtherSensorDataListenerImpl;
    if (CORBA::is_nil (listener.in ())) {
        cerr << "ERROR : ExchangeEvent listener is nil." << endl;
    }
    listenerImpl = dynamic_cast<DdsOtherSensorDataListenerImpl*>(listener.in());

    reader = subscriber->create_datareader(topic.in(),
                                           DATAREADER_QOS_DEFAULT,
                                           listener.in(),
                                           OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (CORBA::is_nil(reader.in())) {
        cerr << "ERROR : create datareader for " << messageTopic << " failed." << endl;
    }

    SkyworthMsgs::ObsDataDataReader_var reader_i = SkyworthMsgs::ObsDataDataReader::_narrow(reader);

    if (CORBA::is_nil(reader_i.in())) {
        cerr << "ERROR : datareader could not be narrowed." << endl;
    }

}

void DdsOtherSensorData::listening() {
    // Block until Publisher completes
    DDS::StatusCondition_var condition = reader->get_statuscondition();
    condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);

    DDS::WaitSet_var ws = new DDS::WaitSet;
    ws->attach_condition(condition);

    while (true) {
        DDS::SubscriptionMatchedStatus matches;
        if (reader->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
            cerr << "ERROR : get_subscription_matched_status failed!" << endl;
        }

        if (matches.current_count == 0 && matches.total_count > 0) {
            break;
        }

        DDS::ConditionSeq conditions;
        DDS::Duration_t timeout = { 60, 0 };
        if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
            cerr << "ERROR : wait failed!" << endl;
        }
    }

    ws->detach_condition(condition);
}
