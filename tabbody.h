#ifndef TABBODY_H
#define TABBODY_H

#include <QDialog>

namespace Ui {
class TabBody;
}

class TabBody : public QDialog
{
    Q_OBJECT

public:
    explicit TabBody(QWidget *parent = 0);
    ~TabBody();

    int getRFCNumber();
    void setRFCNumber(int number);
    const QString &getCurrentTitle();
    void findTextNext(const QString &text);
    void findTextPrev(const QString &text);
    void zoomIn();
    void zoomOut();

signals:
    titleChanged();
    loadStarted();
    loadProgress(const QString &cur_url, int progress);
    loadFinished(const QString &cur_url);

private slots:    
    void on_webView_urlChanged(const QUrl &arg1);

    void on_webView_titleChanged(const QString &title);

    void on_webView_loadStarted();
    void on_webView_loadProgress(int progress);
    void on_webView_loadFinished();

private:
    Ui::TabBody *ui;

    QString mCurUrlString;
    QString mCurTitle;

    qreal mZoomFactor;

    int mRFCNumber;
};

#endif // TABBODY_H
