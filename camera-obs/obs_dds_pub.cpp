#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddsothersensordata.h"

int main(int argc, char* argv[]) {
    DdsOtherSensorData pub(argc, argv);
    pub.initPublisher();
    sleep(1);
    pub.sendData();
    return 0;
}
