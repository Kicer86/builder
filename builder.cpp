
#include <QDebug>
#include <QDir>
#include <QDockWidget>
#include <QMenuBar>
#include <QStandardItemModel>

#include "data_containers/projectsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "dialogs/configdialog.hpp"
#include "misc/settings.hpp"
#include "widgets/projectinfowidget.hpp"
#include "widgets/widgetlist/widgetlistproxymodel.hpp"
#include "widgets/widgetlist/widgetlistview.hpp"

#include "builder.hpp"
#include "dialogs/newprojectwizard.hpp"
#include <QMessageBox>

builder::builder()
{
  WidgetListView *projectList=new WidgetListView(this);
  proxy=new WidgetListProxyModel(this);

  proxy->setSourceModel(ProjectsManager::instance()->getModel());
  projectList->setModel(proxy);

  projectInfoWidget=new ProjectInfoWidget();
  QDockWidget *projectListWidgetDock=new QDockWidget(tr("project info"));
  projectListWidgetDock->setWidget(projectList);
  projectListWidgetDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

  ProjectsManager::instance()->setProjectInfoWidget(projectInfoWidget);

  //scan projects dir
  QDir directory(Settings::instance()->getProjectsPath());
  qDebug() << QString("scanning %1 for projects").arg(Settings::instance()->getProjectsPath());
  QStringList dirs=directory.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);

  setCentralWidget(projectInfoWidget);
  addDockWidget(Qt::LeftDockWidgetArea, projectListWidgetDock);

  foreach(QString item, dirs)
  {
    ProjectInfo *projectInfo=new ProjectInfo(item);
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
  NewProjectWizard wizard;
  wizard.setModal(true);
  if (wizard.exec()==QDialog::Accepted)  //accepted?
  {
    QString projsPath=Settings::instance()->getProjectsPath();
    QString projectName=wizard.getProjectName();

    QDir projDir(projsPath);
    if (projDir.mkdir(wizard.getProjectName())==false)
    {
      QMessageBox::critical(this, tr("Error"), tr("Could not create project's directory (%1) in %2.")
                            .arg(projectName, projsPath)
                           );
    }
    else   //it's ok, add releases directories
    {
      bool ok=true;
      foreach(QString relName, wizard.getReleasesNames())
      {
        QDir relDir(projsPath+"/"+projectName);
        if (relDir.mkdir(relName)==false)
        {
          ok=false;
          break;
        }
      }

      if (ok)
      {
        ProjectInfo *projectInfo=new ProjectInfo(projectName);
        ProjectsManager::instance()->registerProject(projectInfo);
      }
      else
        QMessageBox::critical(this, tr("Error"), tr("Could not create one of project's releases directory in %1.")
                              .arg(projsPath)
                             );
    }
  }
}

void builder::closeEvent(QCloseEvent* e)
{
  ProjectsManager::instance()->destroyProjects();
  QWidget::closeEvent(e);
}
