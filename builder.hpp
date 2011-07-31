#ifndef builder_H
#define builder_H

#include <QtGui/QMainWindow>

class ProjectInfoWidget;
class WidgetListProxyModel;

class Builder: public QMainWindow
{
    Q_OBJECT

    ProjectInfoWidget *projectInfoWidget;
    WidgetListProxyModel *proxy;

    void loadPlugins();    
    void registerPlugin(QObject *);

  private slots:
    void optionsDialog();
    void projectsDialog();

  protected:
    virtual void closeEvent(QCloseEvent* );

  public:
    Builder();
    virtual ~Builder();
};

#endif // builder_H
