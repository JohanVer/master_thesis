#include "CImg.h"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <algorithm>
#include <set>

#define SAVE
//#define SHOW

using namespace cimg_library;

const size_t cars[] = {268,269,287,274,271,292,290,289,272,291,295,297,273,286,288,296,298,276,293,280,294,284,281,277,278,279,282,275,270,285,283};
const size_t moving_cars[] = {268,269,273,284,282,286,285,283};

bool listImagesInDir(const std::string &path, std::vector<std::string> &list){
    if (!path.empty())
    {        namespace fs = boost::filesystem;

        fs::path apk_path(path);
        fs::recursive_directory_iterator end;

        for (fs::recursive_directory_iterator i(apk_path); i != end; ++i)
        {
            const fs::path cp = (*i);
            list.push_back(cp.string());
        }
    }
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

int main() {

    const std::string folder_name = "35mm_focallength/scene_forwards/slow/left/";

    std::string im_path("/home/vertensj/Training_data/Blender/object_index/" + folder_name);
    //std::string im_rendered_path("/home/vertensj/Training_data/Blender/frames_finalpass/" + folder_name); // Finalpass
    std::string im_rendered_path("/home/vertensj/Training_data/Blender/frames_cleanpass/" + folder_name); // Cleanpass

    std::set<size_t> car_set (cars, cars+32);
    std::set<size_t> moving_car_set (moving_cars, moving_cars+8);

    // load object index images
    std::vector<std::string> object_file_list;
    listImagesInDir(im_path, object_file_list);
    std::sort (object_file_list.begin(), object_file_list.end());

    // load rendered images
    std::vector<std::string> rendered_file_list;
    listImagesInDir(im_rendered_path, rendered_file_list);
    std::sort (rendered_file_list.begin(), rendered_file_list.end());

    std::cout << "Object index size: " << object_file_list.size() << " rendered size: " << rendered_file_list.size() << std::endl;
    assert(object_file_list.size() == rendered_file_list.size());

    for(auto i = 0; i < object_file_list.size(); i++){
        std::cout << "File name nr: " << i << " is: " << object_file_list.at(i) << std::endl;
    }

    const char *const keycode_next = "ARROWRIGHT";
    const char *const keycode_esc = "ESC";
    const char *const keycode_enter = "ENTER";

#ifdef SHOW
    CImgDisplay disp;
    disp.show_mouse();
    for(auto i = 0; i < object_file_list.size(); i++){
        std::cout << "Displaying image nr: " << i << std::endl;
        // get object index image
        CImg<unsigned int> object_image(object_file_list.at(i).c_str());
        // filter image for cars
        //filterImageForIndex<unsigned int>(object_image, car_set);  // all cars
        filterImageForIndex<unsigned int>(object_image, moving_car_set);  // moving cars

        // get rendered image
        CImg<unsigned int> rendered_image(rendered_file_list.at(i).c_str());

        // create image list
        CImgList<unsigned char> list;
        list.push_back<unsigned char>(object_image);
        list.push_back<unsigned char>(rendered_image);

        // show object index image and rendered image
        disp.display(list, 'y');
        while (!disp.is_closed()) {

            if(disp.button()&1){
                int x = disp.mouse_x();
                int y = disp.mouse_y();
                const unsigned int index = object_image(x,y);
                std::cout << "Current index: " << std::to_string(index) << std::endl;
                disp.wait(100);
            }

            if (disp.is_key(keycode_next)) {
                break;
            }
            if(disp.is_key(keycode_esc)) return 0;

        }
        disp.wait(50);
    }

#endif

#ifdef SAVE

    const std::string save_folder = "/home/vertensj/Training_data/Blender/filtered_images/"  + folder_name;

    boost::filesystem::path dir(save_folder);
    if(boost::filesystem::create_directories(dir)) {
        std::cout << "Created save folder" << "\n";
    }

    for(auto i = 0; i < object_file_list.size(); i++){
        std::cout << "Saving image nr: " << i << " of " << object_file_list.size() <<  std::endl;
        CImg<unsigned int> object_image(object_file_list.at(i).c_str());
        // filter image for cars
        //filterImageForIndex<unsigned int>(object_image, car_set);  // all cars
        filterImageForIndex<unsigned int>(object_image, moving_car_set);  // moving cars
        object_image.save((save_folder + "img" + std::to_string(i) +".bmp").c_str());
    }

#endif

    return 0;
}
