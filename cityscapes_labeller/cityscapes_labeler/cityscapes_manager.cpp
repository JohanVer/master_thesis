#include "cityscapes_manager.h"

namespace cityscapes_labeller{

using namespace std;

cityscapes_manager::cityscapes_manager()
{

}

bool cityscapes_manager::listImagesInDir(const std::string &path, std::vector<boost::filesystem::path> &paths, const std::string &extension){
    if (!path.empty())
    {
        boost::filesystem::path apk_path(path);
        boost::filesystem::recursive_directory_iterator end;

        for (boost::filesystem::recursive_directory_iterator i(apk_path); i != end; ++i)
        {
            const boost::filesystem::path cp = (*i);
            if(boost::filesystem::is_regular_file(*i) && cp.extension().string().compare(extension) == 0){
                paths.push_back(cp);
            }
        }
    }
}

bool cityscapes_manager::listAllDirsInPath(const std::string &path, std::vector<boost::filesystem::path> &paths){
    if (!path.empty())
    {        namespace fs = boost::filesystem;

        fs::path apk_path(path);
        fs::directory_iterator end;

        for (fs::directory_iterator i(apk_path); i != end; ++i)
        {
            if(boost::filesystem::is_directory(*i)){
                const fs::path cp = (*i);
                paths.push_back(cp);
            }

        }
    }
}

bool cityscapes_manager::isInPathVector(const std::vector<boost::filesystem::path> &in, std::string query){
    for(auto i = 0; i < in.size(); i++){
        if((in.at(i).filename().string()).compare(query) == 0){
            return true;
        }
    }
    return false;
}

void cityscapes_manager::separateFileName(const std::string &filename, size_t &seq, size_t &frame_id, std::string &type){
    std::string delimiter = "_";

    istringstream ss( filename );
    std::string field;
    std::vector<std::string> separated;
    while (std::getline( ss, field, '_' ))
    {
        separated.push_back(field);
    }

    assert(separated.size() == 5);

    seq = std::stoi(separated.at(1));
    frame_id = std::stoi(separated.at(2));
    type = separated.at(4);
}

bool cityscapes_manager::checkTypeAndSplit(const std::string &base_path,const std::string &type, const std::string &split){
    // List types
    std::vector<boost::filesystem::path> types;
    listAllDirsInPath(base_path, types);
    if(!isInPathVector(types, type)){
        std::cerr << "Type is not in given directory...\n";
        return false;
    }

    // List splits
    std::vector<boost::filesystem::path> splits;
    listAllDirsInPath(base_path + type , splits);
    if(!isInPathVector(splits, split)){
        std::cerr << "Split is not in given directory...\n";
        return false;
    }
    return true;
}

bool cityscapes_manager::genCorrSeqImageName(size_t seq, size_t frame_id, const std::string &city_string, std::string &filename){
    // Search corresponding sequence image
    std::stringstream seq_string;
    seq_string << std::setw(6) << std::setfill('0') << seq;

    std::stringstream frame_id_string;
    frame_id_string << std::setw(6) << std::setfill('0') << frame_id;

    filename = city_string + "_"  + (seq_string).str() + "_" +(frame_id_string).str() + "_" +  "leftImg8bit.png";

    return true;
}

bool cityscapes_manager::doesFileExist(const std::string &path){
    if ( !boost::filesystem::exists( path) )
    {
        return false;
    }
    return true;
}

void cityscapes_manager::blendImages(const cv::Mat &im1, const cv::Mat &im2, cv::Mat &out, double alpha){
    cv::Mat label_cv =  im1;
    cv::Mat seq_cv =  im2;
    cv::Mat blended_cv;
    double beta = ( 1.0 - alpha );
    cv::addWeighted( label_cv, alpha, seq_cv, beta, 0.0, blended_cv);
    out = blended_cv;
}

void cityscapes_manager::createColorCodedLabels(const cv::Mat &labels, cv::Mat &color_coded, std::map<unsigned char, cv::Scalar> map){

    for(auto y = 0; y < labels.rows; y++){
        for(auto x = 0; x < labels.cols; x++){
            unsigned char label = labels.at<uchar>(y, x);
            if(map.find(label) !=  map.end()){
                cv::Scalar color = map.at(label);
                (color_coded.at<cv::Vec3b>(y, x))[0] = color[0];
                (color_coded.at<cv::Vec3b>(y, x))[1] = color[1];
                (color_coded.at<cv::Vec3b>(y, x))[2] = color[2];
            }else{
                (color_coded.at<cv::Vec3b>(y, x))[0] = 255;
                (color_coded.at<cv::Vec3b>(y, x))[1] = 255;
                (color_coded.at<cv::Vec3b>(y, x))[2] = 255;
            }
        }
    }
}

void cityscapes_manager::filterImageForIndex(cv::Mat &inout, const std::map<unsigned char, cv::Scalar> &index){
    for(auto y = 0; y < inout.rows; y++){
        for(auto x = 0; x < inout.cols; x++){
            unsigned char pixel = inout.at<uchar>(y, x);

            // Create index
            if(index.find((size_t)pixel) == index.end()){
                inout.at<uchar>(y, x) = 255;
            }else{
                inout.at<uchar>(y, x) = pixel;
            }
        }
    }

}

void cityscapes_manager::fillMaskedArea(cv::Mat &inout, const cv::Mat &mask, unsigned char fill){
    for(auto y = 0; y < inout.rows; y++){
        for(auto x = 0; x < inout.cols; x++){
            unsigned char mask_val = mask.at<uchar>(y+1, x+1);
            if(mask_val){
                inout.at<uchar>(y, x) = fill;
            }
        }
    }
}

void cityscapes_manager::fillCircleArea(cv::Mat &inout, const std::vector<cv::Point2d> &circles, unsigned char fill, size_t thickness){
    for(auto i = 0; i < circles.size(); i++){
            cv::circle(inout, circles.at(i), thickness, fill, CV_FILLED);
    }
}


}
