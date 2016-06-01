#ifndef DISPLAY_WINDOW_H
#define DISPLAY_WINDOW_H

#include <QMainWindow>
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
    void displayLabelAndSeq(const boost::filesystem::path &label_img, const boost::filesystem::path &seq_img, const std::set<size_t> filter_set);
    bool isLabelFileAvailable(size_t seq, size_t frame_id, const std::string &city);
    void saveImage(const cv::Mat &labeling, size_t seq, size_t frame_id, const std::string &city);
    void loadMotionLabel(size_t seq, size_t frame_id, const std::string &city, cv::Mat &out);
    void start(const std::string &city, const std::string &split);
    bool getCityIndex(const std::string &city_name, size_t &index);
    void updateLabelImage();

public Q_SLOTS:
    void selectRegion(cv::Point2d point);
    void eraseRegion(cv::Point2d point);

private slots:

    void on_comboBox_activated(const QString &arg1);

    void on_draw_thickness_slider_sliderMoved(int position);

    void on_saveButton_clicked();

    void on_city_select_activated(const QString &arg1);

private:
    Ui::display_window *ui;

    cv::Mat label_image;
    cv::Mat seq_image;

    cityscapes_labeller::cityscapes_manager manager;
    std::vector<boost::filesystem::path> cities;
    std::vector<boost::filesystem::path> labelled_files;
    std::vector<boost::filesystem::path> seq_files;

    boost::filesystem::path seq_path_;

    size_t global_current_frame_;
    size_t current_seq_frame_id_, current_sequence_, label_frame_id_;
    std::string current_city;

    cityscapes_labeller::CityScapesScene scene_top_, scene_bottom_;
    QGraphicsPixmapItem*  top_img_item_;
    QGraphicsPixmapItem*  bottom_img_item_;

    std::set<size_t> filter_set_;

    std::set<std::pair<size_t, size_t> > last_area_;

    size_t draw_thickness_;
    bool delete_;
};

#endif // DISPLAY_WINDOW_H
