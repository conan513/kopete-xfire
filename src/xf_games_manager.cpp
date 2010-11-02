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

#include <QtXml>
#include <QDomDocument>
#include <kmessagebox.h>

#include "ui_edit_games.h"
#include "xf_games_manager.h"
#include "xf_account.h"
#include "xf_games_list.h"

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
	QList<QString> configured = mAccount->mGamesList->configuredGames();

	QList<QString> list = mAccount->mGamesList->getGamesList();
	for(int i = 0; i < list.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget *)0, QStringList(QString(list.at(i))));

		if(configured.contains(list.at(i)) == true)
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
	if(pStatus == 0)
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
	MyThread *newThread = new MyThread();
	newThread->start();
}

void XfireGamesManager::slotConfiguringGameChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(previous);

	if(current->parent() == configuredItem)
	{
		mDialog->enableCheckBox->setChecked(true);

		QDomNode game = mAccount->mGamesList->getConfiguredGame(current->text(0));
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

	QDomNode game = mAccount->mGamesList->getConfiguredGame(item->text(0));
	QDomNode command = game.firstChild();

	mDialog->launchRequester->setText(command.firstChildElement("launch").text());
	mDialog->detectionRequester->setText(command.firstChildElement("detect").text());
}

void XfireGamesManager::slotApplyGameConfiguration()
{
	QTreeWidgetItem *item = mDialog->treeWidget->currentItem();

	if(mDialog->enableCheckBox->isChecked())
	{
		if(mAccount->mGamesList->gameIsConfigured(item->text(0)) == false)
		{
			QDomElement root = mAccount->mGamesList->mConfiguredGamesList->firstChildElement("game_config");
			QDomElement game = mAccount->mGamesList->mConfiguredGamesList->createElement("game");
			QDomElement command = mAccount->mGamesList->mConfiguredGamesList->createElement("command");
			QDomElement launch = mAccount->mGamesList->mConfiguredGamesList->createElement("launch");
			QDomElement detect = mAccount->mGamesList->mConfiguredGamesList->createElement("detect");

			QDomText launchText = mAccount->mGamesList->mConfiguredGamesList->createTextNode(mDialog->launchRequester->text());
			QDomText detectText = mAccount->mGamesList->mConfiguredGamesList->createTextNode(mDialog->detectionRequester->text());

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
			mAccount->mGamesList->updateConfiguredGame(item->text(0), mDialog->launchRequester->text(), mDialog->detectionRequester->text());
	}
	else
	{
		QDomElement game = mAccount->mGamesList->getConfiguredGame(item->text(0));
		game.parentNode().removeChild(game);

		mAccount->mGamesList->saveConfiguredGamesList();
		notConfiguredItem->addChild(configuredItem->takeChild(configuredItem->indexOfChild(item)));
		mDialog->treeWidget->sortItems(0, Qt::AscendingOrder);
	}

	mAccount->mGamesList->saveConfiguredGamesList();
}

void MyThread::run()
{
	/* // FIXME: This is some test, should be moved elsewhere
	QDomNodeList domGames = mAccount->mGamesList->mGamesList->elementsByTagName("game");
	for (int i = 0; i < domGames.count(); i++)
	{
		quint32 gameID = domGames.at(i).attributes().namedItem("id").nodeValue().toInt();

		QDomNode detectionNode = domGames.at(i).firstChild();
		QString launcherExe = detectionNode.firstChildElement("launch_exe").text();
		QString detectExe = detectionNode.firstChildElement("detect_exe").text();

		qDebug() << domGames.at(i).attributes().namedItem("name").nodeValue();

		QDirIterator crawler(QDir::homePath(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

		if (launcherExe == "")
			continue;

		// Search the launcher executable
		QStringList launcherMatches;
		while (crawler.hasNext())
		{
			crawler.next();
			if (crawler.fileName().contains(launcherExe))
				launcherMatches.append(crawler.filePath());
		}

		if (launcherMatches.size() == 0)
			continue;

		if (detectExe == "")
		{
			qDebug() << "Game found, with detect and launcher as same executable:";

			for (int i = 0; i < launcherMatches.size(); i++)
				qDebug() << launcherMatches.at(i);

			continue;

		}

		// Search the detection executable
		QDirIterator crawler2(QDir::homePath(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
		// Search the launcher executable
		QStringList detectionMatches;
		while (crawler2.hasNext())
		{
			crawler2.next();
			if (crawler2.fileName().contains(detectExe))
				detectionMatches.append(crawler2.filePath());
		}

		if(detectionMatches.size() == 0)
		{
			qDebug() << "There was a candidate, but it failed!";
			continue;
		}

		qDebug() << "Game found, with different detect and launcher executable:";

		qDebug() << "Launcher matches:";
		for(int i = 0; i < launcherMatches.size(); i++)
			qDebug() << launcherMatches.at(i);

		qDebug() << "Detection matches:";
		for(int i = 0; i < detectionMatches.size(); i++)
			qDebug() << detectionMatches.at(i);
	} */
}

#endif // XF_GAMES_MANAGER_CPP
