
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

using commonTypes::EigenVec;
using printUtils::printSolutionFormat1;

void fetchData(int N, int Ns, double a) {//todo values are not printed instantly
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    // Construct the URL
    //QString formattedString = QString("My name is %1 and I am %2 years old.").arg(name).arg(age);
    QUrl url(QString("http://localhost:18080/calculate/%1/%2/%3").arg(N).arg(Ns).arg(a));
    //QUrl url("http://localhost:18080/calculate/2/4/1.0e-20");
    QNetworkRequest request(url);

    // Send the request
    QNetworkReply *reply = manager->get(request);

    // Connect the finished signal to a lambda function to handle the response
    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Parse the JSON response
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObj = jsonDoc.object();

            // Extract the "retStruct" object
            QJsonObject retStruct = jsonObj["retStruct"].toObject();

            auto Ns = retStruct["Ns"].toInt();
            auto N = retStruct["N"].toInt();
            // Extract the "data" array
            QJsonArray dataArray = retStruct["data"].toArray();

            // Convert to std::vector<DoubleVector>
            std::vector<EigenVec> res(Ns, EigenVec(4 * N));

            int index = 0;

            for (const QJsonValue &rowValue: dataArray) {
                QJsonArray rowArray = rowValue.toArray();
                int i = 0;
                for (const QJsonValue &value: rowArray) {
                    res[index][i++] = (value.toDouble());
                }
                index++;
            }

            // Print the results for verification
            for (const EigenVec &vec: res) {

                /*for (double val: vec) {
                    qDebug() << val;
                }*/
                std::stringstream ss;
                printSolutionFormat1(ss, vec);
                qDebug() << QString::fromStdString(ss.str());
                //qDebug() << "-----";
            }
        } else {
            qDebug() << "Error:" << reply->errorString();
        }

        // Clean up
        reply->deleteLater();
    });
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    int T = 10;
    std::cin >> T;
    for (int i = 0; i < T; ++i) {
        int Ns, N;
        double a;
        std::cin >> N >> Ns >> a;
        fetchData(N, Ns, a);
    }
    app.quit();//how to exit qt epplication(canse clients)
    return app.exec();
}