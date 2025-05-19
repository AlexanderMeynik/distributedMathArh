
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>
#include <QJsonObject>
#include <QCoreApplication>
#include <iostream>

#include "common/commonTypes.h"
#include "common/Printers.h"

using common_types::EigenVec;
using print_utils::PrintSolutionFormat1;

void FetchData(int n, int ns, double a) {///@todo values are not printed instantly
  QNetworkAccessManager *manager = new QNetworkAccessManager();

  QUrl url(QString("http://localhost:18080/calculate/%1/%2/%3").arg(n).arg(ns).arg(a));

  QNetworkRequest request(url);

  QNetworkReply *reply = manager->get(request);

  QObject::connect(reply, &QNetworkReply::finished, [reply]() {
    if (reply->error() == QNetworkReply::NoError) {

      QByteArray response_data = reply->readAll();
      QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
      QJsonObject json_obj = json_doc.object();

      QJsonObject ret_struct = json_obj["retStruct"].toObject();

      auto nss = ret_struct["Ns"].toInt();
      auto n_in = ret_struct["N"].toInt();

      QJsonArray data_array = ret_struct["data"].toArray();

      std::vector<EigenVec> res(nss, EigenVec(4 * n_in));

      int index = 0;

      for (const QJsonValue &kRowValue : data_array) {
        QJsonArray row_array = kRowValue.toArray();
        int i = 0;
        for (const QJsonValue &kValue : row_array) {
          res[index][i++] = (kValue.toDouble());
        }
        index++;
      }

      for (const EigenVec &kVec : res) {

        std::stringstream ss;
        PrintSolutionFormat1(ss, kVec);
        qDebug() << QString::fromStdString(ss.str());
      }
    } else {
      qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater();
  });
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  int t = 10;
  std::cin >> t;
  for (int i = 0; i < t; ++i) {
    int ns, n;
    double a;
    std::cin >> n >> ns >> a;
    FetchData(n, ns, a);
  }
  app.quit();//how to exit qt epplication(canse clients)
  return app.exec();
}