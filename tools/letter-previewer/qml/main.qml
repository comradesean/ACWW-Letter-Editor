import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.15
import QtQuick.Window 2.15
import LetterPreviewer 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 620
    height: 580
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
    readonly property color textPrimary: "#F1F5F9"
    readonly property color textSecondary: "#94A3B8"
    readonly property color textMuted: "#64748B"
    readonly property color divider: "#1E293B"

    Material.theme: Material.Dark
    Material.accent: accentPrimary
    Material.background: bgBase

    color: "transparent"

    // Manual maximize state (more reliable for frameless windows)
    property bool isMaximized: false
    property rect normalGeometry: Qt.rect(100, 100, 620, 580)

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
                                { separator: true },
                                { text: "Import Letter...", shortcut: "Ctrl+I", actionId: "importLtr", enabledWhen: "loaded" },
                                { text: "Export Letter...", shortcut: "Ctrl+E", actionId: "exportLtr", enabledWhen: "loaded" },
                                { text: "Export as PNG...", shortcut: "Ctrl+P", actionId: "exportPng", enabledWhen: "loaded" },
                                { separator: true },
                                { text: "Exit", shortcut: "Alt+F4", actionId: "exit" }
                            ]},
                            { title: "Edit", items: [
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
                                                                      (modelData.enabledWhen === "loaded" && !backend.loaded)

                                            width: 160
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

            // Toolbar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 52
                color: bgBase

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    spacing: 16

                    Row {
                        spacing: 10

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
                                y: paperCombo.height + 6
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
                                        verticalOffset: 6
                                        radius: 16
                                        samples: 33
                                        color: "#50000000"
                                    }
                                }
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    // Status badge
                    Rectangle {
                        visible: backend.loaded
                        width: statusContent.width + 14
                        height: 26
                        radius: 6
                        color: Qt.rgba(accentGreen.r, accentGreen.g, accentGreen.b, 0.12)

                        Behavior on opacity { NumberAnimation { duration: 200 } }

                        Row {
                            id: statusContent
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
                }
            }

            // Canvas area
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

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

                        Keys.onPressed: {
                            if (event.key === Qt.Key_Backspace) {
                                canvas.backspace()
                                event.accepted = true
                            } else if (event.key === Qt.Key_Delete) {
                                canvas.deleteChar()
                                event.accepted = true
                            } else if (event.key === Qt.Key_Left) {
                                canvas.moveCursorLeft()
                                event.accepted = true
                            } else if (event.key === Qt.Key_Right) {
                                canvas.moveCursorRight()
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
                                visible: greetingHover.containsMouse && (backend.recipientName !== "" || backend.recipientTown !== "")
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
                                        if (backend.recipientName !== "" && backend.recipientTown !== "")
                                            return "To: " + backend.recipientName + " from " + backend.recipientTown
                                        else if (backend.recipientName !== "")
                                            return "To: " + backend.recipientName
                                        else if (backend.recipientTown !== "")
                                            return "To: " + backend.recipientTown
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

                        // Main click handler - on top
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.IBeamCursor
                            onClicked: {
                                canvas.handleClick(mouse.x, mouse.y)
                                canvas.forceActiveFocus()
                            }
                        }
                    }
                }
            }

            // Footer
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                color: bgSurface
                radius: isMaximized ? 0 : 12

                // Square off top corners
                Rectangle {
                    width: parent.width
                    height: 12
                    color: bgSurface
                    visible: !isMaximized
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: divider
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20

                    Text {
                        text: backend.loaded ? "Type to edit  ·  Arrow keys to navigate  ·  Enter for new line" : "File → Open ROM to begin"
                        font.pixelSize: 11
                        color: textMuted
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        visible: backend.loaded
                        text: (paperCombo.currentIndex + 1) + " / 64"
                        font.pixelSize: 11
                        color: textMuted
                    }
                }
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
        onActivated: { canvas.clearText(); paperCombo.currentIndex = 0 }
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
