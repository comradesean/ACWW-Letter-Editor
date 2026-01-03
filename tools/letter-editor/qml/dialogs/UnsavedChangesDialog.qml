import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: unsavedChangesDialog
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
    property color warningColor: "#F59E0B"
    property color errorColor: "#EF4444"
    property color textPrimary: "#F1F5F9"
    property color textSecondary: "#94A3B8"
    property color textMuted: "#64748B"
    property color divider: "#1E293B"

    // Callback references
    property var onDiscard: null
    property var onSave: null

    signal discarded()
    signal saved()
    signal cancelled()

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

                // Warning icon
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 48
                    height: 48
                    radius: 24
                    color: Qt.rgba(warningColor.r, warningColor.g, warningColor.b, 0.15)

                    Text {
                        anchors.centerIn: parent
                        text: "!"
                        font.pixelSize: 24
                        font.weight: Font.Bold
                        color: warningColor
                    }
                }

                // Title
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Unsaved Changes")
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    color: textPrimary
                }

                // Description
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    text: qsTr("You have unsaved changes to this letter. What would you like to do?")
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
                spacing: 8

                // Discard button (destructive)
                Rectangle {
                    width: 80
                    height: 36
                    radius: 8
                    color: discardArea.containsMouse ? Qt.rgba(errorColor.r, errorColor.g, errorColor.b, 0.15) : "transparent"
                    border.color: errorColor
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Discard")
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: errorColor
                    }

                    MouseArea {
                        id: discardArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            unsavedChangesDialog.close()
                            discarded()
                        }
                    }
                }

                // Save button (primary action)
                Rectangle {
                    width: 80
                    height: 36
                    radius: 8
                    color: saveArea.pressed ? Qt.darker(accentGreen, 1.1) :
                           saveArea.containsMouse ? Qt.lighter(accentGreen, 1.05) : accentGreen

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Save")
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: bgBase
                    }

                    MouseArea {
                        id: saveArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            unsavedChangesDialog.close()
                            saved()
                        }
                    }
                }

                // Cancel button
                Rectangle {
                    width: 80
                    height: 36
                    radius: 8
                    color: cancelArea.containsMouse ? bgHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Cancel")
                        font.pixelSize: 12
                        color: textSecondary
                    }

                    MouseArea {
                        id: cancelArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            unsavedChangesDialog.close()
                            cancelled()
                        }
                    }
                }
            }
        }
    }
}
