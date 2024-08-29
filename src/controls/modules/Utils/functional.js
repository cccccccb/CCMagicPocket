function mapToTarget(source, target, rect)
{
    return source.mapToItem(target, source.mapFromItem(source.parent, rect))
}

function commonParent(item1, item2)
{
    var item1Wp = item1
    var item2Wp = item2

    while (item1Wp !== item2Wp) {
        item1Wp = (item1Wp === null) ? item2 : item1Wp.parent
        item2Wp = (item2Wp === null) ? item1 : item2Wp.parent
    }

    return item1Wp
}

function parentsTo(item, parent)
{
    var parents = []
    var itemWp = item
    while (itemWp !== null) {
        if (itemWp === parent)
            break

        parents.push(itemWp)
        itemWp = itemWp.parent
    }

    return parents
}
