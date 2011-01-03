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
#include <KDebug>

#include <kmessagebox.h>

#include "ui_edit_games.h"
#include "xf_account.h"
#include "xf_games_list.h"
#include "xf_games_manager.h"

XfireGamesManager::XfireGamesManager ( XfireAccount *p_account ) : QDialog()
{
    m_account = p_account;

    m_dialog = new Ui::Dialog();
    m_dialog->setupUi ( this );

    m_configuredItem = new QTreeWidgetItem ( ( QTreeWidget * ) 0, QStringList ( QString ( "Configured games" ) ) );
    m_notConfiguredItem = new QTreeWidgetItem ( ( QTreeWidget * ) 0, QStringList ( QString ( "Not configured games" ) ) );

    m_dialog->treeWidget->insertTopLevelItem ( 0, m_configuredItem );
    m_dialog->treeWidget->insertTopLevelItem ( 1, m_notConfiguredItem );

    // Signals
    connect ( m_dialog->btnConfigureAutomaitcally, SIGNAL ( clicked() ), this, SLOT ( slotDetectGames() ) );
    connect ( m_dialog->enableCheckBox, SIGNAL ( stateChanged ( int ) ), this, SLOT ( slotGameDetectionStatusChanged ( int ) ) );
    connect ( m_dialog->treeWidget, SIGNAL ( currentItemChanged ( QTreeWidgetItem *,QTreeWidgetItem * ) ), this, SLOT ( slotConfiguringGameChanged ( QTreeWidgetItem*,QTreeWidgetItem* ) ) );
    connect ( m_dialog->buttonBox->button ( QDialogButtonBox::Reset ), SIGNAL ( clicked() ), this, SLOT ( slotResetGameConfiguration() ) );
    connect ( m_dialog->buttonBox->button ( QDialogButtonBox::Apply ), SIGNAL ( clicked() ), this, SLOT ( slotApplyGameConfiguration() ) );
    connect ( m_dialog->detectionRequester, SIGNAL ( urlSelected(const KUrl &) ), this, SLOT ( slotDetectionPathChanged(const KUrl &) ));
}

XfireGamesManager::~XfireGamesManager()
{
}

void XfireGamesManager::slotDetectionPathChanged(const KUrl &p_url)
{
    m_dialog->launchRequester->setUrl(p_url);
}

void XfireGamesManager::slotGameDetectionStatusChanged ( int pStatus )
{
    if ( pStatus == 0 )
    {
        m_dialog->label->setEnabled ( false );
        m_dialog->label_2->setEnabled ( false );
        m_dialog->launchRequester->setEnabled ( false );
        m_dialog->detectionRequester->setEnabled ( false );
    }
    else
    {
        m_dialog->label->setEnabled ( true );
        m_dialog->label_2->setEnabled ( true );
        m_dialog->launchRequester->setEnabled ( true );
        m_dialog->detectionRequester->setEnabled ( true );
    }
}

void XfireGamesManager::slotDetectGames()
{
    // FIXME: Not implemented yet
}

void XfireGamesManager::slotConfiguringGameChanged ( QTreeWidgetItem *p_current, QTreeWidgetItem *p_previous )
{
    Q_UNUSED ( p_previous );

    if ( p_current->parent() == m_configuredItem )
    {
        m_dialog->enableCheckBox->setChecked ( true );

        QDomNode game = m_account->m_gamesList->getConfiguredGame ( p_current->text ( 0 ) );
        QDomNode command = game.firstChild();

        m_dialog->launchRequester->setText ( command.firstChildElement ( "launch" ).text() );
        m_dialog->detectionRequester->setText ( command.firstChildElement ( "detect" ).text() );
    }
    else
    {
        m_dialog->enableCheckBox->setChecked ( false );

        m_dialog->detectionRequester->clear();
        m_dialog->launchRequester->clear();
    }
}

void XfireGamesManager::slotResetGameConfiguration()
{
    QTreeWidgetItem *item = m_dialog->treeWidget->currentItem();

    QDomNode game = m_account->m_gamesList->getConfiguredGame ( item->text ( 0 ) );
    QDomNode command = game.firstChild();

    m_dialog->launchRequester->setText ( command.firstChildElement ( "launch" ).text() );
    m_dialog->detectionRequester->setText ( command.firstChildElement ( "detect" ).text() );
}

void XfireGamesManager::slotApplyGameConfiguration()
{
    QTreeWidgetItem *item = m_dialog->treeWidget->currentItem();

    if ( m_dialog->enableCheckBox->isChecked() )
    {
        if ( m_account->m_gamesList->gameIsConfigured ( item->text ( 0 ) ) == false )
        {
            QDomElement root = m_account->m_gamesList->mConfiguredGamesList->firstChildElement ( "game_config" );
            QDomElement game = m_account->m_gamesList->mConfiguredGamesList->createElement ( "game" );
            QDomElement command = m_account->m_gamesList->mConfiguredGamesList->createElement ( "command" );
            QDomElement launch = m_account->m_gamesList->mConfiguredGamesList->createElement ( "launch" );
            QDomElement detect = m_account->m_gamesList->mConfiguredGamesList->createElement ( "detect" );

            QDomText launchText = m_account->m_gamesList->mConfiguredGamesList->createTextNode ( m_dialog->launchRequester->text() );
            QDomText detectText = m_account->m_gamesList->mConfiguredGamesList->createTextNode ( m_dialog->detectionRequester->text() );

            root.appendChild ( game );
            game.setAttribute ( "name", item->text ( 0 ) );

            game.appendChild ( command );
            launch.appendChild ( launchText );
            command.appendChild ( launch );
            detect.appendChild ( detectText );
            command.appendChild ( detect );

            m_configuredItem->addChild ( m_notConfiguredItem->takeChild ( m_notConfiguredItem->indexOfChild ( item ) ) );
            m_dialog->treeWidget->sortItems ( 0, Qt::AscendingOrder );
        }
        else
            m_account->m_gamesList->updateConfiguredGame ( item->text ( 0 ), m_dialog->launchRequester->text(), m_dialog->detectionRequester->text() );
    }
    else
    {
        QDomElement game = m_account->m_gamesList->getConfiguredGame ( item->text ( 0 ) );
        game.parentNode().removeChild ( game );

        m_account->m_gamesList->saveConfiguredGamesList();
        m_notConfiguredItem->addChild ( m_configuredItem->takeChild ( m_configuredItem->indexOfChild ( item ) ) );
        m_dialog->treeWidget->sortItems ( 0, Qt::AscendingOrder );
    }

    m_account->m_gamesList->saveConfiguredGamesList();
}

void XfireGamesManager::slotUpdate()
{
    // Remove items
    while ( int i = m_configuredItem->childCount() )
        delete m_configuredItem->takeChild ( i - 1 );

    while ( int i = m_notConfiguredItem->childCount() )
        delete m_notConfiguredItem->takeChild ( i - 1 );

    // Get configured games
    QList<QString> configured = m_account->m_gamesList->configuredGames();
    QList<QString> list = m_account->m_gamesList->getGamesList();

    for ( int i = 0; i < list.size(); i++ )
    {
        QTreeWidgetItem *item = new QTreeWidgetItem ( ( QTreeWidget * ) 0, QStringList ( QString ( list.at ( i ) ) ) );
        if ( configured.contains ( list.at ( i ) ) == true )
            m_configuredItem->addChild ( item );
        else
            m_notConfiguredItem->addChild ( item );
    }

    m_dialog->treeWidget->sortItems ( 0, Qt::AscendingOrder );
    m_configuredItem->setExpanded ( true );
}

#endif // XF_GAMES_MANAGER_CPP
