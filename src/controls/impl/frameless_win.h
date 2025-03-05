#ifndef _FRAMELESS_WIN_H_
#define _FRAMELESS_WIN_H_

#include <QObject>
#include <QQuickItem>
#include <QAbstractNativeEventFilter>

class WinFrameless : public QObject, QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit WinFrameless(QWindow *window, QObject *parent = nullptr);
    ~WinFrameless() override;

    void setEnabled(bool enabled);
    bool enabled() const;

    void setCanWindowResize(bool canResize);
    bool canWindowResize() const;

    void setEffect(const QString &effect);

    QVector<QQuickItem *> *moveUnderPointer();
    QVector<QQuickItem *> *moveExcludePointer();

    QWindow *window() const;

    static QStringList availableEffects();

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    bool isFullScreen();
    bool isMaximized();
    void showSystemMenu(QPoint point);

    bool isMoveContains();
    void doFrameless(bool enabled);

private:
    bool                            _enabled = false;
    QWindow *                       _window = nullptr;
    quint64                         _current = 0;
    int                             _margins = 8;
    QString                         _effect;
    QVector<QQuickItem *>           _moveUnderContainer;
    QVector<QQuickItem *>           _moveExcludeContainer;
    bool                            _canWindowResize = true;
};

#endif  // _FRAMELESS_WIN_H_
