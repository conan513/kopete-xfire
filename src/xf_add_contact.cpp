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

#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>

#include <kdebug.h>
#include <kmessagebox.h>

#include <kopeteaccount.h>
#include <kopetemetacontact.h>
#include <kopeteuiglobal.h>

#include "xf_account.h"
#include "xf_add_contact.h"
#include "xf_protocol.h"
#include "xf_server.h"
#include "ui_add_contact.h"

XfireAddContactPage::XfireAddContactPage(Kopete::Account *account, QWidget *parent): AddContactPage(parent), mAccount(account)
{
    mWidget = new Ui::XfireAddContact();
    mWidget->setupUi(this);
    mWidget->prefUsername->setFocus();
}

XfireAddContactPage::~XfireAddContactPage()
{
    delete mWidget;
}

bool XfireAddContactPage::apply(Kopete::Account *account, Kopete::MetaContact *metaContact)
{
    QString contactId = mWidget->prefUsername->text().trimmed();
    QString message = mWidget->prefMessage->toPlainText();

    XfireAccount *acc = static_cast<XfireAccount *>(account);
    acc->server()->sendFriendInvitation(contactId, message);
    return account->addContact(contactId, metaContact, Kopete::Account::Temporary);
}

bool XfireAddContactPage::validateData ()
{
    if (!mAccount->isConnected())
    {
        KMessageBox::sorry(this, i18n("You need to be connected in order to add contacts."), i18n("Not connected"), QFlags<KMessageBox::Option>());
        return false;
    }

    if (mWidget->prefUsername->text().isEmpty())
    {
        KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(), KMessageBox::Sorry, i18n("<qt>You must enter a valid Xfire username.</qt>"), i18n("Xfire protocol plugin"));
        return false;
    }

    return true;
}
