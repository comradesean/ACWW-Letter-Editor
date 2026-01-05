import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: noPlayersDialog
    modal: true
    closePolicy: Popup.NoAutoClose
    anchors.centerIn: parent
    width: 320
    padding: 0

    // Theme colors (from parent)
    property color bgBase: "#0B1120"
    property color bgElevated: "#1E293B"
    property color bgHover: "#2D3B4F"
    property color accentPrimary: "#38BDF8"
    property color errorColor: "#EF4444"
    property color textPrimary: "#F1F5F9"
    property color textSecondary: "#94A3B8"
    property color divider: "#1E293B"

    signal acknowledged()

    background: Rectangle {
        color: bgElevated
        radius: 12
        border.color: divider
        border.width: 1
    }

    contentItem: Column {
        spacing: 0

        // Content area
        Item {
            width: parent.width
            height: contentColumn.height + 40

            Column {
                id: contentColumn
                anchors.centerIn: parent
                width: parent.width - 40
                spacing: 16

                // Error icon
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 48
                    height: 48
                    radius: 24
                    color: Qt.rgba(errorColor.r, errorColor.g, errorColor.b, 0.15)

                    Text {
                        anchors.centerIn: parent
                        text: "!"
                        font.pixelSize: 24
                        font.weight: Font.Bold
                        color: errorColor
                    }
                }

                // Title
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("No Players Found")
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    color: textPrimary
                }

                // Description
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    text: qsTr("This save file has no players. You must create a character in-game before editing letters.")
                    font.pixelSize: 13
                    color: textSecondary
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        // Divider
        Rectangle {
            width: parent.width
            height: 1
            color: divider
        }

        // Button area
        Item {
            width: parent.width
            height: 56

            Rectangle {
                anchors.centerIn: parent
                width: 80
                height: 36
                radius: 8
                color: okArea.pressed ? Qt.darker(accentPrimary, 1.1) :
                       okArea.containsMouse ? Qt.lighter(accentPrimary, 1.05) : accentPrimary

                Text {
                    anchors.centerIn: parent
                    text: qsTr("OK")
                    font.pixelSize: 12
                    font.weight: Font.Medium
                    color: bgBase
                }

                MouseArea {
                    id: okArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        noPlayersDialog.close()
                        acknowledged()
                    }
                }
            }
        }
    }
}
