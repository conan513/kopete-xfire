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

#ifndef XF_GAMES_MANAGER_H
#define XF_GAMES_MANAGER_H

#include <QDialog>
#include <QThread>

#include "ui_edit_games.h"

class XfireAccount;

class XfireGamesManager : public QDialog
{
    Q_OBJECT;

public:
    XfireGamesManager(XfireAccount *p_account);
    ~XfireGamesManager();

private:
    XfireAccount *m_account;
    Ui::Dialog *m_dialog;

    QTreeWidgetItem *m_configuredItem;
    QTreeWidgetItem *m_notConfiguredItem;


public slots:
    void slotUpdate();

private slots:
    void slotGameDetectionStatusChanged(int p_status);
    void slotConfiguringGameChanged(QTreeWidgetItem *p_current, QTreeWidgetItem *p_previous);
    void slotApplyGameConfiguration();
    void slotResetGameConfiguration();
    void slotDetectGames();
    void slotDetectionPathChanged(const KUrl &p_url);
};

#endif // XF_GAMES_MANAGER_H
