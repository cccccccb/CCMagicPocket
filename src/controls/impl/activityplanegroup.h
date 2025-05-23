#ifndef ACTIVITYPLANEGROUP_H
#define ACTIVITYPLANEGROUP_H

#include <QQuickItem>

#include <private/qquicktransition_p.h>
#include <private/qquicktransitionmanager_p_p.h>

#include "activityplaneitem.h"

class PlanePushTransitionManager;

class ActivityPlaneGroup : public QQuickItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActivityPlaneGroup)

    Q_PROPERTY(ActivityPlaneItem * windowPlane READ windowPlane WRITE setWindowPlane NOTIFY windowPlaneChanged FINAL)
    Q_PROPERTY(ActivityPlaneItem * fullscreenPlane READ fullscreenPlane WRITE setFullscreenPlane NOTIFY fullscreenPlaneChanged FINAL)
    Q_PROPERTY(ActivityPlaneItem * topPlane READ topPlane WRITE setTopPlane NOTIFY topPlaneChanged FINAL)
    Q_PROPERTY(ActivityPlaneItem * layoutPlane READ layoutPlane WRITE setLayoutPlane NOTIFY layoutPlaneChanged FINAL)
    Q_PROPERTY(ActivityPlaneItem * overlayPlane READ overlayPlane WRITE setOverlayPlane NOTIFY overlayPlaneChanged FINAL)

    Q_PROPERTY(ActivityPlaneItem * currentPlane READ currentPlane WRITE setCurrentPlane NOTIFY currentPlaneChanged FINAL)

    Q_PROPERTY(QQuickTransition * enter READ enter WRITE setEnter NOTIFY enterChanged FINAL)
    Q_PROPERTY(QQuickTransition * leave READ leave WRITE setLeave NOTIFY leaveChanged FINAL)

public:
    explicit ActivityPlaneGroup(QQuickItem *parent = nullptr);
    ~ActivityPlaneGroup() override;

    ActivityPlaneItem *windowPlane() const;
    void setWindowPlane(ActivityPlaneItem *windowPlane);

    ActivityPlaneItem *fullscreenPlane() const;
    void setFullscreenPlane(ActivityPlaneItem *fullscreenPlane);

    ActivityPlaneItem *topPlane() const;
    void setTopPlane(ActivityPlaneItem *topPlane);

    ActivityPlaneItem *layoutPlane() const;
    void setLayoutPlane(ActivityPlaneItem *layoutPlane);

    ActivityPlaneItem *overlayPlane() const;
    void setOverlayPlane(ActivityPlaneItem *overlayPlane);

    QQuickTransition *enter() const;
    void setEnter(QQuickTransition *trans);

    QQuickTransition *leave() const;
    void setLeave(QQuickTransition *trans);

    ActivityPlaneItem *currentPlane() const;
    void setCurrentPlane(ActivityPlaneItem *currentPlane);

Q_SIGNALS:
    void windowPlaneChanged();
    void fullscreenPlaneChanged();
    void topPlaneChanged();
    void layoutPlaneChanged();
    void overlayPlaneChanged();
    void currentPlaneChanged();

    void enterChanged();
    void leaveChanged();

private:
    ActivityPlaneItem * _windowPlane            = nullptr;
    ActivityPlaneItem * _fullscreenPlane        = nullptr;
    ActivityPlaneItem * _topPlane               = nullptr;
    ActivityPlaneItem * _layoutPlane            = nullptr;
    ActivityPlaneItem * _overlayPlane           = nullptr;
    ActivityPlaneItem * _currentPlane           = nullptr;

    QQuickTransition *  _enter                  = nullptr;
    QQuickTransition *  _leave                  = nullptr;

    PlanePushTransitionManager *_pushTransitionManager = nullptr;
    PlanePushTransitionManager *_popTransitionManager  = nullptr;
};

#endif // ACTIVITYPLANEGROUP_H
