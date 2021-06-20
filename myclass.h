#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLabel>

class MyClass : public QLabel
{
    Q_OBJECT
    public:
      MyClass(QWidget *parent = 0);


    public slots:
      void replyFinished(QNetworkReply * reply) ;

    private:
      QLabel *label;
      QNetworkAccessManager *manager ;
} ;
