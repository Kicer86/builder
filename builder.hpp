#ifndef builder_H
#define builder_H

#include <QtGui/QMainWindow>

class ProjectInfoWidget;

class builder : public QMainWindow
{
    Q_OBJECT
    
    ProjectInfoWidget *projectInfoWidget;

  private slots:
    void optionsDialog();
    void projectsDialog();

  protected:
    virtual void closeEvent(QCloseEvent* );

  public:
    builder();
    virtual ~builder();
};

#endif // builder_H
