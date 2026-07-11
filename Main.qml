import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

ApplicationWindow {
    id: window
    width: 960
    height: 640
    minimumWidth: 600
    minimumHeight: 400
    visible: true
    title: "Project Florence"

    // Dil değişikliğini tüm bağlamalara otomatik yansıtmak için

    // --- API Client ---
    ApiClient {
        id: api
        useMock: true
    }

    // Tema renkleri
    property bool lightMode: Application.styleHints.colorScheme === Qt.Light
    property color bgColor: lightMode ? "#f5f5f5" : "#1a1a1a"
    property color surfaceColor: lightMode ? "#ffffff" : "#262626"
    property color textColor: lightMode ? "#1a1a1a" : "#e0e0e0"
    property color accentColor: "#4CAF50"
    property color gainColor: "#4CAF50"
    property color lossColor: "#f44336"

    // Ham şirket verileri
    property var allCompanies: [
        { ticker: "ASELS", name: "Aselsan Elektronik Sanayi ve Ticaret A.Ş.", city: "Ankara" },
        { ticker: "THYAO", name: "Türk Hava Yolları A.O.", city: "İstanbul" },
        { ticker: "GARAN", name: "Türkiye Garanti Bankası A.Ş.", city: "İstanbul" },
        { ticker: "KCHOL", name: "Koç Holding A.Ş.", city: "İstanbul" },
        { ticker: "SISE", name: "Şişe Cam Sanayii A.Ş.", city: "İstanbul" },
        { ticker: "EREGL", name: "Ereğli Demir ve Çelik Fabrikaları T.A.Ş.", city: "Zonguldak" },
        { ticker: "TUPRS", name: "Türkiye Petrol Rafinerileri A.Ş.", city: "Kocaeli" },
        { ticker: "AKBNK", name: "Akbank T.A.Ş.", city: "İstanbul" },
        { ticker: "PETKM", name: "Petkim Petrokimya Holding A.Ş.", city: "İzmir" },
        { ticker: "BIMAS", name: "BİM Birleşik Mağazalar A.Ş.", city: "İstanbul" },
        { ticker: "TCELL", name: "Turkcell İletişim Hizmetleri A.Ş.", city: "İstanbul" },
        { ticker: "SAHOL", name: "Hacı Ömer Sabancı Holding A.Ş.", city: "İstanbul" },
        { ticker: "YKBNK", name: "Yapı ve Kredi Bankası A.Ş.", city: "İstanbul" },
        { ticker: "VESTL", name: "Vestel Elektronik Sanayi ve Ticaret A.Ş.", city: "Manisa" },
        { ticker: "TOASO", name: "Tofaş Türk Otomobil Fabrikası A.Ş.", city: "Bursa" }
    ]

    property var filteredCompanies: []
    property string searchText: ""

    // Detay sayfası
    property bool showDetail: false
    property var selectedCompany: ({})
    property string selectedTicker: ""
    property var companyInfo: ({})
    property bool loadingInfo: false

    function filterCompanies() {
        filteredCompanies = []
        var lowerSearch = searchText.trim().toLowerCase()

        if (lowerSearch === "") {
            for (var i = 0; i < allCompanies.length; i++)
                filteredCompanies.push(allCompanies[i])
        } else {
            for (var j = 0; j < allCompanies.length; j++) {
                var c = allCompanies[j]
                if (c.ticker.toLowerCase().indexOf(lowerSearch) !== -1
                        || c.name.toLowerCase().indexOf(lowerSearch) !== -1)
                    filteredCompanies.push(c)
            }
        }
        companyModel.clear()
        for (var k = 0; k < filteredCompanies.length; k++)
            companyModel.append(filteredCompanies[k])
    }

    function openDetail(ticker, name, city) {
        showDetail = true
        selectedTicker = ticker
        selectedCompany = { ticker: ticker, name: name, city: city }
        companyInfo = ({})

        loadingInfo = true
        api.get("/api/v1/companies/info/" + ticker, function(response) {
            companyInfo = response
            loadingInfo = false
        })
    }

    function closeDetail() {
        showDetail = false
        selectedTicker = ""
        selectedCompany = ({})
        companyInfo = ({})
    }

    function fmtPrice(val) {
        if (val === undefined || val === null) return "—"
        return Number(val).toLocaleString(Qt.locale("tr_TR"), "f", 2)
    }

    function fmtLarge(val) {
        if (val === undefined || val === null) return "—"
        var n = Number(val)
        if (n >= 1e12) return (n / 1e12).toFixed(2) + " T"
        if (n >= 1e9)  return (n / 1e9).toFixed(2) + " B"
        if (n >= 1e6)  return (n / 1e6).toFixed(2) + " M"
        return Number(n).toLocaleString(Qt.locale("tr_TR"), "f", 0)
    }

    function fmtPercent(val) {
        if (val === undefined || val === null) return "—"
        return (val * 100).toFixed(1) + "%"
    }

    color: bgColor

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ---------- ÜST KISIM ----------
        Rectangle {
            Layout.fillWidth: true
            height: 48
            color: surfaceColor

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 8

                Label {
                    text: TranslationManager.currentLanguage ? TranslationManager.tr("app.title") : ""
                    font.pixelSize: 18
                    font.bold: true
                    color: textColor
                    Layout.fillWidth: true
                }

                Button {
                    text: {
                        TranslationManager.currentLanguage;
                        return lightMode ? "☽ " + TranslationManager.tr("dark") : "☀ " + TranslationManager.tr("light");
                    }
                    onClicked: lightMode = !lightMode

                    contentItem: Text {
                        text: parent.text
                        color: textColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        implicitWidth: 90
                        implicitHeight: 30
                        radius: 6
                        color: lightMode ? "#e0e0e0" : "#444444"
                    }
                }
            }
        }

        // ---------- TAB BAR ----------
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            background: Rectangle {
                color: surfaceColor
            }

            TabButton {
                text: TranslationManager.currentLanguage ? TranslationManager.tr("companies") : ""
                width: 140
            }
            TabButton {
                text: TranslationManager.currentLanguage ? TranslationManager.tr("settings") : ""
                width: 140
            }
        }

        // ---------- İÇERİK ALANI ----------
        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // --- Sekme 0: Companies ---
            Item {
                id: companiesTab
                clip: true

                // ------ LİSTE GÖRÜNÜMÜ ------
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12
                    visible: !showDetail

                    TextField {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: TranslationManager.currentLanguage ? TranslationManager.tr("search.placeholder") : ""
                        font.pixelSize: 15
                        leftPadding: 12
                        rightPadding: 12
                        topPadding: 10
                        bottomPadding: 10

                        onTextChanged: {
                            searchText = text
                            filterCompanies()
                        }

                        background: Rectangle {
                            radius: 8
                            color: surfaceColor
                            border.width: 1
                            border.color: lightMode ? "#cccccc" : "#555555"
                        }

                        color: textColor
                    }

                    ListView {
                        id: companyList
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 6

                        model: ListModel { id: companyModel }

                        Component.onCompleted: filterCompanies()

                        delegate: Rectangle {
                            width: companyList.width
                            height: 56
                            radius: 8
                            color: surfaceColor

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 16
                                anchors.rightMargin: 16
                                spacing: 12

                                Label {
                                    text: ticker
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: accentColor
                                    Layout.preferredWidth: 80
                                }

                                Label {
                                    text: name
                                    font.pixelSize: 14
                                    color: textColor
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }

                                Label {
                                    text: city
                                    font.pixelSize: 12
                                    color: lightMode ? "#888888" : "#aaaaaa"
                                    Layout.preferredWidth: 80
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: openDetail(ticker, name, city)
                            }
                        }

                        Label {
                            anchors.centerIn: parent
                            text: TranslationManager.currentLanguage ? TranslationManager.tr("no.companies") : ""
                            font.pixelSize: 16
                            color: lightMode ? "#888888" : "#666666"
                            visible: companyModel.count === 0
                        }
                    }
                }

                // ------ DETAY GÖRÜNÜMÜ ------
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 16
                    visible: showDetail

                    Button {
                        text: TranslationManager.currentLanguage ? TranslationManager.tr("back.to.list") : ""
                        flat: true

                        contentItem: Text {
                            text: parent.text
                            color: accentColor
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            implicitHeight: 32
                            color: "transparent"
                        }

                        onClicked: closeDetail()
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        ColumnLayout {
                            width: parent.width
                            spacing: 16

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
                                        text: selectedTicker
                                        font.pixelSize: 28
                                        font.bold: true
                                        color: accentColor
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Label {
                                            text: companyInfo.name || selectedCompany.name || ""
                                            font.pixelSize: 16
                                            color: textColor
                                            Layout.fillWidth: true
                                            wrapMode: Text.WordWrap
                                        }

                                        Label {
                                            text: {
                                                var sec = companyInfo.sector || ""
                                                var ind = companyInfo.industry || ""
                                                return sec ? sec + (ind ? " · " + ind : "") : (selectedCompany.city || "")
                                            }
                                            font.pixelSize: 13
                                            color: lightMode ? "#888888" : "#aaaaaa"
                                        }
                                    }

                                    ColumnLayout {
                                        spacing: 2
                                        Layout.alignment: Qt.AlignVCenter
                                        visible: !loadingInfo && companyInfo.market !== undefined

                                        Label {
                                            text: fmtPrice(companyInfo.market ? companyInfo.market.currentPrice : undefined)
                                            font.pixelSize: 24
                                            font.bold: true
                                            color: textColor
                                            Layout.alignment: Qt.AlignRight
                                        }

                                        Label {
                                            text: companyInfo.currency || "TRY"
                                            font.pixelSize: 11
                                            color: lightMode ? "#888888" : "#aaaaaa"
                                            Layout.alignment: Qt.AlignRight
                                        }
                                    }
                                }
                            }

                            BusyIndicator {
                                Layout.alignment: Qt.AlignHCenter
                                running: loadingInfo
                                visible: loadingInfo
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 12

                                Rectangle {
                                    id: marketCard
                                    Layout.fillWidth: true
                                    radius: 10
                                    color: surfaceColor
                                    height: marketContent.implicitHeight + 20

                                    ColumnLayout {
                                        id: marketContent
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.leftMargin: 10
                                        anchors.rightMargin: 10
                                        anchors.topMargin: 10
                                        spacing: 4

                                        Label {
                                            text: TranslationManager.currentLanguage ? TranslationManager.tr("market.data") : ""
                                            font.pixelSize: 11
                                            font.bold: true
                                            color: lightMode ? "#888888" : "#aaaaaa"
                                        }

                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("price") : ""; value: fmtPrice(companyInfo.market ? companyInfo.market.currentPrice : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("market.cap") : ""; value: fmtLarge(companyInfo.market ? companyInfo.market.marketCap : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("day.range") : ""; value: fmtPrice(companyInfo.market ? companyInfo.market.dayLow : undefined) + " - " + fmtPrice(companyInfo.market ? companyInfo.market.dayHigh : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("volume") : ""; value: fmtLarge(companyInfo.market ? companyInfo.market.regularMarketVolume : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("range.52w") : ""; value: fmtPrice(companyInfo.market ? companyInfo.market.fiftyTwoWeekLow : undefined) + " - " + fmtPrice(companyInfo.market ? companyInfo.market.fiftyTwoWeekHigh : undefined) }
                                    }
                                }

                                Rectangle {
                                    id: valuationCard
                                    Layout.fillWidth: true
                                    radius: 10
                                    color: surfaceColor
                                    height: valuationContent.implicitHeight + 20

                                    ColumnLayout {
                                        id: valuationContent
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.leftMargin: 10
                                        anchors.rightMargin: 10
                                        anchors.topMargin: 10
                                        spacing: 4

                                        Label {
                                            text: TranslationManager.currentLanguage ? TranslationManager.tr("valuation") : ""
                                            font.pixelSize: 11
                                            font.bold: true
                                            color: lightMode ? "#888888" : "#aaaaaa"
                                        }

                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("pe.ttm") : ""; value: fmtPrice(companyInfo.valuation ? companyInfo.valuation.trailingPE : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("forward.pe") : ""; value: fmtPrice(companyInfo.valuation ? companyInfo.valuation.forwardPE : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("price.to.book") : ""; value: fmtPrice(companyInfo.valuation ? companyInfo.valuation.priceToBook : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("div.yield") : ""; value: fmtPercent(companyInfo.valuation ? companyInfo.valuation.dividendYield : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("target.price") : ""; value: fmtPrice(companyInfo.valuation ? companyInfo.valuation.targetMeanPrice : undefined) }
                                    }
                                }

                                Rectangle {
                                    id: financialsCard
                                    Layout.fillWidth: true
                                    radius: 10
                                    color: surfaceColor
                                    height: financialsContent.implicitHeight + 20

                                    ColumnLayout {
                                        id: financialsContent
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.leftMargin: 10
                                        anchors.rightMargin: 10
                                        anchors.topMargin: 10
                                        spacing: 4

                                        Label {
                                            text: TranslationManager.currentLanguage ? TranslationManager.tr("financials") : ""
                                            font.pixelSize: 11
                                            font.bold: true
                                            color: lightMode ? "#888888" : "#aaaaaa"
                                        }

                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("revenue") : ""; value: fmtLarge(companyInfo.financials ? companyInfo.financials.totalRevenue : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("net.income") : ""; value: fmtLarge(companyInfo.financials ? companyInfo.financials.netIncomeToCommon : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("profit.margin") : ""; value: fmtPercent(companyInfo.financials ? companyInfo.financials.profitMargins : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("revenue.growth") : ""; value: fmtPercent(companyInfo.financials ? companyInfo.financials.revenueGrowth : undefined) }
                                        InfoRow { label: TranslationManager.currentLanguage ? TranslationManager.tr("ebitda") : ""; value: fmtLarge(companyInfo.financials ? companyInfo.financials.ebitda : undefined) }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // --- Sekme 1: Settings ---
            SettingsScreen {
                surfaceColor: window.surfaceColor
                textColor: window.textColor
                accentColor: window.accentColor
                currentLightMode: window.lightMode
                onLightModeToggled: window.lightMode = !window.lightMode
            }
        }
    }

    // ----- InfoRow bileşeni -----
    component InfoRow: RowLayout {
        property string label: ""
        property string value: "—"

        Label {
            text: parent.label
            font.pixelSize: 12
            color: lightMode ? "#888888" : "#aaaaaa"
        }

        Item { Layout.fillWidth: true }

        Label {
            text: parent.value
            font.pixelSize: 13
            font.bold: true
            color: textColor
        }
    }
}
