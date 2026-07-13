#ifndef  QODOO_EXCEPTION_H
# define QODOO_EXCEPTION_H

# include <QException>
# include <QMetaType>
# include <QString>
# include <QJsonArray>
# include "QOdoo_global.h"

class QTODOO_LIBRARY_EXPORT QOdooException : public QException
{
public:
  QOdooException() = default;
  QOdooException(int httpStatus, QString name, QString message, QJsonArray arguments = QJsonArray())
    : _httpStatus(httpStatus), _name(std::move(name)), _message(std::move(message)), _arguments(std::move(arguments))
  {
    _whatMessage = this->message.toUtf8();
  }

  void              raise() const override { throw *this; }
  QOdooException*   clone() const override { return new QOdooException(*this); }
  const char*       what() const noexcept override { return _whatMessage.constData(); }
  const QString&    name() const { return _name; }
  const QString&    message() const { return _message; }
  const QJsonArray& arguments() const { return _arguments; }

private:
  int        _httpStatus = 0;
  QString    _name;
  QString    _message;
  QJsonArray _arguments;
  QByteArray _whatMessage;
};

Q_DECLARE_METATYPE(QOdooException)

#endif
