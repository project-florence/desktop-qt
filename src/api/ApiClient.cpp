#include "ApiClient.h"
#include <QQmlContext>
#include <QUrlQuery>
#include <QUrl>
#include <QDateTime>
#include <QRandomGenerator>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
}

QString ApiClient::baseUrl() const { return m_baseUrl; }
void ApiClient::setBaseUrl(const QString &url) {
    if (m_baseUrl != url) { m_baseUrl = url; emit baseUrlChanged(); }
}

bool ApiClient::useMock() const { return m_useMock; }
void ApiClient::setUseMock(bool mock) {
    if (m_useMock != mock) { m_useMock = mock; emit useMockChanged(); }
}

void ApiClient::get(const QString &path, QJSValue callback)
{
    if (m_useMock) {
        handleMock("GET", path, QByteArray(), callback);
        return;
    }
    doRequest("GET", path, QByteArray(), callback);
}

void ApiClient::post(const QString &path, const QJsonObject &body, QJSValue callback)
{
    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);
    if (m_useMock) {
        handleMock("POST", path, data, callback);
        return;
    }
    doRequest("POST", path, data, callback);
}

void ApiClient::doRequest(const QString &method, const QString &path,
                           const QByteArray &body, QJSValue callback)
{
    QUrl url(m_baseUrl + path);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = nullptr;
    if (method == "GET") {
        reply = m_manager->get(req);
    } else if (method == "POST") {
        reply = m_manager->post(req, body);
    } else if (method == "DELETE") {
        reply = m_manager->deleteResource(req);
    }

    if (!reply) return;

    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        reply->deleteLater();

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJSValue result;

        if (doc.isObject()) {
            result = QQmlEngine::contextForObject(this)->engine()->toScriptValue(doc.object());
        } else if (doc.isArray()) {
            result = QQmlEngine::contextForObject(this)->engine()->toScriptValue(doc.array());
        } else {
            result = QQmlEngine::contextForObject(this)->engine()->toScriptValue(
                QJsonObject{{"error", reply->errorString()}});
        }

        if (callback.isCallable()) {
            QJSValueList args = { result };
            callback.call(args);
        }
    });
}

static QString extractTicker(const QString &path)
{
    // /api/v1/.../{ticker}...
    int lastSlash = path.lastIndexOf('/');
    if (lastSlash < 0) return {};
    QString after = path.mid(lastSlash + 1);
    int qmark = after.indexOf('?');
    if (qmark > 0) after = after.left(qmark);
    return after.toUpper();
}

void ApiClient::handleMock(const QString &method, const QString &path,
                            const QByteArray &body, QJSValue callback)
{
    auto engine = QQmlEngine::contextForObject(this)->engine();
    auto toVal = [&](const QJsonDocument &doc) -> QJSValue {
        if (doc.isObject()) return engine->toScriptValue(doc.object());
        if (doc.isArray()) return engine->toScriptValue(doc.array());
        return engine->toScriptValue(QJsonObject{});
    };

    QJsonDocument doc;

    // --- BIST Companies ---
    if (path == "/api/v1/bist/companies") {
        QJsonArray arr;
        QStringList tickers = {"A1CAP","A1YEN","ACP","ACSEL","ADEL","THYAO","ASELS","GARAN","KCHOL","SISE",
                               "EREGL","TUPRS","AKBNK","PETKM","BIMAS","TCELL","SAHOL","YKBNK","VESTL","TOASO"};
        QStringList names = {
            "A1 CAPİTAL YATIRIM MENKUL DEĞERLER A.Ş.",
            "A1 YENİ FİKİR TEKNOLOJİ A.Ş.",
            "ACP GAYRİMENKUL YATIRIM ORTAKLIĞI A.Ş.",
            "ACSEL A.Ş.",
            "ADEL KALEMCİLİK TİCARET VE SANAYİ A.Ş.",
            "TÜRK HAVA YOLLARI A.O.",
            "ASELSAN ELEKTRONİK SANAYİ VE TİCARET A.Ş.",
            "TÜRKİYE GARANTİ BANKASI A.Ş.",
            "KOÇ HOLDİNG A.Ş.",
            "TÜRKİYE ŞİŞE VE CAM FABRİKALARI A.Ş.",
            "EREĞLİ DEMİR VE ÇELİK FABRİKALARI T.A.Ş.",
            "TÜRKİYE PETROL RAFİNERİLERİ A.Ş.",
            "AKBANK T.A.Ş.",
            "PETKİM PETROKİMYA HOLDİNG A.Ş.",
            "BİM BİRLEŞİK MAĞAZALAR A.Ş.",
            "TURKCELL İLETİŞİM HİZMETLERİ A.Ş.",
            "HACI ÖMER SABANCI HOLDİNG A.Ş.",
            "YAPI VE KREDİ BANKASI A.Ş.",
            "VESTEL ELEKTRONİK SANAYİ VE TİCARET A.Ş.",
            "TOFAŞ TÜRK OTOMOBİL FABRİKASI A.Ş."
        };
        QStringList cities = {"İSTANBUL","İSTANBUL","İSTANBUL","ANKARA","İSTANBUL","İSTANBUL","ANKARA",
                              "İSTANBUL","İSTANBUL","İSTANBUL","ZONGULDAK","KOCAELİ","İSTANBUL","İZMİR",
                              "İSTANBUL","İSTANBUL","İSTANBUL","İSTANBUL","MANİSA","BURSA"};
        for (int i = 0; i < tickers.size(); ++i) {
            arr.append(QJsonObject{
                {"ticker", tickers[i]},
                {"name", names[i]},
                {"city", cities[i]},
                {"summary_page", QString("https://www.kap.org.tr/tr/sirket/%1").arg(tickers[i])},
                {"auditor", "BAĞIMSIZ DENETİM A.Ş."}
            });
        }
        doc = QJsonDocument(arr);
    }
    // --- BIST Tickers ---
    else if (path == "/api/v1/bist/tickers") {
        doc = QJsonDocument(QJsonArray{"A1CAP","A1YEN","ACP","ACSEL","ADEL","THYAO","ASELS","GARAN","KCHOL","SISE",
                                       "EREGL","TUPRS","AKBNK","PETKM","BIMAS","TCELL","SAHOL","YKBNK","VESTL","TOASO"});
    }
    // --- Company Info ---
    else if (path.startsWith("/api/v1/companies/info/")) {
        QString ticker = extractTicker(path);
        doc = QJsonDocument(QJsonObject{
            {"symbol", ticker + ".IS"},
            {"name", ticker == "ASELS" ? "ASELSAN" : ticker},
            {"sector", "Industrials"},
            {"industry", "Aerospace & Defense"},
            {"currency", "TRY"},
            {"exchange", "IST"},
            {"market", QJsonObject{
                {"currentPrice", 370.50},
                {"marketCap", 1687199940608},
                {"dayHigh", 375.25},
                {"dayLow", 355.00},
                {"regularMarketVolume", 54713081},
                {"fiftyTwoWeekHigh", 450.00},
                {"fiftyTwoWeekLow", 151.40}
            }},
            {"valuation", QJsonObject{
                {"trailingPE", 51.82},
                {"forwardPE", 18.10},
                {"priceToBook", 6.0},
                {"dividendYield", 0.12},
                {"payoutRatio", 0.03},
                {"targetMeanPrice", 380.66},
                {"targetHighPrice", 495.00},
                {"targetLowPrice", 249.50}
            }},
            {"financials", QJsonObject{
                {"totalRevenue", 184925585408},
                {"grossProfits", 59037466624},
                {"netIncomeToCommon", 32511891456},
                {"profitMargins", 0.18},
                {"operatingMargins", 0.21},
                {"revenueGrowth", 0.15},
                {"earningsGrowth", 0.86},
                {"returnOnEquity", 0.15},
                {"ebitda", 48649863168}
            }},
            {"balanceSheet", QJsonObject{
                {"totalCash", 31005730816},
                {"totalDebt", 53089816576},
                {"debtToEquity", 18.78},
                {"currentRatio", 1.54}
            }}
        });
    }
    // --- News ---
    else if (path.startsWith("/api/v1/news/")) {
        QString ticker = extractTicker(path);
        QJsonArray articles;
        QStringList titles = {
            ticker + " borsa performansıyla dünyanın önde gelen şirketlerini solladı",
            ticker + " yeni yatırım planlarını duyurdu",
            ticker + " hissesinde rekor hacim",
            ticker + "'ten stratejik ortaklık açıklaması",
            ticker + " bilanço beklentileri aştı"
        };
        for (int i = 0; i < titles.size(); ++i) {
            articles.append(QJsonObject{
                {"title", titles[i]},
                {"url", "https://example.com/news/" + ticker.toLower() + "/" + QString::number(i)},
                {"lang", "TURKISH"},
                {"date", QDateTime::currentDateTimeUtc().addSecs(-i * 3600).toString(Qt::ISODate)}
            });
        }
        doc = QJsonDocument(articles);
    }
    // --- Simulations: Probability ---
    else if (path.startsWith("/api/v1/simulations/probability/")) {
        QString ticker = extractTicker(path);
        doc = QJsonDocument(QJsonObject{
            {"percent", 23},
            {"ticker", ticker},
            {"time", "3mo"},
            {"target", "400"}
        });
    }
    // --- Simulations: Confidence Interval ---
    else if (path.startsWith("/api/v1/simulations/confidence-interval/")) {
        QString ticker = extractTicker(path);
        doc = QJsonDocument(QJsonObject{
            {"min", 85},
            {"max", 140},
            {"currency", "TRY"},
            {"ticker", ticker},
            {"time", "3mo"},
            {"bound", 95}
        });
    }
    // --- Economy: Gold ---
    else if (path == "/api/v1/economy/gold-prices") {
        doc = QJsonDocument(QJsonObject{
            {"gram", 2450.75},
            {"quarter", 4005.00},
            {"half", 8010.00},
            {"full", 16020.00}
        });
    }
    // --- Economy: Silver ---
    else if (path == "/api/v1/economy/silver-prices") {
        doc = QJsonDocument(QJsonObject{{"price_per_gram", 28.50}});
    }
    // --- Economy: Symbols ---
    else if (path == "/api/v1/economy/symbols") {
        doc = QJsonDocument(QJsonArray{"USD", "EUR", "GBP", "CHF", "JPY", "RUB", "CNY"});
    }
    // --- Economy: Currency ---
    else if (path == "/api/v1/economy/currency") {
        doc = QJsonDocument(QJsonArray{
            QJsonObject{{"code", "USD"}, {"name", "US Dolar"}, {"rate", 37.85}},
            QJsonObject{{"code", "EUR"}, {"name", "Euro"}, {"rate", 41.23}},
            QJsonObject{{"code", "GBP"}, {"name", "İngiliz Sterlini"}, {"rate", 48.50}},
            QJsonObject{{"code", "CHF"}, {"name", "İsviçre Frangı"}, {"rate", 42.10}},
            QJsonObject{{"code", "JPY"}, {"name", "Japon Yeni"}, {"rate", 0.26}}
        });
    }
    // --- Macro Economy ---
    else if (path == "/api/v1/macroeconomy/all") {
        doc = QJsonDocument(QJsonObject{
            {"interest_rate", 50.0},
            {"inflation", 65.0},
            {"unemployment", 9.5},
            {"current_deficit", 45000000000},
            {"gdp_growth", 3.2}
        });
    }
    // --- IPO ---
    else if (path == "/api/v1/ipos/upcoming") {
        doc = QJsonDocument(QJsonArray{
            QJsonObject{{"ticker", "XYZ"}, {"company", "XYZ Teknoloji A.Ş."}, {"date", "2026-08-15"}, {"price_min", 35}, {"price_max", 45}},
            QJsonObject{{"ticker", "ABC"}, {"company", "ABC Enerji A.Ş."}, {"date", "2026-09-01"}, {"price_min", 28}, {"price_max", 38}}
        });
    }
    // --- Scout ---
    else if (path == "/api/v1/scout/best-tickers") {
        doc = QJsonDocument(QJsonObject{
            {"query", QJsonObject{
                {"investment_budget", 50000},
                {"investment_horizon", "long_term"},
                {"risk_tolerance", "moderate"}
            }},
            {"results", QJsonArray{
                QJsonObject{
                    {"ticker", "ASELS"},
                    {"company_name", "Aselsan Elektronik Sanayi ve Ticaret A.Ş."},
                    {"sector", "defense_electronics"},
                    {"current_price", 78.45},
                    {"currency", "TRY"},
                    {"affordable_lots", 637},
                    {"analysis", QJsonObject{
                        {"overall_score", 87.3},
                        {"risk_level", "moderate"},
                        {"expected_return_range", QJsonObject{{"low", 12.5}, {"high", 34.0}, {"unit", "percent_annual"}}},
                        {"score_breakdown", QJsonObject{
                            {"volatility", 0.62}, {"growth", 0.81}, {"dividend_yield", 0.30},
                            {"valuation", 0.55}, {"liquidity", 0.90}, {"news_sentiment", 0.74}
                        }},
                        {"confidence", 0.82}
                    }}
                },
                QJsonObject{
                    {"ticker", "THYAO"},
                    {"company_name", "Türk Hava Yolları A.O."},
                    {"sector", "aviation"},
                    {"current_price", 295.50},
                    {"currency", "TRY"},
                    {"affordable_lots", 169},
                    {"analysis", QJsonObject{
                        {"overall_score", 82.1},
                        {"risk_level", "moderate"},
                        {"expected_return_range", QJsonObject{{"low", 8.5}, {"high", 28.0}, {"unit", "percent_annual"}}},
                        {"score_breakdown", QJsonObject{
                            {"volatility", 0.55}, {"growth", 0.75}, {"dividend_yield", 0.10},
                            {"valuation", 0.60}, {"liquidity", 0.85}, {"news_sentiment", 0.65}
                        }},
                        {"confidence", 0.78}
                    }}
                }
            }},
            {"meta", QJsonObject{
                {"total_candidates_evaluated", 214},
                {"algorithm_version", "the algoritma"}
            }}
        });
    }
    // --- Reports ---
    else if (path.startsWith("/api/v1/generate/report/quick/")) {
        doc = QJsonDocument(QJsonObject{
            {"ticker", extractTicker(path)},
            {"report", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. " + QString(300, '.')}
        });
    }
    else if (path.startsWith("/api/v1/generate/report/deep/")) {
        doc = QJsonDocument(QJsonObject{
            {"ticker", extractTicker(path)},
            {"report", "Deep analysis report placeholder. Lorem ipsum dolor sit amet."}
        });
    }
    // --- Auth ---
    else if (path == "/api/v1/auth/register" && method == "POST") {
        doc = QJsonDocument(QJsonObject{{"message", "Kayit olundu! Burada daha sonradan JWT alisverisi olacak."}});
    }
    else if (path == "/api/v1/auth/login" && method == "POST") {
        doc = QJsonDocument(QJsonObject{{"message", "Giris yapildi! Burada daha sonradan JWT alisverisi olacak."}});
    }
    // --- Favorites ---
    else if (path.startsWith("/api/v1/favorites/") && method == "POST") {
        doc = QJsonDocument(QJsonObject{{"message", extractTicker(path) + " favorilere eklendi!"}});
    }
    else if (path.startsWith("/api/v1/favorites/") && method == "DELETE") {
        doc = QJsonDocument(QJsonObject{{"message", extractTicker(path) + " favorilerden cikarildi!"}});
    }
    else if (path == "/api/v1/favorites" && method == "GET") {
        doc = QJsonDocument(QJsonObject{
            {"favorites", QJsonArray{"ASELS", "THYAO", "GARAN", "KCHOL", "SISE"}}
        });
    }
    // --- Price History ---
    else if (path.startsWith("/api/v1/price/history/")) {
        QString ticker = extractTicker(path);
        QJsonArray arr;
        QDateTime now = QDateTime::currentDateTimeUtc();
        double basePrice = 370.0;
        for (int i = 30; i >= 0; --i) {
            double open = basePrice + qSin(i * 0.5) * 20 + i * 1.5;
            double close = open + qSin(i * 0.7) * 10;
            double high = qMax(open, close) + qAbs(qCos(i * 0.3) * 8);
            double low = qMin(open, close) - qAbs(qSin(i * 0.4) * 6);
            arr.append(QJsonObject{
                {"ts", now.addDays(-i).toString(Qt::ISODate)},
                {"open", QString::number(open, 'f', 2).toDouble()},
                {"high", QString::number(high, 'f', 2).toDouble()},
                {"low", QString::number(low, 'f', 2).toDouble()},
                {"close", QString::number(close, 'f', 2).toDouble()},
                {"volume", QRandomGenerator::global()->bounded(20000000, 80000000)}
            });
        }
        doc = QJsonDocument(arr);
    }
    // --- Fallback ---
    else {
        doc = QJsonDocument(QJsonObject{
            {"error", "Unknown endpoint"},
            {"path", path},
            {"method", method}
        });
    }

    if (callback.isCallable()) {
        QJSValueList args = { toVal(doc) };
        callback.call(args);
    }
}
