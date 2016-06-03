#include "cityscapes_widgets.h"

namespace cityscapes_labeller{

CityScapesScene::CityScapesScene(QWidget *parent) :
    QGraphicsScene(parent) {
    pressed_ = false;
    draw_thickness_ = 20;
}

void CityScapesScene::setDrawColor(unsigned char r, unsigned char g, unsigned char b){
    draw_color_.setRed(r);
    draw_color_.setGreen(g);
    draw_color_.setBlue(b);
}

void CityScapesScene::sendCirclePositions(){
    std::vector<cv::Point2d> circles;
    for(auto i = 0; i < temp_circles_.size(); i++){
        const QRectF rect = temp_circles_.at(i)->rect();
        cv::Point2d circle(rect.x() + draw_thickness_/2  , rect.y() + draw_thickness_/2);
        circles.push_back(circle);
    }
    Q_EMIT sendCircles(circles);
}

void CityScapesScene::insertCircle(size_t x, size_t y, QColor color, size_t radius){
    size_t tl_c_x = x - (radius/2);
    size_t tl_c_y = y - (radius/2);

    temp_circles_.push_back(new QGraphicsEllipseItem(tl_c_x, tl_c_y, radius, radius));
    (temp_circles_.back())->setBrush(QBrush(color));
    (temp_circles_.back())->setPen(QPen(color, 1, Qt::SolidLine));
    this->addItem(temp_circles_.back());
}

void CityScapesScene::deleteAllCircleItems(){
    for(auto i = 0; i < temp_circles_.size(); i++){
        this->removeItem(temp_circles_.at(i));
        delete temp_circles_.at(i);
    }
    temp_circles_.clear();
}

void CityScapesScene::mousePressEvent(QGraphicsSceneMouseEvent* event){
    size_t x = event->scenePos().x();
    size_t y = event->scenePos().y();
    cv::Point2d point(x,y);
    if(event->button() == Qt::LeftButton){
        Q_EMIT click(point);
    }else if(event->button() == Qt::RightButton){
        pressed_ = true;
        insertCircle(x, y, draw_color_, draw_thickness_);
        //Q_EMIT rightClick(point);
    }
}

void CityScapesScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event){
    int x = event->scenePos().x();
    int y = event->scenePos().y();
    if(pressed_){
        insertCircle(x, y, draw_color_, draw_thickness_);
    }else{
        //update(QRectF(x, y, draw_thickness_,draw_thickness_));
    }

}

void CityScapesScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(pressed_){
        sendCirclePositions();
        deleteAllCircleItems();
        pressed_ = false;
    }

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
