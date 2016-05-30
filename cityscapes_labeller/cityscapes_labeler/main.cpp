#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <algorithm>
#include <set>

// Opencv
#include <opencv2/core.hpp>
#define cimg_plugin1 "cvMat.h"
#include "CImg.h"

#include <opencv2/imgproc.hpp>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/image_encodings.h>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
using namespace cimg_library;

const char *const keycode_next = "ARROWRIGHT";
const char *const keycode_before = "ARROWLEFT";
const char *const keycode_up = "ARROWUP";
const char *const keycode_down = "ARROWDOWN";
const char *const keycode_esc = "ESC";
const char *const keycode_enter = "ENTER";


const size_t cars[] = {26,27,28};

bool listImagesInDir(const std::string &path, std::vector<boost::filesystem::path> &paths){
    if (!path.empty())
    {
        boost::filesystem::path apk_path(path);
        boost::filesystem::recursive_directory_iterator end;

        for (boost::filesystem::recursive_directory_iterator i(apk_path); i != end; ++i)
        {
            if(boost::filesystem::is_regular_file(*i)){
                const boost::filesystem::path cp = (*i);
                paths.push_back(cp);
            }
        }
    }
}

bool listAllDirsInPath(const std::string &path, std::vector<boost::filesystem::path> &paths){
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

bool isInPathVector(const std::vector<boost::filesystem::path> &in, std::string query){
    for(auto i = 0; i < in.size(); i++){
        if((in.at(i).filename().string()).compare(query) == 0){
            return true;
        }
    }
    return false;
}

void separateFileName(const std::string &filename, size_t &seq, size_t &frame_id, std::string &type){
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

bool checkTypeAndSplit(const std::string &base_path,const std::string &type, const std::string &split){
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

bool genCorrSeqImageName(size_t seq, size_t frame_id, const std::string &city_string, std::string &filename){
    // Search corresponding sequence image
    std::stringstream seq_string;
    seq_string << std::setw(6) << std::setfill('0') << seq;

    std::stringstream frame_id_string;
    frame_id_string << std::setw(6) << std::setfill('0') << frame_id;

    filename = city_string + "_"  + (seq_string).str() + "_" +(frame_id_string).str() + "_" +  "leftImg8bit.png";

    return true;
}

bool doesFileExist(const std::string &path){
    if ( !boost::filesystem::exists( path) )
    {
        std::cerr << "Can't find corresponding seq file!" << std::endl;
        return false;
    }
    return true;
}

template<typename T>
void filterImageForIndex(CImg<T> &inout, const std::set<size_t> &index){
    for(auto i = 0; i < inout.size(); i++){
        if(index.find((size_t)inout(i)) == index.end()){
            inout(i) = 255;
        }else{
            inout(i) = 0;
        }
    }
}

template<typename T>
void blendImages(const CImg<T> &im1, const CImg<T> &im2, CImg<T> &out, double alpha){
    cv::Mat label_cv =  im1.get_MAT();
    cv::cvtColor(label_cv, label_cv, cv::COLOR_GRAY2BGR);
    cv::Mat seq_cv =  im2.get_MAT();
    cv::Mat blended_cv;
    double beta = ( 1.0 - alpha );
    cv::addWeighted( label_cv, alpha, seq_cv, beta, 0.0, blended_cv);
    CImg<T> blended_image(blended_cv);
    out = blended_image;
}

template<typename T>
void findNeighbors(CImg<T> inout, size_t x, size_t y, std::set<std::pair<size_t, size_t> > &occ_set){
    if(occ_set.find(std::pair<size_t, size_t>(x,y)) != occ_set.end()){
        return;
    }

    occ_set.insert(std::pair<size_t, size_t>(x,y));

    if(x+1 < inout.width()){
        if((T)inout(x+1,y) == 0){
            findNeighbors(inout, x+1, y, occ_set);
        }
    }

    if(x > 0){
        if((T)inout(x-1,y) == 0){
            findNeighbors(inout, x-1, y, occ_set);
        }
    }

    if(y+1 < inout.height()){
        if((T)inout(x,y+1) == 0){
            findNeighbors(inout, x, y+1, occ_set);
        }
    }

    if(y >0){
        if((T)inout(x,y-1) == 0){
            findNeighbors(inout, x, y-1, occ_set);
        }
    }
}

template<typename T>
void growNeighbors(CImg<T> inout, size_t x, size_t y){
    std::set<std::pair<size_t, size_t> > occ_set;
    findNeighbors(inout, x, y, occ_set);
    for(auto i = occ_set.begin(); i != occ_set.end(); i++){
        size_t x = i->first;
        size_t y = i->second;
        inout(x, y) = 255;
        std::cout << "Grow point: " << x << " / " << y << std::endl;
    }
}

int main(int argc, char *argv[])
{

    const std::string base_path = "/home/vertensj/Training_data/Cityscapes/Cityscapes_dataset/";
    const std::string type = "gtFine";
    const std::string split = "train";
    const std::string seq_type = "leftImg8bit_sequence";

    std::set<size_t> car_set (cars, cars+3);

    if(!checkTypeAndSplit(base_path, type, split)) return 1;
    if(!checkTypeAndSplit(base_path, seq_type, split)) return 1;

    // Create display
    CImgDisplay disp;
    disp.show_mouse();

    // Get cities
    std::vector<boost::filesystem::path> cities;
    listAllDirsInPath(base_path + type + "/" + split , cities);
    for(auto i = 0; i < cities.size(); i++){
        const boost::filesystem::path &city = cities.at(i);
        const std::string city_string = city.filename().string();
        std::cout << "Switching to city: " << city_string << std::endl;

        // list images in labelled dir
        std::vector<boost::filesystem::path> labelled_files;
        listImagesInDir(city.string(), labelled_files);
        std::sort (labelled_files.begin(), labelled_files.end());
        assert(labelled_files.size() > 0);

        // list images in sequence dir
        std::vector<boost::filesystem::path> seq_files;
        const std::string seq_dir = base_path + seq_type +"/" + split + "/" + city_string + "/";
        listImagesInDir(seq_dir, seq_files);
        std::sort (seq_files.begin(), seq_files.end());
        assert(seq_files.size() > 0);

        // loop through images of city x
        for(auto f_i = 0; f_i < labelled_files.size(); f_i++){
            size_t seq, frame_id;
            std::string img_type;
            separateFileName(labelled_files.at(f_i).filename().string(), seq, frame_id, img_type);
            if((labelled_files.at(i).extension().string()).compare(".png") == 0 && img_type.compare("labelIds.png") == 0){

                std::cout << "Current image: " << labelled_files.at(f_i).filename().string() << std::endl;

                // Search corresponding sequence image
                std::string seq_image_name;
                if(!genCorrSeqImageName(seq, frame_id, city_string, seq_image_name)) return 1;
                if(!doesFileExist(seq_dir + seq_image_name)) return 1;

                // Load images
                CImg<unsigned char> label_image(labelled_files.at(f_i).string().c_str());
                filterImageForIndex<unsigned char>(label_image, car_set);
                CImg<unsigned char> seq_image((seq_dir + seq_image_name).c_str());
                // Blend images
                CImg<unsigned char> blended_image(seq_image);
                blendImages<unsigned char>(label_image, seq_image, blended_image, 0.33);

                // create image list
                CImgList<unsigned char> list;
                list.push_back<unsigned char>(label_image);
                list.push_back<unsigned char>(seq_image);

                // Display images
                disp.display(list, 'y');

                size_t dyn_frame_id = frame_id;

                while (!disp.is_closed()) {

                    if(disp.button()&1){
                        double x = (double) disp.mouse_x() / (double) disp.width();
                        double y = (double) disp.mouse_y() / ((double) disp.height() /2) ;

                        size_t x_proj = x * (double) label_image.width();
                        size_t y_proj = y * (double) label_image.height();

                        std::cout << "Mouse coord: " << x_proj << "/" << y_proj << std::endl;
                        growNeighbors<unsigned char>(label_image, x_proj, y_proj);
                        const unsigned char index = label_image(x_proj, y_proj);
                        //label_image.display();
                        std::cout << "Index: " << std::to_string(index) << std::endl;
                        disp.wait(100);
                    }

                    if (disp.is_key(keycode_up)) {
                        break;
                    }

                    if(disp.is_key(keycode_before)){
                        if(dyn_frame_id){
                            dyn_frame_id -= 1;
                            list.clear();
                            list.push_back<unsigned char>(blended_image);
                            std::string prev_file;
                            if(!genCorrSeqImageName(seq, dyn_frame_id, city_string, prev_file)) return 1;
                            if(!doesFileExist((seq_dir + prev_file))) return 1;
                            CImg<unsigned int> seq_image((seq_dir + prev_file).c_str());
                            list.push_back<unsigned char>(seq_image);
                            disp.display(list, 'y');
                        }
                    }

                    if(disp.is_key(keycode_next)){
                        if(dyn_frame_id < 28){
                            dyn_frame_id += 1;
                            list.clear();
                            list.push_back<unsigned char>(blended_image);
                            std::string prev_file;
                            if(!genCorrSeqImageName(seq, dyn_frame_id, city_string, prev_file)) return 1;
                            if(!doesFileExist((seq_dir + prev_file))) return 1;
                            CImg<unsigned int> seq_image((seq_dir + prev_file).c_str());
                            list.push_back<unsigned char>(seq_image);
                            disp.display(list, 'y');
                        }
                    }

                    if(disp.is_key(keycode_esc)) return 0;

                }
                disp.wait(50);
            }
        }
    }

    return 0;
}
