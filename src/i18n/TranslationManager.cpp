#include "TranslationManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
{
    loadLanguage("en");
    loadLanguage("tr");
}

QString TranslationManager::currentLanguage() const
{
    return m_currentLanguage;
}

void TranslationManager::setCurrentLanguage(const QString &lang)
{
    if (m_currentLanguage == lang || !m_translations.contains(lang))
        return;
    m_currentLanguage = lang;
    emit languageChanged();
}

QStringList TranslationManager::availableLanguages() const
{
    return m_availableLanguages;
}

void TranslationManager::loadLanguage(const QString &lang)
{
    QFile file(QString(":/i18n/%1.json").arg(lang));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Translation file not found:" << file.fileName();
        m_translations[lang] = QJsonObject();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isObject()) {
        m_translations[lang] = doc.object();
    } else {
        m_translations[lang] = QJsonObject();
    }
    file.close();
}

QString TranslationManager::findFallback(const QString &key) const
{
    // Try English fallback
    auto it = m_translations.constFind("en");
    if (it != m_translations.constEnd()) {
        QJsonValue val = it.value().value(key);
        if (val.isString())
            return val.toString();
    }
    // Return key itself as last resort
    return key;
}

QString TranslationManager::tr(const QString &key) const
{
    auto it = m_translations.constFind(m_currentLanguage);
    if (it != m_translations.constEnd()) {
        QJsonValue val = it.value().value(key);
        if (val.isString())
            return val.toString();
    }
    return findFallback(key);
}
