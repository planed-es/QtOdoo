#include "QOdooState.h"

const ODOO_ENUM_BEGIN(QOdooState::states, QOdooState::State, QString, QOdooState::DraftState)
  {QOdooState::PostedState,   "posted"},
  {QOdooState::DraftState,    "draft"},
  {QOdooState::CanceledState, "cancel"}
ODOO_ENUM_END()
