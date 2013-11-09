#include "LifeGrid.h"
#include <QtGui>

LifeGrid::LifeGrid(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    curColor = Qt::black;
    zoom = 8;
    image = QImage(16, 16, QImage::Format_ARGB32);
    image.fill(qRgba(0, 0, 0, 0));
}

QSize LifeGrid::sizeHint() const
{
    QSize size = zoom * image.size();
    if (zoom >= 3)
        size += QSize(1, 1);
    return size;
}

void LifeGrid::setPenColor(const QColor &newColor) {
    curColor = newColor;
}

void LifeGrid::setIconImage(const QImage &newImage) {
    if (newImage != image) {
        image = newImage.convertToFormat(QImage::Format_ARGB32); update();
        updateGeometry();
    }
}

void LifeGrid::setZoomFactor(int newZoom)
{
    if (newZoom < 1)
        newZoom = 1;
    if (newZoom != zoom)
    {
        zoom = newZoom;
        update();
        updateGeometry();
    }
}

void LifeGrid::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (zoom >= 3)
    {
        painter.setPen(palette().foreground().color());
        for (int i = 0; i <= image.width(); ++i)
            painter.drawLine(zoom * i, 0, zoom * i, zoom * image.height());
            for (int j = 0; j <= image.height(); ++j)
                painter.drawLine(0, zoom * j, zoom * image.width(), zoom * j);
    }

    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            QRect rect = pixelRect(i, j);
            if (!event->region().intersect(rect).isEmpty()) {
                QColor color = QColor::fromRgba(image.pixel(i, j));
                painter.fillRect(rect, color);
            }
        }
    }
}

QRect LifeGrid::pixelRect(int i, int j) const
{
    if (zoom >= 3) {
        return QRect(zoom * i + 1, zoom * j + 1, zoom - 1, zoom - 1);
    }else{
        return QRect(zoom * i, zoom * j, zoom, zoom);
    }
}

void LifeGrid::setImagePixel(const QPoint &pos, bool opaque) {
    int i = pos.x() / zoom; int j = pos.y() / zoom;
    if (image.rect().contains(i, j)) {
        if (opaque) {
            image.setPixel(i, j, penColor().rgba());
        }else{
            image.setPixel(i, j, qRgba(0, 0, 0, 0));
        }
            update(pixelRect(i, j));
    }
}
