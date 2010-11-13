/**
* Copyright 2010  Warren Dumortier <nwarrenfl@gmail.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef XF_EDIT_ACCOUNT_H
#define XF_EDIT_ACCOUNT_H

#include <qwidget.h>
#include <editaccountwidget.h>

#include "xf_account.h"
#include "ui_edit_account.h"

class QVBoxLayout;
namespace Kopete { class Account; }
namespace Ui { class XfireAccountPreferences; }

class XfireEditAccountWidget : public QWidget, public KopeteEditAccountWidget
{
	Q_OBJECT

public:
	// Constructor and destructor
	XfireEditAccountWidget(QWidget *parent,	Kopete::Account *account);
	~XfireEditAccountWidget();

	virtual Kopete::Account *apply();
	virtual bool validateData();

protected:
	Ui::XfireEditAccount *mWidget;

private:
	XfireAccount *mAccount;
	void updatePreferences();

private slots:
	void slotOpenRegister();
};

#endif // XF_EDIT_ACCOUNT_H
