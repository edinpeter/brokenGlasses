#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <ctime>
#include "geometry_msgs/Vector3.h"
#include "std_msgs/Float32.h"
#include <math.h>
#include </home/peter/brokenglasses/src/image_testing/libs/RiptideVision.h>
//#include <RiptideVision.h>
#include <vector>

#define PI 3.14159

using namespace cv;
using namespace std;

Mat src; Mat src_gray, canny_output;
int thresh = 100;
int max_thresh = 255;

//0 to 180 for opencv
int hLow = 0 / 2;
int hUp = 80 / 2;

// 0 to 255 for opencv
int sLow = 170;
int sUp = 255;

//0 to 255 for oopencv
int vLow = 110;
int vUp = 255;

class ImageConverter
{
    ros::NodeHandle nh;
    image_transport::ImageTransport it;
    image_transport::Subscriber image_sub;
    image_transport::Publisher image_pub;
    ros::Subscriber imu_sub;
    ros::Subscriber pitch_sub;
    ros::Subscriber roll_sub;

public:
    ImageConverter()
    : it(nh)
    {
        image_sub = it.subscribe("/stereo/left/image_rect_color/compressed", 1,
            &ImageConverter::imageCb, this);
        image_pub = it.advertise("/img/line_follower", 1);

    }
    ~ImageConverter()
    {}
    void imageCb(const sensor_msgs::ImageConstPtr& msg)
    {
        cv_bridge::CvImagePtr cv_ptr;
        try
        {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }

       // ROS_INFO("%s","Here");
        Mat image = cv_ptr->image;

        Mat M;
        Point q;
        RiptideVision::linePoint q2;
        RiptideVision::linePoint q3;

        RiptideVision::colorAverage(image,YELLOWS,q);


        RiptideVision::seperateColors(image,YELLOWS,M);
        

        //RiptideVision::orientation(image,YELLOWS,q,q3);
        RiptideVision::orientationv3(image,YELLOWS,q2, M);

        line(M,q3.top,q3.bot,Scalar(255,255,0),2);

		//

        //ROS_INFO("%s","Published");
        
        cv_ptr->image = M;
        image_pub.publish(cv_ptr->toImageMsg());

    }
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "line_follower");
    ImageConverter ic;
    ros::spin();
    return 0;
}
