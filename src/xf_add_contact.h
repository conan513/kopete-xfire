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

#ifndef XF_ADD_CONTACT_H
#define XF_ADD_CONTACT_H

#include <addcontactpage.h>

class XfireAccount;
namespace Ui { class XfireAddContact; }

class XfireAddContactPage : public AddContactPage
{
    Q_OBJECT
public:
    XfireAddContactPage(Kopete::Account *account, QWidget *parent = 0);
    ~XfireAddContactPage();

    virtual bool apply(Kopete::Account *a, Kopete::MetaContact *m);
    virtual bool validateData();

protected:
    Ui::XfireAddContact *mWidget;
    Kopete::Account * mAccount;
};

#endif // XF_ADD_CONTACT_H
