#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QStringList>
#include <QMap>
#include <QtQmlIntegration/qqmlintegration.h>

class TranslationManager : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT)

public:
    explicit TranslationManager(QObject *parent = nullptr);

    QString currentLanguage() const;
    void setCurrentLanguage(const QString &lang);

    QStringList availableLanguages() const;

    Q_INVOKABLE QString tr(const QString &key) const;

signals:
    void languageChanged();

private:
    void loadLanguage(const QString &lang);
    QString findFallback(const QString &key) const;

    QString m_currentLanguage = "en";
    QStringList m_availableLanguages = {"en", "tr"};
    QMap<QString, QJsonObject> m_translations;  // lang → JSON object
};

#endif // TRANSLATIONMANAGER_H
