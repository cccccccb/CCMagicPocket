#ifndef MODULARLISTMODEL_H
#define MODULARLISTMODEL_H

#include <QAbstractListModel>
#include <QQmlComponent>

class ModularListModelPrivate;

class ModularListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ModularListModel)

    Q_PROPERTY(QAbstractListModel* preModel READ preModel WRITE setPreModel NOTIFY preModelChanged FINAL)
    Q_PROPERTY(QAbstractListModel* midModel READ midModel WRITE setMidModel NOTIFY midModelChanged FINAL)
    Q_PROPERTY(QAbstractListModel* postModel READ postModel WRITE setPostModel NOTIFY postModelChanged FINAL)

    Q_PRIVATE_SLOT(dd, void _q_onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles))
    Q_PRIVATE_SLOT(dd, void _q_onModelRowsAboutToBeInserted(const QModelIndex &parent, int first, int last))
    Q_PRIVATE_SLOT(dd, void _q_onModelRowsInserted(const QModelIndex &parent, int first, int last))
    Q_PRIVATE_SLOT(dd, void _q_onModelRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last))
    Q_PRIVATE_SLOT(dd, void _q_onModelRowsRemoved(const QModelIndex &parent, int first, int last))
    Q_PRIVATE_SLOT(dd, void _q_onModelAboutToBeReset())
    Q_PRIVATE_SLOT(dd, void _q_onModelReset())
    Q_PRIVATE_SLOT(dd, void _q_onModelRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow))
    Q_PRIVATE_SLOT(dd, void _q_onModelRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow))

public:
    explicit ModularListModel(QObject *parent = nullptr);
    ~ModularListModel();

    QAbstractListModel *preModel() const;
    void setPreModel(QAbstractListModel *newPreModel);

    QAbstractListModel *midModel() const;
    void setMidModel(QAbstractListModel *newMidModel);

    QAbstractListModel *postModel() const;
    void setPostModel(QAbstractListModel *newPostModel);

Q_SIGNALS:
    void preModelChanged();
    void midModelChanged();
    void postModelChanged();

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    friend class ModularListModelPrivate;
    QScopedPointer<ModularListModelPrivate> dd;
};

#endif // MODULARLISTMODEL_H
