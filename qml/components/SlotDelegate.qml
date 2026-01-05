import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: slotDelegate

    // Required properties from model
    property var slotData: modelData
    property int slotIndex: index
    property bool isSelected: false
    property bool isModified: false

    // Theme colors (passed from parent)
    property color bgHover: "#2D3B4F"
    property color bgActive: "#3B4963"
    property color accentPrimary: "#38BDF8"
    property color accentGreen: "#2DD4BF"
    property color warningColor: "#F59E0B"
    property color textPrimary: "#F1F5F9"
    property color textSecondary: "#94A3B8"
    property color textMuted: "#64748B"

    // Signals
    signal clicked()

    height: 44
    radius: 6
    color: {
        if (isSelected) return bgActive
        if (mouseArea.containsMouse) return bgHover
        if (isModified) return Qt.rgba(warningColor.r, warningColor.g, warningColor.b, 0.08)
        return "transparent"
    }

    Behavior on color {
        ColorAnimation {
            duration: 100
            easing.type: Easing.OutCubic
        }
    }

    // Selection accent bar (left edge)
    Rectangle {
        width: isSelected ? 3 : 0
        height: parent.height - 8
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        radius: 1.5
        color: accentPrimary
        opacity: isSelected ? 1.0 : 0.0

        Behavior on width {
            NumberAnimation {
                duration: 100
                easing.type: Easing.OutCubic
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 100
                easing.type: Easing.OutCubic
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 10
        spacing: 10

        // Letter icon or empty slot indicator
        Item {
            width: 16
            height: 16
            Layout.alignment: Qt.AlignVCenter

            // Letter icon for occupied slots
            Image {
                id: letterIcon
                visible: slotData && !slotData.isEmpty && slotData.iconFlags > 0
                anchors.centerIn: parent
                width: 16
                height: 16
                source: (slotData && !slotData.isEmpty && slotData.iconFlags > 0)
                    ? "image://lettericons/" + slotData.iconFlags + "/" + (slotData.hasAttachment ? "1" : "0")
                    : ""
                sourceSize: Qt.size(16, 16)
                smooth: false
                cache: true
            }

            // Fallback dot for occupied slots without valid icon
            Rectangle {
                visible: slotData && !slotData.isEmpty && (!letterIcon.visible || letterIcon.status !== Image.Ready)
                width: 8
                height: 8
                radius: 4
                anchors.centerIn: parent
                color: (slotData && slotData.hasAttachment) ? accentPrimary : accentGreen
            }

            // Ring outline for empty slots
            Rectangle {
                visible: !slotData || slotData.isEmpty
                width: 8
                height: 8
                radius: 4
                anchors.centerIn: parent
                color: "transparent"
                border.width: 1.5
                border.color: textMuted
            }
        }

        // Text content
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1

            // Primary text: recipient name or "Empty Slot N"
            Text {
                text: {
                    if (!slotData || slotData.isEmpty) {
                        return qsTr("Empty Slot %1").arg(slotIndex + 1)
                    }
                    return slotData.recipientName || qsTr("Letter %1").arg(slotIndex + 1)
                }
                font.pixelSize: 12
                font.weight: isSelected ? Font.Medium : Font.Normal
                color: (!slotData || slotData.isEmpty) ? textMuted : textPrimary
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            // Secondary text: sender name
            Text {
                visible: slotData && !slotData.isEmpty && slotData.senderName
                text: slotData ? qsTr("From: %1").arg(slotData.senderName || "") : ""
                font.pixelSize: 10
                color: textMuted
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }

        // Modified indicator dot (when this specific slot has unsaved changes)
        Rectangle {
            visible: isModified
            width: 6
            height: 6
            radius: 3
            color: warningColor
            Layout.alignment: Qt.AlignVCenter
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: slotDelegate.clicked()
    }
}
