import QtQuick 2.2
import QtQuick.Controls 2.2

Item {

    id: root

    property alias slider: slider

    signal valueChanged()

    Slider {
        id: slider
        value: 0

        onValueChanged: {
            label.text = Math.floor(slider.value * 100)
            root.valueChanged()
        }
    }

    Label {
        id: label
        x: slider.x + slider.width
        y: slider.y - 5
        width: 25
        height: 25
        text: qsTr("0")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
