#include "display_window.h"
#include "ui_display_window.h"

const size_t cars[] = {26,27,28};

const std::string base_path = "/home/vertensj/Training_data/Cityscapes/Cityscapes_dataset/";
const std::string type = "gtFine";
const std::string split = "train";
const std::string seq_type = "leftImg8bit_sequence";

void display_window::countLabelledImages(const std::string &base_path, const std::string &split, const std::string &city){
    const std::string save_path = base_path + "/motion_data/" + split + "/" + city +"/";
    if(!manager.doesFileExist(save_path)) return;
    std::vector<boost::filesystem::path> list;
    manager.listImagesInDir(save_path, list, ".png");

    size_t labelled_size = list.size();
    size_t total = labelled_files_ids__.size();

    std::string text = std::to_string(labelled_size) + "/" + std::to_string(total);
    ui->count_label->setText(QString::fromStdString(text));
}

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
    if(!loadLabelIfAvailable(current_sequence_, label_frame_id_, current_city, label_image)){
        label_image = cv::imread(label_img.string().c_str(), 0);
        manager.filterImageForIndex(label_image, filter_set);
    }

    label_image_org_ = cv::imread(label_img.string().c_str(), 0);
    manager.filterImageForIndex(label_image_org_, filter_set);

    seq_image = cv::imread(seq_img.string().c_str());
    cv::cvtColor(seq_image, seq_image, CV_BGR2RGB);

    // Display open cv image on view
    cv::Mat blended_image;
    manager.blendImages(label_image, seq_image, blended_image, 0.33);
    displayOpenCvImage(blended_image, Ui::VIEW::TOP);
    displayOpenCvImage(seq_image, Ui::VIEW::BOTTOM);
}

void display_window::listAllLabelImages(const std::vector<boost::filesystem::path> &files, std::vector<boost::filesystem::path> &id_files){
    for(auto i = 0; i < files.size(); i++){
        std::string img_type;
        size_t seq, frame_id;
        manager.separateFileName(labelled_files.at(i).filename().string(), seq, frame_id, img_type);
        if(img_type.compare("labelIds.png") == 0){

            id_files.push_back(labelled_files.at(i));
        }
    }
}

bool display_window::getNextLabelImg(size_t &seq, size_t &frame_id, boost::filesystem::path &img_path){
    if(global_current_frame_ < labelled_files_ids__.size()){
        global_current_frame_++;
        std::string img_type;
        manager.separateFileName(labelled_files_ids__.at(global_current_frame_).filename().string(), seq, frame_id, img_type);

        img_path = labelled_files_ids__.at(global_current_frame_);
        current_seq_frame_id_ = frame_id;
        label_frame_id_ = frame_id;
        current_sequence_ = seq;
        ui->actual_frame->setText(QString::fromStdString(std::to_string(global_current_frame_)));
        return true;

    }
    return false;
}

bool display_window::getPrevLabelImg(size_t &seq, size_t &frame_id, boost::filesystem::path &img_path){
    if(global_current_frame_ > 0){
        global_current_frame_--;
        std::string img_type;
        manager.separateFileName(labelled_files_ids__.at(global_current_frame_).filename().string(), seq, frame_id, img_type);

        img_path = labelled_files_ids__.at(global_current_frame_);
        current_seq_frame_id_ = frame_id;
        label_frame_id_ = frame_id;
        current_sequence_ = seq;
        ui->actual_frame->setText(QString::fromStdString(std::to_string(global_current_frame_)));
        return true;

    }
    return false;
}

void display_window::displayOpenCvImage(const cv::Mat &image, enum Ui::VIEW view_select){

    QImage qt_image= QImage((uchar*) image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);

    switch(view_select){

    case Ui::VIEW::TOP:
        if(top_img_item_ != NULL) delete top_img_item_;
        top_img_item_ = NULL;
        qDeleteAll(scene_top_.items());

        top_img_item_ = new QGraphicsPixmapItem(QPixmap::fromImage(qt_image));
        scene_top_.addItem(top_img_item_);
        break;

    case Ui::VIEW::BOTTOM:

        if(bottom_img_item_ != NULL) delete bottom_img_item_;
        bottom_img_item_ = NULL;
        qDeleteAll(scene_bottom_.items());

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

bool display_window::getCityIndex(const std::string &city_name, size_t &index){
    for(auto i = 0; i < cities.size(); i++){
        if(cities.at(i).filename().string().compare(city_name) == 0){
            index = i;
            return true;
        }
    }
    return false;
}

void display_window::start(const std::string &city, const std::string &split){

    delete_ = true;
    cities.clear();
    labelled_files.clear();
    seq_files.clear();
    global_current_frame_ = 0;
    current_seq_frame_id_ = 0;
    current_sequence_ = 0;
    label_frame_id_= 0;       

    if(!manager.checkTypeAndSplit(base_path, type, split)){ std::cerr << "Type or split does not exist\n";};
    if(!manager.checkTypeAndSplit(base_path, seq_type, split)){ std::cerr << "Type or split does not exist\n";};

    // Get cities
    manager.listAllDirsInPath(base_path + type + "/" + split , cities);

    size_t city_index = 0;
    if(!getCityIndex(city,city_index)){
        std::cerr << "Could not load city: " << city << std::endl;
        return;
    }
    const boost::filesystem::path &city_path = cities.at(city_index);
    current_city = city_path.filename().string();
    std::cout << "Switching to city: " << current_city << std::endl;

    seq_path_ = boost::filesystem::path(base_path + seq_type +"/" + split + "/" + current_city + "/");
    loadFiles(city_path, seq_path_);

    // Get next labeled image
    listAllLabelImages(labelled_files, labelled_files_ids__);

    size_t seq, frame_id;
    boost::filesystem::path img_path;
    std::string img_type;
    manager.separateFileName(labelled_files_ids__.at(global_current_frame_).filename().string(), seq, frame_id, img_type);
    img_path = labelled_files_ids__.at(global_current_frame_);
    current_seq_frame_id_ = frame_id;
    label_frame_id_ = frame_id;
    current_sequence_ = seq;

    ui->actual_frame->setText(QString::fromStdString(std::to_string(global_current_frame_)));

    boost::filesystem::path seq_img_path = getSeqImage(seq, frame_id, current_city, seq_path_);
    displayLabelAndSeq(img_path, seq_img_path, filter_set_);
    countLabelledImages(base_path, split, current_city);
}

display_window::display_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::display_window),
    global_current_frame_(0),
    filter_set_(cars, cars+3),
    draw_thickness_(10)
{
    top_img_item_ = NULL;
    bottom_img_item_ = NULL;

    ui->setupUi(this);

    ui->viewTop->setScene(&scene_top_);
    ui->viewBottom->setScene(&scene_bottom_);
    ui->viewTop->show();
    ui->viewBottom->show();

    ui->count_label->setStyleSheet("QLabel { background-color : gray; color : red; }");
    ui->actual_frame->setStyleSheet("QLabel { background-color : gray; color : blue; }");

    connect(&scene_top_, SIGNAL(click(cv::Point2d)), this,
            SLOT(selectRegion(cv::Point2d)));

    connect(&scene_top_, SIGNAL(rightClick(cv::Point2d)), this,
            SLOT(eraseRegion(cv::Point2d)));

    delete_ = true;

    if(!manager.checkTypeAndSplit(base_path, type, split)){ std::cerr << "Type or split does not exist\n";};
    if(!manager.checkTypeAndSplit(base_path, seq_type, split)){ std::cerr << "Type or split does not exist\n";};

    // Get cities
    manager.listAllDirsInPath(base_path + type + "/" + split , cities);

    for(auto i = 0; i < cities.size(); i++){
        QString city = QString::fromStdString(cities.at(i).filename().string());
        ui->city_select->addItem(city);
    }

    start(cities.front().filename().string(), split);
}

display_window::~display_window()
{
    delete ui;
}

void display_window::updateLabelImage(){
    cv::Mat blended_image;
    manager.blendImages(label_image, seq_image, blended_image, 0.33);
    displayOpenCvImage(blended_image, Ui::VIEW::TOP);
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

bool display_window::loadLabelIfAvailable(size_t seq, size_t frame_id, const std::string &city, cv::Mat &out)
{
    if(isLabelFileAvailable(seq, frame_id, city)){
        ui->labelled_indicator->setStyleSheet("QLabel { background-color : green; color : blue; }");
        loadMotionLabel(seq, frame_id, city, out);
        return true;

    }else{
        ui->labelled_indicator->setStyleSheet("QLabel { background-color : white; color : blue; }");
        return false;
    }
}

bool display_window::isLabelFileAvailable(size_t seq, size_t frame_id, const std::string &city){
    const std::string file_name = city + "_" +std::to_string(seq) + "_" + std::to_string(frame_id) + ".png";
    const std::string save_path = base_path + "/motion_data/" + split + "/" + city +"/";
    return manager.doesFileExist(save_path + file_name);
}

void display_window::loadMotionLabel(size_t seq, size_t frame_id, const std::string &city, cv::Mat &out){
    const std::string file_name = city + "_" +std::to_string(seq) + "_" + std::to_string(frame_id) + ".png";
    const std::string save_path = base_path + "/motion_data/" + split + "/" + city +"/";
    out = cv::imread(save_path + file_name,0);
}

void display_window::saveImage(const cv::Mat &labeling, size_t seq, size_t frame_id, const std::string &city){
    const std::string file_name = city + "_" +std::to_string(seq) + "_" + std::to_string(frame_id) + ".png";

    const std::string save_path = base_path + "/motion_data/" + split + "/" + city +"/";
    boost::filesystem::path dir(save_path);
    if(boost::filesystem::create_directories(dir)) {
        std::cout << "Created save folder" << "\n";
    }

    imwrite( save_path + file_name, labeling );

    QMessageBox msgBox;
    msgBox.setWindowTitle("Saved");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    countLabelledImages(base_path, split, current_city);
}

void display_window::selectRegion(cv::Point2d point){
    std::set<std::pair<unsigned int, unsigned int> > occ_set;
    //manager.growNeighbors(label_image, point.x , point.y, occ_set);
    cv::floodFill(label_image,point,255);

    // Blend modified image with real image and refresh view
    updateLabelImage();

    last_area_ = occ_set;
}

void display_window::eraseRegion(cv::Point2d point){

    if(delete_){
        cv::Scalar white (255, 255, 255);
        cv::circle(label_image, point, draw_thickness_, white, CV_FILLED);
    }else{
        cv::Scalar white (0, 0, 0);
        cv::circle(label_image, point, draw_thickness_, white, CV_FILLED);
    }

    updateLabelImage();
}

void display_window::on_comboBox_activated(const QString &arg1)
{
    if(arg1 == "Delete"){
        delete_ = true;
    }else if(arg1 == "Add"){
        delete_ = false;
    }
}

void display_window::on_draw_thickness_slider_sliderMoved(int position)
{
    draw_thickness_ = position;
}

void display_window::on_saveButton_clicked()
{
    saveImage(label_image, current_sequence_, label_frame_id_, current_city);
}

void display_window::on_city_select_activated(const QString &arg1)
{
    start(arg1.toStdString(), split);
}

void display_window::on_reset_button_clicked()
{
    label_image = label_image_org_.clone();
    updateLabelImage();
}
