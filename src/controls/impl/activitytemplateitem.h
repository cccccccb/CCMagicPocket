#ifndef ACTIVITYTEMPLATEITEM_H
#define ACTIVITYTEMPLATEITEM_H

#include <QQuickItem>

class ActivityTemplateItem : public QQuickItem
{
    friend class ActivityManagerPrivate;
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString displayName READ displayName CONSTANT FINAL)
    Q_PROPERTY(QString activityName READ activityName CONSTANT FINAL)
    Q_PROPERTY(QUrl iconPath READ iconPath CONSTANT FINAL)

    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY isCurrentItemChanged FINAL)
    Q_PROPERTY(QQuickItem * surface READ surface WRITE setSurface NOTIFY surfaceChanged FINAL)

    Q_PROPERTY(QQuickTransition * openTrans READ openTrans WRITE setOpenTrans NOTIFY openTransChanged FINAL)
    Q_PROPERTY(QQuickTransition * showTrans READ showTrans WRITE setShowTrans NOTIFY showTransChanged FINAL)

    Q_PROPERTY(QQuickTransition * minimizedTrans READ minimizedTrans WRITE setMinimizedTrans NOTIFY minimizedTransChanged FINAL)
    Q_PROPERTY(QQuickTransition * layoutTrans READ layoutTrans WRITE setLayoutTrans NOTIFY layoutTransChanged FINAL)
    Q_PROPERTY(QQuickTransition * closeTrans READ closeTrans WRITE setCloseTrans NOTIFY closeTransChanged FINAL)
    Q_PROPERTY(QQuickTransition * exitLayoutTrans READ exitLayoutTrans WRITE setExitLayoutTrans NOTIFY exitLayoutTransChanged FINAL)

public:
    explicit ActivityTemplateItem(QQuickItem *parent = nullptr);

    bool isCurrentItem() const;
    void setIsCurrentItem(bool current);

    QString displayName() const;
    void setDisplayName(const QString &displayName);

    QString activityName() const;
    void setActivityName(const QString &activityName);

    QUrl iconPath() const;
    void setIconPath(const QUrl &newIconPath);

    QQuickItem *surface() const;
    void setSurface(QQuickItem *surface);

    QQuickItem *fillOn() const;

    QQuickTransition *openTrans() const;
    void setOpenTrans(QQuickTransition *openTrans);

    QQuickTransition *showTrans() const;
    void setShowTrans(QQuickTransition *showTrans);

    QQuickTransition *minimizedTrans() const;
    void setMinimizedTrans(QQuickTransition *minimizedTrans);

    QQuickTransition *layoutTrans() const;
    void setLayoutTrans(QQuickTransition *layoutTrans);

    QQuickTransition *closeTrans() const;
    void setCloseTrans(QQuickTransition *closeTrans);

    QQuickTransition *exitLayoutTrans() const;
    void setExitLayoutTrans(QQuickTransition *exitLayoutTrans);

Q_SIGNALS:
    void minimized(const QString &activityName);
    void layouted(const QString &activityName);
    void closed(const QString &activityName);

    void isCurrentItemChanged();
    void surfaceChanged();

    void openTransChanged();
    void showTransChanged();

    void minimizedTransChanged();
    void layoutTransChanged();
    void closeTransChanged();
    void exitLayoutTransChanged();


private:
    QString             _displayName;
    QString             _activityName;
    QUrl                _iconPath;

    bool                _isCurrentItem      = false;
    QQuickItem *        _surface            = nullptr;
    QQuickTransition *  _minimizedTrans     = nullptr;
    QQuickTransition *  _layoutTrans        = nullptr;
    QQuickTransition *  _closeTrans         = nullptr;
    QQuickTransition *  _showTrans          = nullptr;
    QQuickTransition *  _openTrans          = nullptr;
    QQuickTransition *  _exitLayoutTrans    = nullptr;
};

#endif // ACTIVITYTEMPLATEITEM_H
