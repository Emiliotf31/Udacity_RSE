#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z){

    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
	ROS_ERROR("Failed to call service");

}

void process_image_callback(const sensor_msgs::Image img){

    int white_pixel = 255;
    int left_section = 800%img.step; //800
    int right_section = 1600%img.step; //1600
    int actual_section = 0;
    
    // The loop is checking which are the white pixels.
    for (int i=0; i < img.height*img.step; i+=3){
	if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel){
	   // Knowing in wich section is the actual white pixel
	    actual_section = i%img.step;
	}
	   // Here we will have just 3 sections (left, forward, right) 
	   //with the ranges of the residuals (0-800,801-1599,1599-2400)
	if (actual_section > 0 && actual_section <= left_section){
	    ROS_INFO_STREAM("LEFT");
	    drive_robot(0.5, 0.8);
	    break;
	}
	else if (actual_section > left_section && actual_section < right_section){
	    ROS_INFO_STREAM("FORWARD");
	    drive_robot(0.5, 0.0);
	    break;
	}
	else if (actual_section >= right_section){
	    ROS_INFO_STREAM("RIGHT");
	    drive_robot(0.5, -0.8);
	    break;
	}
    }
}


int main(int argc, char** argv){

    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw",10,process_image_callback);

    ros::spin();

    return 0;

}
