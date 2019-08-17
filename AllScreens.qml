import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.3

Column {
    id: root
    spacing: 8

    Label {
        id: countLabel
        text: screenInfo.count + " MONITOR(S) DETECTED"
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
        spacing: 10

        Repeater {
            id: screenInfo
            model: Qt.application.screens

            Item {
                width: 200
                height: 100

                Rectangle {
                    id: rect
                    anchors.fill: parent
                    color: "#4F71FF"
                    radius: 5
                    border.width: 2
                    border.color: "blue"
                }
                Label {
                    anchors.centerIn: rect
                    lineHeight: 1.5
                    text: "MONITOR " + (index + 1) + "\n" + modelData.width + " x " + modelData.height
                    font.bold: true
                    color: "white"
                }
            }
        }

        ButtonGroup {
            id: radioGroup
        }

        Column {
            id: multiScreenModes

            CustomButton {
                checked: true
                text: qsTr("Set Different Wallpaper for each Monitor")
                ButtonGroup.group: radioGroup
                iconSource: "/resources/icons/screens-1.svg"
            }

            CustomButton {
                text: qsTr("Set Single Wallpaper Across All Monitors")
                ButtonGroup.group: radioGroup
                iconSource: "/resources/icons/screens-2.svg"
            }

            CustomButton {
                text: qsTr("Duplicate Single Wallpaper")
                ButtonGroup.group: radioGroup
                iconSource: "/resources/icons/screens-3.svg"
            }

            /*Rectangle {
                anchors.top: multiScreenModes.top
                anchors.left: multiScreenModes.left
                width : 30
                height : 30
                color: "red"
            }*/
        }
    }

    Component.onCompleted: {
        var screens = Qt.application.screens;
        for (var i = 0; i < screens.length; ++i)
            console.log("screen " + screens[i].name + " has geometry " +
                        screens[i].virtualX + ", " + screens[i].virtualY + " " +
                        screens[i].width + "x" + screens[i].height)
    }
}
