import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

Window {
    visible: true
    width: 640
    height: 600
    title: qsTr("Video Wallpaper")

    FileDialog {
        id: browseVideoDialog
        title: "Please choose your video"
        folder: shortcuts.home
        nameFilters: [ "Video files (*.mp4 *.avi)", "All files (*)" ]
        onAccepted: {
            console.log("Video path: " + fileDialog.fileUrls)
        }
    }

    FileDialog {
        id: browseAudioDialog
        title: "Please choose your audio"
        folder: shortcuts.home
        nameFilters: [ "Audio files (*.mp3 *.wav)", "All files (*)" ]
        onAccepted: {
            console.log("Audio path: " + fileDialog.fileUrls)
        }
    }

    TabBar {
        id: tabBar
        x: 0
        y: 176
        width: 640
        height: 216
        position: TabBar.Header

        TabButton {
            width: 100
            text: qsTr("Monitor 1")
            font.bold: true
        }
    }

    StackLayout {
        anchors.rightMargin: 0
        anchors.bottomMargin: 208
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Item {
            id: firstTab

            Label {
                id: label
                x: 56
                y: 241
                width: 111
                height: 19
                text: qsTr("VIDEO SETTINGS")
                font.bold: true
            }

            TextInput {
                id: textInputVideoPath
                x: 56
                y: 266
                width: 242
                height: 20
                text: qsTr("Choose your video")
                font.weight: Font.Thin
                leftPadding: 5
                padding: 0
                font.pixelSize: 12
            }

            Button {
                id: buttonBrowseVideo
                x: 299
                y: 264
                width: 100
                height: 24
                text: qsTr("BROWSE")

                onClicked: {
                    browseVideoDialog.open()
                }
            }

            Button {
                id: buttonApplyVideo
                x: 405
                y: 264
                width: 100
                height: 24
                text: qsTr("APPLY")
                spacing: 0
                highlighted: false
            }

            Button {
                id: buttonRemoveVideo
                x: 511
                y: 264
                width: 100
                height: 24
                text: qsTr("REMOVE")
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
                x: 154
                y: 313
                width: 105
                height: 17
            }

            CustomSlider {
                id: sliderVideoPosition
                slider.x: 140
                slider.y: 347
                slider.width: 144
                slider.height: 21
                slider.value: 0
            }

            CustomSlider {
                id: sliderVideoOverlay
                slider.x: 444
                slider.y: 309
                slider.width: 144
                slider.height: 21
                slider.value: 0
            }

            CustomSlider {
                id: sliderVideoVolume
                slider.x: 444
                slider.y: 348
                slider.width: 144
                slider.height: 21
                slider.value: 0
            }

            CheckBox {
                id: checkBoxVideoVolume
                x: 317
                y: 338
                text: qsTr("VIDEO VOLUME")
            }

            CheckBox {
                id: checkBoxVideoOverlay
                x: 317
                y: 300
                text: qsTr("VIDEO OVERLAY")
            }

            Rectangle {
                id: rectangle
                x: 213
                y: 28
                width: 215
                height: 122
                color: "#495a94"
            }

            Label {
                id: label5
                x: 265
                y: 8
                width: 111
                height: 19
                text: qsTr("1 MONITOR DETECTED")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

        }
    }

    Label {
        id: label3
        x: 57
        y: 422
        text: qsTr("MUSIC SETTINGS")
        font.bold: true
    }

    TextInput {
        id: textInputMusicPath
        x: 56
        y: 447
        width: 242
        height: 20
        text: qsTr("Choose your music")
        leftPadding: 5
        padding: 0
        font.pixelSize: 12
        font.weight: Font.Thin
    }

    Button {
        id: buttonBrowseMusic
        x: 299
        y: 445
        width: 100
        height: 24
        text: qsTr("BROWSE")

        onClicked: {
            browseAudioDialog.open()
        }
    }

    Button {
        id: buttonApplyMusic
        x: 405
        y: 445
        width: 100
        height: 24
        text: qsTr("APPLY")
        highlighted: false
        spacing: 0
    }

    Button {
        id: buttonRemoveMusic
        x: 511
        y: 445
        width: 100
        height: 24
        text: qsTr("REMOVE")
    }

    Label {
        id: label4
        x: 194
        y: 488
        width: 111
        height: 0
        text: qsTr("MUSIC VOLUME")
    }

    CustomSlider {
        id: sliderMusicVolume
        slider.x: 273
        slider.y: 483
        slider.width: 144
        slider.height: 21
        slider.value: 0
    }

    Rectangle {
        id: footerRectangle
        x: 0
        y: 519
        width: 640
        height: 81
        color: "#495a94"
        Button {
            id: buttonRemoveWallpaper
            x: 26
            y: 28
            width: 139
            height: 26
            text: qsTr("REMOVE WALLPAPER")
        }

        Button {
            id: buttonRemoveWallpaper1
            x: 387
            y: 28
            width: 226
            height: 26
            text: qsTr("SAVE WALLPAPER && CLOSE WINDOW")
        }
    }
}
