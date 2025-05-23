#ifndef ACTIVITYPLANEITEM_H
#define ACTIVITYPLANEITEM_H

#include <QQuickItem>
#include <private/qquicktransition_p.h>

class ActivityPlaneGroup;

class ActivityPlaneItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(ActivityPlaneItem::PlaneLevel level READ level NOTIFY levelChanged FINAL)
    Q_PROPERTY(ActivityPlaneGroup *group READ group NOTIFY groupChanged FINAL)

    Q_PROPERTY(QQuickTransition *pushUp READ pushUp WRITE setPushUp NOTIFY pushUpChanged FINAL)
    Q_PROPERTY(QQuickTransition *popUp READ popUp WRITE setPopUp NOTIFY popUpChanged FINAL)

    Q_PROPERTY(QQuickTransition *pushDown READ pushDown WRITE setPushDown NOTIFY pushDownChanged FINAL)
    Q_PROPERTY(QQuickTransition *popDown READ popDown WRITE setPopDown NOTIFY popDownChanged FINAL)

    friend class ActivityPlaneGroup;
public:
    explicit ActivityPlaneItem(QQuickItem *parent = nullptr);
    ~ActivityPlaneItem() override;

    enum PlaneLevel {
        UnknowLevel = -1,
        WindowLevel,
        FullscreenLevel,
        TopLevel,
        LayoutLevel,
        OverlayLevel,
    };

    Q_ENUM(PlaneLevel)

    PlaneLevel level() const;
    ActivityPlaneGroup *group() const;

    QQuickTransition *pushUp() const;
    void setPushUp(QQuickTransition *push);

    QQuickTransition *popUp() const;
    void setPopUp(QQuickTransition *pop);

    QQuickTransition *pushDown() const;
    void setPushDown(QQuickTransition *push);

    QQuickTransition *popDown() const;
    void setPopDown(QQuickTransition *pop);

Q_SIGNALS:
    void pushUpChanged();
    void popUpChanged();

    void pushDownChanged();
    void popDownChanged();

    void groupChanged();
    void levelChanged();

private:
    void setLevel(PlaneLevel level);
    void setGroup(ActivityPlaneGroup *group);

private:
    QQuickTransition *      _pushUpTrans     = nullptr;
    QQuickTransition *      _popUpTrans      = nullptr;

    QQuickTransition *      _pushDownTrans     = nullptr;
    QQuickTransition *      _popDownTrans      = nullptr;

    PlaneLevel              _level         = PlaneLevel::UnknowLevel;
    ActivityPlaneGroup *    _group         = nullptr;
};

#endif // ACTIVITYPLANEITEM_H
