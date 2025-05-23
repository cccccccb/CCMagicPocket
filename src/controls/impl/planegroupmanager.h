#ifndef PLANEGROUPMANAGER_H
#define PLANEGROUPMANAGER_H

#include <QQuickItem>
#include <QQmlListProperty>

#include <private/qquicktransitionmanager_p_p.h>

class ActivityPlaneGroup;
class PlaneTransitionManager;

class PlaneGroupManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQmlListProperty<ActivityPlaneGroup> roots READ roots CONSTANT)
    Q_PROPERTY(ActivityPlaneGroup * currentPlaneGroup READ currentPlaneGroup WRITE setCurrentPlaneGroup NOTIFY currentPlaneGroupChanged FINAL)

public:
    explicit PlaneGroupManager(QObject *parent = nullptr);

    QQmlListProperty<ActivityPlaneGroup> roots();

    ActivityPlaneGroup *currentPlaneGroup() const;
    void setCurrentPlaneGroup(ActivityPlaneGroup *planeGroup);

Q_SIGNALS:
    void currentPlaneGroupChanged();

private:
    void updateVisibleRoot(int newIndex);
    void onItemLeaved(ActivityPlaneGroup *group);
    void onItemEntered(ActivityPlaneGroup *group);

    static void appendRoot(QQmlListProperty<ActivityPlaneGroup>* list, ActivityPlaneGroup* item);
    static qsizetype countRoot(QQmlListProperty<ActivityPlaneGroup>* list);
    static ActivityPlaneGroup* atRoot(QQmlListProperty<ActivityPlaneGroup>* list, qsizetype index);
    static void clearRoots(QQmlListProperty<ActivityPlaneGroup>* list);

private:
    QList<ActivityPlaneGroup *>     _roots;
    PlaneTransitionManager *        _enterTransitionManager = nullptr;
    PlaneTransitionManager *        _leaveTransitionManager = nullptr;
    ActivityPlaneGroup *            _currentPlaneGroup = nullptr;
};

#endif // PLANEGROUPMANAGER_H
