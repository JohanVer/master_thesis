#include "cityscapes_widgets.h"

namespace cityscapes_labeller{

CityScapesScene::CityScapesScene(QWidget *parent) :
    QGraphicsScene(parent) {
    pressed_ = false;

}

void CityScapesScene::mousePressEvent(QGraphicsSceneMouseEvent* event){
    size_t x = event->scenePos().x();
    size_t y = event->scenePos().y();
    cv::Point2d point(x,y);
    if(event->button() == Qt::LeftButton){
        Q_EMIT click(point);
    }else if(event->button() == Qt::RightButton){
        pressed_ = true;
        Q_EMIT rightClick(point);
    }
}

void CityScapesScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event){
    if(pressed_){
        size_t x = event->scenePos().x();
        size_t y = event->scenePos().y();
        cv::Point2d point(x,y);
        Q_EMIT rightClick(point);
    }

}

void CityScapesScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(pressed_) pressed_ = false;

}


CityScapesViewer::CityScapesViewer() {
    // TODO Auto-generated constructor stub

}

CityScapesViewer::~CityScapesViewer() {
    // TODO Auto-generated destructor stub
}

CityScapesViewer::CityScapesViewer(QWidget *parent) :
    QGraphicsView(parent) {

}

void CityScapesViewer::wheelEvent(QWheelEvent * event) {
    //if ctrl pressed, use original functionality
    if (event->modifiers() & Qt::ControlModifier) {
        QGraphicsView::wheelEvent(event);
    }

    else {
        this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        if (event->delta() > 0) {
            scale(1.2, 1.2);
        } else {
            scale(0.8, 0.8);
        }
    }
}

}
