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

#include <tr1/memory>

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
        Q_OBJECT

    public:
        typedef std::tr1::shared_ptr<ImageLayer> ImageLayerPtr;

        explicit ImageWidget(QWidget *p = 0, Qt::WindowFlags f = 0);
        virtual ~ImageWidget();

        int prependLayer(ImageLayerPtr layer);
        int appendLayer(ImageLayerPtr layer);
        void clear();

    signals:
        //TODO: add some option to turn on automatic repaint
        void rerender();             //signal emited when widget requires to be updated

    protected:
        virtual void paintEvent(QPaintEvent *);

    private:
        QSize size;

        QList<ImageLayerPtr> layers;
        int addLayer(ImageLayerPtr layer);
};

#endif // IMAGEWIDGET_HPP
