#include "cityscapes_widgets.h"

namespace cityscapes_labeller{

CityScapesScene::CityScapesScene(QWidget *parent) :
    QGraphicsScene(parent) {

}

void CityScapesScene::mousePressEvent(QGraphicsSceneMouseEvent* event){
    if(event->button() == Qt::LeftButton){
        size_t x = event->scenePos().x();
        size_t y = event->scenePos().y();

        cv::Point2d point(x,y);
        Q_EMIT click(point);
    }
}

void CityScapesScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event){


}

void CityScapesScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){


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

/*
void CityScapesViewer::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_Right )
    {
        std::cerr << "Right key pressed...\n";
    }

    if( event->key() == Qt::Key_A )
    {
        std::cerr << "A key pressed...\n";
    }
}
*/

}
