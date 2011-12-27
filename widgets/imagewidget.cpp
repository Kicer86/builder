/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Michał Walenciak <Kicer86@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QPaintEvent>
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>

#include "imagewidget.hpp"

ImageLayer::ImageLayer(const QString &imagePath):QPixmap(imagePath), renderer(0), path(imagePath)
{
}


ImageLayer::ImageLayer(const QString &imagePath, int w, int h):QPixmap(w, h), renderer(0), path(imagePath)
{
  setAlphaChannel(*this);
  fill(Qt::transparent);

  QPainter painter(this);
  renderer= new QSvgRenderer(path);
  renderer->render(&painter, QRectF(0,0,w,h));

  qDebug() << QString("Loading svg layer with%1 animations (%2 fps)").arg(renderer->animated()? "":"out").arg(renderer->framesPerSecond());
}


ImageLayer::ImageLayer(const QByteArray &): QPixmap(), renderer(0)
{
}


ImageLayer::ImageLayer(const QPixmap &image): QPixmap(image), renderer(0)
{
}


ImageLayer::ImageLayer(const ImageLayer &il): QPixmap(il), renderer(0), path(il.path)
{
  if (il.renderer)
  {
    renderer=new QSvgRenderer;
    renderer->load(path);
  }
}


ImageLayer::~ImageLayer()
{
  delete renderer;
}


QSvgRenderer* ImageLayer::getRenderer() const
{
  return renderer;
}



ImageWidget::ImageWidget(QWidget* p, Qt::WindowFlags f): QWidget(p, f), size(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)
{ }


ImageWidget::~ImageWidget()
{}


void ImageWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  foreach(const ImageLayer *layer, layers)
  {
    if (layer->getRenderer())
      layer->getRenderer()->render(&painter, rect());
    else
      painter.drawPixmap(rect(), *layer, rect());
  }
}


int ImageWidget::addLayer(ImageLayer* layer)
{
  if (layer->getRenderer())
    connect(layer->getRenderer(), SIGNAL(repaintNeeded()), this, /*SLOT(update()*/ SIGNAL(rerender()) );

  if (layer->size().width()<size.width())
    size.setWidth(layer->size().width());

  if (layer->size().height()<size.height())
    size.setHeight(layer->size().height());

  setFixedSize(size);
  return layers.size();
}


int ImageWidget::prependLayer(ImageLayer *layer)
{
  layers.prepend(layer);
  return addLayer(layer);
}


int ImageWidget::appendLayer(ImageLayer *layer)
{
  layers.append(layer);
  return addLayer(layer);
}


void ImageWidget::clear()
{
  layers.clear();
  size.setHeight(QWIDGETSIZE_MAX);
  size.setWidth(QWIDGETSIZE_MAX);
}
