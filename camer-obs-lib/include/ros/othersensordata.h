#ifndef OTHERSENSORDATA_H
#define OTHERSENSORDATA_H

#include <iostream>
#include "csignal"
#include "ros/ros.h"
#include "sensor_msgs/PointCloud.h"

using namespace std;

class OtherSensorData
{
public:
    OtherSensorData(int argc, char* argv[], string nodeName);
    static void SigintHandler(int sig);
    void initPublisher();
    void publishMsg(geometry_msgs::Point32* points, int length);
    void publishMsg(sensor_msgs::PointCloud pc);
    void initSubscriber();
    static void callback(const sensor_msgs::PointCloud::ConstPtr& msg);

  private:
    ros::NodeHandle *nodeHandle;
    ros::Publisher pub;
    sensor_msgs::PointCloud pointCloud;
    ros::Subscriber sub;
    string topicName;
};



#endif // OTHERSENSORDATA_H
