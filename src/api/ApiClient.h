#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJSValue>
#include <QQmlEngine>

class ApiClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(bool useMock READ useMock WRITE setUseMock NOTIFY useMockChanged)

public:
    explicit ApiClient(QObject *parent = nullptr);

    QString baseUrl() const;
    void setBaseUrl(const QString &url);

    bool useMock() const;
    void setUseMock(bool mock);

    Q_INVOKABLE void get(const QString &path, QJSValue callback);
    Q_INVOKABLE void post(const QString &path, const QJsonObject &body, QJSValue callback);

signals:
    void baseUrlChanged();
    void useMockChanged();

private:
    void doRequest(const QString &method, const QString &path,
                   const QByteArray &body, QJSValue callback);

    void handleMock(const QString &method, const QString &path,
                    const QByteArray &body, QJSValue callback);

    QString m_baseUrl = "http://localhost:7055";
    bool m_useMock = true;
    QNetworkAccessManager *m_manager;
};

#endif // APICLIENT_H
