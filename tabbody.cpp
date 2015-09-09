#include <QtDebug>
#include "tabbody.h"
#include "ui_tabbody.h"

const qreal zoom_max = 1.6f;
const qreal zoom_min = 0.8f;
const qreal zoom_step = 0.1f;

TabBody::TabBody(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TabBody)
{    
    qDebug() << "TabBody::TabBody()";

    ui->setupUi(this);

    ui->webView->setContent("<p style='font-family:Consolas;font-size:12px;'>Loading...</p>");

    mCurUrlString = "";
    mCurTitle = "";

    mZoomFactor = 1.1f;
    ui->webView->setZoomFactor(mZoomFactor);

    mRFCNumber = 3231;
}

TabBody::~TabBody()
{
    delete ui;

    qDebug() << "TabBody::~TabBody()";
}

int TabBody::getRFCNumber()
{
    return mRFCNumber;
}

void TabBody::setRFCNumber(int number)
{
    mRFCNumber = number;
    ui->webView->load(QUrl(QString("http://tools.ietf.org/html/rfc%1").arg(number)));
}

const QString& TabBody::getCurrentTitle()
{
    return mCurTitle;
}

void TabBody::findTextNext(const QString &text)
{
    ui->webView->findText(text);
    ui->webView->setFocus();
}

void TabBody::findTextPrev(const QString &text)
{
    ui->webView->findText(text, QWebPage::FindBackward);
    ui->webView->setFocus();
}

void TabBody::zoomIn()
{
    mZoomFactor = qMin(mZoomFactor + zoom_step, zoom_max);

    ui->webView->setZoomFactor(mZoomFactor);
}

void TabBody::zoomOut()
{
    mZoomFactor = qMax(mZoomFactor - zoom_step, zoom_min);

    ui->webView->setZoomFactor(mZoomFactor);
}

void TabBody::on_webView_urlChanged(const QUrl &arg1)
{
    mCurUrlString = arg1.toDisplayString();
}

void TabBody::on_webView_titleChanged(const QString &title)
{
    mCurTitle = title;
    emit titleChanged();
}

void TabBody::on_webView_loadStarted()
{
    mCurUrlString = "";
    mCurTitle = "";

    emit loadStarted();
}

void TabBody::on_webView_loadProgress(int progress)
{
    if (!mCurUrlString.isEmpty())
    {
        emit loadProgress(mCurUrlString, progress);
    }
}

void TabBody::on_webView_loadFinished()
{
    emit loadFinished(mCurUrlString);
}
