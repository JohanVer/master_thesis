#ifndef CITYSCAPES_MANAGER_H
#define CITYSCAPES_MANAGER_H

#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <algorithm>
#include <set>
#include <iomanip>
#include <map>
#include <vector>

// Opencv
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/image_encodings.h>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace cityscapes_labeller{

class cityscapes_manager
{
public:
    cityscapes_manager();

    bool listImagesInDir(const std::string &path, std::vector<boost::filesystem::path> &paths, const std::string &extension);

    bool listAllDirsInPath(const std::string &path, std::vector<boost::filesystem::path> &paths);

    bool isInPathVector(const std::vector<boost::filesystem::path> &in, std::string query);

    void separateFileName(const std::string &filename, size_t &seq, size_t &frame_id, std::string &type);

    bool checkTypeAndSplit(const std::string &base_path,const std::string &type, const std::string &split);

    bool genCorrSeqImageName(size_t seq, size_t frame_id, const std::string &city_string, std::string &filename);

    bool doesFileExist(const std::string &path);

    void blendImages(const cv::Mat &im1, const cv::Mat &im2, cv::Mat &out, double alpha);

    void filterImageForIndex(cv::Mat &inout, const std::map<unsigned char, cv::Scalar> &index);

    void createColorCodedLabels(const cv::Mat &labels, cv::Mat &color_coded, std::map<unsigned char, cv::Scalar> map);

    void fillMaskedArea(cv::Mat &inout, const cv::Mat &mask, unsigned char fill);

    void fillCircleArea(cv::Mat &inout, const std::vector<cv::Point2d> &circles, unsigned char fill, size_t thickness);

};

}
#endif // CITYSCAPES_MANAGER_H
