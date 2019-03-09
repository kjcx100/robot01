#include "othersensordata.h"

OtherSensorData::OtherSensorData(int argc, char *argv[], string nodeName) {
    topicName = "/other_sensor_data";
    ros::init(argc, argv, nodeName, ros::init_options::NoSigintHandler);
    signal(SIGINT, OtherSensorData::SigintHandler);
}

void OtherSensorData::SigintHandler(int sig) {
    ros::shutdown();
    exit(0);
}


void OtherSensorData::initPublisher() {
    if(ros::master::check()) {
        nodeHandle = new ros::NodeHandle();
        pub = nodeHandle->advertise<sensor_msgs::PointCloud>(topicName, 1000, true);
    }
}

void OtherSensorData::publishMsg(geometry_msgs::Point32* points, int length) {
    if(pub) {
        pointCloud.header.frame_id = "/other_sensor";
        pointCloud.header.stamp = ros::Time::now();
        for(int i = 0; i < length; i++) {
            pointCloud.points.push_back(points[i]);
        }
        pub.publish(pointCloud);
    }
}

void OtherSensorData::publishMsg(sensor_msgs::PointCloud pc) {
    if(pub) {
        pub.publish(pc);
    }
}

void OtherSensorData::initSubscriber() {
    if(ros::master::check()) {
        nodeHandle = new ros::NodeHandle();
        sub = nodeHandle->subscribe(topicName, 1000, OtherSensorData::callback);
        ros::spin();
    }
}


void OtherSensorData::callback(const sensor_msgs::PointCloud::ConstPtr& msg) {

}
