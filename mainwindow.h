#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMap>
#include <QMainWindow>
#include "tabbody.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void onActionOpenRFC();
    void onActionClearRecent();

    void onFileSavedDone(QString fine_name);

    void onMenuFileAboutToShow();
    void onMenuEditAboutToShow();
    void onMenuFavoriteAboutToShow();

    void onRFCLoadStarted();
    void onRFCLoadProgress(const QString &cur_url, int progress);
    void OnRFCLoadFinished(const QString &cur_url);

    void onTabTitleChanged();

    void on_actionOpen_triggered();
    void on_actionClose_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();

    void on_actionFind_triggered();
    void on_actionFindNext_triggered();
    void on_actionFindPrev_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();

    void on_actionAddtoFavorite_triggered();
    void on_actionRemovefromFavorite_triggered();
    void on_actionClearallFavorites_triggered();    
    void on_actionEditFavorite_triggered();

    void on_actionAbout_triggered();
    void on_actionAboutQT_triggered();

    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_tabBarDoubleClicked(int index);
    void on_tabWidget_currentChanged(int index);

    void on_actionCloseAll_triggered();

private:
    Ui::MainWindow *ui;

    QString mFindText;

    QList< QPointer<QAction> > mFavActionList;

    QMap<int, QPointer<TabBody> > mTabCacheMap;

    QString mLastSavedDir;

    QStringList mRecentList;

    void openTabBody(int number);
};

#endif // MAINWINDOW_H
