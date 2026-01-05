import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.15
import QtQuick.Window 2.15
import LetterPreviewer 1.0
import "components"
import "dialogs"

ApplicationWindow {
    id: window
    visible: true
    width: 900
    height: 720
    minimumWidth: 580
    minimumHeight: 520
    title: "Letter Previewer"
    flags: Qt.FramelessWindowHint | Qt.Window

    // Modern Color System - Ocean palette
    readonly property color bgBase: "#0B1120"
    readonly property color bgSurface: "#111827"
    readonly property color bgElevated: "#1E293B"
    readonly property color bgHover: "#2D3B4F"
    readonly property color bgActive: "#3B4963"
    readonly property color accentPrimary: "#38BDF8"
    readonly property color accentGreen: "#2DD4BF"
    readonly property color warningColor: "#F59E0B"
    readonly property color errorColor: "#EF4444"
    readonly property color textPrimary: "#F1F5F9"
    readonly property color textSecondary: "#94A3B8"
    readonly property color textMuted: "#64748B"
    readonly property color divider: "#1E293B"

    // Dirty state tracking for letter modifications
    // Note: Only tracks Letter Structure Data fields (stored in .ltr/save file)
    // GUI/Display fields (recipientNameStart/End, letterHeader/Body/Footer parsing) are not tracked
    QtObject {
        id: dirtyState
        property bool letterModified: false

        // ========================================
        // Letter Structure Data (stored in .ltr/save file)
        // ========================================
        // Stationery
        property int cleanPaper: 0

        // Letter content (canvas text fields map to letterText in backend)
        property string cleanHeader: ""
        property string cleanBody: ""
        property string cleanFooter: ""

        // Recipient info
        property string cleanRecipientName: ""
        property string cleanRecipientTown: ""
        property int cleanRecipientTownId: 0
        property int cleanRecipientPlayerId: 0

        // Sender info
        property string cleanSenderName: ""
        property string cleanSenderTown: ""
        property int cleanSenderTownId: 0
        property int cleanSenderPlayerId: 0

        // Attached item
        property int cleanAttachedItem: 0xFFF1

        // Letter metadata
        property int cleanRecipientGender: 0
        property int cleanRecipientRelation: 0
        property int cleanSenderGender: 0
        property int cleanSenderRelation: 0
        property int cleanNamePosition: 0
        property int cleanLetterIconFlags: 0
        property int cleanLetterSource: 0

        function capture() {
            // Stationery
            cleanPaper = backend.currentPaper
            // Letter content
            cleanHeader = canvas.header
            cleanBody = canvas.body
            cleanFooter = canvas.footer
            // Recipient info
            cleanRecipientName = backend.recipientName
            cleanRecipientTown = backend.recipientTown
            cleanRecipientTownId = backend.recipientTownId
            cleanRecipientPlayerId = backend.recipientPlayerId
            // Sender info
            cleanSenderName = backend.senderName
            cleanSenderTown = backend.senderTown
            cleanSenderTownId = backend.senderTownId
            cleanSenderPlayerId = backend.senderPlayerId
            // Attached item
            cleanAttachedItem = backend.attachedItem
            // Letter metadata
            cleanRecipientGender = backend.recipientGender
            cleanRecipientRelation = backend.recipientRelation
            cleanSenderGender = backend.senderGender
            cleanSenderRelation = backend.senderRelation
            cleanNamePosition = backend.namePosition
            cleanLetterIconFlags = backend.letterIconFlags
            cleanLetterSource = backend.letterSource

            letterModified = false
        }

        function check() {
            letterModified =
                // Stationery
                (backend.currentPaper !== cleanPaper) ||
                // Letter content
                (canvas.header !== cleanHeader) ||
                (canvas.body !== cleanBody) ||
                (canvas.footer !== cleanFooter) ||
                // Recipient info
                (backend.recipientName !== cleanRecipientName) ||
                (backend.recipientTown !== cleanRecipientTown) ||
                (backend.recipientTownId !== cleanRecipientTownId) ||
                (backend.recipientPlayerId !== cleanRecipientPlayerId) ||
                // Sender info
                (backend.senderName !== cleanSenderName) ||
                (backend.senderTown !== cleanSenderTown) ||
                (backend.senderTownId !== cleanSenderTownId) ||
                (backend.senderPlayerId !== cleanSenderPlayerId) ||
                // Attached item
                (backend.attachedItem !== cleanAttachedItem) ||
                // Letter metadata
                (backend.recipientGender !== cleanRecipientGender) ||
                (backend.recipientRelation !== cleanRecipientRelation) ||
                (backend.senderGender !== cleanSenderGender) ||
                (backend.senderRelation !== cleanSenderRelation) ||
                (backend.namePosition !== cleanNamePosition) ||
                (backend.letterIconFlags !== cleanLetterIconFlags) ||
                (backend.letterSource !== cleanLetterSource)
        }

        function confirmDiscard(callback) {
            if (letterModified) {
                pendingAction = callback
                unsavedChangesDialog.open()
            } else {
                callback()
            }
        }
    }

    property var pendingAction: null

    // Close save file with dirty check
    function closeSaveFile() {
        if (backend.saveModified || dirtyState.letterModified) {
            dirtyState.check()
            pendingAction = function() {
                backend.closeSave()
                dirtyState.capture()
                canvas.forceActiveFocus()
            }
            unsavedChangesDialog.open()
        } else {
            backend.closeSave()
            dirtyState.capture()
            canvas.forceActiveFocus()
        }
    }

    Material.theme: Material.Dark
    Material.accent: accentPrimary
    Material.background: bgBase

    color: "transparent"

    // Manual maximize state (more reliable for frameless windows)
    property bool isMaximized: false
    property rect normalGeometry: Qt.rect(100, 100, 900, 720)

    function toggleMaximize() {
        if (isMaximized) {
            // Restore
            window.x = normalGeometry.x
            window.y = normalGeometry.y
            window.width = normalGeometry.width
            window.height = normalGeometry.height
            isMaximized = false
        } else {
            // Save current geometry and maximize to current screen only
            normalGeometry = Qt.rect(window.x, window.y, window.width, window.height)
            var screen = window.screen
            window.x = screen.virtualX
            window.y = screen.virtualY
            window.width = screen.width
            window.height = screen.height
            isMaximized = true
        }
    }

    Backend {
        id: backend
    }

    // Handle save file loading/closing - update canvas
    Connections {
        target: backend
        function onSaveLoadedChanged() {
            if (backend.saveLoaded) {
                // Load slot 1 content into canvas
                canvas.setLetterContent(backend.letterHeader, backend.letterBody, backend.letterFooter)
                paperCombo.currentIndex = backend.currentPaper
                dirtyState.capture()
            } else {
                // Clear canvas when save is closed
                canvas.clearText()
                paperCombo.currentIndex = 0
                dirtyState.capture()
            }
        }
        function onUnknownByteWarning(message) {
            warningToast.text = message
            warningToast.open()
        }
    }

    // Warning toast notification
    Popup {
        id: warningToast
        property string text: ""
        x: (parent.width - width) / 2
        y: parent.height - height - 40
        width: Math.min(toastText.implicitWidth + 32, parent.width - 40)
        height: 36
        modal: false
        closePolicy: Popup.CloseOnPressOutside

        background: Rectangle {
            color: warningColor
            radius: 6
        }

        contentItem: Text {
            id: toastText
            text: warningToast.text
            color: "#FFFFFF"
            font.pixelSize: 11
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        // Auto-close after 5 seconds
        Timer {
            running: warningToast.visible
            interval: 5000
            onTriggered: warningToast.close()
        }
    }

    // Main container with rounded corners
    Rectangle {
        id: mainContainer
        anchors.fill: parent
        anchors.margins: isMaximized ? 0 : 1
        radius: isMaximized ? 0 : 12
        color: bgBase
        clip: true

        Behavior on anchors.margins { NumberAnimation { duration: 100 } }
        Behavior on radius { NumberAnimation { duration: 100 } }

        // Subtle border (hidden when maximized)
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "transparent"
            border.color: Qt.rgba(255, 255, 255, 0.06)
            border.width: isMaximized ? 0 : 1
            visible: !isMaximized
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // Custom Title Bar
            Rectangle {
                id: titleBar
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                color: bgSurface
                radius: isMaximized ? 0 : 12

                // Square off bottom corners
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 12
                    color: bgSurface
                    visible: !isMaximized
                }

                // Drag area (behind the controls)
                MouseArea {
                    id: titleBarDrag
                    anchors.fill: parent
                    z: 0
                    property point clickPos

                    onPressed: {
                        clickPos = Qt.point(mouse.x, mouse.y)
                    }

                    onPositionChanged: {
                        if (pressed && !isMaximized) {
                            var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                            window.x += delta.x
                            window.y += delta.y
                        }
                    }

                    onDoubleClicked: toggleMaximize()
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 8
                    spacing: 10
                    z: 1

                    // App icon
                    Rectangle {
                        width: 22
                        height: 22
                        radius: 6
                        color: accentPrimary

                        Text {
                            anchors.centerIn: parent
                            text: "✉"
                            font.pixelSize: 11
                            color: "#FFFFFF"
                        }
                    }

                    // Title
                    Text {
                        text: "Letter Previewer"
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: textPrimary
                    }

                    Item { Layout.fillWidth: true }

                    // Window controls
                    Row {
                        id: windowControls
                        spacing: 2
                        z: 10

                        // Minimize
                        Rectangle {
                            width: 36
                            height: 28
                            radius: 6
                            color: minimizeArea.containsMouse ? bgHover : "transparent"

                            Behavior on color { ColorAnimation { duration: 100 } }

                            Text {
                                anchors.centerIn: parent
                                text: "─"
                                font.pixelSize: 10
                                font.weight: Font.Bold
                                color: textSecondary
                            }

                            MouseArea {
                                id: minimizeArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: window.showMinimized()
                            }
                        }

                        // Maximize
                        Rectangle {
                            width: 36
                            height: 28
                            radius: 6
                            color: maximizeArea.containsMouse ? bgHover : "transparent"

                            Behavior on color { ColorAnimation { duration: 100 } }

                            Text {
                                anchors.centerIn: parent
                                text: isMaximized ? "❐" : "□"
                                font.pixelSize: 11
                                color: textSecondary
                            }

                            MouseArea {
                                id: maximizeArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    toggleMaximize()
                                }
                            }
                        }

                        // Close
                        Rectangle {
                            width: 36
                            height: 28
                            radius: 6
                            color: closeArea.containsMouse ? "#DC2626" : "transparent"

                            Behavior on color { ColorAnimation { duration: 100 } }

                            Text {
                                anchors.centerIn: parent
                                text: "✕"
                                font.pixelSize: 10
                                font.weight: Font.Bold
                                color: closeArea.containsMouse ? "#FFFFFF" : textSecondary
                            }

                            MouseArea {
                                id: closeArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: Qt.quit()
                            }
                        }
                    }
                }

                // Divider
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: divider
                }
            }

            // Menu Bar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                color: bgSurface

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    spacing: 2

                    Repeater {
                        model: [
                            { title: "File", items: [
                                { text: "Open ROM...", shortcut: "Ctrl+O", actionId: "openRom", disabledWhen: "loaded" },
                                { text: "Open Save File...", shortcut: "Ctrl+Shift+O", actionId: "openSave", enabledWhen: "loaded" },
                                { text: "Close Save File", shortcut: "Ctrl+W", actionId: "closeSave", enabledWhen: "saveLoaded" },
                                { separator: true },
                                { text: "Save", shortcut: "Ctrl+S", actionId: "saveSave", enabledWhen: "saveLoaded" },
                                { text: "Save As...", shortcut: "Ctrl+Shift+S", actionId: "saveAs", enabledWhen: "saveLoaded" },
                                { separator: true },
                                { text: "Import Letter...", shortcut: "Ctrl+I", actionId: "importLtr", enabledWhen: "loaded" },
                                { text: "Export Letter...", shortcut: "Ctrl+E", actionId: "exportLtr", enabledWhen: "loaded" },
                                { text: "Export as PNG...", shortcut: "Ctrl+P", actionId: "exportPng", enabledWhen: "loaded" },
                                { separator: true },
                                { text: "Exit", shortcut: "Alt+F4", actionId: "exit" }
                            ]},
                            { title: "Edit", items: [
                                { text: "Edit Letter Info...", actionId: "editLetterInfo", enabledWhen: "loaded" },
                                { text: "Edit Attached Item...", actionId: "editAttachedItem", enabledWhen: "loaded" },
                                { text: "Import Addressee from Save", actionId: "importAddressee", enabledWhen: "saveLoaded" },
                                { separator: true },
                                { text: "View Hex...", actionId: "viewHex", enabledWhen: "loaded" },
                                { separator: true },
                                { text: "Clear Letter", shortcut: "Ctrl+N", actionId: "clearLetter" }
                            ]},
                            { title: "View", items: [
                                { text: "Next Paper", shortcut: "Ctrl+]", actionId: "nextPaper" },
                                { text: "Previous Paper", shortcut: "Ctrl+[", actionId: "prevPaper" }
                            ]}
                        ]

                        Rectangle {
                            width: menuText.width + 16
                            height: 28
                            radius: 6
                            color: menuArea.containsMouse || menuPopup.visible ? bgHover : "transparent"

                            Behavior on color { ColorAnimation { duration: 80 } }

                            Text {
                                id: menuText
                                anchors.centerIn: parent
                                text: modelData.title
                                font.pixelSize: 12
                                color: menuArea.containsMouse || menuPopup.visible ? textPrimary : textSecondary
                            }

                            MouseArea {
                                id: menuArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: menuPopup.visible ? menuPopup.close() : menuPopup.open()
                            }

                            Popup {
                                id: menuPopup
                                y: parent.height + 4
                                x: 0
                                padding: 6
                                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
                                modal: false
                                focus: true

                                background: Rectangle {
                                    color: bgElevated
                                    radius: 10
                                    border.color: divider
                                    border.width: 1
                                }

                                contentItem: Column {
                                    spacing: 2

                                    Repeater {
                                        model: modelData.items

                                        Rectangle {
                                            property bool isSeparator: modelData.separator === true
                                            property bool isDisabled: (modelData.disabledWhen === "loaded" && backend.loaded) ||
                                                                      (modelData.enabledWhen === "loaded" && !backend.loaded) ||
                                                                      (modelData.enabledWhen === "saveLoaded" && !backend.saveLoaded)

                                            width: 200
                                            height: isSeparator ? 9 : 32
                                            radius: isSeparator ? 0 : 6
                                            color: (!isSeparator && !isDisabled && itemMouseArea.containsMouse) ? bgHover : "transparent"

                                            Behavior on color { ColorAnimation { duration: 80 } }

                                            // Separator line
                                            Rectangle {
                                                visible: isSeparator
                                                anchors.centerIn: parent
                                                width: parent.width - 12
                                                height: 1
                                                color: divider
                                            }

                                            // Menu item content
                                            RowLayout {
                                                visible: !isSeparator
                                                anchors.fill: parent
                                                anchors.leftMargin: 10
                                                anchors.rightMargin: 10
                                                spacing: 8

                                                Text {
                                                    text: modelData.text || ""
                                                    font.pixelSize: 12
                                                    color: isDisabled ? textMuted : (itemMouseArea.containsMouse ? textPrimary : textSecondary)
                                                    Layout.fillWidth: true
                                                }

                                                Text {
                                                    text: modelData.shortcut || ""
                                                    font.pixelSize: 11
                                                    color: textMuted
                                                    visible: text !== ""
                                                    opacity: isDisabled ? 0.5 : 1
                                                }
                                            }

                                            MouseArea {
                                                id: itemMouseArea
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                visible: !isSeparator
                                                enabled: !isDisabled

                                                onClicked: {
                                                    var actionId = modelData.actionId
                                                    if (actionId === "openRom") {
                                                        fileDialog.open()
                                                    } else if (actionId === "openSave") {
                                                        openSaveDialog.open()
                                                    } else if (actionId === "closeSave") {
                                                        closeSaveFile()
                                                    } else if (actionId === "saveSave") {
                                                        backend.saveSave()
                                                    } else if (actionId === "saveAs") {
                                                        saveSaveAsDialog.open()
                                                    } else if (actionId === "importLtr") {
                                                        importDialog.open()
                                                    } else if (actionId === "exportLtr") {
                                                        exportDialog.open()
                                                    } else if (actionId === "exportPng") {
                                                        pngDialog.open()
                                                    } else if (actionId === "exit") {
                                                        Qt.quit()
                                                    } else if (actionId === "clearLetter") {
                                                        canvas.clearText()
                                                        paperCombo.currentIndex = 0
                                                        // Clear letter info
                                                        backend.recipientName = ""
                                                        backend.recipientTown = ""
                                                        backend.recipientPlayerId = 0
                                                        backend.recipientTownId = 0
                                                        backend.recipientNameStart = -1
                                                        backend.recipientNameEnd = -1
                                                        backend.recipientGender = 0
                                                        backend.recipientRelation = 0
                                                        backend.senderName = ""
                                                        backend.senderTown = ""
                                                        backend.senderPlayerId = 0
                                                        backend.senderTownId = 0
                                                        backend.senderGender = 0
                                                        backend.senderRelation = 0
                                                        backend.letterSource = 0
                                                        backend.letterIconFlags = 0
                                                        backend.namePosition = 0  // TODO: verify 0 is the correct default
                                                        backend.attachedItem = 0xFFF1
                                                    } else if (actionId === "editLetterInfo") {
                                                        letterInfoDialog.selectedTab = 0
                                                        recipientTownIdField.text = backend.recipientTownId.toString()
                                                        recipientTownField.text = backend.recipientTown
                                                        recipientPlayerIdField.text = backend.recipientPlayerId.toString()
                                                        recipientNameField.text = backend.recipientName
                                                        senderTownIdField.text = backend.senderTownId.toString()
                                                        senderTownField.text = backend.senderTown
                                                        senderPlayerIdField.text = backend.senderPlayerId.toString()
                                                        senderNameField.text = backend.senderName
                                                        letterInfoDialog.open()
                                                    } else if (actionId === "editAttachedItem") {
                                                        attachedItemDialogField.text = backend.attachedItem.toString(16).toUpperCase().padStart(4, '0')
                                                        attachedItemDialog.open()
                                                    } else if (actionId === "viewHex") {
                                                        // Sync canvas text to backend before getting hex
                                                        backend.letterText = canvas.text
                                                        hexViewerDialog.hexContent = backend.getLetterHex()
                                                        hexViewerDialog.open()
                                                    } else if (actionId === "importAddressee") {
                                                        backend.importAddresseeFromSave()
                                                        // Update canvas with new header
                                                        canvas.setLetterContent(backend.letterHeader, backend.letterBody, backend.letterFooter)
                                                    } else if (actionId === "nextPaper") {
                                                        if (backend.loaded && paperCombo.currentIndex < 63)
                                                            paperCombo.currentIndex++
                                                    } else if (actionId === "prevPaper") {
                                                        if (backend.loaded && paperCombo.currentIndex > 0)
                                                            paperCombo.currentIndex--
                                                    }
                                                    menuPopup.close()
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: divider
                }
            }

            // Main content area (sidebar + canvas)
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                // Save browser sidebar
                SaveBrowser {
                    id: saveBrowser
                    Layout.preferredWidth: backend.saveLoaded ? 220 : 0
                    Layout.fillHeight: true
                    visible: Layout.preferredWidth > 0
                    opacity: backend.saveLoaded ? 1.0 : 0.0
                    clip: true
                    backend: backend

                    // Smooth slide-in animation
                    Behavior on Layout.preferredWidth {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 150
                            easing.type: Easing.OutCubic
                        }
                    }

                    // Pass colors from window
                    bgBase: window.bgBase
                    bgSurface: window.bgSurface
                    bgElevated: window.bgElevated
                    bgHover: window.bgHover
                    bgActive: window.bgActive
                    accentPrimary: window.accentPrimary
                    accentGreen: window.accentGreen
                    warningColor: window.warningColor
                    textPrimary: window.textPrimary
                    textSecondary: window.textSecondary
                    textMuted: window.textMuted
                    divider: window.divider

                    onSlotSelected: {
                        canvas.setLetterContent(backend.letterHeader, backend.letterBody, backend.letterFooter)
                        paperCombo.currentIndex = backend.currentPaper
                        dirtyState.capture()
                        canvas.forceActiveFocus()
                    }

                    onSlotSaved: {
                        dirtyState.capture()
                    }

                    // Handle slot change requests with dirty check
                    onRequestSlotChange: {
                        dirtyState.check()
                        dirtyState.confirmDiscard(function() {
                            saveBrowser.loadSlot(newSlot)
                        })
                    }

                    // Handle storage change requests with dirty check
                    onRequestStorageChange: {
                        dirtyState.check()
                        dirtyState.confirmDiscard(function() {
                            saveBrowser.changeStorage(newStorage)
                        })
                    }

                    // Handle player change requests with dirty check
                    onRequestPlayerChange: {
                        dirtyState.check()
                        dirtyState.confirmDiscard(function() {
                            saveBrowser.changePlayer(newPlayer)
                        })
                    }
                }

                // Divider between sidebar and canvas
                Rectangle {
                    Layout.preferredWidth: backend.saveLoaded ? 1 : 0
                    Layout.fillHeight: true
                    color: divider
                    visible: Layout.preferredWidth > 0
                    opacity: backend.saveLoaded ? 1.0 : 0.0

                    Behavior on Layout.preferredWidth {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                    }

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 150
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                // Canvas area
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Status badges - top right
                    Row {
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.topMargin: 12
                        anchors.rightMargin: 16
                        spacing: 8
                        z: 10

                        // ROM Loaded badge
                        Rectangle {
                            visible: backend.loaded
                            width: romStatusContent.width + 14
                            height: 26
                            radius: 6
                            color: Qt.rgba(accentGreen.r, accentGreen.g, accentGreen.b, 0.12)

                            Row {
                                id: romStatusContent
                                anchors.centerIn: parent
                                spacing: 6

                                Rectangle {
                                    width: 6
                                    height: 6
                                    radius: 3
                                    color: accentGreen
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Text {
                                    text: "ROM Loaded"
                                    font.pixelSize: 11
                                    font.weight: Font.Medium
                                    color: accentGreen
                                }
                            }
                        }

                        // SAVE Loaded badge
                        Rectangle {
                            visible: backend.saveLoaded
                            width: saveStatusContent.width + 14
                            height: 26
                            radius: 6
                            color: Qt.rgba(accentPrimary.r, accentPrimary.g, accentPrimary.b, 0.12)

                            Row {
                                id: saveStatusContent
                                anchors.centerIn: parent
                                spacing: 6

                                Rectangle {
                                    width: 6
                                    height: 6
                                    radius: 3
                                    color: accentPrimary
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Text {
                                    text: "SAVE Loaded"
                                    font.pixelSize: 11
                                    font.weight: Font.Medium
                                    color: accentPrimary
                                }
                            }
                        }
                    }

                    // Focus ring
                    Rectangle {
                        anchors.centerIn: canvasFrame
                    width: canvasFrame.width + 6
                    height: canvasFrame.height + 6
                    radius: 15
                    color: "transparent"
                    border.color: canvas.activeFocus ? accentPrimary : "transparent"
                    border.width: 2
                    opacity: canvas.activeFocus ? 0.6 : 0

                    Behavior on opacity { NumberAnimation { duration: 150 } }
                    Behavior on border.color { ColorAnimation { duration: 150 } }
                }

                // Canvas with rounded corners
                Rectangle {
                    id: canvasFrame
                    anchors.centerIn: parent
                    width: 520
                    height: 392
                    radius: 12
                    color: bgElevated
                    clip: true

                    layer.enabled: true
                    layer.effect: DropShadow {
                        transparentBorder: true
                        horizontalOffset: 0
                        verticalOffset: 12
                        radius: 32
                        samples: 65
                        color: "#30000000"
                    }

                    LetterCanvas {
                        id: canvas
                        anchors.centerIn: parent
                        width: 512
                        height: 384
                        backend: backend
                        focus: true

                        onRecipientNameClicked: {
                            letterInfoDialog.selectedTab = 0  // Start on Receiver tab
                            // Refresh all fields before opening
                            recipientTownIdField.text = backend.recipientTownId.toString()
                            recipientTownField.text = backend.recipientTown
                            recipientPlayerIdField.text = backend.recipientPlayerId.toString()
                            recipientNameField.text = backend.recipientName
                            senderTownIdField.text = backend.senderTownId.toString()
                            senderTownField.text = backend.senderTown
                            senderPlayerIdField.text = backend.senderPlayerId.toString()
                            senderNameField.text = backend.senderName
                            letterInfoDialog.open()
                        }

                        Keys.onPressed: {
                            if (event.key === Qt.Key_Backspace) {
                                canvas.backspace()
                                event.accepted = true
                            } else if (event.key === Qt.Key_Delete) {
                                canvas.deleteChar()
                                event.accepted = true
                            } else if (event.key === Qt.Key_Left) {
                                if (event.modifiers & Qt.ShiftModifier) {
                                    canvas.extendSelectionLeft()
                                } else {
                                    canvas.moveCursorLeft()
                                }
                                event.accepted = true
                            } else if (event.key === Qt.Key_Right) {
                                if (event.modifiers & Qt.ShiftModifier) {
                                    canvas.extendSelectionRight()
                                } else {
                                    canvas.moveCursorRight()
                                }
                                event.accepted = true
                            } else if (event.key === Qt.Key_Up) {
                                if (event.modifiers & Qt.ShiftModifier) {
                                    canvas.extendSelectionUp()
                                } else {
                                    canvas.moveCursorUp()
                                }
                                event.accepted = true
                            } else if (event.key === Qt.Key_Down) {
                                if (event.modifiers & Qt.ShiftModifier) {
                                    canvas.extendSelectionDown()
                                } else {
                                    canvas.moveCursorDown()
                                }
                                event.accepted = true
                            } else if (event.key === Qt.Key_Home) {
                                if (event.modifiers & Qt.ShiftModifier) {
                                    canvas.extendSelectionHome()
                                } else {
                                    canvas.moveCursorHome()
                                }
                                event.accepted = true
                            } else if (event.key === Qt.Key_End) {
                                if (event.modifiers & Qt.ShiftModifier) {
                                    canvas.extendSelectionEnd()
                                } else {
                                    canvas.moveCursorEnd()
                                }
                                event.accepted = true
                            } else if (event.key === Qt.Key_A && (event.modifiers & Qt.ControlModifier)) {
                                canvas.selectAll()
                                event.accepted = true
                            } else if (event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier)) {
                                canvas.copySelection()
                                event.accepted = true
                            } else if (event.key === Qt.Key_X && (event.modifiers & Qt.ControlModifier)) {
                                canvas.cutSelection()
                                event.accepted = true
                            } else if (event.key === Qt.Key_V && (event.modifiers & Qt.ControlModifier)) {
                                canvas.paste()
                                event.accepted = true
                            } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                                canvas.newLine()
                                event.accepted = true
                            } else if (event.text.length > 0) {
                                canvas.insertChar(event.text)
                                event.accepted = true
                            }
                        }

                        // Tooltip overlay for greeting line (recipient) - hover only
                        MouseArea {
                            id: greetingHover
                            x: 96   // Header at x=48 in 1x, scaled 2x = 96
                            y: 80   // Header at y=40 in 1x, scaled 2x = 80
                            width: 300
                            height: 32
                            hoverEnabled: true
                            visible: backend.recipientName !== "" || backend.recipientTown !== ""
                            acceptedButtons: Qt.NoButton  // Don't capture clicks

                            ToolTip {
                                id: recipientTooltip
                                visible: greetingHover.containsMouse && (backend.displayRecipientName !== "" || (!backend.isRecipientStranger && backend.recipientTown !== ""))
                                delay: 400
                                timeout: 5000

                                background: Rectangle {
                                    color: bgElevated
                                    radius: 6
                                    border.color: divider
                                    border.width: 1
                                }

                                contentItem: Text {
                                    text: {
                                        // Use displayRecipientName which shows "Some Stranger" when flag is 7
                                        var displayName = backend.displayRecipientName
                                        var displayTown = backend.isRecipientStranger ? "" : backend.recipientTown
                                        if (displayName !== "" && displayTown !== "")
                                            return "To: " + displayName + " from " + displayTown
                                        else if (displayName !== "")
                                            return "To: " + displayName
                                        else if (displayTown !== "")
                                            return "To: " + displayTown
                                        return ""
                                    }
                                    font.pixelSize: 12
                                    color: textPrimary
                                }
                            }
                        }

                        // Tooltip overlay for signature line (sender) - hover only
                        MouseArea {
                            id: signatureHover
                            x: 200  // Footer is right-aligned, roughly in this area
                            y: 272  // Footer at y=136 in 1x, scaled 2x = 272
                            width: 280
                            height: 32
                            hoverEnabled: true
                            visible: backend.senderName !== "" || backend.senderTown !== ""
                            acceptedButtons: Qt.NoButton  // Don't capture clicks

                            ToolTip {
                                id: senderTooltip
                                visible: signatureHover.containsMouse && (backend.senderName !== "" || backend.senderTown !== "")
                                delay: 400
                                timeout: 5000

                                background: Rectangle {
                                    color: bgElevated
                                    radius: 6
                                    border.color: divider
                                    border.width: 1
                                }

                                contentItem: Text {
                                    text: {
                                        if (backend.senderName !== "" && backend.senderTown !== "")
                                            return "From: " + backend.senderName + " from " + backend.senderTown
                                        else if (backend.senderName !== "")
                                            return "From: " + backend.senderName
                                        else if (backend.senderTown !== "")
                                            return "From: " + backend.senderTown
                                        return ""
                                    }
                                    font.pixelSize: 12
                                    color: textPrimary
                                }
                            }
                        }

                        // Main click/drag handler - on top
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.IBeamCursor
                            property bool dragging: false

                            onPressed: {
                                canvas.startSelection(mouse.x, mouse.y)
                                dragging = true
                                canvas.forceActiveFocus()
                            }
                            onPositionChanged: {
                                if (dragging) {
                                    canvas.updateSelection(mouse.x, mouse.y)
                                }
                            }
                            onReleased: {
                                dragging = false
                                // If no selection was made (just a click), treat as cursor positioning
                                if (!canvas.hasSelection) {
                                    canvas.handleClick(mouse.x, mouse.y)
                                }
                            }
                            onDoubleClicked: {
                                canvas.handleDoubleClick(mouse.x, mouse.y)
                            }
                        }
                    }
                }

                // Stationery selector - bottom right under canvas
                Row {
                    anchors.right: canvasFrame.right
                    anchors.top: canvasFrame.bottom
                    anchors.topMargin: 12
                    spacing: 10
                    visible: backend.loaded

                    // Opened checkbox with overlay for tooltip on disabled state
                    Item {
                        width: letterOpenedCheckbox.width
                        height: letterOpenedCheckbox.height
                        anchors.verticalCenter: parent.verticalCenter

                        CheckBox {
                            id: letterOpenedCheckbox
                            // Disabled for writing states (0x01 = letter writing, 0x04 = bottle writing)
                            property int iconType: backend.letterIconFlags & 0x0F
                            property bool isWritingState: iconType === 0x01 || iconType === 0x04
                            enabled: !isWritingState
                            checked: backend.isLetterOpened
                            onCheckedChanged: {
                                if (checked !== backend.isLetterOpened) {
                                    backend.isLetterOpened = checked
                                }
                            }

                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                x: letterOpenedCheckbox.leftPadding
                                y: parent.height / 2 - height / 2
                                radius: 3
                                color: letterOpenedCheckbox.enabled
                                    ? (letterOpenedCheckbox.checked ? accentPrimary : bgHover)
                                    : Qt.darker(bgHover, 1.3)
                                border.color: letterOpenedCheckbox.enabled
                                    ? (letterOpenedCheckbox.checked ? accentPrimary : divider)
                                    : Qt.darker(divider, 1.3)
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: letterOpenedCheckbox.enabled ? "✓" : "✕"
                                    font.pixelSize: 10
                                    font.weight: Font.Bold
                                    color: letterOpenedCheckbox.enabled ? bgBase : textMuted
                                    visible: letterOpenedCheckbox.checked || !letterOpenedCheckbox.enabled
                                }
                            }
                            contentItem: Text {
                                text: "Opened"
                                font.pixelSize: 12
                                font.weight: Font.Medium
                                color: letterOpenedCheckbox.enabled ? textMuted : Qt.darker(textMuted, 1.3)
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: letterOpenedCheckbox.indicator.width + 4
                            }
                        }

                        // Invisible mouse area overlay for tooltip on disabled checkbox
                        MouseArea {
                            id: openedMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.NoButton
                            propagateComposedEvents: true

                            ToolTip {
                                visible: letterOpenedCheckbox.isWritingState && openedMouseArea.containsMouse
                                text: qsTr("Letters being written cannot be marked as opened")
                                delay: 300
                                timeout: 5000

                                background: Rectangle {
                                    color: bgElevated
                                    radius: 6
                                    border.color: divider
                                    border.width: 1
                                }

                                contentItem: Text {
                                    text: qsTr("Letters being written cannot be marked as opened")
                                    font.pixelSize: 12
                                    color: textPrimary
                                }
                            }
                        }
                    }

                    // Vertical divider
                    Rectangle {
                        width: 1
                        height: 20
                        color: divider
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Label {
                        text: "Stationery"
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: textMuted
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    ComboBox {
                        id: paperCombo
                        width: 160
                        model: backend.paperNames
                        enabled: backend.loaded
                        currentIndex: backend.currentPaper
                        font.pixelSize: 13

                        onCurrentIndexChanged: {
                            if (backend.loaded) {
                                backend.currentPaper = currentIndex
                            }
                        }

                        delegate: ItemDelegate {
                            width: paperCombo.width
                            height: 34

                            contentItem: Text {
                                text: modelData
                                color: highlighted ? textPrimary : textSecondary
                                font.pixelSize: 13
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 10
                            }

                            background: Rectangle {
                                color: highlighted ? bgHover : "transparent"
                                radius: 4

                                Behavior on color { ColorAnimation { duration: 80 } }
                            }

                            highlighted: paperCombo.highlightedIndex === index
                        }

                        indicator: Canvas {
                            x: paperCombo.width - width - 10
                            y: (paperCombo.height - height) / 2
                            width: 10
                            height: 6

                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.reset()
                                ctx.fillStyle = textMuted
                                ctx.moveTo(0, 0)
                                ctx.lineTo(width, 0)
                                ctx.lineTo(width / 2, height)
                                ctx.closePath()
                                ctx.fill()
                            }
                        }

                        contentItem: Text {
                            leftPadding: 12
                            rightPadding: 28
                            text: paperCombo.displayText
                            font.pixelSize: 13
                            color: paperCombo.enabled ? textPrimary : textMuted
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }

                        background: Rectangle {
                            implicitHeight: 34
                            radius: 8
                            color: paperCombo.down ? bgActive : (paperCombo.hovered ? bgHover : bgElevated)

                            Behavior on color { ColorAnimation { duration: 80 } }
                        }

                        popup: Popup {
                            y: -implicitHeight - 6  // Pop up above the combobox
                            width: paperCombo.width
                            implicitHeight: Math.min(contentItem.implicitHeight + 12, 280)
                            padding: 6

                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: paperCombo.popup.visible ? paperCombo.delegateModel : null
                                currentIndex: paperCombo.highlightedIndex
                                ScrollIndicator.vertical: ScrollIndicator {}
                            }

                            background: Rectangle {
                                color: bgElevated
                                radius: 10
                                border.color: divider
                                border.width: 1

                                layer.enabled: true
                                layer.effect: DropShadow {
                                    transparentBorder: true
                                    horizontalOffset: 0
                                    verticalOffset: -6
                                    radius: 16
                                    samples: 33
                                    color: "#50000000"
                                }
                            }
                        }
                    }
                }
                }  // Close Canvas area Item
            }  // Close RowLayout (Main content area)

            // Footer / Status Bar
            StatusBar {
                id: statusBar
                Layout.fillWidth: true
                backend: backend
                letterModified: dirtyState.letterModified

                // Pass colors
                bgSurface: window.bgSurface
                accentGreen: window.accentGreen
                warningColor: window.warningColor
                textPrimary: window.textPrimary
                textMuted: window.textMuted
                divider: window.divider
            }
        }
    }

    // Resize handles (hidden when maximized)
    MouseArea {
        id: resizeLeft
        visible: !isMaximized
        width: 5
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 40
        anchors.bottomMargin: 5
        cursorShape: Qt.SizeHorCursor
        property point clickPos

        onPressed: clickPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            if (pressed) {
                var delta = mouse.x - clickPos.x
                if (window.width - delta >= window.minimumWidth) {
                    window.x += delta
                    window.width -= delta
                }
            }
        }
    }

    MouseArea {
        id: resizeRight
        visible: !isMaximized
        width: 5
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 40
        anchors.bottomMargin: 5
        cursorShape: Qt.SizeHorCursor
        property point clickPos

        onPressed: clickPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            if (pressed) {
                var delta = mouse.x - clickPos.x
                if (window.width + delta >= window.minimumWidth) {
                    window.width += delta
                }
            }
        }
    }

    MouseArea {
        id: resizeBottom
        visible: !isMaximized
        height: 5
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        cursorShape: Qt.SizeVerCursor
        property point clickPos

        onPressed: clickPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            if (pressed) {
                var delta = mouse.y - clickPos.y
                if (window.height + delta >= window.minimumHeight) {
                    window.height += delta
                }
            }
        }
    }

    MouseArea {
        id: resizeCorner
        visible: !isMaximized
        width: 12
        height: 12
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cursorShape: Qt.SizeFDiagCursor
        property point clickPos

        onPressed: clickPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            if (pressed) {
                var deltaX = mouse.x - clickPos.x
                var deltaY = mouse.y - clickPos.y
                if (window.width + deltaX >= window.minimumWidth) {
                    window.width += deltaX
                }
                if (window.height + deltaY >= window.minimumHeight) {
                    window.height += deltaY
                }
            }
        }
    }

    // Welcome overlay
    Rectangle {
        anchors.fill: mainContainer
        anchors.margins: isMaximized ? 0 : 1
        radius: isMaximized ? 0 : 12
        color: Qt.rgba(bgBase.r, bgBase.g, bgBase.b, 0.94)
        visible: !backend.loaded

        Behavior on opacity { NumberAnimation { duration: 200 } }

        MouseArea {
            anchors.fill: parent
            onClicked: {}
        }

        Column {
            anchors.centerIn: parent
            spacing: 28

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 72
                height: 72
                radius: 16
                color: bgElevated

                Text {
                    anchors.centerIn: parent
                    text: "✉"
                    font.pixelSize: 32
                    color: accentPrimary
                }
            }

            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Letter Previewer"
                    font.pixelSize: 20
                    font.weight: Font.DemiBold
                    color: textPrimary
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Preview and compose letters with original game stationery"
                    font.pixelSize: 13
                    color: textSecondary
                }
            }

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 150
                height: 42
                radius: 10
                color: openButton.pressed ? Qt.darker(accentPrimary, 1.15) :
                       openButton.containsMouse ? Qt.lighter(accentPrimary, 1.1) : accentPrimary

                Behavior on color { ColorAnimation { duration: 100 } }

                Text {
                    anchors.centerIn: parent
                    text: "Open ROM"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    color: "#FFFFFF"
                }

                MouseArea {
                    id: openButton
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: fileDialog.open()
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "or press Ctrl+O"
                font.pixelSize: 11
                color: textMuted
            }
        }
    }

    // Keyboard shortcuts
    Shortcut {
        sequence: "Ctrl+O"
        enabled: !backend.loaded
        onActivated: fileDialog.open()
    }

    Shortcut {
        sequence: "Ctrl+W"
        enabled: backend.saveLoaded
        onActivated: closeSaveFile()
    }

    Shortcut {
        sequence: "Ctrl+I"
        enabled: backend.loaded
        onActivated: importDialog.open()
    }

    Shortcut {
        sequence: "Ctrl+E"
        enabled: backend.loaded
        onActivated: exportDialog.open()
    }

    Shortcut {
        sequence: "Ctrl+P"
        enabled: backend.loaded
        onActivated: pngDialog.open()
    }

    Shortcut {
        sequence: "Ctrl+N"
        enabled: backend.loaded
        onActivated: {
            canvas.clearText()
            paperCombo.currentIndex = 0
            backend.recipientName = ""
            backend.recipientTown = ""
            backend.recipientPlayerId = 0
            backend.recipientTownId = 0
            backend.recipientNameStart = -1
            backend.recipientNameEnd = -1
            backend.senderName = ""
            backend.senderTown = ""
            backend.senderPlayerId = 0
            backend.senderTownId = 0
            backend.attachedItem = 0xFFF1
        }
    }

    Shortcut {
        sequence: "Ctrl+]"
        enabled: backend.loaded && paperCombo.currentIndex < 63
        onActivated: paperCombo.currentIndex++
    }

    Shortcut {
        sequence: "Ctrl+["
        enabled: backend.loaded && paperCombo.currentIndex > 0
        onActivated: paperCombo.currentIndex--
    }

    // File dialogs
    FileDialog {
        id: fileDialog
        title: "Select Animal Crossing: Wild World ROM"
        nameFilters: ["NDS ROM Files (*.nds)", "All Files (*)"]
        onAccepted: {
            if (backend.loadRom(fileDialog.fileUrl)) {
                canvas.forceActiveFocus()
            }
        }
    }

    FileDialog {
        id: importDialog
        title: "Import Letter"
        nameFilters: ["Letter Files (*.ltr)", "All Files (*)"]
        onAccepted: {
            if (backend.importLtr(importDialog.fileUrl)) {
                canvas.setLetterContent(backend.letterHeader, backend.letterBody, backend.letterFooter)
                paperCombo.currentIndex = backend.currentPaper
                canvas.forceActiveFocus()
            }
        }
    }

    FileDialog {
        id: exportDialog
        title: "Export Letter"
        selectExisting: false
        nameFilters: ["Letter Files (*.ltr)", "All Files (*)"]
        onAccepted: {
            // Sync canvas text to backend before export
            backend.letterText = canvas.text
            backend.exportLtr(exportDialog.fileUrl)
        }
    }

    FileDialog {
        id: pngDialog
        title: "Export as PNG"
        selectExisting: false
        nameFilters: ["PNG Images (*.png)", "All Files (*)"]
        onAccepted: {
            // Sync canvas text to backend before export
            backend.letterText = canvas.text
            backend.exportPng(pngDialog.fileUrl, 2)  // 2x scale (512x384)
        }
    }

    FileDialog {
        id: openSaveDialog
        title: "Open Save File"
        nameFilters: ["Save Files (*.sav)", "All Files (*)"]
        onAccepted: {
            if (backend.loadSave(openSaveDialog.fileUrl)) {
                canvas.forceActiveFocus()
            }
        }
    }

    FileDialog {
        id: saveSaveAsDialog
        title: "Save As"
        selectExisting: false
        nameFilters: ["Save Files (*.sav)", "All Files (*)"]
        onAccepted: {
            backend.saveSave(saveSaveAsDialog.fileUrl)
        }
    }

    // Letter Info Dialog (Receiver/Sender tabs)
    Popup {
        id: letterInfoDialog
        modal: true
        anchors.centerIn: parent
        width: 280
        padding: 0

        property int selectedTab: 0

        background: Rectangle {
            color: bgElevated
            radius: 8
            border.color: divider
            border.width: 1
        }

        contentItem: Column {
            spacing: 0

            // Header with tabs
            Item {
                width: parent.width
                height: 36

                Row {
                    anchors.centerIn: parent
                    spacing: 24

                    Text {
                        text: "To"
                        font.pixelSize: 12
                        font.weight: letterInfoDialog.selectedTab === 0 ? Font.DemiBold : Font.Normal
                        color: letterInfoDialog.selectedTab === 0 ? accentPrimary : textSecondary
                        opacity: receiverTabArea.containsMouse && letterInfoDialog.selectedTab !== 0 ? 0.8 : 1

                        Rectangle {
                            visible: letterInfoDialog.selectedTab === 0
                            anchors.top: parent.bottom
                            anchors.topMargin: 2
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width + 8
                            height: 2
                            radius: 1
                            color: accentPrimary
                        }

                        MouseArea {
                            id: receiverTabArea
                            anchors.fill: parent
                            anchors.margins: -8
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: letterInfoDialog.selectedTab = 0
                        }
                    }

                    Text {
                        text: "From"
                        font.pixelSize: 12
                        font.weight: letterInfoDialog.selectedTab === 1 ? Font.DemiBold : Font.Normal
                        color: letterInfoDialog.selectedTab === 1 ? accentPrimary : textSecondary
                        opacity: senderTabArea.containsMouse && letterInfoDialog.selectedTab !== 1 ? 0.8 : 1

                        Rectangle {
                            visible: letterInfoDialog.selectedTab === 1
                            anchors.top: parent.bottom
                            anchors.topMargin: 2
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width + 8
                            height: 2
                            radius: 1
                            color: accentPrimary
                        }

                        MouseArea {
                            id: senderTabArea
                            anchors.fill: parent
                            anchors.margins: -8
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: letterInfoDialog.selectedTab = 1
                        }
                    }
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: divider
                }
            }

            // Form content
            Item {
                width: parent.width
                height: contentCol.height + 24

                Column {
                    id: contentCol
                    anchors.centerIn: parent
                    width: parent.width - 24
                    spacing: 10

                    // Row 1: Player Name
                    Column {
                        width: parent.width
                        spacing: 3
                        Text { text: "Name"; font.pixelSize: 10; color: textMuted }
                        TextField {
                            id: recipientNameField
                            visible: letterInfoDialog.selectedTab === 0
                            enabled: !backend.isRecipientStranger
                            width: parent.width; height: 28
                            text: backend.isRecipientStranger ? "" : backend.recipientName
                            placeholderText: backend.recipientRelation === 6 ? "(Event Bottle)" : (backend.recipientRelation === 7 ? "(Stranger)" : "")
                            maximumLength: 8
                            font.pixelSize: 11; color: enabled ? textPrimary : textMuted
                            leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                            background: Rectangle {
                                color: recipientNameField.enabled ? bgHover : bgBase
                                radius: 4
                                border.color: recipientNameField.activeFocus ? accentPrimary : divider
                                border.width: 1
                                opacity: recipientNameField.enabled ? 1.0 : 0.5
                            }
                        }
                        TextField {
                            id: senderNameField
                            visible: letterInfoDialog.selectedTab === 1
                            width: parent.width; height: 28
                            text: backend.senderName
                            maximumLength: 8
                            font.pixelSize: 11; color: textPrimary
                            leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                            background: Rectangle {
                                color: bgHover; radius: 4
                                border.color: senderNameField.activeFocus ? accentPrimary : divider
                                border.width: 1
                            }
                        }
                    }

                    // Row 2: Town Name
                    Column {
                        width: parent.width
                        spacing: 3
                        Text { text: "Town"; font.pixelSize: 10; color: textMuted }
                        TextField {
                            id: recipientTownField
                            visible: letterInfoDialog.selectedTab === 0
                            enabled: !backend.isRecipientStranger
                            width: parent.width; height: 28
                            text: backend.isRecipientStranger ? "" : backend.recipientTown
                            placeholderText: backend.isRecipientStranger ? "(None)" : ""
                            maximumLength: 8
                            font.pixelSize: 11; color: enabled ? textPrimary : textMuted
                            leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                            background: Rectangle {
                                color: recipientTownField.enabled ? bgHover : bgBase
                                radius: 4
                                border.color: recipientTownField.activeFocus ? accentPrimary : divider
                                border.width: 1
                                opacity: recipientTownField.enabled ? 1.0 : 0.5
                            }
                        }
                        TextField {
                            id: senderTownField
                            visible: letterInfoDialog.selectedTab === 1
                            width: parent.width; height: 28
                            text: backend.senderTown
                            maximumLength: 8
                            font.pixelSize: 11; color: textPrimary
                            leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                            background: Rectangle {
                                color: bgHover; radius: 4
                                border.color: senderTownField.activeFocus ? accentPrimary : divider
                                border.width: 1
                            }
                        }
                    }

                    // Row 3: Player ID + Town ID
                    Row {
                        spacing: 8
                        width: parent.width

                        Column {
                            width: (parent.width - 8) / 2
                            spacing: 3
                            Text { text: "Player ID"; font.pixelSize: 10; color: textMuted }
                            TextField {
                                id: recipientPlayerIdField
                                visible: letterInfoDialog.selectedTab === 0
                                enabled: !backend.isRecipientStranger
                                width: parent.width; height: 28
                                text: backend.isRecipientStranger ? "0" : backend.recipientPlayerId.toString()
                                validator: IntValidator { bottom: 0; top: 65535 }
                                font.pixelSize: 11; color: enabled ? textPrimary : textMuted
                                leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                                background: Rectangle {
                                    color: recipientPlayerIdField.enabled ? bgHover : bgBase
                                    radius: 4
                                    border.color: recipientPlayerIdField.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                    opacity: recipientPlayerIdField.enabled ? 1.0 : 0.5
                                }
                            }
                            TextField {
                                id: senderPlayerIdField
                                visible: letterInfoDialog.selectedTab === 1
                                width: parent.width; height: 28
                                text: backend.senderPlayerId.toString()
                                validator: IntValidator { bottom: 0; top: 65535 }
                                font.pixelSize: 11; color: textPrimary
                                leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                                background: Rectangle {
                                    color: bgHover; radius: 4
                                    border.color: senderPlayerIdField.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                }
                            }
                        }

                        Column {
                            width: (parent.width - 8) / 2
                            spacing: 3
                            Text { text: "Town ID"; font.pixelSize: 10; color: textMuted }
                            TextField {
                                id: recipientTownIdField
                                visible: letterInfoDialog.selectedTab === 0
                                enabled: !backend.isRecipientStranger
                                width: parent.width; height: 28
                                text: backend.isRecipientStranger ? "0" : backend.recipientTownId.toString()
                                validator: IntValidator { bottom: 0; top: 65535 }
                                font.pixelSize: 11; color: enabled ? textPrimary : textMuted
                                leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                                background: Rectangle {
                                    color: recipientTownIdField.enabled ? bgHover : bgBase
                                    radius: 4
                                    border.color: recipientTownIdField.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                    opacity: recipientTownIdField.enabled ? 1.0 : 0.5
                                }
                            }
                            TextField {
                                id: senderTownIdField
                                visible: letterInfoDialog.selectedTab === 1
                                width: parent.width; height: 28
                                text: backend.senderTownId.toString()
                                validator: IntValidator { bottom: 0; top: 65535 }
                                font.pixelSize: 11; color: textPrimary
                                leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                                background: Rectangle {
                                    color: bgHover; radius: 4
                                    border.color: senderTownIdField.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                }
                            }
                        }
                    }

                    // Row 4: Flag + Gender
                    Row {
                        spacing: 8
                        width: parent.width

                        Column {
                            width: (parent.width - 8) / 2
                            spacing: 3
                            Text { text: "Flag"; font.pixelSize: 10; color: textMuted }
                            ComboBox {
                                id: recipientRelationCombo
                                visible: letterInfoDialog.selectedTab === 0
                                width: parent.width
                                font.pixelSize: 11
                                topPadding: 0; bottomPadding: 0
                                topInset: 0; bottomInset: 0
                                model: ListModel {
                                    ListElement { text: "Future Self"; value: 1 }
                                    ListElement { text: "Player"; value: 2 }
                                    ListElement { text: "Villager"; value: 3 }
                                    ListElement { text: "Green Letter"; value: 5 }
                                    ListElement { text: "Bottle (System)"; value: 6 }
                                    ListElement { text: "Bottle (Player)"; value: 7 }
                                }
                                textRole: "text"
                                currentIndex: recipientRelationCombo.findIndex(backend.recipientRelation)
                                onActivated: {
                                    var newValue = model.get(currentIndex).value
                                    backend.recipientRelation = newValue
                                    // Force gender to male (0) when not Future Self or Player
                                    if (newValue !== 1 && newValue !== 2) {
                                        backend.recipientGender = 0
                                    }
                                }
                                function findIndex(val) {
                                    for (var i = 0; i < model.count; i++) {
                                        if (model.get(i).value === val) return i
                                    }
                                    return -1
                                }
                                background: Rectangle {
                                    implicitHeight: 28
                                    color: bgHover; radius: 4
                                    border.color: recipientRelationCombo.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                }
                                contentItem: Text {
                                    leftPadding: 8
                                    text: recipientRelationCombo.displayText
                                    font.pixelSize: 11
                                    color: textPrimary
                                    verticalAlignment: Text.AlignVCenter
                                }
                                delegate: ItemDelegate {
                                    width: recipientRelationCombo.width
                                    height: 24
                                    contentItem: Text {
                                        text: model.text
                                        font.pixelSize: 11
                                        color: textPrimary
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 4
                                    }
                                    background: Rectangle {
                                        color: highlighted ? bgHover : "transparent"
                                    }
                                    highlighted: recipientRelationCombo.highlightedIndex === index
                                }
                                popup: Popup {
                                    y: recipientRelationCombo.height
                                    width: recipientRelationCombo.width
                                    implicitHeight: contentItem.implicitHeight + 8
                                    padding: 4
                                    contentItem: ListView {
                                        clip: true
                                        implicitHeight: contentHeight
                                        model: recipientRelationCombo.popup.visible ? recipientRelationCombo.delegateModel : null
                                        currentIndex: recipientRelationCombo.highlightedIndex
                                    }
                                    background: Rectangle {
                                        color: bgElevated
                                        border.color: divider
                                        border.width: 1
                                        radius: 4
                                    }
                                }
                            }
                            ComboBox {
                                id: senderRelationCombo
                                visible: letterInfoDialog.selectedTab === 1
                                width: parent.width
                                font.pixelSize: 11
                                topPadding: 0; bottomPadding: 0
                                topInset: 0; bottomInset: 0
                                model: ListModel {
                                    ListElement { text: "Player/WFC"; value: 2 }
                                    ListElement { text: "Villager"; value: 3 }
                                    ListElement { text: "System"; value: 4 }
                                }
                                textRole: "text"
                                currentIndex: senderRelationCombo.findIndex(backend.senderRelation)
                                onActivated: {
                                    var newValue = model.get(currentIndex).value
                                    backend.senderRelation = newValue
                                    // Force gender to male (0) when not Player/WFC
                                    if (newValue !== 2) {
                                        backend.senderGender = 0
                                    }
                                }
                                function findIndex(val) {
                                    for (var i = 0; i < model.count; i++) {
                                        if (model.get(i).value === val) return i
                                    }
                                    return -1
                                }
                                background: Rectangle {
                                    implicitHeight: 28
                                    color: bgHover; radius: 4
                                    border.color: senderRelationCombo.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                }
                                contentItem: Text {
                                    leftPadding: 8
                                    text: senderRelationCombo.displayText
                                    font.pixelSize: 11
                                    color: textPrimary
                                    verticalAlignment: Text.AlignVCenter
                                }
                                delegate: ItemDelegate {
                                    width: senderRelationCombo.width
                                    height: 24
                                    contentItem: Text {
                                        text: model.text
                                        font.pixelSize: 11
                                        color: textPrimary
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 4
                                    }
                                    background: Rectangle {
                                        color: highlighted ? bgHover : "transparent"
                                    }
                                    highlighted: senderRelationCombo.highlightedIndex === index
                                }
                                popup: Popup {
                                    y: senderRelationCombo.height
                                    width: senderRelationCombo.width
                                    implicitHeight: contentItem.implicitHeight + 8
                                    padding: 4
                                    contentItem: ListView {
                                        clip: true
                                        implicitHeight: contentHeight
                                        model: senderRelationCombo.popup.visible ? senderRelationCombo.delegateModel : null
                                        currentIndex: senderRelationCombo.highlightedIndex
                                    }
                                    background: Rectangle {
                                        color: bgElevated
                                        border.color: divider
                                        border.width: 1
                                        radius: 4
                                    }
                                }
                            }
                        }

                        Column {
                            width: (parent.width - 8) / 2
                            spacing: 3
                            Text { text: "Gender"; font.pixelSize: 10; color: textMuted }
                            // Recipient gender (visible on To tab)
                            Row {
                                visible: letterInfoDialog.selectedTab === 0
                                height: 28
                                spacing: 12
                                enabled: backend.recipientRelation === 1 || backend.recipientRelation === 2  // Only enabled for Future Self or Player
                                opacity: enabled ? 1.0 : 0.4
                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 4
                                    Rectangle {
                                        width: 14; height: 14; radius: 7
                                        color: "transparent"
                                        border.color: backend.recipientGender === 0 ? accentPrimary : textMuted
                                        border.width: 1
                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8; height: 8; radius: 4
                                            color: accentPrimary
                                            visible: backend.recipientGender === 0
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: parent.parent.parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                            onClicked: if (parent.parent.parent.enabled) backend.recipientGender = 0
                                        }
                                    }
                                    Text { text: "♂"; font.pixelSize: 12; color: textPrimary; anchors.verticalCenter: parent.verticalCenter }
                                }
                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 4
                                    Rectangle {
                                        width: 14; height: 14; radius: 7
                                        color: "transparent"
                                        border.color: backend.recipientGender === 1 ? accentPrimary : textMuted
                                        border.width: 1
                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8; height: 8; radius: 4
                                            color: accentPrimary
                                            visible: backend.recipientGender === 1
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: parent.parent.parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                            onClicked: if (parent.parent.parent.enabled) backend.recipientGender = 1
                                        }
                                    }
                                    Text { text: "♀"; font.pixelSize: 12; color: textPrimary; anchors.verticalCenter: parent.verticalCenter }
                                }
                            }
                            // Sender gender (visible on From tab)
                            Row {
                                visible: letterInfoDialog.selectedTab === 1
                                height: 28
                                spacing: 12
                                enabled: backend.senderRelation === 2  // Only enabled for Player/WFC
                                opacity: enabled ? 1.0 : 0.4
                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 4
                                    Rectangle {
                                        width: 14; height: 14; radius: 7
                                        color: "transparent"
                                        border.color: backend.senderGender === 0 ? accentPrimary : textMuted
                                        border.width: 1
                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8; height: 8; radius: 4
                                            color: accentPrimary
                                            visible: backend.senderGender === 0
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: parent.parent.parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                            onClicked: if (parent.parent.parent.enabled) backend.senderGender = 0
                                        }
                                    }
                                    Text { text: "♂"; font.pixelSize: 12; color: textPrimary; anchors.verticalCenter: parent.verticalCenter }
                                }
                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 4
                                    Rectangle {
                                        width: 14; height: 14; radius: 7
                                        color: "transparent"
                                        border.color: backend.senderGender === 1 ? accentPrimary : textMuted
                                        border.width: 1
                                        Rectangle {
                                            anchors.centerIn: parent
                                            width: 8; height: 8; radius: 4
                                            color: accentPrimary
                                            visible: backend.senderGender === 1
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            cursorShape: parent.parent.parent.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                                            onClicked: if (parent.parent.parent.enabled) backend.senderGender = 1
                                        }
                                    }
                                    Text { text: "♀"; font.pixelSize: 12; color: textPrimary; anchors.verticalCenter: parent.verticalCenter }
                                }
                            }
                        }
                    }

                }
            }

            // Footer
            Item {
                width: parent.width
                height: 40

                Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: divider }

                Row {
                    anchors.centerIn: parent
                    spacing: 8

                    Text {
                        text: "Cancel"
                        font.pixelSize: 11
                        color: cancelArea.containsMouse ? textPrimary : textSecondary
                        anchors.verticalCenter: parent.verticalCenter

                        MouseArea {
                            id: cancelArea
                            anchors.fill: parent
                            anchors.margins: -8
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: letterInfoDialog.close()
                        }
                    }

                    Rectangle { width: 1; height: 16; color: divider; anchors.verticalCenter: parent.verticalCenter }

                    Rectangle {
                        width: 52; height: 24; radius: 4
                        color: saveArea.pressed ? Qt.darker(accentPrimary, 1.1) :
                               saveArea.containsMouse ? Qt.lighter(accentPrimary, 1.05) : accentPrimary
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            anchors.centerIn: parent
                            text: "Save"
                            font.pixelSize: 11
                            font.weight: Font.Medium
                            color: "#FFFFFF"
                        }

                        MouseArea {
                            id: saveArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                // Recipient info
                                backend.recipientTownId = parseInt(recipientTownIdField.text) || 0
                                backend.recipientTown = recipientTownField.text
                                backend.recipientPlayerId = parseInt(recipientPlayerIdField.text) || 0
                                backend.recipientName = recipientNameField.text
                                // Note: The name is rendered visually at namePosition, not stored in canvas text

                                // Sender info
                                backend.senderTownId = parseInt(senderTownIdField.text) || 0
                                backend.senderTown = senderTownField.text
                                backend.senderPlayerId = parseInt(senderPlayerIdField.text) || 0
                                backend.senderName = senderNameField.text

                                letterInfoDialog.close()
                                canvas.forceActiveFocus()
                                canvas.update()  // Redraw to show new name
                            }
                        }
                    }
                }
            }
        }
    }

    // Edit Attached Item Dialog
    Popup {
        id: attachedItemDialog
        modal: true
        anchors.centerIn: parent
        width: 380
        height: 480
        padding: 0

        property var itemList: []
        property var filteredItems: []
        property string selectedCategory: "All"

        function loadItems() {
            loadCategory(selectedCategory)
        }

        function loadCategory(categoryName) {
            selectedCategory = categoryName
            itemList = backend.getItemsByCategory(categoryName)
            filterItems(itemSearchField.text)
        }

        function filterItems(searchText) {
            if (searchText === "") {
                filteredItems = itemList
            } else {
                var search = searchText.toLowerCase()
                filteredItems = itemList.filter(function(item) {
                    return item.display.toLowerCase().indexOf(search) !== -1
                })
            }
        }

        function selectItem(hexCode) {
            attachedItemDialogField.text = hexCode.toString(16).toUpperCase().padStart(4, '0')
        }

        function scrollToSelectedItem() {
            var currentHex = backend.attachedItem
            for (var i = 0; i < filteredItems.length; i++) {
                if (filteredItems[i].hex === currentHex) {
                    itemListView.positionViewAtIndex(i, ListView.Beginning)
                    break
                }
            }
        }

        onOpened: {
            selectedCategory = "All"
            categoryCombo.currentIndex = 0
            loadItems()
            // Delay scroll to ensure list is populated
            Qt.callLater(scrollToSelectedItem)
        }

        background: Rectangle {
            color: bgElevated
            radius: 8
            border.color: divider
            border.width: 1
        }

        contentItem: Column {
            spacing: 0

            // Header
            Rectangle {
                width: parent.width
                height: 36
                color: bgSurface
                radius: 8

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 8
                    color: bgSurface
                }

                Text {
                    anchors.centerIn: parent
                    text: "Edit Attached Item"
                    font.pixelSize: 12
                    font.weight: Font.DemiBold
                    color: textPrimary
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: divider
                }
            }

            // Form content
            Item {
                width: parent.width
                height: attachedItemDialog.height - 36 - 40  // Header and footer

                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    // Hex input row
                    Row {
                        width: parent.width
                        spacing: 8

                        Column {
                            width: 80
                            spacing: 3
                            Text { text: "Item ID (hex)"; font.pixelSize: 10; color: textMuted }
                            TextField {
                                id: attachedItemDialogField
                                width: parent.width; height: 28
                                text: backend.attachedItem.toString(16).toUpperCase().padStart(4, '0')
                                font.pixelSize: 11; color: textPrimary; font.family: "Consolas, monospace"
                                leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                                maximumLength: 4
                                validator: RegularExpressionValidator { regularExpression: /[0-9A-Fa-f]{0,4}/ }
                                background: Rectangle {
                                    color: bgHover; radius: 4
                                    border.color: attachedItemDialogField.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                }
                            }
                        }

                        Column {
                            width: parent.width - 88
                            spacing: 3
                            Text { text: "Item Name"; font.pixelSize: 10; color: textMuted }
                            Rectangle {
                                width: parent.width; height: 28
                                color: bgHover; radius: 4
                                border.color: divider; border.width: 1

                                Text {
                                    anchors.fill: parent
                                    anchors.leftMargin: 8
                                    anchors.rightMargin: 8
                                    verticalAlignment: Text.AlignVCenter
                                    text: {
                                        var hex = parseInt(attachedItemDialogField.text, 16)
                                        if (isNaN(hex)) return ""
                                        return backend.getItemName(hex)
                                    }
                                    font.pixelSize: 11
                                    color: textPrimary
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }

                    // Category and Search row
                    Row {
                        width: parent.width
                        spacing: 8

                        Column {
                            width: (parent.width - 8) / 2
                            spacing: 3
                            Text { text: "Category"; font.pixelSize: 10; color: textMuted }
                            Rectangle {
                                width: parent.width
                                height: 28
                                color: bgHover
                                radius: 4
                                border.color: categoryCombo.down ? accentPrimary : divider
                                border.width: 1

                                ComboBox {
                                    id: categoryCombo
                                    anchors.fill: parent
                                    model: backend.getItemCategories()
                                    font.pixelSize: 11

                                    onCurrentTextChanged: {
                                        if (attachedItemDialog.visible) {
                                            attachedItemDialog.loadCategory(currentText)
                                        }
                                    }

                                    delegate: ItemDelegate {
                                        width: categoryCombo.width
                                        height: 26

                                        contentItem: Text {
                                            text: modelData
                                            color: highlighted ? textPrimary : textSecondary
                                            font.pixelSize: 11
                                            verticalAlignment: Text.AlignVCenter
                                            leftPadding: 8
                                        }

                                        background: Rectangle {
                                            color: highlighted ? bgHover : "transparent"
                                            radius: 4
                                        }

                                        highlighted: categoryCombo.highlightedIndex === index
                                    }

                                    indicator: Text {
                                        anchors.right: parent.right
                                        anchors.rightMargin: 8
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "▼"
                                        font.pixelSize: 8
                                        color: textMuted
                                    }

                                    contentItem: Text {
                                        leftPadding: 8
                                        rightPadding: 20
                                        text: categoryCombo.displayText
                                        font.pixelSize: 11
                                        color: textPrimary
                                        verticalAlignment: Text.AlignVCenter
                                        elide: Text.ElideRight
                                    }

                                    background: Rectangle {
                                        color: "transparent"
                                    }

                                    popup: Popup {
                                        y: categoryCombo.height + 2
                                        width: categoryCombo.width
                                        implicitHeight: Math.min(contentItem.implicitHeight + 8, 250)
                                        padding: 4

                                        contentItem: ListView {
                                            clip: true
                                            implicitHeight: contentHeight
                                            model: categoryCombo.popup.visible ? categoryCombo.delegateModel : null
                                            currentIndex: categoryCombo.highlightedIndex
                                            ScrollIndicator.vertical: ScrollIndicator {}
                                        }

                                        background: Rectangle {
                                            color: bgElevated
                                            radius: 4
                                            border.color: divider
                                            border.width: 1
                                        }
                                    }
                                }
                            }
                        }

                        Column {
                            width: (parent.width - 8) / 2
                            spacing: 3
                            Text { text: "Search"; font.pixelSize: 10; color: textMuted }
                            TextField {
                                id: itemSearchField
                                width: parent.width; height: 28
                                placeholderText: "Filter..."
                                font.pixelSize: 11; color: textPrimary
                                leftPadding: 8; rightPadding: 8; topPadding: 0; bottomPadding: 0
                                background: Rectangle {
                                    color: bgHover; radius: 4
                                    border.color: itemSearchField.activeFocus ? accentPrimary : divider
                                    border.width: 1
                                }
                                onTextChanged: attachedItemDialog.filterItems(text)
                            }
                        }
                    }

                    // Item count and wrapped gift checkbox
                    Row {
                        width: parent.width
                        height: 14
                        spacing: 8

                        Text {
                            text: attachedItemDialog.filteredItems.length + " items"
                            font.pixelSize: 10
                            color: textMuted
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Item { width: parent.width - 120; height: 1 }

                        CheckBox {
                            id: wrappedGiftCheckbox
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 15
                            padding: 0
                            topPadding: 0
                            bottomPadding: 0
                            leftPadding: 0
                            rightPadding: 0
                            spacing: 0
                            enabled: {
                                var hex = parseInt(attachedItemDialogField.text, 16)
                                return !isNaN(hex) && hex !== 0xFFF1
                            }
                            checked: backend.isGiftWrapped
                            onCheckedChanged: {
                                if (checked !== backend.isGiftWrapped) {
                                    backend.isGiftWrapped = checked
                                }
                            }
                            onEnabledChanged: {
                                // When disabled (no item), uncheck and clear wrapped flag
                                if (!enabled && backend.isGiftWrapped) {
                                    backend.isGiftWrapped = false
                                }
                            }
                            indicator: Rectangle {
                                implicitWidth: 14
                                implicitHeight: 14
                                x: 0
                                y: (wrappedGiftCheckbox.height - height) / 2
                                radius: 2
                                color: wrappedGiftCheckbox.enabled
                                    ? (wrappedGiftCheckbox.checked ? accentPrimary : bgHover)
                                    : Qt.darker(bgHover, 1.2)
                                border.color: wrappedGiftCheckbox.enabled
                                    ? (wrappedGiftCheckbox.checked ? accentPrimary : divider)
                                    : Qt.darker(divider, 1.2)
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "✓"
                                    font.pixelSize: 10
                                    font.weight: Font.Bold
                                    color: bgBase
                                    visible: wrappedGiftCheckbox.checked
                                }
                            }
                            contentItem: Text {
                                text: "Wrapped"
                                font.pixelSize: 10
                                color: wrappedGiftCheckbox.enabled ? textMuted : Qt.darker(textMuted, 1.3)
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 18
                            }
                        }
                    }

                    // Item list
                    Rectangle {
                        width: parent.width
                        height: parent.height - 115  // Remaining space
                        color: bgBase
                        radius: 4
                        border.color: divider
                        border.width: 1
                        clip: true

                        ListView {
                            id: itemListView
                            anchors.fill: parent
                            anchors.margins: 2
                            model: attachedItemDialog.filteredItems
                            clip: true

                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AsNeeded
                            }

                            delegate: Rectangle {
                                width: itemListView.width - 8
                                height: 26
                                radius: 4
                                color: {
                                    var currentHex = parseInt(attachedItemDialogField.text, 16)
                                    if (modelData.hex === currentHex) return bgActive
                                    return itemDelegateArea.containsMouse ? bgHover : "transparent"
                                }

                                Text {
                                    anchors.fill: parent
                                    anchors.leftMargin: 8
                                    anchors.rightMargin: 8
                                    verticalAlignment: Text.AlignVCenter
                                    text: modelData.display
                                    font.pixelSize: 11
                                    font.family: "Consolas, monospace"
                                    color: {
                                        var currentHex = parseInt(attachedItemDialogField.text, 16)
                                        if (modelData.hex === currentHex) return accentPrimary
                                        return textSecondary
                                    }
                                    elide: Text.ElideRight
                                }

                                MouseArea {
                                    id: itemDelegateArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        attachedItemDialog.selectItem(modelData.hex)
                                    }
                                }
                            }
                        }

                        // Empty state
                        Text {
                            anchors.centerIn: parent
                            text: "No items found"
                            font.pixelSize: 11
                            color: textMuted
                            visible: attachedItemDialog.filteredItems.length === 0
                        }
                    }
                }
            }

            // Footer
            Item {
                width: parent.width
                height: 40

                Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: divider }

                Row {
                    anchors.centerIn: parent
                    spacing: 8

                    Text {
                        text: "Cancel"
                        font.pixelSize: 11
                        color: attachedItemCancelArea.containsMouse ? textPrimary : textSecondary
                        anchors.verticalCenter: parent.verticalCenter

                        MouseArea {
                            id: attachedItemCancelArea
                            anchors.fill: parent
                            anchors.margins: -8
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                itemSearchField.text = ""
                                attachedItemDialog.close()
                            }
                        }
                    }

                    Rectangle { width: 1; height: 16; color: divider; anchors.verticalCenter: parent.verticalCenter }

                    Rectangle {
                        width: 52; height: 24; radius: 4
                        color: attachedItemSaveArea.pressed ? Qt.darker(accentPrimary, 1.1) :
                               attachedItemSaveArea.containsMouse ? Qt.lighter(accentPrimary, 1.05) : accentPrimary
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            anchors.centerIn: parent
                            text: "Save"
                            font.pixelSize: 11
                            font.weight: Font.Medium
                            color: "#FFFFFF"
                        }

                        MouseArea {
                            id: attachedItemSaveArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                var itemHex = attachedItemDialogField.text.toUpperCase()
                                backend.attachedItem = parseInt(itemHex, 16) || 0xFFF1
                                itemSearchField.text = ""
                                attachedItemDialog.close()
                                canvas.forceActiveFocus()
                            }
                        }
                    }
                }
            }
        }
    }

    // Unsaved Changes Dialog
    UnsavedChangesDialog {
        id: unsavedChangesDialog
        anchors.centerIn: parent

        // Pass colors
        bgBase: window.bgBase
        bgElevated: window.bgElevated
        bgHover: window.bgHover
        accentPrimary: window.accentPrimary
        accentGreen: window.accentGreen
        warningColor: window.warningColor
        errorColor: window.errorColor
        textPrimary: window.textPrimary
        textSecondary: window.textSecondary
        textMuted: window.textMuted
        divider: window.divider

        onDiscarded: {
            dirtyState.letterModified = false
            if (pendingAction) {
                pendingAction()
                pendingAction = null
            }
        }

        onSaved: {
            backend.saveCurrentSlot()
            dirtyState.capture()
            if (pendingAction) {
                pendingAction()
                pendingAction = null
            }
        }

        onCancelled: {
            pendingAction = null
        }
    }

    // Hex Viewer Dialog
    Popup {
        id: hexViewerDialog
        modal: true
        anchors.centerIn: parent
        width: 620
        height: 450
        padding: 0

        property string hexContent: ""

        background: Rectangle {
            color: bgElevated
            radius: 12
            border.color: divider
            border.width: 1
        }

        contentItem: Column {
            spacing: 0

            // Header
            Rectangle {
                width: parent.width
                height: 40
                color: bgSurface
                radius: 12

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 12
                    color: bgSurface
                }

                Text {
                    anchors.centerIn: parent
                    text: "Letter Hex View"
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                    color: textPrimary
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: divider
                }

                // Close button
                Rectangle {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    width: 28
                    height: 28
                    radius: 6
                    color: hexCloseArea.containsMouse ? bgHover : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "×"
                        font.pixelSize: 16
                        color: hexCloseArea.containsMouse ? textPrimary : textSecondary
                    }

                    MouseArea {
                        id: hexCloseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: hexViewerDialog.close()
                    }
                }
            }

            // Hex content area
            Rectangle {
                width: parent.width
                height: hexViewerDialog.height - 40
                color: "transparent"

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 12
                    clip: true

                    TextArea {
                        id: hexTextArea
                        text: hexViewerDialog.hexContent
                        readOnly: true
                        font.family: "Consolas, Monaco, 'Courier New', monospace"
                        font.pixelSize: 11
                        color: textPrimary
                        wrapMode: TextArea.NoWrap
                        selectByMouse: true

                        background: Rectangle {
                            color: bgBase
                            radius: 6
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        // Try to find ROM in executable directory first
        var localRom = backend.findLocalRom()
        if (localRom !== "") {
            backend.loadRom(Qt.resolvedUrl("file://" + localRom))
            if (backend.loaded) {
                canvas.forceActiveFocus()
                return
            }
        }
        // No local ROM found, show file dialog
        fileDialog.open()
    }
}
