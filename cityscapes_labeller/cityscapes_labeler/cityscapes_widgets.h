#ifndef CITYSCAPES_WIDGETS_H
#define CITYSCAPES_WIDGETS_H

#include <QtWidgets>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsObject>
#include <QColor>
#include <iostream>

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


class CityScapesScene: public QGraphicsScene {
    Q_OBJECT
public:
    explicit CityScapesScene(QWidget *parent = 0);

    void setDrawThickness(size_t pixel){
        draw_thickness_ = pixel*2;
    }

    void insertCircle(size_t x, size_t y, QColor color, size_t radius);
    void deleteAllCircleItems();
    void setDrawColor(unsigned char r, unsigned char g, unsigned char b);
    void sendCirclePositions();

Q_SIGNALS:
    void click(cv::Point2d point);
    void rightClick(cv::Point2d point);
    void sendCircles(std::vector<cv::Point2d> circles);

public Q_SLOTS:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    std::vector<QGraphicsEllipseItem *> temp_circles_;
    bool pressed_;
    size_t draw_thickness_;
    QColor draw_color_;
};

class CityScapesViewer: public QGraphicsView {
    Q_OBJECT
public:
    explicit CityScapesViewer(QWidget *parent = 0);
    CityScapesViewer();
    virtual ~CityScapesViewer();
    //void keyPressEvent(QKeyEvent *event);

Q_SIGNALS:


public Q_SLOTS:
    virtual void wheelEvent(QWheelEvent * event);

private:
};
}

#endif // CITYSCAPES_WIDGETS_H
