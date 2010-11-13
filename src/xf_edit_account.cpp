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

#include <KMessageBox>
#include <KToolInvocation>

#include <kopeteaccount.h>
#include <kopeteaccountmanager.h>
#include <kopetecontact.h>
#include <kopetepassword.h>
#include <kopeteuiglobal.h>

#include "xf_account.h"
#include "xf_edit_account.h"
#include "xf_protocol.h"

XfireEditAccountWidget::XfireEditAccountWidget(QWidget *parent, Kopete::Account *account) : QWidget(parent), KopeteEditAccountWidget(account)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	QWidget *widget = new QWidget(this);

	mWidget = new Ui::XfireEditAccount();
	mWidget->setupUi(widget);
	layout->addWidget(widget);

	connect(mWidget->pbtRegister, SIGNAL(clicked()), this, SLOT(slotOpenRegister()));

	// Set values if edting already-added account
	if(account)
	{
		mAccount = static_cast<XfireAccount*>(account);
		mWidget->prefUsername->setText(mAccount->accountId());
		mWidget->prefPassword->load(&mAccount->password());

		updatePreferences();
	}

	// Set correct widget tab orders
	QWidget::setTabOrder(mWidget->prefUsername, mWidget->prefPassword->mRemembered);
	QWidget::setTabOrder(mWidget->prefPassword->mRemembered, mWidget->prefPassword->mPassword);
	QWidget::setTabOrder(mWidget->prefPassword->mPassword, mWidget->prefExcludeConnect);
}

XfireEditAccountWidget::~XfireEditAccountWidget()
{
	delete mWidget;
}

void XfireEditAccountWidget::updatePreferences()
{
	mWidget->prefGameDetection->setChecked(account()->configGroup()->readEntry("GameDetection", true));
	mWidget->prefInformAccounts->setChecked(account()->configGroup()->readEntry("InformAccounts", false));
	mWidget->prefFriendsOfFriends->setChecked(account()->configGroup()->readEntry("FriendsOfFriends", true));
	mWidget->prefPeerToPeer->setChecked(account()->configGroup()->readEntry("PeerToPeer", false));

	if(account()->configGroup()->readEntry("CustomServer", false))
		mWidget->prefOverrideServer->animateClick(); // FIXME: Best way to trigger the clicked() slot?

	mWidget->prefServer->setText(account()->configGroup()->readEntry("ServerIP", "cs.xfire.com"));
	mWidget->prefPort->setValue(account()->configGroup()->readEntry("ServerPort", 25999));

	mWidget->prefUpdateVersion->setChecked(account()->configGroup()->readEntry("UpdateVersion", true));
	mWidget->prefVersion->setValue(account()->configGroup()->readEntry("ProtocolVersion", XFIRE_PROTO_VERSION));
}

bool XfireEditAccountWidget::validateData()
{
	if(!account() && mWidget->prefUsername->text().isEmpty())
	{
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Sorry, i18n("<qt>You must enter a valid Xfire username.</qt>"), i18n( "Xfire Protocol Plugin"));
		return false;
	}

	return true;
}

Kopete::Account* XfireEditAccountWidget::apply()
{
	if(!account())
		setAccount(new XfireAccount(XfireProtocol::protocol(), mWidget->prefUsername->text().trimmed()));

	XfireAccount *acc = static_cast<XfireAccount*>(account());

	account()->setExcludeConnect(mWidget->prefExcludeConnect->isChecked());
	mWidget->prefPassword->save(&acc->password());

	// Save settings
	account()->configGroup()->writeEntry("GameDetection", mWidget->prefGameDetection->isChecked());
	account()->configGroup()->writeEntry("InformAccounts", mWidget->prefInformAccounts->isChecked());
	account()->configGroup()->writeEntry("CustomServer", mWidget->prefOverrideServer->isChecked());
	account()->configGroup()->writeEntry("ServerIP", mWidget->prefServer->text());
	account()->configGroup()->writeEntry("ServerPort", mWidget->prefPort->value());
	account()->configGroup()->writeEntry("UpdateVersion", mWidget->prefUpdateVersion->isChecked());
	account()->configGroup()->writeEntry("ProtocolVersion", mWidget->prefVersion->value());
	account()->configGroup()->writeEntry("PeerToPeer", mWidget->prefPeerToPeer->isChecked());

	return account();
}

void XfireEditAccountWidget::slotOpenRegister()
{
	KToolInvocation::invokeBrowser("http://www.xfire.com/register/");
}
