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


#ifndef IMAGEWIDGET_HPP
#define IMAGEWIDGET_HPP

#include <QWidget>
#include <QPixmap>

class QSvgRenderer;

class ImageLayer: public QPixmap
{
    QSize imageSize;
    QSvgRenderer *renderer;
    QString path;

  public:
    ImageLayer(const QString& imagePath);  //Fixed size images
    ImageLayer(const QString& imagePath, int w, int h);  //svg
    ImageLayer(const QByteArray &imageData);
    ImageLayer(const QPixmap &image);      //QImage, QPixmap, QBitmap and QPicture
    ImageLayer(const ImageLayer& il );
    virtual ~ImageLayer();

    QSvgRenderer *getRenderer() const;  
};

class ImageWidget : public QWidget
{  
    QList<ImageLayer*> layers;
    QSize size;
    
  protected:
    virtual void paintEvent(QPaintEvent* );

  public:
    explicit ImageWidget(QWidget* p = 0, Qt::WindowFlags f = 0);
    virtual ~ImageWidget();

    int appendLayer(ImageLayer* layer);
    void clear();
};

#endif // IMAGEWIDGET_HPP
