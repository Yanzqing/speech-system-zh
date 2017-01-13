#include "../../lib/audio_bace/include/AudioDeviceBace.h"
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"

class RosAudioCapture{
public:
	RosAudioCapture(){

		//使用参数解析器，初始化块大小 200ms
        ros::param::param<int>("~block_size", _size, 512);
        ros::param::param<int>("~channels", _channle, 1);
        ros::param::param<int>("~depth", _sample, 16);
        ros::param::param< int>("~sample_rate", _rate, 16000);
        ros::param::param<std::string>("~name", _name, "plughw:2,0");


        printf("\n========================================================\n"
					"      block_size: %d \n"
					"      channels: %d\n"
					"      depth: %d\n"
					"      sample_rate: %d\n"
					"      device_name: %s"
        		   "\n=========================================================\n"
        		,_size,_channle,_sample,_rate,_name.c_str());

        _dev.config(_name,_channle,_sample,_rate,MODE_RECPRDER,SND_PCM_ACCESS_RW_INTERLEAVED);
        _data = new int16_t[_size];
        //发布消息，缓存空间 50，true 表示其它节点个同步接受信息。
        _pub = _nh.advertise<audio_msgs::AudioData>("audio_data", 50, true);
        audio_msgs::AudioData msg;
        //重新消息定义数组大小
       msg.data.resize(_size);

	   msg.data_size = _size;
        uint64_t count = 0;
        while(ros::ok())
        {
			_dev.read(_data,_size);
			std::memcpy(&msg.data[0],_data,_size*2);
			_pub.publish(msg);
			ROS_INFO_THROTTLE(60, "Audio capture, everything is OK ! Have got audio data block [%d]",count);
        	count++;
			ros::spinOnce();
        }
	}
	~RosAudioCapture(){
		delete []_data;
	}

private:
    ros::NodeHandle _nh;
    ros::Publisher _pub;
    AudioDeviceBace _dev;
    int16_t* _data;
    int  _size;
    std::string _name;
    int  _channle,_sample, _rate;
};


int main (int argc, char **argv)
{
  ros::init(argc, argv, "audio_capture");
  RosAudioCapture server;
}
