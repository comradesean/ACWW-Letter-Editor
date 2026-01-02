import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: saveBrowser
    color: bgSurface

    // Properties from parent (window colors)
    property color bgBase: "#0B1120"
    property color bgSurface: "#111827"
    property color bgElevated: "#1E293B"
    property color bgHover: "#2D3B4F"
    property color bgActive: "#3B4963"
    property color accentPrimary: "#38BDF8"
    property color accentGreen: "#2DD4BF"
    property color textPrimary: "#F1F5F9"
    property color textSecondary: "#94A3B8"
    property color textMuted: "#64748B"
    property color divider: "#1E293B"

    // Backend reference
    property var backend

    // Signal when a slot is selected
    signal slotSelected(int slot)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // Header
        Text {
            text: "Save Browser"
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: textPrimary
        }

        // Region info
        Text {
            text: backend.saveLoaded ? "Region: " + backend.saveRegion : "No save loaded"
            font.pixelSize: 11
            color: textMuted
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: divider
        }

        // Player selector
        Text {
            text: "Player"
            font.pixelSize: 11
            color: textSecondary
        }

        ComboBox {
            id: playerCombo
            Layout.fillWidth: true
            model: backend.saveLoaded ? backend.playerNames : []
            currentIndex: backend.currentPlayer
            enabled: backend.saveLoaded

            onCurrentIndexChanged: {
                if (backend.saveLoaded && currentIndex !== backend.currentPlayer) {
                    backend.currentPlayer = currentIndex
                }
            }

            background: Rectangle {
                implicitHeight: 32
                color: playerCombo.pressed ? bgActive : (playerCombo.hovered ? bgHover : bgElevated)
                radius: 6
                border.color: playerCombo.activeFocus ? accentPrimary : "transparent"
                border.width: 1
            }

            contentItem: Text {
                leftPadding: 10
                text: playerCombo.displayText
                font.pixelSize: 12
                color: textPrimary
                verticalAlignment: Text.AlignVCenter
            }
        }

        // Storage type tabs
        Text {
            text: "Storage"
            font.pixelSize: 11
            color: textSecondary
            Layout.topMargin: 4
        }

        Row {
            Layout.fillWidth: true
            spacing: 4

            Repeater {
                model: ["Inventory", "Mailbox", "Bank"]

                Rectangle {
                    width: (parent.width - 8) / 3
                    height: 28
                    radius: 6
                    color: backend.currentStorageType === index ? accentPrimary : (storageMouseArea.containsMouse ? bgHover : bgElevated)

                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        font.pixelSize: 10
                        font.weight: backend.currentStorageType === index ? Font.DemiBold : Font.Normal
                        color: backend.currentStorageType === index ? bgBase : textSecondary
                    }

                    MouseArea {
                        id: storageMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (backend.saveLoaded) {
                                backend.currentStorageType = index
                            }
                        }
                    }
                }
            }
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: divider
            Layout.topMargin: 4
        }

        // Slot count label
        Text {
            text: "Letters (" + (backend.saveLoaded ? backend.slotCount : 0) + " slots)"
            font.pixelSize: 11
            color: textSecondary
        }

        // Letter slot list
        ListView {
            id: slotList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 2

            model: backend.saveLoaded ? backend.getSlotSummaries() : []

            delegate: Rectangle {
                width: slotList.width
                height: 36
                radius: 6
                color: backend.currentSlot === index ? bgActive : (slotMouseArea.containsMouse ? bgHover : "transparent")

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 8

                    // Empty/occupied indicator
                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        color: modelData.isEmpty ? textMuted : accentGreen
                    }

                    // Slot info
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        Text {
                            text: modelData.isEmpty ? "Slot " + (index + 1) : (modelData.recipientName || "Letter " + (index + 1))
                            font.pixelSize: 11
                            font.weight: Font.Medium
                            color: textPrimary
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Text {
                            visible: !modelData.isEmpty && modelData.senderName
                            text: "From: " + (modelData.senderName || "")
                            font.pixelSize: 9
                            color: textMuted
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }

                    // Attachment indicator
                    Text {
                        visible: modelData.hasAttachment
                        text: "📎"
                        font.pixelSize: 10
                    }
                }

                MouseArea {
                    id: slotMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        backend.currentSlot = index
                        backend.loadCurrentSlot()
                        slotSelected(index)
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }

        // Save slot button
        Rectangle {
            Layout.fillWidth: true
            height: 32
            radius: 6
            color: saveSlotMouseArea.pressed ? Qt.darker(accentGreen, 1.2) : (saveSlotMouseArea.containsMouse ? Qt.lighter(accentGreen, 1.1) : accentGreen)
            visible: backend.saveLoaded

            Text {
                anchors.centerIn: parent
                text: "Save to Slot"
                font.pixelSize: 11
                font.weight: Font.DemiBold
                color: bgBase
            }

            MouseArea {
                id: saveSlotMouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    backend.saveCurrentSlot()
                }
            }
        }

        // Modified indicator
        Text {
            visible: backend.saveModified
            text: "* Unsaved changes"
            font.pixelSize: 10
            font.italic: true
            color: "#F59E0B"
            Layout.alignment: Qt.AlignHCenter
        }
    }

    // Refresh slot list when storage type or player changes
    Connections {
        target: backend
        function onCurrentPlayerChanged() { slotList.model = backend.getSlotSummaries() }
        function onCurrentStorageTypeChanged() { slotList.model = backend.getSlotSummaries() }
        function onSaveLoadedChanged() { slotList.model = backend.getSlotSummaries() }
    }
}
