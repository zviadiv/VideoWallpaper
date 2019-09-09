import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.3

Column {
    id: root
    spacing: 8

    property int currentIndex

    QtObject {
        id: internal
        property int maxWidth: 0
    }

    signal wallpaperModeChanged(var screenMode)

    Label {
        id: countLabel
        text: screenInfo.count + " MONITOR(S) DETECTED"
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Row {
        spacing: 4

        Repeater {
            id: screenInfo
            model: Qt.application.screens

            Item {
                width: 200 * modelData.width / internal.maxWidth
                height: width * (modelData.height / modelData.width)

                Rectangle {
                    id: rect
                    anchors.fill: parent
                    color: index == currentIndex ? "#4F71FF" : "white"
                    radius: 5
                    border.width: 2
                    border.color: "#3554cd"
                }
                Label {
                    anchors.centerIn: rect
                    lineHeight: 1.5
                    text: "MONITOR " + (index + 1) + "\n" + modelData.width + " x " + modelData.height
                    font.bold: true
                    color: index == currentIndex ? "white" : "#3554cd"
                }
            }
        }

        ButtonGroup {
            id: radioGroup
            buttons: multiScreenModes.children
            onClicked: wallpaperModeChanged(radioGroup.checkedButton.tag)
        }

        Column {
            id: multiScreenModes
            visible: screenInfo.count != 0

            CustomButton {
                checked: true
                text: qsTr("Set Different Wallpaper for each Monitor")
                iconSource: "/resources/icons/screens-1.svg"
                tag: Constants.ScreenMode.Unique
            }

            CustomButton {
                text: qsTr("Set Single Wallpaper Across All Monitors")
                iconSource: "/resources/icons/screens-2.svg"
                tag: Constants.ScreenMode.Shared
            }

            CustomButton {
                text: qsTr("Duplicate Single Wallpaper")
                iconSource: "/resources/icons/screens-3.svg"
                tag: Constants.ScreenMode.Copy
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
        internal.maxWidth = 0;
        for (var i = 0; i < screens.length; ++i)
            if (screens[i].width > internal.maxWidth)
                internal.maxWidth = screens[i].width;
    }
}
