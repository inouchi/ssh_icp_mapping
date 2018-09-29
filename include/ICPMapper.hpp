#pragma once

// C++ specific includes
#include <iostream>
#include <vector>
#include <string>

// ROS specific includes
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include "ssh_icp_mapping/GenerateModel.h"
#include "ssh_icp_mapping/RestorePreviousModel.h"

// PCL specific includes
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/approximate_voxel_grid.h>
#include <pcl/segmentation/sac_segmentation.h> 
#include <pcl/filters/extract_indices.h>
#include <pcl/registration/icp.h>
#include <pcl/common/angles.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/statistical_outlier_removal.h>


struct Filter
{
  Filter()
  {
    leafSizes.resize(3);
    minAxis.resize(3);
    maxAxis.resize(3);

    // Default parametrs
    leafSizes[0] = leafSizes[1] = leafSizes [2] = 0.01;
    minAxis[0] = -0.2;  maxAxis[0] = 0.2;
    minAxis[1] = -0.3;  maxAxis[1] = 0.17;
    minAxis[2] =  0.5;  maxAxis[2] = 1.1;
    stddevMulThresh = 1.0;
  }

  std::vector<float> leafSizes;
  std::vector<float> minAxis;
  std::vector<float> maxAxis;
  float stddevMulThresh;
};


struct ICPParameter
{
  ICPParameter()
  {
    // Default parameters
    maxIterations = 100;
    maxCorrespondenceDistance = 0.1;
    transformationEpsilon = 1e-6;
    euclideanFitnessEpsilon = 1.0;
  }

  int maxIterations;
  float maxCorrespondenceDistance;
  float transformationEpsilon;
  float euclideanFitnessEpsilon;
};


class ICPMapper
{

  public:
    ICPMapper(ros::NodeHandle* nodeHandlePtr, ros::NodeHandle* localNodeHandlePtr);
    ~ICPMapper();


  protected:
    void cloudCb(const sensor_msgs::PointCloud2ConstPtr& input);
    void matching(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr input,
                  const pcl::PointCloud<pcl::PointXYZRGB>::Ptr target,
                  pcl::PointCloud<pcl::PointXYZRGB>::Ptr output);
    void publishPointCloud(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& cloud, const std::string& frameID);
    void filtering(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& input, pcl::PointCloud<pcl::PointXYZRGB>::Ptr& output, const Filter& filter);
    void rotatePointCloud(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr& input, pcl::PointCloud<pcl::PointXYZRGB>::Ptr& output, const float degree);
    void print4x4Matrix(const Eigen::Matrix4f& matrix);
    bool generateModel(ssh_icp_mapping::GenerateModel::Request& req, ssh_icp_mapping::GenerateModel::Response& res);
    bool restorePreviousModel(ssh_icp_mapping::RestorePreviousModel::Request& req, ssh_icp_mapping::RestorePreviousModel::Response& res);


  private:  
    ros::NodeHandle* nodeHandlePtr_;
    ros::NodeHandle* localNodeHandlePtr_;

    ros::Subscriber    cloudSub_;                 // For subscribing a cloud data from Kinect
    ros::Publisher     modelPub_;                 // For publishing a model generated by ICP
    ros::ServiceServer generateModelSrv_;         // It is called, and generate a model using ICP and publish the model
    ros::ServiceServer restorePreviousModelSrv_;  // It is called, and publish a previous model, then set currentModel_ to previousModel_

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr originalCloud_;      // Keep up-to-date a cloud data from Kinect
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr previousIcpResult_;  // Keep previous result of ICP processing
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr currentModel_;       // Keep up-to-date a model generated by ICP
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr previousModel_;      // Keep previous a model generated by ICP

    Filter       filter_;   // For determing scope of point cloud used in ICP 
    ICPParameter icpParam_;
    float        distance_; // For using in the function of rotatePointCloud

};