#ifndef  QODOO_STATE_H
# define QODOO_STATE_H

# include <QObject>
# include "QOdooEnum.h"

class QTODOO_LIBRARY_EXPORT QOdooState
{
  Q_GADGET
public:
  enum State
  {
    NoState = 0,
    DraftState,
    PostedState,
    CanceledState
  };
  Q_ENUM(State)

  static const OdooEnum<State, QString, DraftState> states;
};

#endif
