pragma Singleton

import QtQuick
import CCMagicPocket.impl

Loader {
    source: MagicPocket.isAndroid ? "mobile/MobileStyle.qml" : "desktop/DesktopStyle.qml"
}
