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

#include <QObject>
#include <QtGui/QDialog>
#include <QThread>

#include "ui_edit_games.h"

class XfireAccount;

class XfireGamesManager : public QDialog
{
    Q_OBJECT;

public:
    XfireGamesManager(XfireAccount *pAccount);
    ~XfireGamesManager();

private:
    XfireAccount *mAccount;
    Ui::Dialog *mDialog;

    QTreeWidgetItem *configuredItem;
    QTreeWidgetItem *notConfiguredItem;

private slots:
    void slotGameDetectionStatusChanged(int pStatus);
    void slotConfiguringGameChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void slotApplyGameConfiguration();
    void slotResetGameConfiguration();
    void slotDetectGames();
};

class MyThread : public QThread
{
public:
    void run();
};

#endif // XF_GAMES_MANAGER_H
