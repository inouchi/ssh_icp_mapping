#pragma once

// C++ specific includes
#include <iostream>
#include <vector>
#include <string>
#include <mutex>

// ROS specific includes
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include "ssh_icp_mapping/GenerateModel.h"

// PCL specific includes
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/approximate_voxel_grid.h>
#include <pcl/segmentation/sac_segmentation.h> 
#include <pcl/filters/extract_indices.h>
#include <pcl/registration/icp.h>


struct Filter
{
  std::vector<float> min;
  std::vector<float> max;

  Filter()
  {
    min.resize(3);
    max.resize(3);
  }
};

class ICPMapper
{

  public:
    ICPMapper(ros::NodeHandle* nodeHandlePtr, ros::NodeHandle* localNodeHandlePtr);
    ~ICPMapper();


  protected:
    void cloudCb(const sensor_msgs::PointCloud2ConstPtr& input);
    void matching(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr input, const pcl::PointCloud<pcl::PointXYZRGB>::Ptr target,
                  pcl::PointCloud<pcl::PointXYZRGB>::Ptr output);
    void publishPointCloud(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, const std::string& frameID);
    bool generateModel(ssh_icp_mapping::GenerateModelRequest& req, ssh_icp_mapping::GenerateModelResponse& res);


  private:  
    ros::NodeHandle* nodeHandlePtr_;
    ros::NodeHandle* localNodeHandlePtr_;

    ros::Subscriber cloudSub_;  // For subscribing a cloud data from Kinect
    ros::Publisher modelPub_;   // For publishing a model generated by ICP
    ros::ServiceServer generateModelSrv_;

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr originalCloud_;  // Keep up-to-date a cloud data from Kinect
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr model_;          // Generated model by ICP


};
