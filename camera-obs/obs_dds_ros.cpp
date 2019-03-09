#include "ddsothersensordata.h"
#include<iostream>
#include "othersensordata.h"

#include <chrono>
#include <thread>

using namespace std;

OtherSensorData *pub;
sensor_msgs::PointCloud pc;
geometry_msgs::Point32 point32;

//opendds get the data, then send to ros
void receivedData(const SkyworthMsgs::ObsData& msg) {
    pc.points.clear();
    for(int i = 0; i < 360; i++) {
        point32.x = msg.pointList[i].x;
        point32.y = msg.pointList[i].y;
        point32.z = msg.pointList[i].z;
        pc.points.push_back(point32);
    }

    pub->publishMsg(pc);
}

int main(int argc, char* argv[]) {
    pub = new OtherSensorData(argc, argv, "other_sensor");
    pub->initPublisher();
    pc.header.frame_id = "/other_sensor";
    pc.header.stamp = ros::Time::now();

    DdsOtherSensorData sub(argc, argv);
    sub.initSubscriber();
    sub.setReceivedTopicFunction(receivedData);
    const auto sleepTime { 1s };
    while (true) {
        this_thread::sleep_for(sleepTime);
    }
    return 0;
}
