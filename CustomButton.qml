import QtQuick 2.2
import QtQuick.Controls 2.2

Button {
    id: control

    property alias iconSource: control.icon.source

    contentItem: Row {
        spacing: 5

        Image {
            source: control.icon.source
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            font: control.font
            color: control.checked ? "white" : "black"

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        color: control.checked ? "#ff8563" : "white"
        radius: 2
    }

    onClicked: {
        control.checked = true
    }
}
