/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "holepropertiesdialog.h"

#include "../geometry/cmd/cmdholeedit.h"
#include "../geometry/hole.h"
#include "../undostack.h"
#include "ui_holepropertiesdialog.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

HolePropertiesDialog::HolePropertiesDialog(
    Hole& hole,
    UndoStack& undoStack,
    QWidget* parent) noexcept
  : QDialog(parent),
    mHole(hole),
    mUndoStack(undoStack),
    mUi(new Ui::HolePropertiesDialog) {
  mUi->setupUi(this);
  mUi->edtDiameter->setSingleStep(0.1);  // [mm]
  connect(
      mUi->buttonBox,
      &QDialogButtonBox::clicked,
      this,
      &HolePropertiesDialog::on_buttonBox_clicked);

  // load text attributes
  mUi->edtDiameter->setValue(mHole.getDiameter());
  mUi->edtPosX->setValue(mHole.getPosition().getX());
  mUi->edtPosY->setValue(mHole.getPosition().getY());
}

HolePropertiesDialog::~HolePropertiesDialog() noexcept {
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void HolePropertiesDialog::on_buttonBox_clicked(QAbstractButton* button) {
  switch (mUi->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::ApplyRole:
      applyChanges();
      break;
    case QDialogButtonBox::AcceptRole:
      if (applyChanges()) {
        accept();
      }
      break;
    case QDialogButtonBox::RejectRole:
      reject();
      break;
    default:
      Q_ASSERT(false);
      break;
  }
}

bool HolePropertiesDialog::applyChanges() noexcept {
  try {
    QScopedPointer<CmdHoleEdit> cmd(new CmdHoleEdit(mHole));
    cmd->setDiameter(mUi->edtDiameter->getValue(), false);
    cmd->setPosition(
        Point(mUi->edtPosX->getValue(), mUi->edtPosY->getValue()), false);
    mUndoStack.execCmd(cmd.take());
    return true;
  } catch (const Exception& e) {
    QMessageBox::critical(this, tr("Error"), e.getMsg());
    return false;
  }
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
