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

Q_SIGNALS:
void click(cv::Point2d point);

public Q_SLOTS:
void mousePressEvent(QGraphicsSceneMouseEvent *event);
void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
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
