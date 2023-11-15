#ifndef  QODOO_PRODUCT_TEMPLATE_H
# define QODOO_PRODUCT_TEMPLATE_H

# include "QOdooProduct.h"

class QTODOO_LIBRARY_EXPORT QOdooProductTemplate : public QOdooProduct
{
public:
  const char* odooTypename() const override { return "product.template"; }

  QOdooProductTemplate(QObject* parent = nullptr) : QOdooProduct(parent)
  {
  }
};

#endif
