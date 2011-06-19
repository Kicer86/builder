
#include <assert.h>

#include <QLayout>
#include <QLineEdit>

#include "newprojectwizard.hpp"
#include "ui_newprojectwizard.h"

NewProjectWizard::NewProjectWizard(QWidget *p) :
    QWizard(p),
    ui(new Ui::NewProjectWizard)
{
  ui->setupUi(this);

  //add releases' QLineEdits
  addReleseLineEdit();               //create first line
}

NewProjectWizard::~NewProjectWizard()
{
  delete ui;
}


void NewProjectWizard::addReleseLineEdit()
{
  QLineEdit *lineEdit=new QLineEdit;
  edits.append(lineEdit);
  ui->releasesLayout->addWidget(lineEdit);
  connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
}

void NewProjectWizard::textChanged(const QString& )
{
  //check qlineedits to determine if add or remove next one
  int eSize=edits.size();

  assert(edits.size()>0);   //at least one

  //add some?
  if (edits[eSize-1]->text().isEmpty()==false)  //last one is not empty?
    addReleseLineEdit();

  //remove all trailing empties
  while (edits.size()>1 && 
         edits[edits.size()-2]->text().isEmpty() &&
         edits[edits.size()-1]->text().isEmpty()
        )    //last one is empty, and almost last also ?
  {
    ui->releasesLayout->removeWidget(edits[edits.size()-1]);   //remove last one from layout
    edits.takeLast()->deleteLater();                           //remove from list and from memory
  }
}


QString NewProjectWizard::getProjectName() const
{
  return ui->projectName->text();
}


QStringList NewProjectWizard::getReleasesNames() const
{
  QStringList ret;
  foreach (const QLineEdit *relName, edits)
    if (relName->text().isEmpty()==false)
      ret<<relName->text();
    
  return ret;
}
