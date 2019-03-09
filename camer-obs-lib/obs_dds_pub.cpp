#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddsothersensordata.h"

int main(int argc, char* argv[]) {
	for(int a = 0;a < argc ;a++)
		printf("argc==%d, argv[]==%s\n",a, argv[a]);
    DdsOtherSensorData pub(argc, argv);
    pub.initPublisher();
    sleep(1);
    pub.sendData();
    return 0;
}
