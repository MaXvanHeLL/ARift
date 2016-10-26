/**
* This file is part of LSD-SLAM.
*
* Copyright 2013 Jakob Engel <engelj at in dot tum dot de> (Technical University of Munich)
* For more information see <http://vision.in.tum.de/lsdslam>
*
* LSD-SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* LSD-SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with LSD-SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <Windows.h>
#include "io_wrapper\output_3d_wrapper.h"
#include <vector>
#include <opencv2/core/core.hpp>

#define LsdSlam_Threshold 0.00008
#define LsdSlam_ScaleFactor 5 // 20
#define LsdSlam_UNDISTORTION 0
#define LsdSlam_Translation 1
#define LsdSlam_Rotation 0

namespace lsd_slam
{

	class Frame;
	class KeyFrameGraph;

	struct KeyFrameMessage
	{
		int id;
		float time;
		bool isKeyframe;

		// camToWorld as serialization of sophus sim(3).
		// may change with keyframeGraph - updates.
		Sophus::Sim3f camToWorld;


		// camera parameter(fx fy cx cy), width, height
		// will never change, but required for display.
		float fx;
		float fy;
		float cx;
		float cy;
		unsigned int height;
		unsigned int width;


		// data as InputPointDense(float idepth, float idepth_var, uchar color[4]), width x height
		// may be empty, in that case no associated pointcloud is ever shown.
		//InputPointDense pointcloud;

	};


	struct InputPointDense
	{
		float idepth;
		float idepth_var;
		unsigned char color[4];
	};

	struct GraphConstraint
	{
		int from;
		int to;
		float err;
	};

	struct GraphFramePose
	{
		int id;
		float camToWorld[7];
	};

	/** Addition to LiveSLAMWrapper for ROS interoperability. */
	class LsdSlam3D : public Output3DWrapper
	{
	public:
		struct RiftPosition3D
		{
			float x;
			float y;
			float z;
		};
		HANDLE riftPosition3DMutex_;
		HANDLE riftRotationMatrixMutex_;

		// initializes cam-calib independent stuff
		LsdSlam3D(int width, int height);
		~LsdSlam3D();

		virtual void publishKeyframeGraph(KeyFrameGraph* graph);

		// publishes a keyframe. if that frame already existis, it is overwritten, otherwise it is added.
		virtual void publishKeyframe(Frame* f);

		// published a tracked frame that did not become a keyframe (i.e. has no depth data)
		virtual void publishTrackedFrame(Frame* f);

		// publishes graph and all constraints, as well as updated KF poses.
		virtual void publishTrajectory(std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> trajectory, std::string identifier);

		virtual void publishTrajectoryIncrement(const Eigen::Matrix<float, 3, 1>& pt, std::string identifier);

		virtual void publishDebugInfo(const Eigen::Matrix<float, 20, 1>& data);

		// Oculus Rift Real-World coordinates
		RiftPosition3D* getRiftPosition3D();
		Eigen::MatrixXd* getRiftRotation3D();

		int publishLvl;

	private:
		int width, height;

		std::string liveframe_channel;
		//ros::Publisher liveframe_publisher;

		std::string keyframe_channel;
		//ros::Publisher keyframe_publisher;

		std::string graph_channel;
		//ros::Publisher graph_publisher;

		std::string debugInfo_channel;
		//ros::Publisher debugInfo_publisher;


		std::string pose_channel;
		//ros::Publisher pose_publisher;

		cv::Mat tracker_display;

		//ros::NodeHandle nh_;

		// added
		struct RiftPosition3D* riftPosition3D_;
		Eigen::MatrixXd riftRotationMatrix_;
	};
}
