#include "modularlistmodel.h"

class ModularListModelPrivate {
public:
    ModularListModelPrivate(ModularListModel *qq)
        : _qq(qq)
    {

    }

    void initConnections(QAbstractListModel *model);

    void _q_onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);
    void _q_onModelRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void _q_onModelRowsInserted(const QModelIndex &parent, int first, int last);
    void _q_onModelRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void _q_onModelRowsRemoved(const QModelIndex &parent, int first, int last);
    void _q_onModelAboutToBeReset();
    void _q_onModelReset();
    void _q_onModelRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void _q_onModelRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);

    ModularListModel *_qq = nullptr;

    QAbstractListModel *_preModel = nullptr;
    QAbstractListModel *_midModel = nullptr;
    QAbstractListModel *_postModel = nullptr;
};

void ModularListModelPrivate::initConnections(QAbstractListModel *model)
{
    QObject::connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QList<int>)), _qq, SLOT(_q_onModelDataChanged(QModelIndex,QModelIndex,QList<int>)));
    QObject::connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), _qq, SLOT(_q_onModelRowsAboutToBeInserted(QModelIndex,int,int)));
    QObject::connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), _qq, SLOT(_q_onModelRowsInserted(QModelIndex,int,int)));
    QObject::connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), _qq, SLOT(_q_onModelRowsAboutToBeRemoved(QModelIndex,int,int)));
    QObject::connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), _qq, SLOT(_q_onModelRowsRemoved(QModelIndex,int,int)));
    QObject::connect(model, SIGNAL(modelAboutToBeReset()), _qq, SLOT(_q_onModelAboutToBeReset()));
    QObject::connect(model, SIGNAL(modelReset()), _qq, SLOT(_q_onModelReset()));
    QObject::connect(model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), _qq, SLOT(_q_onModelRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    QObject::connect(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), _qq, SLOT(_q_onModelRowsMoved(QModelIndex,int,int,QModelIndex,int)));
}

void ModularListModelPrivate::_q_onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    QList<int> pendingRoles = roles;
    pendingRoles.removeIf([](int role) {
        return role >= Qt::UserRole;
    });

    if (pendingRoles.isEmpty())
        return;

    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    int topLeftRow = topLeft.row();
    int bottomRightRow = bottomRight.row();

    if (model == _preModel) {
        Q_EMIT _qq->dataChanged(_qq->createIndex(topLeftRow, topLeft.column()), _qq->createIndex(bottomRightRow, bottomRight.column()), pendingRoles);
        return;
    }

    if (_preModel) {
        topLeftRow += _preModel->rowCount();
        bottomRightRow += _preModel->rowCount();
    }

    if (model == _midModel) {
        Q_EMIT _qq->dataChanged(_qq->createIndex(topLeftRow, topLeft.column()), _qq->createIndex(bottomRightRow, bottomRight.column()), pendingRoles);
        return;
    }

    if (_midModel) {
        topLeftRow += _midModel->rowCount();
        bottomRightRow += _midModel->rowCount();
    }

    Q_EMIT _qq->dataChanged(_qq->createIndex(topLeftRow, topLeft.column()), _qq->createIndex(bottomRightRow, bottomRight.column()), pendingRoles);
}

void ModularListModelPrivate::_q_onModelRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    if (model == _preModel) {
        _qq->beginInsertRows(QModelIndex(), first, last);
        return;
    }

    if (_preModel) {
        first += _preModel->rowCount();
        last += _preModel->rowCount();
    }

    if (model == _midModel) {
        _qq->beginInsertRows(QModelIndex(), first, last);
        return;
    }

    if (_midModel) {
        first += _midModel->rowCount();
        last += _midModel->rowCount();
    }

    _qq->beginInsertRows(QModelIndex(), first, last);
}

void ModularListModelPrivate::_q_onModelRowsInserted(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    _qq->endInsertRows();
}

void ModularListModelPrivate::_q_onModelRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    if (model == _preModel) {
        _qq->beginRemoveRows(QModelIndex(), first, last);
        return;
    }

    if (_preModel) {
        first += _preModel->rowCount();
        last += _preModel->rowCount();
    }

    if (model == _midModel) {
        _qq->beginRemoveRows(QModelIndex(), first, last);
        return;
    }

    if (_midModel) {
        first += _midModel->rowCount();
        last += _midModel->rowCount();
    }

    _qq->beginRemoveRows(QModelIndex(), first, last);
}

void ModularListModelPrivate::_q_onModelRowsRemoved(const QModelIndex &parent, int first, int last)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    _qq->endRemoveRows();
}

void ModularListModelPrivate::_q_onModelAboutToBeReset()
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    _qq->beginResetModel();
}

void ModularListModelPrivate::_q_onModelReset()
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    _qq->endResetModel();
}

void ModularListModelPrivate::_q_onModelRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    if (model == _preModel) {
        _qq->beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
        return;
    }

    if (_preModel) {
        sourceStart += _preModel->rowCount();
        sourceEnd += _preModel->rowCount();
        destinationRow += _preModel->rowCount();
    }

    if (model == _midModel) {
        _qq->beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
        return;
    }

    if (_midModel) {
        sourceStart += _midModel->rowCount();
        sourceEnd += _midModel->rowCount();
        destinationRow += _midModel->rowCount();
    }

    _qq->beginMoveRows(QModelIndex(), sourceStart, sourceEnd, QModelIndex(), destinationRow);
}

void ModularListModelPrivate::_q_onModelRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    QAbstractListModel *model = qobject_cast<QAbstractListModel *>(_qq->sender());
    if (!model)
        return;

    _qq->endMoveRows();
}

ModularListModel::ModularListModel(QObject *parent)
    : QAbstractListModel(parent)
    , dd(new ModularListModelPrivate(this))
{}

ModularListModel::~ModularListModel()
{

}

int ModularListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return (dd->_preModel ? dd->_preModel->rowCount() : 0)
           + (dd->_midModel ? dd->_midModel->rowCount() : 0)
           + (dd->_postModel ? dd->_postModel->rowCount() : 0);
}

bool ModularListModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

bool ModularListModel::canFetchMore(const QModelIndex &parent) const
{
    return false;
}

void ModularListModel::fetchMore(const QModelIndex &parent)
{

}

QVariant ModularListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int currentRow = index.row();
    if (dd->_preModel) {
        if (currentRow < dd->_preModel->rowCount())
            return dd->_preModel->data(dd->_preModel->index(currentRow), role);

        currentRow -= dd->_preModel->rowCount();
    }

    if (dd->_midModel) {
        if (currentRow < dd->_midModel->rowCount()) {
            auto val = dd->_midModel->data(dd->_midModel->index(currentRow), role);
            return val;
        }

        currentRow -= dd->_midModel->rowCount();
    }

    if (dd->_postModel && currentRow < dd->_postModel->rowCount())
        return dd->_postModel->data(dd->_postModel->index(currentRow), role);

    return QVariant();
}

bool ModularListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) == value)
        return false;

    int currentRow = index.row();

    do {
        if (dd->_preModel) {
            if (currentRow < dd->_preModel->rowCount()) {
                dd->_preModel->setData(dd->_preModel->index(currentRow), value, role);
                break;
            }

            currentRow -= dd->_preModel->rowCount();
        }

        if (dd->_midModel) {
            if (currentRow < dd->_midModel->rowCount()) {
                dd->_midModel->setData(dd->_midModel->index(currentRow), value, role);
                break;
            }

            currentRow -= dd->_midModel->rowCount();
        }

        if (dd->_postModel && currentRow < dd->_postModel->rowCount()) {
            dd->_postModel->setData(dd->_postModel->index(currentRow), value, role);
            break;
        }

    } while (false);

    Q_EMIT dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> ModularListModel::roleNames() const
{
    if (dd->_midModel) {
        return dd->_midModel->roleNames();
    }

    qWarning() << "[ModularListModel] The midModel need specify!";
    return {};
}

Qt::ItemFlags ModularListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (!index.isValid())
        return Qt::NoItemFlags;

    int currentRow = index.row();
    if (dd->_preModel) {
        if (currentRow < dd->_preModel->rowCount())
            return dd->_preModel->flags(dd->_preModel->index(currentRow));

        currentRow -= dd->_preModel->rowCount();
    }

    if (dd->_midModel) {
        if (currentRow < dd->_midModel->rowCount())
            return dd->_midModel->flags(dd->_midModel->index(currentRow));

        currentRow -= dd->_midModel->rowCount();
    }

    if (dd->_postModel && currentRow < dd->_postModel->rowCount())
        return dd->_postModel->flags(dd->_postModel->index(currentRow));

    return QAbstractItemModel::flags(index);
}

QAbstractListModel *ModularListModel::preModel() const
{
    return dd->_preModel;
}

void ModularListModel::setPreModel(QAbstractListModel *preModel)
{
    if (dd->_preModel == preModel)
        return;

    if (dd->_preModel) {
        disconnect(dd->_preModel, nullptr, this, nullptr);
    }

    dd->_preModel = preModel;
    dd->initConnections(dd->_preModel);
    Q_EMIT preModelChanged();
}

QAbstractListModel *ModularListModel::midModel() const
{
    return dd->_midModel;
}

void ModularListModel::setMidModel(QAbstractListModel *midModel)
{
    if (dd->_midModel == midModel)
        return;

    if (dd->_midModel) {
        disconnect(dd->_midModel, nullptr, this, nullptr);
    }

    dd->_midModel = midModel;
    dd->initConnections(dd->_midModel);
    Q_EMIT midModelChanged();
}

QAbstractListModel *ModularListModel::postModel() const
{
    return dd->_postModel;
}

void ModularListModel::setPostModel(QAbstractListModel *postModel)
{
    if (dd->_postModel == postModel)
        return;

    if (dd->_midModel) {
        disconnect(dd->_midModel, nullptr, this, nullptr);
    }

    dd->_postModel = postModel;
    dd->initConnections(dd->_midModel);
    Q_EMIT postModelChanged();
}

#include "moc_modularlistmodel.cpp"
