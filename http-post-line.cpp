
#include<string.h>  
#include<unistd.h>  

#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <math.h>

//callback function
size_t post_write_data(void *ptr, size_t size, size_t nmemb, void *stream) 
{
    std::string data((const char*) ptr, (size_t) size * nmemb);

    std::cout << data << std::endl;

    return size * nmemb;
}

//POST json
void post_in_curl(std::string json_str)
{
    CURL *curl= curl_easy_init();
    CURLcode res;
    if (NULL==curl)
    {
    	std::cout << "init curl error" << std::endl;
        return;
    }
    std::string strUrl="http://192.168.31.200:8080/otherSensorData";
    std::string postret;
    std::string strPostData = json_str;
    curl_easy_setopt(curl,CURLOPT_URL,strUrl.c_str());
    curl_easy_setopt(curl,CURLOPT_POST, 1);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&postret);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, post_write_data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPostData.c_str());
    res = curl_easy_perform(curl);
    
    /* Check for errors */ 
    if(res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

    /* always cleanup */ 
    curl_easy_cleanup(curl);
}

void post_othersensor_data(uint16_t* data)
{
	Json::Value item;
	Json::Value arrayObj;

	double x,y,z,qx,qy,qz,qw;
	double angle;
	float rx[360] = {1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5};
	float ry[360] = {0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5};
	float px[360],py[360],pz[360];
	int i=0;
	float distance = 0;
	std::string out_str;
	
	for(i=0;i < 360; i++)
	{
		//每度一个点，从0度开始到359度
		distance = data[i];//(data[i]/cos((3.14159*i)/180))/1000.0;
		rx[i] = distance * cos((3.14159*i)/180);
		ry[i] = distance * sin((3.14159*i)/180);
	
		px[i] = rx[i];
		py[i] = ry[i];
		pz[i] = 0;
		if(data[i] < 3000)
		{
			item["x"] = ((int)(px[i]*100))/100.0;
			item["y"] = ((int)(py[i]*100))/100.0;
			item["z"] = ((int)(pz[i]*100))/100.0;
			arrayObj.append(item);
		}
	}

	out_str = arrayObj.toStyledString();

	out_str = "msg=" + out_str;

	post_in_curl(out_str);
}

//	g++ -o xxx  xxx.cpp -ljson -lm -lcurl -fno-stack-protector

