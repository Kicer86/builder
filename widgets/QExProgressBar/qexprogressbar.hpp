
#ifndef QEXPROGRESSBAR_HPP
#define QEXPROGRESSBAR_HPP


//class which extends QProgrssBar by posibility of storing and restoring it's content

#include <QProgressBar>

// data
class QExProgressBarProxy: public QObject
{
        Q_OBJECT

        int m_value;
        int m_minimum, m_maximum;

    public:

    public slots:
        void setMaximum ( int maximum );
        void setMinimum ( int minimum );
        void setRange ( int minimum, int maximum );
        void setValue ( int value );
};


// view
class QExProgressBar: public QProgressBar
{
    public:
        explicit QExProgressBar(QWidget *parent = 0);

};

#endif // QEXPROGRESSBAR_HPP
