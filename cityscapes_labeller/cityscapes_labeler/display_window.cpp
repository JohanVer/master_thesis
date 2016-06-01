#include "display_window.h"
#include "ui_display_window.h"

const size_t cars[] = {26,27,28};

const std::string base_path = "/home/vertensj/Training_data/Cityscapes/Cityscapes_dataset/";
const std::string type = "gtFine";
const std::string split = "train";
const std::string seq_type = "leftImg8bit_sequence";

void display_window::loadFiles(const boost::filesystem::path &city, const boost::filesystem::path &seq_dir){
    // list images in labelled dir
    labelled_files.clear();
    manager.listImagesInDir(city.string(), labelled_files, ".png");
    std::sort (labelled_files.begin(), labelled_files.end());
    assert(labelled_files.size() > 0);

    // list images in sequence dir
    seq_files.clear();
    manager.listImagesInDir(seq_dir.string(), seq_files, ".png");
    std::sort (seq_files.begin(), seq_files.end());
    assert(seq_files.size() > 0);
}

void display_window::displayLabelAndSeq(const boost::filesystem::path &label_img, const boost::filesystem::path &seq_img, const std::set<size_t> filter_set){
    // Filter label image
    cv::Mat label_image = cv::imread(label_img.string().c_str(), 0);
    manager.filterImageForIndex(label_image, filter_set);

    cv::Mat seq_image = cv::imread(seq_img.string().c_str());
    cv::cvtColor(seq_image, seq_image, CV_BGR2RGB);

    // Display open cv image on view
    cv::Mat blended_image;
    manager.blendImages(label_image, seq_image, blended_image, 0.33);
    displayOpenCvImage(blended_image, Ui::VIEW::TOP);
    displayOpenCvImage(seq_image, Ui::VIEW::BOTTOM);
}

bool display_window::getNextLabelImg(size_t &seq, size_t &frame_id, boost::filesystem::path &img_path){
    while(global_current_frame_ < labelled_files.size()){
        global_current_frame_++;
        std::string img_type;
        manager.separateFileName(labelled_files.at(global_current_frame_).filename().string(), seq, frame_id, img_type);
        if(img_type.compare("labelIds.png") == 0){
            img_path = labelled_files.at(global_current_frame_);
            current_seq_frame_id_ = frame_id;
            current_sequence_ = seq;
            return true;
        }
    }
    return false;
}

bool display_window::getPrevLabelImg(size_t &seq, size_t &frame_id, boost::filesystem::path &img_path){
    while(global_current_frame_ > 0){
        global_current_frame_--;
        std::string img_type;
        manager.separateFileName(labelled_files.at(global_current_frame_).filename().string(), seq, frame_id, img_type);
        if(img_type.compare("labelIds.png") == 0){
            img_path = labelled_files.at(global_current_frame_);
            current_seq_frame_id_ = frame_id;
            current_sequence_ = seq;
            return true;
        }
    }
    return false;
}

void display_window::displayOpenCvImage(const cv::Mat &image, enum Ui::VIEW view_select){
    QImage qt_image= QImage((uchar*) image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);

    switch(view_select){

    case Ui::VIEW::TOP:
        top_img_item_ = new QGraphicsPixmapItem(QPixmap::fromImage(qt_image));
        scene_top_.addItem(top_img_item_);
        break;

    case Ui::VIEW::BOTTOM:
        QImage img_scaled = qt_image.scaled(ui->viewBottom->width(), ui->viewBottom->height(), Qt::IgnoreAspectRatio);
        bottom_img_item_ = new QGraphicsPixmapItem(QPixmap::fromImage(img_scaled));
        scene_bottom_.addItem(bottom_img_item_);
        break;
    }
}

boost::filesystem::path display_window::getSeqImage(size_t seq, size_t frame_id, const std::string &city, const boost::filesystem::path &seq_path){
    std::string seq_image_name;
    if(!manager.genCorrSeqImageName(seq, frame_id, city, seq_image_name)){ std::cerr << "Generating corr seq name failed...\n";};
    const boost::filesystem::path seq_img_path(seq_path.string() + seq_image_name);
    if(!manager.doesFileExist(seq_img_path.string())) { std::cerr << "Finding corr seq file failed...\n";};
    return seq_img_path;
}

display_window::display_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::display_window),
    global_current_frame_(0),
    filter_set_(cars, cars+3)
{
    ui->setupUi(this);

    ui->viewTop->setScene(&scene_top_);
    ui->viewBottom->setScene(&scene_bottom_);
    ui->viewTop->show();
    ui->viewBottom->show();

    connect(&scene_top_, SIGNAL(click(cv::Point2d)), this,
            SLOT(selectRegion(cv::Point2d)));

    if(!manager.checkTypeAndSplit(base_path, type, split)){ std::cerr << "Type or split does not exist\n";};
    if(!manager.checkTypeAndSplit(base_path, seq_type, split)){ std::cerr << "Type or split does not exist\n";};

    // Get cities
    manager.listAllDirsInPath(base_path + type + "/" + split , cities);

    const boost::filesystem::path &city_path = cities.front();
    current_city = city_path.filename().string();
    std::cout << "Switching to city: " << current_city << std::endl;
    seq_path_ = boost::filesystem::path(base_path + seq_type +"/" + split + "/" + current_city + "/");
    loadFiles(city_path, seq_path_);

    // Get next labeled image
    size_t seq, frame_id;
    boost::filesystem::path img_path;
    getNextLabelImg(seq, frame_id, img_path);

    boost::filesystem::path seq_img_path = getSeqImage(seq, frame_id, current_city, seq_path_);
    displayLabelAndSeq(img_path, seq_img_path, filter_set_);
}

display_window::~display_window()
{
    delete ui;
}

void display_window::keyPressEvent(QKeyEvent * event){
    if( event->key() == Qt::Key_6 )
    {
        if(current_seq_frame_id_ < 28){
            current_seq_frame_id_++;

            const boost::filesystem::path seq_img_path = getSeqImage(current_sequence_, current_seq_frame_id_, current_city, seq_path_);

            cv::Mat seq_image = cv::imread(seq_img_path.string().c_str());
            cv::cvtColor(seq_image, seq_image, CV_BGR2RGB);
            displayOpenCvImage(seq_image, Ui::VIEW::BOTTOM);
        }
    }

    if( event->key() == Qt::Key_4 )
    {
        if(current_seq_frame_id_ >  0){
            current_seq_frame_id_--;

            const boost::filesystem::path seq_img_path = getSeqImage(current_sequence_, current_seq_frame_id_, current_city, seq_path_);

            cv::Mat seq_image = cv::imread(seq_img_path.string().c_str());
            cv::cvtColor(seq_image, seq_image, CV_BGR2RGB);
            displayOpenCvImage(seq_image, Ui::VIEW::BOTTOM);
        }
    }

    if( event->key() == Qt::Key_8 )
    {
        size_t seq, frame_id;
        boost::filesystem::path img_path;
        if(getNextLabelImg(seq, frame_id, img_path)){
            boost::filesystem::path seq_img_path = getSeqImage(seq, frame_id, current_city, seq_path_);
            displayLabelAndSeq(img_path, seq_img_path, filter_set_);
        }
    }

    if( event->key() == Qt::Key_2 )
    {
        size_t seq, frame_id;
        boost::filesystem::path img_path;
        if(getPrevLabelImg(seq, frame_id, img_path)){
            boost::filesystem::path seq_img_path = getSeqImage(seq, frame_id, current_city, seq_path_);
            displayLabelAndSeq(img_path, seq_img_path, filter_set_);
        }
    }
}

void display_window::selectRegion(cv::Point2d point){
    std::cerr << "Selected Point : " << point << std::endl;
}
