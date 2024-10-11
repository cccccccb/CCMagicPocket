#ifndef ACTIVITYINFORMATION_H
#define ACTIVITYINFORMATION_H

#include <QQmlEngine>

class ActivityInformation
{
    Q_GADGET
    Q_PROPERTY(QString id READ id CONSTANT FINAL)
    Q_PROPERTY(QString name READ name CONSTANT FINAL)
    Q_PROPERTY(QString version READ version CONSTANT FINAL)
    Q_PROPERTY(QString description READ description CONSTANT FINAL)
    Q_PROPERTY(QString iconPath READ iconPath CONSTANT FINAL)
    Q_PROPERTY(QString localIconPath READ localIconPath CONSTANT FINAL)
    Q_PROPERTY(QString preloadPath READ preloadPath CONSTANT FINAL)
    Q_PROPERTY(QString entityPath READ entityPath CONSTANT FINAL)
    QML_VALUE_TYPE(activityInformation)

public:
    ActivityInformation();
    ActivityInformation(const ActivityInformation &other);
    ActivityInformation(ActivityInformation &other);
    ActivityInformation(ActivityInformation &&other);
    ActivityInformation &operator=(const ActivityInformation &other);
    ActivityInformation &operator=(ActivityInformation &&other);
    bool operator==(const ActivityInformation &other) const;
    bool operator!=(const ActivityInformation &other) const;

    bool isValid() const;

    QString id() const;
    void setId(const QString &id);

    QString name() const;
    void setName(const QString &name);

    QString version() const;
    void setVersion(const QString &version);

    QString description() const;
    void setDescription(const QString &description);

    QString iconPath() const;
    void setIconPath(const QString &iconPath);

    QString localIconPath() const;
    void setLocalIconPath(const QString &localIconPath);

    QString preloadPath() const;
    void setPreloadPath(const QString &preloadPath);

    QString entityPath() const;
    void setEntityPath(const QString &entityPath);

private:
    QString m_id;
    QString m_name;
    QString m_version;
    QString m_description;
    QString m_iconPath;
    QString m_localIconPath;
    QString m_preloadPath;
    QString m_entityPath;
};

#endif // ACTIVITYINFORMATION_H
