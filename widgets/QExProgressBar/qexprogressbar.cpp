
#include "qexprogressbar.hpp"


//data
void QExProgressBarProxy::setMaximum ( int maximum )
{
    m_maximum = maximum;
}


void QExProgressBarProxy::setMinimum ( int minimum )
{
    m_minimum = minimum;
}


void QExProgressBarProxy::setRange(int minimum, int maximum)
{
    m_minimum = minimum;
    m_maximum = maximum;
}


void QExProgressBarProxy::setValue(int v)
{
    m_value = v;
}


//view
QExProgressBar::QExProgressBar(QWidget *p): QProgressBar(p)
{

}
