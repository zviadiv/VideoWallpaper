import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

ApplicationWindow {
    id:root
    visible: true
    width: 640
    height: 600
    title: qsTr("Video Wallpaper ") + Qt.application.version + qsTr(" (pre-alpha)")

    onClosing: {
        console.log("Closing app window");
        //root.close()
    }

    FileDialog {
        id: browseVideoDialog
        title: "Please choose your video"
        folder: shortcuts.home
        nameFilters: [ "Video files (*.mp4 *.avi)", "All files (*)" ]
        onAccepted: {
            console.log("Video path: " + browseVideoDialog.fileUrls)
            viewController.videoUrl = browseVideoDialog.fileUrls[0]
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
                text: {
                 if (viewController.videoUrl.length == 0)
                     return qsTr("Choose your video")
                 else
                     return viewController.videoUrl
                }
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
                enabled: viewController.videoUrl.length != 0

                onClicked: {
                    viewController.playVideo()
                }
            }

            Button {
                id: buttonRemoveVideo
                x: 511
                y: 264
                width: 100
                height: 24
                text: qsTr("REMOVE")
                enabled: viewController.videoUrl.length != 0

                onClicked: {
                    viewController.removeVideo()
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
                width: 99
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
                        viewController.videoFillMode = Constants.VideoFillMode.Cover
                        break;
                    case 1:
                        viewController.videoFillMode = Constants.VideoFillMode.Contain
                        break;
                    case 2:
                        viewController.videoFillMode = Constants.VideoFillMode.Stretch
                        break;
                    }
                }
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
                enabled: checkBoxVideoOverlay.checked
            }

            CustomSlider {
                id: sliderVideoVolume
                slider.x: 444
                slider.y: 348
                slider.width: 144
                slider.height: 21
                slider.value: 0.5
                enabled: checkBoxVideoVolume.checked

                onValueChanged: {
                    viewController.videoVolume = slider.value
                }
            }

            CheckBox {
                id: checkBoxVideoVolume
                x: 317
                y: 338
                text: qsTr("VIDEO VOLUME")
                checked: false

                onCheckedChanged: {
                    viewController.mute = !checkBoxVideoVolume.checked
                }
            }

            CheckBox {
                id: checkBoxVideoOverlay
                x: 317
                y: 300
                text: qsTr("VIDEO OVERLAY")
                checked: false
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
        text: {
         if (viewController.musicUrl.length == 0)
             return qsTr("Choose your music")
         else
             return viewController.musicUrl
        }
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
        enabled: viewController.musicUrl.length != 0

        onClicked: {
            viewController.playMusic()
        }
    }

    Button {
        id: buttonRemoveMusic
        x: 511
        y: 445
        width: 100
        height: 24
        text: qsTr("REMOVE")
        enabled: viewController.musicUrl.length != 0

        onClicked: {
            viewController.removeMusic()
        }
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
        slider.value: 0.5

        onValueChanged: {
            viewController.musicVolume = slider.value
        }
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

            onClicked: {
                viewController.videoUrl = ""
                viewController.musicUrl = ""
                viewController.removeWallpaper()
            }
        }

        Button {
            id: buttonSaveWallpaperAndExit
            x: 387
            y: 28
            width: 226
            height: 26
            text: qsTr("SAVE WALLPAPER && CLOSE WINDOW")
        }
    }
}
