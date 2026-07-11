import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item {
    property color surfaceColor
    property color textColor
    property color accentColor
    property bool currentLightMode: true

    signal lightModeToggled()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Label {
            text: TranslationManager.currentLanguage ? TranslationManager.tr("settings") : ""
            font.pixelSize: 20
            font.bold: true
            color: textColor
        }

        // --- Dil seçimi kartı ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            radius: 10
            color: surfaceColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Label {
                    text: TranslationManager.currentLanguage ? TranslationManager.tr("language") : ""
                    font.pixelSize: 14
                    color: textColor
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: langCombo
                    model: [
                        { text: "English", value: "en" },
                        { text: "Türkçe", value: "tr" }
                    ]
                    textRole: "text"
                    valueRole: "value"

                    currentIndex: {
                        var cur = TranslationManager.currentLanguage
                        for (var i = 0; i < model.length; i++) {
                            if (model[i].value === cur) return i
                        }
                        return 0
                    }

                    onActivated: {
                        TranslationManager.currentLanguage = model[currentIndex].value
                    }

                    font.pixelSize: 14

                    contentItem: Text {
                        text: parent.displayText
                        color: textColor
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                    }

                    background: Rectangle {
                        radius: 6
                        color: currentLightMode ? "#e8e8e8" : "#3a3a3a"
                    }
                }
            }
        }

        // --- Tema seçimi kartı ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            radius: 10
            color: surfaceColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Label {
                    text: TranslationManager.currentLanguage ? TranslationManager.tr("theme") : ""
                    font.pixelSize: 14
                    color: textColor
                    Layout.fillWidth: true
                }

                Button {
                    text: currentLightMode
                          ? "☽ " + (TranslationManager.currentLanguage ? TranslationManager.tr("dark") : "")
                          : "☀ " + (TranslationManager.currentLanguage ? TranslationManager.tr("light") : "")

                    onClicked: lightModeToggled()

                    contentItem: Text {
                        text: parent.text
                        color: textColor
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 32
                        radius: 6
                        color: currentLightMode ? "#e8e8e8" : "#3a3a3a"
                    }
                }
            }
        }

        // --- Alt bilgi ---
        Label {
            Layout.fillWidth: true
            text: TranslationManager.currentLanguage ? TranslationManager.tr("settings.info") : ""
            font.pixelSize: 12
            color: currentLightMode ? "#888888" : "#666666"
            wrapMode: Text.WordWrap
            Layout.topMargin: 8
        }
    }
}
