
#include <QDir>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QCheckBox>
#include <QDockWidget>
#include <QDebug>
#include <QMenuBar>
#include <QStandardItemModel>

#include "data_containers/projectsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "dialogs/configdialog.hpp"
#include "misc/settings.hpp"
#include "widgets/projectinfowidget.hpp"
#include "widgets/widgetlist/widgetlist.hpp"

#include "builder.hpp"

builder::builder()
{    
  WidgetListView *projectList=new WidgetListView(this);
  projectList->setModel(ProjectsManager::instance()->getModel());

  projectInfoWidget=new ProjectInfoWidget();
  QDockWidget *projectInfoWidgetDock=new QDockWidget(tr("project info"));
  projectInfoWidgetDock->setWidget(projectInfoWidget);
  projectInfoWidgetDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

  ProjectsManager::instance()->setProjectInfoWidget(projectInfoWidget);

  //scan projects dir
  QDir directory(Settings::instance()->getProjectsPath());
  qDebug() << QString("scanning %1 for projects").arg(Settings::instance()->getProjectsPath());
  QStringList dirs=directory.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);

  setCentralWidget(projectList);
  addDockWidget(Qt::RightDockWidgetArea, projectInfoWidgetDock);

  foreach(QString item, dirs)
  {
//     ProjectWidget *projectWidget=new ProjectWidget(item);

    ProjectInfo *projectInfo=new ProjectInfo(item);

    //connect things
//     projectList->addWidget(projectWidget);
    ProjectsManager::instance()->registerProject(projectInfo);
  }

  //menu
  QMenuBar *mainMenu=new QMenuBar();
  setMenuBar(mainMenu);

  QMenu *projectsMenu=new QMenu(tr("&Projects"), this);
  connect(projectsMenu->addAction(tr("&Add")), SIGNAL(triggered(bool)), this, SLOT(projectsDialog()));

  QMenu *optionsMenu=new QMenu(tr("&Options"), this);
  connect(optionsMenu->addAction(tr("Configure")), SIGNAL(triggered(bool)), this, SLOT(optionsDialog()));

  mainMenu->addMenu(projectsMenu);
  mainMenu->addMenu(optionsMenu);
}


builder::~builder()
{}


void builder::optionsDialog()
{
  ConfigDialog *config = new ConfigDialog(this);
  if (config->exec()==QDialog::Accepted)
  {
    Settings::instance()->setEnvPath(config->getExtEnvPath());
    Settings::instance()->setEnvType(config->getEnvType());
    Settings::instance()->setProjsPath(config->getProjsPath());
    Settings::instance()->setExtList(config->getExtList());
  }
}


void builder::projectsDialog()
{

}

void builder::closeEvent(QCloseEvent* e)
{
  ProjectsManager::instance()->destroyProjects();
  QWidget::closeEvent(e);
}
