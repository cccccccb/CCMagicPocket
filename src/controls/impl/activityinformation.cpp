#include "activityinformation.h"

ActivityInformation::ActivityInformation()
{

}

ActivityInformation::ActivityInformation(const ActivityInformation &other)
{
    this->m_id = other.m_id;
    this->m_name = other.m_name;
    this->m_version = other.m_version;
    this->m_description = other.m_description;
    this->m_iconPath = other.m_iconPath;
    this->m_localIconPath = other.m_localIconPath;
    this->m_preloadPath = other.m_preloadPath;
    this->m_entityPath = other.m_entityPath;
}

ActivityInformation::ActivityInformation(ActivityInformation &other)
{
    this->m_id = other.m_id;
    this->m_name = other.m_name;
    this->m_version = other.m_version;
    this->m_description = other.m_description;
    this->m_iconPath = other.m_iconPath;
    this->m_localIconPath = other.m_localIconPath;
    this->m_preloadPath = other.m_preloadPath;
    this->m_entityPath = other.m_entityPath;
}

ActivityInformation::ActivityInformation(ActivityInformation &&other)
{
    this->m_id = std::move(other.m_id);
    this->m_name = std::move(other.m_name);
    this->m_version = std::move(other.m_version);
    this->m_description = std::move(other.m_description);
    this->m_iconPath = std::move(other.m_iconPath);
    this->m_localIconPath = std::move(other.m_localIconPath);
    this->m_preloadPath = std::move(other.m_preloadPath);
    this->m_entityPath = std::move(other.m_entityPath);
}

ActivityInformation &ActivityInformation::operator=(const ActivityInformation &other)
{
    this->m_id = other.m_id;
    this->m_name = other.m_name;
    this->m_version = other.m_version;
    this->m_description = other.m_description;
    this->m_iconPath = other.m_iconPath;
    this->m_localIconPath = other.m_localIconPath;
    this->m_preloadPath = other.m_preloadPath;
    this->m_entityPath = other.m_entityPath;

    return *this;
}

ActivityInformation &ActivityInformation::operator=(ActivityInformation &&other)
{
    this->m_id = std::move(other.m_id);
    this->m_name = std::move(other.m_name);
    this->m_version = std::move(other.m_version);
    this->m_description = std::move(other.m_description);
    this->m_iconPath = std::move(other.m_iconPath);
    this->m_localIconPath = std::move(other.m_localIconPath);
    this->m_preloadPath = std::move(other.m_preloadPath);
    this->m_entityPath = std::move(other.m_entityPath);

    return *this;
}

bool ActivityInformation::operator==(const ActivityInformation &other) const
{
    return this->m_id == other.m_id;
}

bool ActivityInformation::operator!=(const ActivityInformation &other) const
{
    return this->m_id != other.m_id;
}

bool ActivityInformation::isValid() const
{
    return !this->m_id.isEmpty();
}

QString ActivityInformation::id() const
{
    return m_id;
}

void ActivityInformation::setId(const QString &id)
{
    if (m_id == id)
        return;

    m_id = id;
}

QString ActivityInformation::name() const
{
    return m_name;
}

void ActivityInformation::setName(const QString &name)
{
    if (m_name == name)
        return;

    m_name = name;
}

QString ActivityInformation::version() const
{
    return m_version;
}

void ActivityInformation::setVersion(const QString &version)
{
    if (m_version == version)
        return;

    m_version = version;
}

QString ActivityInformation::description() const
{
    return m_description;
}

void ActivityInformation::setDescription(const QString &description)
{
    if (m_description == description)
        return;

    m_description = description;
}

QString ActivityInformation::iconPath() const
{
    return m_iconPath;
}

void ActivityInformation::setIconPath(const QString &iconPath)
{
    if (m_iconPath == iconPath)
        return;

    m_iconPath = iconPath;
}

QString ActivityInformation::localIconPath() const
{
    return m_localIconPath;
}

void ActivityInformation::setLocalIconPath(const QString &localIconPath)
{
    if (m_localIconPath == localIconPath)
        return;

    m_localIconPath = localIconPath;
}

QString ActivityInformation::preloadPath() const
{
    return m_preloadPath;
}

void ActivityInformation::setPreloadPath(const QString &preloadPath)
{
    if (m_preloadPath == preloadPath)
        return;

    m_preloadPath = preloadPath;
}

QString ActivityInformation::entityPath() const
{
    return m_entityPath;
}

void ActivityInformation::setEntityPath(const QString &entityPath)
{
    if (m_entityPath == entityPath)
        return;

    m_entityPath = entityPath;
}
