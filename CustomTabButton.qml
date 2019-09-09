import QtQuick 2.2
import QtQuick.Controls 2.2

TabButton {
    id: control

    contentItem: Text {
                        text: parent.text
                        font: parent.font
                        opacity: enabled ? 1.0 : 0.3
                        color: {
                            if (parent.checked)
                                return parent.down ? "#905040" : "#ff8363"
                            return "white"
                        }
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

    background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                color: parent.checked ? "#efeff7" : "#495a94"
                radius: 4
    }
}
