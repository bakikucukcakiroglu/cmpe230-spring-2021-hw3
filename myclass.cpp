#include <QtGui>
#include "myclass.h"
#include <QRegExp>
#include <QHBoxLayout>
#include <QTextStream>
#include <QTableWidget>
#include <QMainWindow>
#include <QStringRef>

QStringList name_list;
QStringList isn;

MyClass::MyClass(QWidget *parent) : QLabel(parent)
{
    
    QTextStream out(stdout);
    QStringList names;


    //kodun bu bölümü dosyadan crypto nameleri okur
    QFile file("/home/cmpe250student/CURRENCY/MYCRYPTOCONVERT.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    
    while (!file.atEnd()) 
    {
        QString line = file.readLine();
        QString m;

        if(line.right(1)=="\n")
        {
            m= line.left(line.size()-1);
        }else{
            m= line;
        }

        names<<m;
    }

    //kodun bu bölümü okunan crypto namelerin idlerini listeye çeker
    QStringList ids;

    QEventLoop loop;
    QNetworkAccessManager nam;
    QNetworkRequest req(QUrl("https://api.coingecko.com/api/v3/coins/list"));
    QNetworkReply *reply = nam.get(req);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QString data = (QString) reply->readAll();



    foreach (QString name, names) 
    {
        QRegExp rx("\"id\":\"(\\w+)\",\"symbol\":\"(\\w+)\",\"name\":\"("+name+"\\b)\"");
        QRegExp ry("\"id\":\"(\\w+)\",\"symbol\":\"("+name+")\",\"name\":\"(\\w+)");
        int pos = 0;

        while ((pos = rx.indexIn(data, pos)) != -1) 
        {
            QStringList a= {rx.cap(1), rx.cap(2), rx.cap(3)};
            isn << a;
            pos += rx.matchedLength();
        }
        pos=0;
        while ((pos = ry.indexIn(data, pos)) != -1) 
        {
            QStringList a= {ry.cap(1), ry.cap(2), ry.cap(3)};
            isn << a;
            pos += ry.matchedLength();
        }
     }

    //kodun bu bölümü cryptoların idlerinden bir network request oluşturur

     QString rqst= "https://api.coingecko.com/api/v3/simple/price?ids=";

    for(int i=0; i<isn.size(); i=i+3 )
    {
        if(i==0)
        {
            rqst= rqst+isn[i];
        }else{
            rqst= rqst+","+isn[i];
        }
     }
    
    rqst=rqst+"&vs_currencies=usd,eur,gbp";

    manager = new QNetworkAccessManager(this) ;

    connect(manager, SIGNAL(finished(QNetworkReply *)),this, SLOT(replyFinished(QNetworkReply *)));

    // Initiate retrieval of  the data from the web page.
    // When completed, slot replyFinished will be called.
    // manager->get(QNetworkRequest(QUrl("https://www.bitstamp.net/api/v2/ticker/btcusd")));
    manager->get(QNetworkRequest(QUrl(rqst)));

}
void MyClass::replyFinished(QNetworkReply *reply)  {

    QString str ;
    QStringList rate_list;
    QStringList name;

    //kosola yazdırmayı sağlıyor
    QTextStream out(stdout);

    // read the data fetched from the web site
    QString data = (QString) reply->readAll();

    // use pattern matching to extract the rate
    QRegExp rx("\"(\\w+)\":[{]\"\\w+\":(\\d+\\.*\\d*),\"\\w+\":(\\d+\\.*\\d*),\"\\w+\":(\\d+\\.*\\d*)");

    int pos = 0;

    while ((pos = rx.indexIn(data, pos)) != -1) 
    {
        name << rx.cap(1);
        rate_list << rx.cap(2);
        rate_list << rx.cap(3);
        rate_list << rx.cap(4);
        str = rx.cap(1)+ " "+ rx.cap(2) + " "+ rx.cap(3);
        pos += rx.matchedLength();
     }

    for(int i=0; i< name.size(); i++)
    {
        for(int z=0; z< isn.size(); z=z+3)
        {
            if(isn[z]==name[i])
            {
                name_list<<isn[z+2];
            }
        }
    }

    QTableWidget *tableWidget = new QTableWidget(rate_list.size()/3, 3, this);

    tableWidget->setHorizontalHeaderLabels({"USD", "EUR", "GBP"});
    tableWidget->setVerticalHeaderLabels(name_list);

    int r=0;
    int c=0;
    
    foreach(QString x, rate_list){
        if(c==3)
        {
          r++;
          c=0;
        }
        QTableWidgetItem *newItem = new QTableWidgetItem(x);
        tableWidget->setItem(r, c, newItem);
        c++;
     }


    QMainWindow *mainwindow= new QMainWindow();
    mainwindow->setCentralWidget(tableWidget);
    mainwindow->setWindowTitle("AMINACOIN");
    mainwindow->show();

}
