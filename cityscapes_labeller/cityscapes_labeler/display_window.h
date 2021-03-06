#ifndef DISPLAY_WINDOW_H
#define DISPLAY_WINDOW_H

#include <QMainWindow>
#include <QtAlgorithms>
#include <QtGui>
#include <QPixmap>
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QKeyEvent>
#include <iostream>
#include <cityscapes_manager.h>
#include <cityscapes_widgets.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <map>

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

namespace Ui {

enum VIEW{
    TOP,
    BOTTOM
};

class display_window;
}

class display_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit display_window(QWidget *parent = 0);

    void keyPressEvent(QKeyEvent * event);
    ~display_window();

    void loadFiles(const boost::filesystem::path &city, const boost::filesystem::path &seq_dir);

    bool getNextLabelImg(size_t &seq, size_t &frame_id, boost::filesystem::path &img_path);

    bool getPrevLabelImg(size_t &seq, size_t &frame_id, boost::filesystem::path &img_path);

    void displayOpenCvImage(const cv::Mat &image, enum Ui::VIEW view_select);

    boost::filesystem::path getSeqImage(size_t seq, size_t frame_id, const std::string &city, const boost::filesystem::path &seq_path);

    bool loadLabelIfAvailable(size_t seq, size_t frame_id, const std::string &city, cv::Mat &out);

    void displayLabelAndSeq(const boost::filesystem::path &label_img, const boost::filesystem::path &seq_img, const std::map<unsigned char, cv::Scalar> &filter_set);

    void countLabelledImages(const std::string &base_path, const std::string &split, const std::string &city);

    bool isLabelFileAvailable(size_t seq, size_t frame_id, const std::string &city);

    void saveImage(const cv::Mat &labeling, size_t seq, size_t frame_id, const std::string &city);

    void loadMotionLabel(size_t seq, size_t frame_id, const std::string &city, cv::Mat &out);

    void start(const std::string &city, const std::string &split);

    bool getCityIndex(const std::string &city_name, size_t &index);

    void updateLabelImage();

    void listAllLabelImages(const std::vector<boost::filesystem::path> &files, std::vector<boost::filesystem::path> &id_files);

    void setupIndexMap();


public Q_SLOTS:
    void selectRegion(cv::Point2d point);
    void eraseRegion(std::vector<cv::Point2d> circles);

private slots:

    void on_comboBox_activated(const QString &arg1);

    void on_draw_thickness_slider_sliderMoved(int position);

    void on_saveButton_clicked();

    void on_city_select_activated(const QString &arg1);

    void on_reset_button_clicked();

    void on_label_type_combo_activated(const QString &arg1);

private:
    Ui::display_window *ui;

    cv::Mat label_image_org_;
    cv::Mat label_image;
    cv::Mat seq_image;
    cv::Mat color_coded_;

    // Undo variables
    bool last_action_was_area_;
    cv::Mat last_area_;
    unsigned char last_area_label_;


    cityscapes_labeller::cityscapes_manager manager;
    std::vector<boost::filesystem::path> cities;
    std::vector<boost::filesystem::path> labelled_files;
    std::vector<boost::filesystem::path> labelled_files_ids__;
    std::vector<boost::filesystem::path> seq_files;

    boost::filesystem::path seq_path_;

    size_t global_current_frame_;
    size_t current_seq_frame_id_, current_sequence_, label_frame_id_;
    std::string current_city;

    cityscapes_labeller::CityScapesScene scene_top_, scene_bottom_;
    QGraphicsPixmapItem*  top_img_item_;
    QGraphicsPixmapItem*  bottom_img_item_;

    std::set<size_t> filter_set_;

    std::map<unsigned char, cv::Scalar> index_color_map_;
    std::map<std::string, unsigned char > index_string_map_;

    size_t draw_thickness_;
    unsigned char draw_label_;
    unsigned char draw_label_org_;
    bool delete_;
    bool draw_static_;
};

#endif // DISPLAY_WINDOW_H
