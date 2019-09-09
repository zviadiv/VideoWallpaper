import QtQuick 2.3
import QtQuick.Controls 2.3

Item {

    signal wallpaperModeChanged(var screenMode)

    ButtonGroup {
        id: radioGroup
        buttons: multiScreenModes.children
        onClicked: wallpaperModeChanged(radioGroup.checkedButton.tag)
    }

    Column {
        id: multiScreenModes
        visible: Qt.application.screens.length !== 0

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
