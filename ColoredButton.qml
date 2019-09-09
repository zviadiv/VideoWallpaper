import QtQuick 2.2
import QtQuick.Controls 2.2

Button {
    id: control

    property color  backgroundColor: "#d1d2ea"
    property color  textColor: "black"

    contentItem: Text {
        text: parent.text
        font: parent.font
        color: textColor
        opacity: enabled ? 1 : 0.4
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        color: backgroundColor
        opacity: enabled ? 1 : 0.4
        radius: 4
    }
}
