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
                        // Bank (index 2) is disabled when not initialized
                        property bool isDisabled: index === 2 && backend && backend.saveLoaded && !backend.bankInitialized

                        color: {
                            if (isDisabled) return Qt.rgba(bgElevated.r, bgElevated.g, bgElevated.b, 0.5)
                            if (isSelected) return accentPrimary
                            if (storageMouseArea.containsMouse) return bgHover
                            return "transparent"
                        }

                        Row {
                            anchors.centerIn: parent
                            spacing: 4

                            Text {
                                text: modelData.name
                                font.pixelSize: 11
                                font.weight: isSelected ? Font.DemiBold : Font.Normal
                                color: {
                                    if (storageTab.isDisabled) return Qt.rgba(textMuted.r, textMuted.g, textMuted.b, 0.5)
                                    if (isSelected) return bgBase
                                    return textSecondary
                                }
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            // Slot count badge
                            Rectangle {
                                visible: !isSelected && !storageTab.isDisabled
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

                        ToolTip {
                            visible: storageTab.isDisabled && storageMouseArea.containsMouse
                            text: qsTr("Save a letter to the bank in-game to initialize it")
                            delay: 300
                        }

                        MouseArea {
                            id: storageMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: storageTab.isDisabled ? Qt.ForbiddenCursor : Qt.PointingHandCursor
                            onClicked: {
                                if (storageTab.isDisabled) return
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
                    if (modelData && modelData.isEmpty) {
                        saveBrowser.requestEmptySlotChange(index, false,
                            backend.currentPlayer, backend.currentStorageType, backend.currentSlot)
                    } else {
                        saveBrowser.requestSlotChange(index)
                    }
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
    signal requestEmptySlotChange(int newSlot, bool isStorageEmpty, int prevPlayer, int prevStorage, int prevSlot)
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

    // Function to create a new letter in an empty slot (bypasses loading invalid data)
    function createLetterInSlot(slot) {
        backend.currentSlot = slot
        backend.clearLetter()
        modifiedSlot = -1
        slotSelected(slot)
    }

    // Find first non-empty slot in current storage, returns -1 if all empty
    function findFirstNonEmptySlot() {
        var summaries = backend.getSlotSummaries()
        for (var i = 0; i < summaries.length; i++) {
            if (!summaries[i].isEmpty) {
                return i
            }
        }
        return -1
    }

    // Find first existing player, returns -1 if none exist
    function findFirstExistingPlayer() {
        for (var i = 0; i < 4; i++) {
            if (backend.playerExists(i)) {
                return i
            }
        }
        return -1
    }

    // Check if any players exist in the save
    function hasAnyPlayers() {
        return findFirstExistingPlayer() >= 0
    }

    // Find first letter across all players and storages
    // Returns {player: int, storage: int, slot: int} or null if none found
    function findFirstLetterInSave() {
        var storageTypes = [0, 1, 2]  // Inventory, Mailbox, Bank

        for (var p = 0; p < 4; p++) {
            if (!backend.playerExists(p)) continue

            for (var s = 0; s < storageTypes.length; s++) {
                var storageType = storageTypes[s]
                // Skip uninitialized bank
                if (storageType === 2 && !backend.bankInitialized) continue

                // Temporarily switch to check this storage
                var oldPlayer = backend.currentPlayer
                var oldStorage = backend.currentStorageType
                backend.currentPlayer = p
                backend.currentStorageType = storageType

                var summaries = backend.getSlotSummaries()
                for (var slot = 0; slot < summaries.length; slot++) {
                    if (!summaries[slot].isEmpty) {
                        // Restore and return result
                        backend.currentPlayer = oldPlayer
                        backend.currentStorageType = oldStorage
                        return {player: p, storage: storageType, slot: slot}
                    }
                }

                backend.currentPlayer = oldPlayer
                backend.currentStorageType = oldStorage
            }
        }
        return null
    }

    // Get first available player and storage for creating a new letter
    // Returns {player: int, storage: int, storageName: string}
    function getFirstAvailableLocation() {
        var storageNames = ["Inventory", "Mailbox", "Bank"]
        var firstPlayer = findFirstExistingPlayer()
        if (firstPlayer < 0) return null

        // Check each storage, skip uninitialized bank
        for (var s = 0; s < 3; s++) {
            if (s === 2 && !backend.bankInitialized) continue
            return {player: firstPlayer, storage: s, storageName: storageNames[s]}
        }
        return {player: firstPlayer, storage: 0, storageName: "Inventory"}
    }

    // Initialize save browser to a specific player/storage/slot and load it
    function initializeToLocation(player, storage, slot) {
        backend.currentPlayer = player
        backend.currentStorageType = storage
        backend.currentSlot = slot
        backend.loadCurrentSlot()
        modifiedSlot = -1
        slotList.model = backend.getSlotSummaries()
        slotSelected(slot)
    }

    // Initialize to a location and create a new letter there
    function initializeAndCreateLetter(player, storage) {
        backend.currentPlayer = player
        backend.currentStorageType = storage
        backend.currentSlot = 0
        backend.clearLetter()
        modifiedSlot = -1
        slotList.model = backend.getSlotSummaries()
        slotSelected(0)
    }

    // Function to perform storage change (called after dirty check passes)
    function changeStorage(storageType) {
        // Save previous location for potential revert
        var prevPlayer = backend.currentPlayer
        var prevStorage = backend.currentStorageType
        var prevSlot = backend.currentSlot

        backend.currentStorageType = storageType
        slotList.model = backend.getSlotSummaries()

        // Find first non-empty slot
        var firstSlot = findFirstNonEmptySlot()
        if (firstSlot >= 0) {
            backend.currentSlot = firstSlot
            backend.loadCurrentSlot()
            modifiedSlot = -1
            slotSelected(firstSlot)
        } else {
            // All slots empty - ask user if they want to create a letter
            requestEmptySlotChange(0, true, prevPlayer, prevStorage, prevSlot)
        }
    }

    // Function to perform player change (called after dirty check passes)
    function changePlayer(player) {
        // Save previous location for potential revert
        var prevPlayer = backend.currentPlayer
        var prevStorage = backend.currentStorageType
        var prevSlot = backend.currentSlot

        backend.currentPlayer = player

        // Search all storages for first letter for this player
        var storageTypes = [0, 1, 2]  // Inventory, Mailbox, Bank
        for (var s = 0; s < storageTypes.length; s++) {
            var storageType = storageTypes[s]
            // Skip uninitialized bank
            if (storageType === 2 && !backend.bankInitialized) continue

            backend.currentStorageType = storageType
            slotList.model = backend.getSlotSummaries()

            var firstSlot = findFirstNonEmptySlot()
            if (firstSlot >= 0) {
                backend.currentSlot = firstSlot
                backend.loadCurrentSlot()
                modifiedSlot = -1
                slotSelected(firstSlot)
                return
            }
        }

        // No letters found in any storage - ask user if they want to create a letter
        // Reset to Inventory for creation
        backend.currentStorageType = 0
        slotList.model = backend.getSlotSummaries()
        requestEmptySlotChange(0, true, prevPlayer, prevStorage, prevSlot)
    }

    // Revert to a previous location (used when user cancels empty slot dialog)
    function revertToLocation(player, storage, slot) {
        backend.currentPlayer = player
        backend.currentStorageType = storage
        backend.currentSlot = slot
        backend.loadCurrentSlot()
        modifiedSlot = -1
        slotList.model = backend.getSlotSummaries()
        slotSelected(slot)
    }
}
