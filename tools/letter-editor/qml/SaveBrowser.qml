import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "components"

Rectangle {
    id: saveBrowser
    color: bgSurface

    // Theme colors (from parent)
    property color bgBase: "#0B1120"
    property color bgSurface: "#111827"
    property color bgElevated: "#1E293B"
    property color bgHover: "#2D3B4F"
    property color bgActive: "#3B4963"
    property color accentPrimary: "#38BDF8"
    property color accentGreen: "#2DD4BF"
    property color warningColor: "#F59E0B"
    property color textPrimary: "#F1F5F9"
    property color textSecondary: "#94A3B8"
    property color textMuted: "#64748B"
    property color divider: "#1E293B"

    // Backend reference
    property var backend

    // Track which slot has local modifications (for dirty indicator)
    property int modifiedSlot: -1

    // Signal when a slot is selected
    signal slotSelected(int slot)

    // Storage configuration
    readonly property var storageConfig: [
        { name: "Inv", fullName: "Inventory", count: 10 },
        { name: "Mail", fullName: "Mailbox", count: 10 },
        { name: "Bank", fullName: "Bank", count: 75 }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: qsTr("Save Browser")
                font.pixelSize: 14
                font.weight: Font.DemiBold
                color: textPrimary
                Layout.fillWidth: true
            }
        }

        // Region info
        Text {
            text: backend && backend.saveLoaded ? qsTr("Region: %1").arg(backend.saveRegion) : qsTr("No save loaded")
            font.pixelSize: 11
            color: textMuted
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: divider
        }

        // Player selector - compact horizontal pills
        Text {
            text: qsTr("Player")
            font.pixelSize: 11
            color: textSecondary
        }

        Row {
            Layout.fillWidth: true
            spacing: 4

            Repeater {
                model: 4

                Rectangle {
                    id: playerPill
                    width: (parent.width - 12) / 4
                    height: 32
                    radius: 6

                    property bool isSelected: backend && backend.currentPlayer === index
                    // Re-evaluate when saveLoaded changes by including it in the binding
                    property bool playerExists: backend && backend.saveLoaded && backend.playerExists(index)

                    color: {
                        if (!playerExists) return Qt.rgba(bgElevated.r, bgElevated.g, bgElevated.b, 0.5)
                        if (isSelected) return accentPrimary
                        if (playerMouseArea.containsMouse) return bgHover
                        return bgElevated
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "P" + (index + 1)
                        font.pixelSize: 11
                        font.weight: isSelected ? Font.DemiBold : Font.Normal
                        color: {
                            if (!playerExists) return Qt.rgba(textMuted.r, textMuted.g, textMuted.b, 0.5)
                            if (isSelected) return bgBase
                            return textPrimary
                        }
                    }

                    // Tooltip with full player name or "Empty" for non-existent
                    ToolTip {
                        visible: playerMouseArea.containsMouse
                        text: playerExists ? (backend.playerNames[index] || "") : qsTr("Empty")
                        delay: 500
                    }

                    MouseArea {
                        id: playerMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: playerExists ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                        onClicked: {
                            if (playerExists && backend.saveLoaded && index !== backend.currentPlayer) {
                                saveBrowser.requestPlayerChange(index)
                            }
                        }
                    }
                }
            }
        }

        // Storage type - segmented control
        Text {
            text: qsTr("Storage")
            font.pixelSize: 11
            color: textSecondary
            Layout.topMargin: 4
        }

        Rectangle {
            Layout.fillWidth: true
            height: 36
            radius: 8
            color: bgElevated

            Row {
                anchors.fill: parent
                anchors.margins: 3

                Repeater {
                    model: storageConfig

                    Rectangle {
                        id: storageTab
                        width: (parent.width) / 3
                        height: parent.height
                        radius: 6

                        property bool isSelected: backend && backend.currentStorageType === index

                        color: isSelected ? accentPrimary : (storageMouseArea.containsMouse ? bgHover : "transparent")

                        Row {
                            anchors.centerIn: parent
                            spacing: 4

                            Text {
                                text: modelData.name
                                font.pixelSize: 11
                                font.weight: isSelected ? Font.DemiBold : Font.Normal
                                color: isSelected ? bgBase : textSecondary
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            // Slot count badge
                            Rectangle {
                                visible: !isSelected
                                width: countText.width + 8
                                height: 16
                                radius: 8
                                color: Qt.rgba(textMuted.r, textMuted.g, textMuted.b, 0.2)
                                anchors.verticalCenter: parent.verticalCenter

                                Text {
                                    id: countText
                                    anchors.centerIn: parent
                                    text: modelData.count
                                    font.pixelSize: 9
                                    color: textMuted
                                }
                            }
                        }

                        MouseArea {
                            id: storageMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (backend && backend.saveLoaded && index !== backend.currentStorageType) {
                                    saveBrowser.requestStorageChange(index)
                                }
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
            text: qsTr("Letters (%1 slots)").arg(backend && backend.saveLoaded ? backend.slotCount : 0)
            font.pixelSize: 11
            color: textSecondary
        }

        // Letter slot list - virtualized
        ListView {
            id: slotList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 2
            cacheBuffer: 200

            model: backend && backend.saveLoaded ? backend.getSlotSummaries() : []

            delegate: SlotDelegate {
                width: slotList.width
                slotData: modelData
                slotIndex: index
                isSelected: backend && backend.currentSlot === index
                isModified: saveBrowser.modifiedSlot === index

                bgHover: saveBrowser.bgHover
                bgActive: saveBrowser.bgActive
                accentPrimary: saveBrowser.accentPrimary
                accentGreen: saveBrowser.accentGreen
                warningColor: saveBrowser.warningColor
                textPrimary: saveBrowser.textPrimary
                textSecondary: saveBrowser.textSecondary
                textMuted: saveBrowser.textMuted

                onClicked: {
                    saveBrowser.requestSlotChange(index)
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: slotList.count > 10 ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
                minimumSize: 0.1
            }
        }

        // Save to slot button
        Rectangle {
            Layout.fillWidth: true
            height: 36
            radius: 6
            color: saveSlotMouseArea.pressed ? Qt.darker(accentGreen, 1.2) :
                   (saveSlotMouseArea.containsMouse ? Qt.lighter(accentGreen, 1.1) : accentGreen)
            visible: backend && backend.saveLoaded

            Text {
                anchors.centerIn: parent
                text: qsTr("Save to Slot")
                font.pixelSize: 12
                font.weight: Font.DemiBold
                color: bgBase
            }

            MouseArea {
                id: saveSlotMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    backend.saveCurrentSlot()
                    saveBrowser.modifiedSlot = -1
                    // Refresh slot list to show updated icon
                    slotList.model = backend.getSlotSummaries()
                    slotSaved()
                }
            }
        }

        // Modified indicator
        Text {
            visible: backend && backend.saveModified
            text: qsTr("* Unsaved changes to save file")
            font.pixelSize: 10
            font.italic: true
            color: warningColor
            Layout.alignment: Qt.AlignHCenter
        }
    }

    // Signals for dirty state management (handled by parent)
    signal requestSlotChange(int newSlot)
    signal requestStorageChange(int newStorage)
    signal requestPlayerChange(int newPlayer)
    signal slotSaved()

    // Refresh slot list when storage type or player changes
    Connections {
        target: backend
        function onCurrentPlayerChanged() { slotList.model = backend.getSlotSummaries() }
        function onCurrentStorageTypeChanged() { slotList.model = backend.getSlotSummaries() }
        function onSaveLoadedChanged() { slotList.model = backend.getSlotSummaries() }
    }

    // Function to perform the actual slot load (called after dirty check passes)
    function loadSlot(slot) {
        backend.currentSlot = slot
        backend.loadCurrentSlot()
        modifiedSlot = -1
        slotSelected(slot)
    }

    // Function to perform storage change (called after dirty check passes)
    function changeStorage(storageType) {
        backend.currentStorageType = storageType
        backend.currentSlot = 0
        backend.loadCurrentSlot()
        modifiedSlot = -1
        slotSelected(0)  // Update canvas and capture clean state
    }

    // Function to perform player change (called after dirty check passes)
    function changePlayer(player) {
        backend.currentPlayer = player
        backend.currentSlot = 0
        backend.loadCurrentSlot()
        modifiedSlot = -1
        slotSelected(0)  // Update canvas and capture clean state
    }
}
