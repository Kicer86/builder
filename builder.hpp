#ifndef builder_H
#define builder_H

#include <QtGui/QMainWindow>

class ProjectInfoWidget;
class WidgetListProxyModel;

class builder : public QMainWindow
{
    Q_OBJECT
    
    ProjectInfoWidget *projectInfoWidget;
    WidgetListProxyModel *proxy;

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
