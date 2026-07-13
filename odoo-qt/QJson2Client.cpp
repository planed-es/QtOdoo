#include "QJson2Client.h"
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QPromise>
#include <memory>

QJson2Client::QJson2Client(QObject* parent) : QObject(parent)
{
}

QUrl QJson2Client::urlFor(const QString& model, const QString& method) const
{
  QUrl result = endpoint;

  result.setPath("/json/2/" + model + '/' + method);
  return result;
}

void QJson2Client::decorateRequest(QNetworkRequest& request) const
{
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
  request.setHeader(QNetworkRequest::UserAgentHeader, "QtOdoo");
  request.setRawHeader("Authorization", "bearer " + apiKey);
  if (!database.isEmpty())
    request.setRawHeader("X-Odoo-Database", database);
}

QFuture<QJsonValue> QJson2Client::call(const QString& model, const QString& method, QJsonObject args)
{
  return post(urlFor(model, method), args);
}

QFuture<QJsonValue> QJson2Client::call(const QString& model, const QString& method, const QVariantList& ids, QJsonObject args)
{
  args["ids"] = QJsonArray::fromVariantList(ids);
  return post(urlFor(model, method), args);
}

// JSON-2 responses aren't always a JSON object or array: search_count
// returns a bare integer, write()/unlink() return a bare boolean. Strict
// QJsonDocument::fromJson() can't hold a scalar at the document root, so
// scalar bodies get parsed by wrapping them in an array first.
static QJsonValue parseJsonValue(const QByteArray& raw)
{
  QJsonParseError error;
  QJsonDocument document = QJsonDocument::fromJson(raw, &error);

  if (error.error == QJsonParseError::NoError)
    return document.isArray() ? QJsonValue(document.array()) : QJsonValue(document.object());

  document = QJsonDocument::fromJson('[' + raw + ']', &error);
  if (error.error == QJsonParseError::NoError && document.array().size() == 1)
    return document.array().first();
  return QJsonValue(QJsonValue::Undefined);
}

static QOdooException exceptionFrom(int status, const QJsonObject& error, const QString& transportError)
{
  return QOdooException(
    status,
    error.value("name").toString(),
    error.value("message").toString(transportError),
    error.value("arguments").toArray()
  );
}

QFuture<QJsonValue> QJson2Client::post(const QUrl& url, const QJsonObject& body)
{
  auto promise = std::make_shared<QPromise<QJsonValue>>();
  QNetworkRequest request(url);
  QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
  QNetworkReply* reply;

  decorateRequest(request);
  promise->start();
  reply = network.post(request, payload);
  connect(reply, &QNetworkReply::finished, this, [reply, promise]()
  {
    QNetworkReply::NetworkError networkError = reply->error();
    int                         status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray                  responseBody = reply->readAll();
    QJsonValue                  responseValue = parseJsonValue(responseBody);
    QString                     transportError = reply->errorString();

    reply->deleteLater();
    if (networkError == QNetworkReply::NoError && status >= 200 && status < 300)
      promise->addResult(responseValue);
    else
      promise->setException(exceptionFrom(responseValue.toObject(), transportError));
    promise->finish();
  });
  return promise->future();
}

QFuture<QByteArray> QJson2Client::version()
{
  QUrl url(endpoint);

  url.setPath("/web/version");
  return post(url, QJsonObject()).then([](QJsonValue response) -> QByteArray
  {
    return response.toObject().value("version").toString().toUtf8();
  });
}
