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

#ifndef XF_GAMES_MANAGER_CPP
#define XF_GAMES_MANAGER_CPP

#include <QDomDocument>
#include <QtXml>

#include <kmessagebox.h>

#include "ui_edit_games.h"
#include "xf_account.h"
#include "xf_games_list.h"
#include "xf_games_manager.h"

XfireGamesManager::XfireGamesManager(XfireAccount *pAccount) : QDialog()
{
    mAccount = pAccount;

    mDialog = new Ui::Dialog();
    mDialog->setupUi(this);

    configuredItem = new QTreeWidgetItem((QTreeWidget *)0, QStringList(QString("Configured games")));
    notConfiguredItem = new QTreeWidgetItem((QTreeWidget *)0, QStringList(QString("Not configured games")));

    mDialog->treeWidget->insertTopLevelItem(0, configuredItem);
    mDialog->treeWidget->insertTopLevelItem(1, notConfiguredItem);

    // Get configured games
    QList<QString> configured = mAccount->m_gamesList->configuredGames();
    QList<QString> list = mAccount->m_gamesList->getGamesList();

    for (int i = 0; i < list.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget *)0, QStringList(QString(list.at(i))));

        if (configured.contains(list.at(i)) == true)
            configuredItem->addChild(item);
        else
            notConfiguredItem->addChild(item);
    }

    mDialog->treeWidget->sortItems(0, Qt::AscendingOrder);
    configuredItem->setExpanded(true);

    // Signals
    connect(mDialog->btnConfigureAutomaitcally, SIGNAL(clicked()), this, SLOT(slotDetectGames()));
    connect(mDialog->enableCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotGameDetectionStatusChanged(int)));
    connect(mDialog->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)), this, SLOT(slotConfiguringGameChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(mDialog->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(slotResetGameConfiguration()));
    connect(mDialog->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(slotApplyGameConfiguration()));
}

XfireGamesManager::~XfireGamesManager()
{
}

void XfireGamesManager::slotGameDetectionStatusChanged(int pStatus)
{
    if (pStatus == 0)
    {
        mDialog->label->setEnabled(false);
        mDialog->label_2->setEnabled(false);
        mDialog->launchRequester->setEnabled(false);
        mDialog->detectionRequester->setEnabled(false);
    }
    else
    {
        mDialog->label->setEnabled(true);
        mDialog->label_2->setEnabled(true);
        mDialog->launchRequester->setEnabled(true);
        mDialog->detectionRequester->setEnabled(true);
    }
}

void XfireGamesManager::slotDetectGames()
{
    // FIXME: Not implemented yet
}

void XfireGamesManager::slotConfiguringGameChanged(QTreeWidgetItem *p_current, QTreeWidgetItem *p_previous)
{
    Q_UNUSED(p_previous);

    if (p_current->parent() == configuredItem)
    {
        mDialog->enableCheckBox->setChecked(true);

        QDomNode game = mAccount->m_gamesList->getConfiguredGame(p_current->text(0));
        QDomNode command = game.firstChild();

        mDialog->launchRequester->setText(command.firstChildElement("launch").text());
        mDialog->detectionRequester->setText(command.firstChildElement("detect").text());
    }
    else
    {
        mDialog->enableCheckBox->setChecked(false);

        mDialog->detectionRequester->clear();
        mDialog->launchRequester->clear();
    }
}

void XfireGamesManager::slotResetGameConfiguration()
{
    QTreeWidgetItem *item = mDialog->treeWidget->currentItem();

    QDomNode game = mAccount->m_gamesList->getConfiguredGame(item->text(0));
    QDomNode command = game.firstChild();

    mDialog->launchRequester->setText(command.firstChildElement("launch").text());
    mDialog->detectionRequester->setText(command.firstChildElement("detect").text());
}

void XfireGamesManager::slotApplyGameConfiguration()
{
    QTreeWidgetItem *item = mDialog->treeWidget->currentItem();

    if (mDialog->enableCheckBox->isChecked())
    {
        if (mAccount->m_gamesList->gameIsConfigured(item->text(0)) == false)
        {
            QDomElement root = mAccount->m_gamesList->mConfiguredGamesList->firstChildElement("game_config");
            QDomElement game = mAccount->m_gamesList->mConfiguredGamesList->createElement("game");
            QDomElement command = mAccount->m_gamesList->mConfiguredGamesList->createElement("command");
            QDomElement launch = mAccount->m_gamesList->mConfiguredGamesList->createElement("launch");
            QDomElement detect = mAccount->m_gamesList->mConfiguredGamesList->createElement("detect");

            QDomText launchText = mAccount->m_gamesList->mConfiguredGamesList->createTextNode(mDialog->launchRequester->text());
            QDomText detectText = mAccount->m_gamesList->mConfiguredGamesList->createTextNode(mDialog->detectionRequester->text());

            root.appendChild(game);
            game.setAttribute("name", item->text(0));

            game.appendChild(command);
            launch.appendChild(launchText);
            command.appendChild(launch);
            detect.appendChild(detectText);
            command.appendChild(detect);

            configuredItem->addChild(notConfiguredItem->takeChild(notConfiguredItem->indexOfChild(item)));
            mDialog->treeWidget->sortItems(0, Qt::AscendingOrder);
        }
        else
            mAccount->m_gamesList->updateConfiguredGame(item->text(0), mDialog->launchRequester->text(), mDialog->detectionRequester->text());
    }
    else
    {
        QDomElement game = mAccount->m_gamesList->getConfiguredGame(item->text(0));
        game.parentNode().removeChild(game);

        mAccount->m_gamesList->saveConfiguredGamesList();
        notConfiguredItem->addChild(configuredItem->takeChild(configuredItem->indexOfChild(item)));
        mDialog->treeWidget->sortItems(0, Qt::AscendingOrder);
    }

    mAccount->m_gamesList->saveConfiguredGamesList();
}

#endif // XF_GAMES_MANAGER_CPP
