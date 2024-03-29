import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.1

ApplicationWindow {
    id:root
    visible: true
    width: 800
    height: 630
    maximumWidth: 800
    minimumWidth: maximumWidth
    maximumHeight: 630
    minimumHeight: maximumHeight
    title: qsTr("Video Wallpaper ") + Qt.application.version
    //flags: Qt.WindowStaysOnTopHint

    /*MessageDialog {
        id: messageDialog
        title: "Video Wallpaper"
        text: "Do you want to quit the application?"
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        modality: Qt.ApplicationModal
        onYes: {
            Qt.quit()
        }
    }*/

    onClosing: {
        //hide()
        //messageDialog.visible = true
    }

    Component {
        id: tabButton
        CustomTabButton { }
    }

    function rebuildTabBar(merged)
    {
        if (tabBar.count > 0)
        {
            tabBar.contentChildren = null
        }

        var mergedTabText = "MONITOR "
        for (var i = 0; i < Qt.application.screens.length; i++)
        {
            if (merged)
            {
                if (i !== 0)
                    mergedTabText += "& "
                mergedTabText += (i + 1) + " "
            }
            else
            {
                var tab = tabButton.createObject(tabBar, {
                                                     width: 150,
                                                     height: 30,
                                                     text: qsTr("MONITOR ") + (i + 1)})
                //tab.font.bold = true
                tabBar.addItem(tab)
            }
        }
        if (merged)
        {
            tab = tabButton.createObject(tabBar, {
                                             width: 150,
                                             height: 30,
                                             text: mergedTabText
                                         })
            //tab.font.bold = true
            tabBar.addItem(tab)
        }
    }

    Row {
        spacing: 10
        y: 10
        height: 200
        anchors.horizontalCenter: parent.horizontalCenter

        AllScreens {
            id: screens
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            currentIndex: 0
        }
        ScreenModeSwitch {
            id: screenMode
            width: 200
            anchors.top : parent.top
            anchors.bottom: parent.bottom

            onWallpaperModeChanged: {
                viewController.setScreenMode(screenMode)
                var mergedTab = (screenMode === Constants.ScreenMode.Shared ||
                                 screenMode === Constants.ScreenMode.Copy)
                rebuildTabBar(mergedTab)
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

        onCurrentIndexChanged: {
            screens.currentIndex = tabBar.currentIndex
            stackLayout.currentIndex = tabBar.currentIndex
        }
        Component.onCompleted: {
            rebuildTabBar(false)
            currentIndex = 0
        }
    }

    Drawer {
        id: drawer
        width: 0.25 * parent.width
        height: parent.height
        edge: Qt.RightEdge

        Column {
            anchors.centerIn: parent
            spacing: 20

            CustomCheckbox {
                id: checkBoxAutoStart
                x: 0
                text: qsTr("Autostart")
                checked: viewController.autoStartEnabled()

                onCheckedChanged: viewController.enableAutoStart(checked)
            }

            ColumnLayout {
                Label {
                    text: "Renderer Quality:"
                }
                RadioButton {
                    checked: true
                    text: qsTr("Default")
                    onCheckedChanged: {
                        if (checked)
                            viewController.setVideoQuality(Constants.RendererQuality.Default)
                    }
                }
                RadioButton {
                    text: qsTr("Best")
                    onCheckedChanged: {
                        if (checked)
                            viewController.setVideoQuality(Constants.RendererQuality.Best)
                    }
                }
                RadioButton {
                    text: qsTr("Fastest")
                    onCheckedChanged: {
                        if (checked)
                            viewController.setVideoQuality(Constants.RendererQuality.Fastest)
                    }
                }
            }
        }
    }

    ColoredButton {
        id: buttonSettings
        width: 100
        height: 30
        text: qsTr("SETTINGS")
        enabled: true
        anchors.top: tabBar.top
        anchors.right: parent.right
        anchors.rightMargin: 24
        backgroundColor: "#495a94"
        textColor: "white"

        onClicked: {
            drawer.visible = true
        }
    }


    StackLayout {
        id: stackLayout
        y: 7
        anchors.rightMargin: 0
        anchors.bottomMargin: 168
        currentIndex: tabBar.currentIndex

        Repeater {
            model: Qt.application.screens

            Item {
                id: tab

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
                        width: 360
                        height: 24
                        placeholderText: qsTr("Choose your video")
                        text: videoUrl
                        font.weight: Font.Thin
                        font.pixelSize: 12
                        leftPadding: 5
                        padding: 0
                    }

                    ColoredButton {
                        id: buttonBrowseVideo
                        width: 100
                        height: 24
                        text: qsTr("BROWSE")

                        onClicked: {
                            browseVideoDialog.open()
                        }
                    }

                    // Spacer
                    Item {
                        width: 25
                        height: parent.height
                    }

                    ColoredButton {
                        id: buttonApplyVideo
                        width: 80
                        height: 24
                        text: qsTr("APPLY")
                        spacing: 0
                        highlighted: false
                        backgroundColor: "#ff8563"
                        textColor: "white"
                        enabled: videoUrl.length !== 0

                        onClicked: {
                            viewController.playVideo(tabBar.currentIndex, videoUrl);
                        }
                    }

                    ColoredButton {
                        id: buttonRemoveVideo
                        width: 80
                        height: 24
                        text: qsTr("REMOVE")
                        anchors.right: root.right
                        enabled: videoUrl.length !== 0

                        onClicked: {
                            viewController.removeVideo(tabBar.currentIndex)
                        }
                    }
                }

                /*Image {
                    width: 16
                    height: 16
                    fillMode: Image.PreserveAspectFit
                    source: "resources/icons/icon-video-size.svg"
                }*/

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
                    slider { x: 140; y: 347; width: 144; height: 21; value: 0.5 }

                    onValueChanged: {
                        viewController.setVideoOffset(tabBar.currentIndex, slider.value)
                    }
                }

                CustomSlider {
                    id: sliderVideoOverlay
                    slider { x: 444; y: 309; width: 144; height: 21; value: 0.5 }
                    enabled: checkBoxVideoOverlay.checked

                    onValueChanged: {
                        viewController.setOverlayOpacity(tabBar.currentIndex, slider.value)
                    }
                }

                CustomSlider {
                    id: sliderVideoVolume
                    slider { x: 444; y: 348; width: 144; height: 21; value: 0.5 }
                    enabled: checkBoxVideoVolume.checked

                    onValueChanged: {
                        viewController.setVideoVolume(tabBar.currentIndex, slider.value)
                    }
                }

                CustomCheckbox {
                    id: checkBoxVideoVolume
                    x: 317
                    y: 338
                    text: qsTr("VIDEO VOLUME")
                    checked: false

                    onCheckedChanged: {
                        viewController.setMuteVideo(tabBar.currentIndex, !checkBoxVideoVolume.checked)
                    }
                }

                CustomCheckbox {
                    id: checkBoxVideoOverlay
                    x: 317
                    y: 300
                    text: qsTr("VIDEO OVERLAY")
                    checked: false

                    onCheckStateChanged: colorBoxes.setOverlayType()
                }

                Row {
                    x: 620
                    y: 309
                    spacing: 4
                    id: colorBoxes

                    property int selectedItem: 0

                    function setOverlayType()
                    {
                        var overlayType = selectedItem == 0 ? Constants.OverlayType.Black :
                                                              Constants.OverlayType.White
                        if (!checkBoxVideoOverlay.checked)
                            overlayType = Constants.OverlayType.None
                        viewController.setOverlayType(tabBar.currentIndex, overlayType)
                    }

                    Rectangle {
                        width: 20
                        height: 20
                        color: "black"
                        border.color: colorBoxes.selectedItem == 0 ? "red" : "gray"
                        border.width: 1
                        radius: 4

                        MouseArea {
                            id: blackArea
                            anchors.fill: parent
                            onClicked: {
                                if (checkBoxVideoOverlay.checked)
                                {
                                    colorBoxes.selectedItem = 0
                                    colorBoxes.setOverlayType()
                                }
                            }
                        }
                    }
                    Rectangle {
                        width: 20
                        height: 20
                        color: "white"
                        border.color: colorBoxes.selectedItem == 1 ? "red" : "gray"
                        border.width: 1
                        radius: 4

                        MouseArea {
                            id: whiteArea
                            anchors.fill: parent
                            onClicked: {
                                if (checkBoxVideoOverlay.checked)
                                {
                                    colorBoxes.selectedItem = 1
                                    colorBoxes.setOverlayType()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Label {
        id: label3
        x: 56
        y: 398
        text: qsTr("MUSIC SETTINGS")
        font.bold: true
    }

    Row {
        x: 56
        y: 421
        width: 593
        height: 28
        spacing: 5

        TextField {
            id: textInputMusicPath
            width: 360
            height: 24
            placeholderText: qsTr("Choose your music")
            text: viewController.musicUrl
            leftPadding: 5
            padding: 0
            font.pixelSize: 12
            font.weight: Font.Thin
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

        ColoredButton {
            id: buttonBrowseMusic
            width: 100
            height: 24
            text: qsTr("BROWSE")

            onClicked: {
                browseAudioDialog.open()
            }
        }

        // Spacer
        Item {
            width: 25
            height: parent.height
        }

        ColoredButton {
            id: buttonApplyMusic
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
            width: 80
            height: 24
            text: qsTr("REMOVE")
            enabled: viewController.musicUrl.length != 0

            onClicked: {
                viewController.removeMusic()
            }
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
        height: 100
        color: "#495a94"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        ColoredButton {
            id: buttonRemoveWallpaper
            y: 14
            width: 139
            height: 33
            text: qsTr("REMOVE WALLPAPER")
            backgroundColor: "#616faa"
            textColor: "white"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                viewController.musicUrl = ""
                viewController.removeWallpaper()
            }
        }

        ColoredButton {
            id: buttonSaveWallpaperAndClose
            x: 560
            y: 14
            width: 216
            height: 34
            text: qsTr("SAVE WALLPAPER && CLOSE WINDOW")
            enabled: true
            anchors.right: parent.right
            anchors.rightMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            backgroundColor: "#ff8563"
            textColor: "white"

            onClicked: viewController.saveWallpaperAndClose()
        }

        Rectangle {
            width: 300
            height: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -20
            color: "#616faa"
            radius: 60

            Label {
                id: label5
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 4
                color: "#ffffff"
                text: qsTr("Copyright © mylivewallpapers.com. All Rights Reserved.")
            }
        }
    }
}

































/*##^## Designer {
    D{i:5;anchors_height:384;anchors_width:688;anchors_x:8;anchors_y:39}D{i:29;anchors_height:81;anchors_width:1200;anchors_x:0;anchors_y:865}
D{i:30;anchors_height:66;anchors_width:710;anchors_x:0;anchors_y:494}D{i:31;anchors_x:26}
}
 ##^##*/
