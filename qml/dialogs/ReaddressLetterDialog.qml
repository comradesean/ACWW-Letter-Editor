import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: readdressDialog
    modal: true
    closePolicy: Popup.CloseOnEscape
    anchors.centerIn: parent
    width: 320
    padding: 0

    // Theme colors (from parent)
    property color bgBase: "#0B1120"
    property color bgElevated: "#1E293B"
    property color bgHover: "#2D3B4F"
    property color accentPrimary: "#38BDF8"
    property color accentGreen: "#2DD4BF"
    property color textPrimary: "#F1F5F9"
    property color textSecondary: "#94A3B8"
    property color textMuted: "#64748B"
    property color divider: "#1E293B"

    signal accepted()
    signal declined()

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

                // Icon
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 48
                    height: 48
                    radius: 24
                    color: Qt.rgba(accentPrimary.r, accentPrimary.g, accentPrimary.b, 0.15)

                    Text {
                        anchors.centerIn: parent
                        text: "@"
                        font.pixelSize: 22
                        font.weight: Font.Bold
                        color: accentPrimary
                    }
                }

                // Title
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Readdress Letter")
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    color: textPrimary
                }

                // Description
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    text: qsTr("Would you like to readdress this letter to yourself?")
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

            Row {
                anchors.centerIn: parent
                spacing: 12

                // No button
                Rectangle {
                    width: 80
                    height: 36
                    radius: 8
                    color: noArea.containsMouse ? bgHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("No")
                        font.pixelSize: 12
                        color: textSecondary
                    }

                    MouseArea {
                        id: noArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            readdressDialog.close()
                            declined()
                        }
                    }
                }

                // Yes button (primary action)
                Rectangle {
                    width: 80
                    height: 36
                    radius: 8
                    color: yesArea.pressed ? Qt.darker(accentGreen, 1.1) :
                           yesArea.containsMouse ? Qt.lighter(accentGreen, 1.05) : accentGreen

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Yes")
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: bgBase
                    }

                    MouseArea {
                        id: yesArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            readdressDialog.close()
                            accepted()
                        }
                    }
                }
            }
        }
    }
}
