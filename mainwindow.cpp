#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPointer>
#include <QSettings>
#include <QtDebug>
#include "filesavemanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

const int recent_max = 10;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QApplication::applicationName());

    ///
    mFindText = "";

    /// READ the settings
    QSettings settings;
    mLastSavedDir = settings.value("lastSavedDir", "./").toString();
    mRecentList = settings.value("recent").toString().split(",", QString::SkipEmptyParts);

    ///
    connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(onMenuFileAboutToShow()));
    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(onMenuEditAboutToShow()));
    connect(ui->menuFavorite, SIGNAL(aboutToShow()), this, SLOT(onMenuFavoriteAboutToShow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openTabBody(int number)
{
    // 1. Check if the specified RFC number is currently opened.
    //
    // If it is opened, just set the corresponding TAB to the current index.
    // Otherwise, create a new TAB, and also set it to the current index.
    //
    QMap<int, QPointer<TabBody> >::iterator iter = mTabCacheMap.find(number);

    if (iter != mTabCacheMap.end())
    {
        QPointer<TabBody> tab_body = (QPointer<TabBody>)iter.value();

        if (!tab_body.isNull())
        {
            tab_body->setRFCNumber(number);

            ui->tabWidget->setCurrentWidget(tab_body);
        }
    }
    else
    {
        TabBody *tab_body = new TabBody(this);
        tab_body->setRFCNumber(number);

        connect(tab_body, SIGNAL(titleChanged()), this, SLOT(onTabTitleChanged()));
        connect(tab_body, SIGNAL(loadStarted()), this, SLOT(onRFCLoadStarted()));
        connect(tab_body, SIGNAL(loadProgress(const QString &, int)), this, SLOT(onRFCLoadProgress(const QString &, int)));
        connect(tab_body, SIGNAL(loadFinished(const QString &)), this, SLOT(OnRFCLoadFinished(const QString &)));

        int tab_index = ui->tabWidget->addTab(tab_body, QString("RFC_%1").arg(number));
        ui->tabWidget->setCurrentIndex(tab_index);

        // Save the tabBody to the tabCacheMap, because it is
        // a new create one.
        //
        mTabCacheMap.insert(number, tab_body);
    }

    // 2. Save the RFC number open history.
    //
    // Firstly, the new opened RFC number MUST be moved to the head.
    // And the total number of the history MUST not more than 5.
    //
    QString rfc_number;
    rfc_number.sprintf("%04d", number);

    int recent_index = mRecentList.indexOf(rfc_number);
    if (recent_index >= 0)
    {
        mRecentList.removeAt(recent_index);
    }

    mRecentList.insert(0, rfc_number);

    while (mRecentList.size() > recent_max)
    {
        mRecentList.removeLast();
    }

    QSettings settings;
    settings.setValue("recent", mRecentList.join(","));

    // 3. Enable the relative menu which is diabled.
    //
    ui->actionSave->setEnabled(true);
    ui->actionClose->setEnabled(true);
    ui->actionCloseAll->setEnabled(true);
    ui->actionFind->setEnabled(true);
    ui->actionFindNext->setEnabled(true);
    ui->actionFindPrev->setEnabled(true);
    ui->actionZoomIn->setEnabled(true);
    ui->actionZoomOut->setEnabled(true);
    ui->actionAddtoFavorite->setEnabled(true);
}

void MainWindow::onActionOpenRFC()
{
    QAction *action = (QAction *)sender();
    openTabBody(action->data().toInt());
}

void MainWindow::onActionClearRecent()
{
    mRecentList.clear();

    QSettings settings;
    settings.remove("recent");
}

void MainWindow::onFileSavedDone(QString fine_name)
{
    QMessageBox::information(NULL, QApplication::applicationName(),
                             QString("File `%1` saved successfully!").arg(fine_name), QMessageBox::Yes);
}

void MainWindow::onMenuFileAboutToShow()
{
    // 1. Check if the menu can be enabled.
    //
    ui->actionSave->setEnabled(false);
    ui->actionClose->setEnabled(false);
    ui->actionCloseAll->setEnabled(false);

    if (ui->tabWidget->count() > 0)
    {
        ui->actionSave->setEnabled(true);
        ui->actionClose->setEnabled(true);
        ui->actionCloseAll->setEnabled(true);
    }

    // 2. Clear the Recent Menu and disable it.
    //
    ui->menuOpen_Recent->clear();
    ui->menuOpen_Recent->setEnabled(false);

    // 3. Check the RFC number open history is empty or not.
    //
    if (mRecentList.size() > 0)
    {
        // A. enable the Recent Menu.
        //
        ui->menuOpen_Recent->setEnabled(true);

        // B. insert the Recent actions to Recent Menu.
        //
        for (int i = 0; i < mRecentList.count(); i++)
        {
            QString RFCNumber = mRecentList.value(i);

            QAction *recent_action = ui->menuOpen_Recent->addAction(QString("%1: RFC_%2").arg(i+1).arg(RFCNumber));
            recent_action->setData(RFCNumber);
            connect(recent_action, SIGNAL(triggered()), this, SLOT(onActionOpenRFC()));
        }

        // C. insert a separator.
        //
        ui->menuOpen_Recent->addSeparator();

        // D. insert a Clear action at the bottom of Recent Menu.
        //
        QAction *actionClearRecent = ui->menuOpen_Recent->addAction("&Clear Recent");
        connect(actionClearRecent, SIGNAL(triggered()), this, SLOT(onActionClearRecent()));
    }
}

void MainWindow::onMenuEditAboutToShow()
{
    ui->actionFind->setEnabled(false);
    ui->actionFindNext->setEnabled(false);
    ui->actionFindPrev->setEnabled(false);
    ui->actionZoomIn->setEnabled(false);
    ui->actionZoomOut->setEnabled(false);

    if (ui->tabWidget->count() > 0)
    {
        ui->actionFind->setEnabled(true);
        ui->actionFindNext->setEnabled(true);
        ui->actionFindPrev->setEnabled(true);
        ui->actionZoomIn->setEnabled(true);
        ui->actionZoomOut->setEnabled(true);
    }
}

void MainWindow::onMenuFavoriteAboutToShow()
{
    // 1. Check if the Add to Favorite menu can be enabled.
    //
    ui->actionAddtoFavorite->setEnabled(false);

    if (ui->tabWidget->count() > 0)
    {
        ui->actionAddtoFavorite->setEnabled(true);
    }

    // 2. Remove original added favorite actions from menu
    //
    foreach(QPointer<QAction> fav_action, mFavActionList)
    {
        if (!fav_action.isNull())
        {
            ui->menuFavorite->removeAction(fav_action);
        }
    }

    mFavActionList.clear();

    // 3. Add new favorite actions to menu, and cache to favActionList
    //
    QSettings settings;

    settings.beginGroup("favorite");

    QStringList favorite_keys = settings.childKeys();

    for (int i = 0; i < favorite_keys.count(); i++)
    {
        QString RFCNumber = favorite_keys.value(i);

        ///
        QAction* favorite_action = ui->menuFavorite->addAction(QString("%1: RFC_%2 - %3")
                                                               .arg(i + 1)
                                                               .arg(RFCNumber)
                                                               .arg(settings.value(RFCNumber).toString()));
        ///
        favorite_action->setData(RFCNumber);
        connect(favorite_action, SIGNAL(triggered()), this, SLOT(onActionOpenRFC()));

        ///
        mFavActionList.append(favorite_action);
    }

    settings.endGroup();

    // 4. Check if the Remove Favorite menu can be enabled.
    //
    ui->actionRemovefromFavorite->setEnabled(false);
    ui->actionClearallFavorites->setEnabled(false);

    if (!mFavActionList.isEmpty())
    {
        ui->actionRemovefromFavorite->setEnabled(true);
        ui->actionClearallFavorites->setEnabled(true);
    }
}

void MainWindow::onRFCLoadStarted()
{
    ui->statusBar->showMessage("Loading...");
}

void MainWindow::onRFCLoadProgress(const QString &cur_url, int progress)
{
    ui->statusBar->showMessage(QString("[%1%] %2").arg(progress).arg(cur_url));
}

void MainWindow::OnRFCLoadFinished(const QString &cur_url)
{
    ui->statusBar->showMessage(QString("[Done] %1").arg(cur_url), 2000);
}

void MainWindow::onTabTitleChanged()
{
    if (ui->tabWidget->count() > 0)
    {
        TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();

        setWindowTitle(tab_body->getCurrentTitle());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    bool ok;
    int number = QInputDialog::getInt(NULL, QApplication::applicationName(),
                                      "Open RFC #:", 2131, 1, 9999, 1, &ok);
    if (ok)
    {
        openTabBody(number);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        ///
        TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();

        QString file_name = QString("rfc%1.txt").arg(tab_body->getRFCNumber());

        ///
        QString file_path_name = QFileDialog::getSaveFileName(NULL, QApplication::applicationName(),
                                                              mLastSavedDir + file_name, "RFC Files (*.txt)");

        if (!file_path_name.isEmpty())
        {
            ///
            FileSaveManager *m = new FileSaveManager(this);

            connect(m, SIGNAL(savedDone(QString)), this, SLOT(onFileSavedDone(QString)));

            m->setFileName(file_name);
            m->setFilePathName(file_path_name);
            m->exec();

            ///
            int end_index = file_path_name.lastIndexOf('/');
            mLastSavedDir = file_path_name.left(end_index + 1);

            QSettings settings;
            settings.setValue("lastSavedDir", mLastSavedDir);
        }
    }
}

void MainWindow::on_actionClose_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        emit ui->tabWidget->tabCloseRequested(ui->tabWidget->currentIndex());
    }
}

void MainWindow::on_actionCloseAll_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        ui->tabWidget->clear();

        foreach(QPointer<TabBody> tab_body, mTabCacheMap)
        {
            tab_body->deleteLater();
        }

        mTabCacheMap.clear();
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionFind_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        bool ok;

        mFindText = QInputDialog::getText(NULL, QApplication::applicationName(),
                                          "Find text:", QLineEdit::Normal, mFindText, &ok);
        if (ok)
        {
            mFindText = mFindText.trimmed();

            emit ui->actionFindNext->triggered();
        }
    }
}

void MainWindow::on_actionFindNext_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        if (!mFindText.isEmpty())
        {
            TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();
            tab_body->findTextNext(mFindText);
        }
    }
}

void MainWindow::on_actionFindPrev_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        if (!mFindText.isEmpty())
        {
            TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();
            tab_body->findTextPrev(mFindText);
        }
    }
}

void MainWindow::on_actionZoomIn_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();

        tab_body->zoomIn();
    }
}

void MainWindow::on_actionZoomOut_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();

        tab_body->zoomOut();
    }
}

void MainWindow::on_actionAddtoFavorite_triggered()
{
    if (ui->tabWidget->count() > 0)
    {
        TabBody *tab_body = (TabBody*)ui->tabWidget->currentWidget();

        QString rfc_number;
        rfc_number.sprintf("%04d", tab_body->getRFCNumber());

        bool ok;
        QString remarks = QInputDialog::getText(NULL, QApplication::applicationName(),
                                                QString("Remarks for RFC_%1:").arg(rfc_number), QLineEdit::Normal, "", &ok);
        if (ok)
        {
            ///
            remarks = remarks.trimmed();

            if (remarks.length() > 64)
            {
                remarks = remarks.left(64).append("...");
            }

            ///
            QSettings settings;

            settings.beginGroup("favorite");
            settings.setValue(rfc_number, remarks);
            settings.endGroup();
        }
    }
}

void MainWindow::on_actionRemovefromFavorite_triggered()
{    
    if (!mFavActionList.isEmpty())
    {
        // 1. Build the QStringList for QInputDialog
        //
        QStringList fav_string_list;

        foreach(QPointer<QAction> fav_action, mFavActionList)
        {
            if (!fav_action.isNull())
            {
                fav_string_list << fav_action->text();
            }
        }

        // 2. Pop the QInputDialog to front
        //
        bool ok;
        QString fav_string = QInputDialog::getItem(NULL, QApplication::applicationName(),
                                                   "Select to remove:", fav_string_list, 0, false, &ok);
        if (ok)
        {
            QRegExp reg_exp("RFC_(\\d+) - .*");

            int index = reg_exp.indexIn(fav_string);
            if (index >= 0)
            {
                QSettings settings;
                settings.beginGroup("favorite");
                settings.remove(reg_exp.cap(1));
                settings.endGroup();
            }
        }
    }
}

void MainWindow::on_actionClearallFavorites_triggered()
{
    int result = QMessageBox::question(NULL, QApplication::applicationName(),
                                       "Are you sure you want to clear all of the favorites?");
    if (result == QMessageBox::Yes)
    {
        QSettings settings;

        settings.beginGroup("favorite");

        foreach (QString RFCNumber, settings.childKeys())
        {
            settings.remove(RFCNumber);
        }

        settings.endGroup();
    }
}

void MainWindow::on_actionEditFavorite_triggered()
{

}

void MainWindow::on_actionAbout_triggered()
{
    QString message("");
    message += QString("<p><font size='+1'><b>About %1</b></font></p>").arg(QApplication::applicationName());
    message += QString("<p>%1 is developed with Qt 5.5.0</p>").arg(QApplication::applicationName());
    message += QString("<p>The RFC webpage is loaded from the URL at IETF:<br>");
    message += QString("<a href='http://tools.ietf.org/html/'>http://tools.ietf.org/html/</a></p>");
    message += QString("<p>The txt file is saved from the URL at IETF:<br>");
    message += QString("<a href='http://tools.ietf.org/rfc/'>http://tools.ietf.org/rfc/</a></p>");
    message += QString("<p>Version: %1</p>").arg(QApplication::applicationVersion());
    message += QString("<p>Build by %1, 2015</p>").arg(QApplication::organizationName());

    QMessageBox message_box(QMessageBox::NoIcon, QApplication::applicationName(), message);
    message_box.setIconPixmap(QPixmap(":/RFC/RFC.png"));
    message_box.exec();
}

void MainWindow::on_actionAboutQT_triggered()
{
    QMessageBox::aboutQt(NULL, windowTitle());
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (index >= 0)
    {
        TabBody *tab_body = (TabBody*)ui->tabWidget->widget(index);

        // Deatroy a opened RFC number.
        //
        // 1. remove the tabBody from tabWidget
        //
        ui->tabWidget->removeTab(index);

        // 2. must remove it from the tabCacheMap
        //
        mTabCacheMap.remove(tab_body->getRFCNumber());

        // 3. release the momery of the tabBody
        //
        tab_body->deleteLater();
    }
}

void MainWindow::on_tabWidget_tabBarDoubleClicked(int index)
{
    if (index >= 0)
    {
        emit ui->tabWidget->tabCloseRequested(index);
    }
    else
    {
        emit ui->actionOpen->triggered();
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index >= 0)
    {
        TabBody *tab_body = (TabBody*)ui->tabWidget->widget(index);

        setWindowTitle(tab_body->getCurrentTitle());
    }
    else
    {
        setWindowTitle(QApplication::applicationName());
    }
}
