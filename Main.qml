import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root

    width: 480
    height: 640
    visible: true
    title: qsTr("NonoDeskPet 控制台")

    Label {
        anchors.centerIn: parent
        text: qsTr("NonoDeskPet")
    }
}
