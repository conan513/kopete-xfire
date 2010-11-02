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

#include <QFile>
#include <QtNetwork>
#include <QtXml>

#include <KDebug>
#include <KStandardDirs>
#include <KNotification>

#include "kopeteinfoevent.h"

#include "xf_games_list.h"

XfireGamesList::XfireGamesList()
{
	mManager = new QNetworkAccessManager(this);
	connect(mManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(slotReceivedGamesList(QNetworkReply *)));

	// Download versions file and initialize both lists
	mManager->get(QNetworkRequest(QUrl("http://gfireproject.org/files/gfire_version.xml")));

	initGamesList();
	initConfiguredGamesList();
}

XfireGamesList::~XfireGamesList()
{
}

void
XfireGamesList::initConfiguredGamesList()
{
	QByteArray content;
	QFile file(KStandardDirs::locateLocal("appdata", "xfire/xfire_games_config.xml"));

	if(file.exists() == false)
	{
		file.open(QIODevice::ReadWrite);
		QXmlStreamWriter stream(&file);

		stream.setAutoFormatting(true);
		stream.writeStartDocument();

		stream.writeStartElement("game_config");
		stream.writeAttribute("version", "2");

		stream.writeEndDocument();
		file.close();
	}

	if(file.open(QIODevice::ReadWrite))
	{
		content = file.readAll();
		file.close();
	}

	mConfiguredGamesList = new QDomDocument();
	mConfiguredGamesList->setContent(content);
}

void
XfireGamesList::initGamesList()
{
	QFile file(KStandardDirs::locateLocal("appdata", "xfire/xfire_games.xml"));

	if(file.exists() == false)
		mLocalGamesListVersion = -1;
	else
	{
		if(file.open(QIODevice::ReadWrite))
		{
			mGamesList = new QDomDocument();
			mGamesList->setContent(file.readAll());
			mLocalGamesListVersion = mGamesList->firstChildElement("games").attributes().namedItem("version").nodeValue().toInt();
			
			file.close();
		}
	}
}

void
XfireGamesList::slotReceivedGamesList(QNetworkReply *pReply)
{
	QByteArray reply = pReply->readAll();

	if(pReply->url().toString().contains("gfire_version.xml"))
	{
		QDomDocument *version = new QDomDocument();
		version->setContent(reply);

		mRemoteGamesListVersion = version->firstChildElement("gfire").attributes().namedItem("games_list_version").nodeValue().toInt();

		if(mRemoteGamesListVersion != mLocalGamesListVersion)
			mManager->get(QNetworkRequest(QUrl("http://gfireproject.org/files/gfire_games.xml"))); // Download games list file
		else
			emit gamesListReady();
	}
	else if(pReply->url().toString().contains("gfire_games.xml"))
	{
		kDebug() << "Received new games list.";

		emit slotGamesListUpdated();
		
		mGamesList = new QDomDocument();
		mGamesList->setContent(reply);
		saveGamesList();
		
		emit gamesListReady();
	}
}

QString
XfireGamesList::getGameNameFromID(quint32 pGameID)
{
	QDomNodeList domGames = mGamesList->elementsByTagName("game");
	for(int i = 0; i < domGames.count(); i++)
	{
		quint32 gameID = domGames.at(i).attributes().namedItem("id").nodeValue().toInt();
		if(gameID == pGameID)
			return domGames.at(i).attributes().namedItem("name").nodeValue();
	}

	return 0L;
}

quint32
XfireGamesList::getGameIDFromName(QString pName)
{
	QDomNodeList domGames = mGamesList->elementsByTagName("game");
	for(int i = 0; i < domGames.count(); i++)
	{
		QString gameName = domGames.at(i).attributes().namedItem("name").nodeValue();
		if(gameName == pName)
			return domGames.at(i).attributes().namedItem("id").nodeValue().toInt();
	}

	return -1;
}

QDomElement
XfireGamesList::getConfiguredGame(QString pName)
{
	QDomElement ret;

	QDomNodeList games = mConfiguredGamesList->elementsByTagName("game");
	for (int i = 0; i < games.count(); i++)
	{
		if(games.at(i).attributes().namedItem("name").nodeValue() == pName)
			ret = games.at(i).toElement();
	}

	return ret;
}

QList<QString>
XfireGamesList::getGamesList()
{
	QList<QString> list;

	QDomNodeList domGames = mGamesList->elementsByTagName("game");
	for(int i = 0; i < domGames.count(); i++)
		list.append(domGames.at(i).attributes().namedItem("name").nodeValue());

	return list;
}

QList<QString>
XfireGamesList::configuredGames()
{
	QList<QString> ret;

	QDomNodeList games = mConfiguredGamesList->elementsByTagName("game");
	for (int i = 0; i < games.count(); i++)
		ret.append(games.at(i).attributes().namedItem("name").nodeValue());

	return ret;
}

void
XfireGamesList::removeConfiguredGame(QString pName)
{
	QDomNode game = getConfiguredGame(pName);
	qDebug() << game.toDocument().toString();
	game.parentNode().removeChild(game);
}

void
XfireGamesList::updateConfiguredGame(QString pName, QString pLaunchExe, QString pDetectExe)
{
	QDomElement game = getConfiguredGame(pName);
	QDomElement command = game.firstChildElement("command");
	QDomElement launch = command.firstChildElement("launch");
	QDomElement detect = command.firstChildElement("detect");

	launch.firstChild().toText().setNodeValue(pLaunchExe);
	detect.firstChild().toText().setNodeValue(pDetectExe);
}

void
XfireGamesList::saveConfiguredGamesList()
{
	QFile file(KStandardDirs::locateLocal("appdata", "xfire/xfire_games_config.xml"));
	file.open(QFile::WriteOnly | QFile::Truncate);

	QTextStream stream(&file);
	stream << mConfiguredGamesList->toString();
	file.close();
}

void
XfireGamesList::saveGamesList()
{
	QFile file(KStandardDirs::locateLocal("appdata", "xfire/xfire_games.xml"));
	file.open(QFile::WriteOnly | QFile::Truncate);

	QTextStream stream(&file);
	stream << mGamesList->toString();
	file.close();
}

bool
XfireGamesList::gameIsConfigured(QString pName)
{
	bool ret = false;

	QDomNodeList games = mConfiguredGamesList->elementsByTagName("game");
	for (int i = 0; i < games.count(); i++)
	{
		if(games.at(i).attributes().namedItem("name").nodeValue() == pName)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void
XfireGamesList::slotGamesListUpdated()
{
	Kopete::InfoEvent *event = new Kopete::InfoEvent();
	event->setTitle("Xfire games list update");
	event->setText("The Xfire games list has been updated to the latest version.");
	event->sendEvent();
}