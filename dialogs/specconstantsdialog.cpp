
#include <assert.h>

#include <QScrollBar>
#include <QLineEdit>
#include <QString>

#include "specconstantsdialog.hpp"
#include "ui_specconstantsdialog.h"
#include "misc/functions.hpp"


class RichLabel: public QLabel
{
        RichLabel *related;                 //a friend
        QString originalText;

        void setColor();
        void clearColor();

    protected:
        virtual void enterEvent (QEvent *);
        virtual void leaveEvent (QEvent *);

    public:
        RichLabel(const QString &, QWidget *parent = 0);
        virtual ~RichLabel();

        void setRelatedLabel(RichLabel *);
};


RichLabel::RichLabel(const QString &t, QWidget *p): QLabel(t, p), related(nullptr), originalText(t)
{}


RichLabel::~RichLabel()
{}


void RichLabel::setColor()
{
    const QString newText( Functions::setColour(originalText, Qt::red ));

    setText(newText);
}


void RichLabel::clearColor()
{
    setText(originalText);
}


void RichLabel::enterEvent(QEvent *e)
{
    assert(related != nullptr);

    setColor();
    related->setColor();

    QLabel::enterEvent(e);
}


void RichLabel::leaveEvent(QEvent *e)
{
    assert(related != nullptr);

    clearColor();
    related->clearColor();

    QLabel::leaveEvent(e);
}


void RichLabel::setRelatedLabel(RichLabel *label)
{
    related = label;
}

// -----------------------------------------------------------------------------


SpecConstantsDialog::SpecConstantsDialog(QWidget *p) :
    QDialog(p),
    ui(new Ui::SpecConstansDialog)
{
    ui->setupUi(this);

    //turn on synchro between scrolareas
    connect(ui->constansScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            ui->valuesScrollArea->verticalScrollBar(), SLOT(setValue(int)));

    connect(ui->valuesScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            ui->constansScrollArea->verticalScrollBar(), SLOT(setValue(int)));
}


SpecConstantsDialog::~SpecConstantsDialog()
{}


void SpecConstantsDialog::manageVariables()
{
    //remove empty variables, and make sure there is one empty at the end od list

    for (size_t i = 0; (i + 1) < variableList.size(); i++)
    {
        Variable &variable = variableList[i];

        if (variable.first->text().size() == 0 &&
            variable.second->text().size() == 0)
        {
            ui->constantsLayout->removeWidget(variable.first);
            ui->valuesLayout->removeWidget(variable.second);

            variable.first->deleteLater();
            variable.second->deleteLater();

            variableList.erase( variableList.begin() + i);
        }
    }

    //no items or last one is not empty?
    const size_t listSize = variableList.size();
    if (listSize == 0 ||
        variableList[listSize - 1].first->text().size() != 0 ||
        variableList[listSize - 1].second->text().size() != 0)
    {
        addVariable(QString(), QString());     //create empty variable
    }
}


void SpecConstantsDialog::addConstant(const QString &name, const QString &value)
{
    RichLabel *nameWidget = new RichLabel(name);
    nameWidget->setTextInteractionFlags(Qt::TextSelectableByMouse);

    RichLabel *valueWidget = new RichLabel(value);
    valueWidget->setTextInteractionFlags(Qt::TextSelectableByMouse);

    nameWidget->setRelatedLabel(valueWidget);
    valueWidget->setRelatedLabel(nameWidget);

    ui->constantsLayout->addWidget(nameWidget);
    ui->valuesLayout->addWidget(valueWidget);
}


void SpecConstantsDialog::addVariable(const QString &name, const QString &value)
{
    QLineEdit *nameWidget = new QLineEdit(name);
    QLineEdit *valueWidget = new QLineEdit(value);

    ui->constantsLayout->addWidget(nameWidget);
    ui->valuesLayout->addWidget(valueWidget);

    //save variable on list
    Variable variable(nameWidget, valueWidget);
    variableList.push_back(variable);

    //watch for editing finished
    connect(nameWidget, SIGNAL(editingFinished()), this, SLOT(manageVariables()));
    connect(valueWidget, SIGNAL(editingFinished()), this, SLOT(manageVariables()));
}


void SpecConstantsDialog::addSeparator()
{
    QFrame *line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    QFrame *line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    ui->constantsLayout->addWidget(line1);
    ui->valuesLayout->addWidget(line2);
}


SpecConstantsDialog::Variables SpecConstantsDialog::getVariables() const
{
    Variables ret;

    for(const Variable &variable: variableList)
        ret.push_back(std::pair<QString, QString>(variable.first->text(), variable.second->text()));

    return ret;
}


void SpecConstantsDialog::exec()
{
    //make sure, there is empty field for new variable
    manageVariables();

    //finish list
    //ui->constantsLayout->addStretch(1);
    //ui->valuesLayout->addStretch(1);

    QDialog::exec();
}
