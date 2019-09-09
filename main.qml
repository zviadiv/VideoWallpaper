import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

ApplicationWindow {
    id:root
    visible: true
    width: 710
    height: 560
    title: qsTr("Video Wallpaper ") + Qt.application.version + qsTr(" (pre-alpha)")
    //flags: Qt.WindowStaysOnTopHint

    onClosing: {
        console.log("Closing app window");
        //root.close()
    }

    Component {
        id: tabButton
        CustomTabButton { }
    }

    function rebuildTabBar()
    {
        for (var i = 0; i < Qt.application.screens.length; i++)
        {
            var tab = tabButton.createObject(tabBar, {
                    width: 100,
                    height: 30,
                    text: qsTr("MONITOR ") + (i + 1)})
            //tab.font.bold = true
            tabBar.addItem(tab)
        }
    }

    AllScreens {
        id: screens
        anchors.right: parent.horizontalCenter
        y: 10
        width: 215
        height: 122
        currentIndex: 0

        onWallpaperModeChanged: {
            viewController.setScreenMode(screenMode)
            if (screenMode == Constants.ScreenMode.Shared ||
                screenMode == Constants.ScreenMode.Copy)
            {
                for (var i = 1; i < Qt.application.screens.length; i++)
                    tabBar.removeItem(tabBar.itemAt(i))
            }
        }
    }

    TabBar {
        id: tabBar
        x: 0
        y: 176
        width: 710
        height: 216
        position: TabBar.Header

        onCurrentIndexChanged: screens.currentIndex = tabBar.currentIndex
        Component.onCompleted: {
            rebuildTabBar()
            currentIndex = 0
        }
    }

    StackLayout {
        y: 7
        anchors.rightMargin: 0
        anchors.bottomMargin: 168
        currentIndex: tabBar.currentIndex

        Repeater {
            model: Qt.application.screens

            Rectangle {
                id: tab
                color: "red"

                property string videoUrl

                FileDialog {
                    id: browseVideoDialog
                    title: "Please choose your video"
                    folder: shortcuts.home
                    nameFilters: [ "Video files (*.mp4 *.avi)", "All files (*)" ]
                    onAccepted: {
                        console.log("Video path: " + browseVideoDialog.fileUrls)
                        videoUrl = browseVideoDialog.fileUrls[0]
                    }
                }

                FileDialog {
                    id: browseAudioDialog
                    title: "Please choose your audio"
                    folder: shortcuts.home
                    nameFilters: [ "Audio files (*.mp3 *.wav)", "All files (*)" ]
                    onAccepted: {
                        console.log("Audio path: " + browseAudioDialog.fileUrls)
                        viewController.musicUrl = browseAudioDialog.fileUrls[0]
                    }
                }

                Label {
                    id: label
                    x: 56
                    y: 241
                    width: 111
                    height: 19
                    text: qsTr("VIDEO SETTINGS")
                    font.bold: true
                }

                Row {
                    id: row
                    x: 56
                    y: 266
                    width: 593
                    height: 28
                    spacing: 5
                    TextField {
                        id: textInputVideoPath
                        x: 56
                        y: 0
                        width: 242
                        height: 24
                        placeholderText: qsTr("Choose your video")
                        text: videoUrl
                        font.weight: Font.Thin
                        font.pixelSize: 12
                        leftPadding: 5
                        padding: 0
                    }

                    Button {
                        id: buttonBrowseVideo
                        x: 299
                        y: 0
                        width: 100
                        height: 24
                        text: qsTr("BROWSE")

                        onClicked: {
                            browseVideoDialog.open()
                        }
                    }

                    ColoredButton {
                        id: buttonApplyVideo
                        x: 405
                        y: 0
                        width: 80
                        height: 24
                        text: qsTr("APPLY")
                        spacing: 0
                        highlighted: false
                        backgroundColor: "#ff8563"
                        textColor: "white"
                        enabled: videoUrl.length != 0

                        onClicked: {
                            viewController.playVideo(tabBar.currentIndex, videoUrl);
                        }
                    }

                    ColoredButton {
                        id: buttonRemoveVideo
                        x: 511
                        y: 0
                        width: 80
                        height: 24
                        text: qsTr("REMOVE")
                        anchors.right: root.right
                        enabled: videoUrl.length != 0

                        onClicked: {
                            viewController.removeVideo(tabBar.currentIndex)
                        }
                    }
                }

                Label {
                    id: label1
                    x: 56
                    y: 313
                    width: 111
                    height: 0
                    text: qsTr("VIDEO SIZE")
                }

                Label {
                    id: label2
                    x: 56
                    y: 347
                    width: 111
                    height: 0
                    text: qsTr("VIDEO POSITION")
                }

                ComboBox {
                    id: comboBoxVideoSize
                    x: 152
                    y: 308
                    width: 127
                    height: 28
                    model: [ "Cover", "Contain", "Stretch" ]

                    enum VideoFillMode
                    {
                        Cover = 0, Contain, Stretch
                    }

                    onCurrentIndexChanged: {
                        switch (comboBoxVideoSize.currentIndex)
                        {
                        case 0:
                            viewController.setVideoFillMode(tabBar.currentIndex, Constants.VideoFillMode.Cover)
                            break;
                        case 1:
                            viewController.setVideoFillMode(tabBar.currentIndex, Constants.VideoFillMode.Contain)
                            break;
                        case 2:
                            viewController.setVideoFillMode(tabBar.currentIndex, Constants.VideoFillMode.Stretch)
                            break;
                        }
                    }
                }

                CustomSlider {
                    id: sliderVideoPosition
                    slider { x: 140; y: 347; width: 144; height: 21; value: 0 }
                }

                CustomSlider {
                    id: sliderVideoOverlay
                    slider { x: 444; y: 309; width: 144; height: 21; value: 0 }
                    enabled: checkBoxVideoOverlay.checked
                }

                CustomSlider {
                    id: sliderVideoVolume
                    slider { x: 444; y: 348; width: 144; height: 21; value: 0.5 }
                    enabled: checkBoxVideoVolume.checked

                    onValueChanged: {
                        viewController.setVideoVolume(tabBar.currentIndex, slider.value)
                    }
                }

                CheckBox {
                    id: checkBoxVideoVolume
                    x: 317
                    y: 338
                    text: qsTr("VIDEO VOLUME")
                    checked: false

                    onCheckedChanged: {
                        viewController.setMuteVideo(tabBar.currentIndex, !checkBoxVideoVolume.checked)
                    }
                }

                CheckBox {
                    id: checkBoxVideoOverlay
                    x: 317
                    y: 300
                    text: qsTr("VIDEO OVERLAY")
                    checked: false
                }
            }
        }
    }

    Label {
        id: label3
        x: 42
        y: 398
        text: qsTr("MUSIC SETTINGS")
        font.bold: true
    }

    TextField {
        id: textInputMusicPath
        x: 41
        y: 421
        width: 242
        height: 24
        placeholderText: qsTr("Choose your music")
        text: viewController.musicUrl
        leftPadding: 5
        padding: 0
        font.pixelSize: 12
        font.weight: Font.Thin
    }

    Button {
        id: buttonBrowseMusic
        x: 284
        y: 421
        width: 100
        height: 24
        text: qsTr("BROWSE")

        onClicked: {
            browseAudioDialog.open()
        }
    }

    ColoredButton {
        id: buttonApplyMusic
        x: 390
        y: 421
        width: 80
        height: 24
        text: qsTr("APPLY")
        highlighted: false
        backgroundColor: "#ff8563"
        textColor: "white"
        spacing: 0
        enabled: viewController.musicUrl.length != 0

        onClicked: {
            viewController.playMusic()
        }
    }

    ColoredButton {
        id: buttonRemoveMusic
        x: 496
        y: 421
        width: 80
        height: 24
        text: qsTr("REMOVE")
        enabled: viewController.musicUrl.length != 0

        onClicked: {
            viewController.removeMusic()
        }
    }

    Label {
        id: label4
        x: 179
        y: 464
        width: 111
        height: 0
        text: qsTr("MUSIC VOLUME")
    }

    CustomSlider {
        id: sliderMusicVolume
        x: -15
        y: -24
        slider { x: 273; y: 483; width: 144; height: 21; value: 0.5 }

        onValueChanged: {
            viewController.musicVolume = slider.value
        }
    }

    Rectangle {
        id: footerRectangle
        y: 486
        height: 74
        color: "#495a94"
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        Button {
            id: buttonRemoveWallpaper
            y: 14
            width: 139
            height: 26
            text: qsTr("REMOVE WALLPAPER")
            anchors.left: parent.left
            anchors.leftMargin: 18

            onClicked: {
                videoUrl = ""
                viewController.musicUrl = ""
                viewController.removeWallpaper()
            }
        }

        Button {
            id: buttonSaveWallpaperAndExit
            x: 473
            y: 17
            width: 216
            height: 26
            text: qsTr("SAVE WALLPAPER && CLOSE WINDOW")
            anchors.right: parent.right
            anchors.rightMargin: 21
        }

        Label {
            id: label5
            x: 218
            y: 53
            color: "#ffffff"
            text: qsTr("Copyright © mylivewallpapers.com. All Rights Reserved.")
        }
    }
}































/*##^## Designer {
    D{i:5;anchors_height:384;anchors_width:688;anchors_x:8;anchors_y:39}D{i:29;anchors_height:81;anchors_width:1200;anchors_x:0;anchors_y:865}
D{i:30;anchors_height:66;anchors_width:710;anchors_x:0;anchors_y:494}D{i:31;anchors_x:26}
}
 ##^##*/
