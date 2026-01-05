import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: statusBar

    // Backend reference
    property var backend

    // Dirty state tracking
    property bool letterModified: false

    // Theme colors
    property color bgSurface: "#111827"
    property color accentGreen: "#2DD4BF"
    property color warningColor: "#F59E0B"
    property color textPrimary: "#F1F5F9"
    property color textMuted: "#64748B"
    property color divider: "#1E293B"

    // Storage type names
    readonly property var storageNames: ["Inventory", "Mailbox", "Bank"]

    height: 36
    color: bgSurface

    // Top border
    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        color: divider
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 16

        // Left: Letter modified indicator
        Row {
            visible: backend && backend.saveLoaded
            spacing: 6
            Layout.alignment: Qt.AlignVCenter

            Rectangle {
                width: 6
                height: 6
                radius: 3
                color: letterModified ? warningColor : accentGreen
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: letterModified ? qsTr("Modified") : qsTr("Saved")
                font.pixelSize: 11
                color: letterModified ? warningColor : textMuted
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // Separator when save is loaded
        Rectangle {
            visible: backend && backend.saveLoaded
            width: 1
            height: 16
            color: divider
            Layout.alignment: Qt.AlignVCenter
        }

        // Center: Context text
        Text {
            text: {
                if (!backend || !backend.loaded) {
                    return qsTr("File > Open ROM to begin")
                }
                if (!backend.saveLoaded) {
                    return qsTr("Type to edit  |  Ctrl+Shift+O to open save")
                }
                var storageName = storageNames[backend.currentStorageType] || "Storage"
                return qsTr("Slot %1 of %2  |  %3")
                    .arg(backend.currentSlot + 1)
                    .arg(backend.slotCount)
                    .arg(storageName)
            }
            font.pixelSize: 11
            color: textMuted
            Layout.fillWidth: true
            elide: Text.ElideRight
        }

        // Separator
        Rectangle {
            visible: backend && backend.saveLoaded && backend.saveModified
            width: 1
            height: 16
            color: divider
            Layout.alignment: Qt.AlignVCenter
        }

        // Save file status (when save is modified)
        Row {
            visible: backend && backend.saveLoaded && backend.saveModified
            spacing: 6
            Layout.alignment: Qt.AlignVCenter

            Rectangle {
                width: 6
                height: 6
                radius: 3
                color: warningColor
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: qsTr("Save Modified")
                font.pixelSize: 11
                color: warningColor
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // Right: Paper counter
        Text {
            visible: backend && backend.loaded
            text: qsTr("%1 / 64").arg(backend ? backend.currentPaper + 1 : 1)
            font.pixelSize: 11
            color: textMuted
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
