
#include <QProgressBar>

#include "qexprogressbar.hpp"


// ********************************** data **********************************

QExProgressBarModel::QExProgressBarModel(QObject *p):
    QObject(p),
    m_value(0),
    m_minimum(0),
    m_maximum(100)
{}


void QExProgressBarModel::setMaximum ( int m )
{
    m_maximum = m;

    emit setPBMaximum(m_maximum);
}


void QExProgressBarModel::setMinimum ( int m )
{
    m_minimum = m;

    emit setPBMimimum(m_minimum);
}


void QExProgressBarModel::setRange(int min, int max)
{
    m_minimum = min;
    m_maximum = max;

    emit setPBRange(m_minimum, m_maximum);
}


void QExProgressBarModel::setValue(int v)
{
    m_value = v;

    emit setPBValue(m_value);
}


// ********************************** view **********************************

QExProgressBarView::QExProgressBarView(QWidget *p): QProgressBar(p), model(nullptr) {}

void QExProgressBarView::setModel(const QExProgressBarModel *m)
{
    //disconnect previous model
    if (model != nullptr)
        disconnect(model);

    model = m;

    if (model != nullptr)
    {
        //make connections
        connect(model, SIGNAL(setPBMaximum(int)), this, SLOT(setMaximum(int)));
        connect(model, SIGNAL(setPBMimimum(int)), this, SLOT(setMinimum(int)));
        connect(model, SIGNAL(setPBRange(int, int)), this, SLOT(setRange(int, int)));
        connect(model, SIGNAL(setPBValue(int)), this, SLOT(setValue(int)));

        //refresh data
        setRange(model->minimum(), model->maximum());
        setValue(model->value());
    }
    else
        reset();
}
