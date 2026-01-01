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

    // Modern Color System - Cool slate palette
    readonly property color bgBase: "#0F1117"
    readonly property color bgSurface: "#161921"
    readonly property color bgElevated: "#1C1F2A"
    readonly property color bgHover: "#252936"
    readonly property color bgActive: "#2E3344"
    readonly property color accentPrimary: "#818CF8"
    readonly property color accentGreen: "#34D399"
    readonly property color textPrimary: "#F1F5F9"
    readonly property color textSecondary: "#94A3B8"
    readonly property color textMuted: "#64748B"
    readonly property color divider: "#1E2330"

    Material.theme: Material.Dark
    Material.accent: accentPrimary
    Material.background: bgBase

    color: "transparent"

    Backend {
        id: backend
    }

    // Main container with rounded corners
    Rectangle {
        id: mainContainer
        anchors.fill: parent
        anchors.margins: 1
        radius: 12
        color: bgBase
        clip: true

        // Subtle border
        Rectangle {
            anchors.fill: parent
            radius: 12
            color: "transparent"
            border.color: Qt.rgba(255, 255, 255, 0.06)
            border.width: 1
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
                radius: 12

                // Square off bottom corners
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 12
                    color: bgSurface
                }

                // Drag area
                MouseArea {
                    id: titleBarDrag
                    anchors.fill: parent
                    anchors.rightMargin: windowControls.width + 8
                    property point clickPos

                    onPressed: {
                        clickPos = Qt.point(mouse.x, mouse.y)
                    }

                    onPositionChanged: {
                        if (pressed) {
                            var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                            window.x += delta.x
                            window.y += delta.y
                        }
                    }

                    onDoubleClicked: {
                        if (window.visibility === Window.Maximized) {
                            window.showNormal()
                        } else {
                            window.showMaximized()
                        }
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 8
                    spacing: 10

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
                                text: window.visibility === Window.Maximized ? "❐" : "□"
                                font.pixelSize: 11
                                color: textSecondary
                            }

                            MouseArea {
                                id: maximizeArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    if (window.visibility === Window.Maximized) {
                                        window.showNormal()
                                    } else {
                                        window.showMaximized()
                                    }
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
                                { text: "Open ROM...", shortcut: "Ctrl+O", action: function() { fileDialog.open() } },
                                { separator: true },
                                { text: "Exit", shortcut: "Alt+F4", action: function() { Qt.quit() } }
                            ]},
                            { title: "Edit", items: [
                                { text: "Clear Letter", shortcut: "", enabled: backend.loaded, action: function() { canvas.clearText() } }
                            ]},
                            { title: "View", items: [
                                { text: "Next Paper", shortcut: "Ctrl+]", enabled: backend.loaded && paperCombo.currentIndex < 63, action: function() { paperCombo.currentIndex++ } },
                                { text: "Previous Paper", shortcut: "Ctrl+[", enabled: backend.loaded && paperCombo.currentIndex > 0, action: function() { paperCombo.currentIndex-- } }
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
                                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

                                background: Rectangle {
                                    color: bgElevated
                                    radius: 10
                                    border.color: divider
                                    border.width: 1

                                    layer.enabled: true
                                    layer.effect: DropShadow {
                                        transparentBorder: true
                                        horizontalOffset: 0
                                        verticalOffset: 8
                                        radius: 24
                                        samples: 49
                                        color: "#50000000"
                                    }
                                }

                                contentItem: Column {
                                    spacing: 2

                                    Repeater {
                                        model: modelData.items

                                        Loader {
                                            sourceComponent: modelData.separator ? separatorComponent : menuItemComponent

                                            Component {
                                                id: separatorComponent
                                                Rectangle {
                                                    width: 160
                                                    height: 9
                                                    color: "transparent"
                                                    Rectangle {
                                                        anchors.centerIn: parent
                                                        width: parent.width - 12
                                                        height: 1
                                                        color: divider
                                                    }
                                                }
                                            }

                                            Component {
                                                id: menuItemComponent
                                                Rectangle {
                                                    width: 160
                                                    height: 32
                                                    radius: 6
                                                    color: itemArea.containsMouse && (modelData.enabled !== false) ? bgHover : "transparent"

                                                    Behavior on color { ColorAnimation { duration: 80 } }

                                                    RowLayout {
                                                        anchors.fill: parent
                                                        anchors.leftMargin: 10
                                                        anchors.rightMargin: 10
                                                        spacing: 8

                                                        Text {
                                                            text: modelData.text
                                                            font.pixelSize: 12
                                                            color: modelData.enabled !== false ? (itemArea.containsMouse ? textPrimary : textSecondary) : textMuted
                                                            Layout.fillWidth: true
                                                        }

                                                        Text {
                                                            text: modelData.shortcut || ""
                                                            font.pixelSize: 11
                                                            color: textMuted
                                                            visible: text !== ""
                                                        }
                                                    }

                                                    MouseArea {
                                                        id: itemArea
                                                        anchors.fill: parent
                                                        hoverEnabled: true
                                                        onClicked: {
                                                            if (modelData.enabled !== false && modelData.action) {
                                                                modelData.action()
                                                                menuPopup.close()
                                                            }
                                                        }
                                                    }
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

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.IBeamCursor
                            onClicked: canvas.forceActiveFocus()
                        }
                    }
                }
            }

            // Footer
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                color: bgSurface
                radius: 12

                // Square off top corners
                Rectangle {
                    width: parent.width
                    height: 12
                    color: bgSurface
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

    // Resize handles
    MouseArea {
        id: resizeLeft
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
        anchors.margins: 1
        radius: 12
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
        onActivated: fileDialog.open()
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

    // File dialog
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

    Component.onCompleted: {
        if (!backend.loaded) {
            fileDialog.open()
        }
    }
}
