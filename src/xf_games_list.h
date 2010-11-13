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

#ifndef XF_GAMES_LIST_H
#define XF_GAMES_LIST_H

#include <QDomDocument>
#include <QtNetwork>

class XfireGamesList : public QObject
{
	Q_OBJECT;

public:
	// Constructor and destructor
	XfireGamesList();
	~XfireGamesList();

	// Configured games list
	QList<QString> configuredGames();
	void updateConfiguredGame(QString pName, QString pLaunchExe, QString pDetectExe);
	bool gameIsConfigured(QString pName);

	QString getGameNameFromID(quint32 p_gameId);

	QList<QString> getGamesList();
	QDomElement getConfiguredGame(QString pName);

	void removeConfiguredGame(QString pName);

	QDomDocument *mConfiguredGamesList; // FIXME: Make private
	void saveConfiguredGamesList();
	void saveGamesList();

	QDomDocument *mGamesList;
	quint32 getGameIDFromName(QString pName);

private:
	
	QNetworkAccessManager *mManager;
	void initConfiguredGamesList();
	void initGamesList();

	quint32 mLocalGamesListVersion;
	quint32 mRemoteGamesListVersion;

private slots:
	void slotReceivedGamesList(QNetworkReply *pReply);
	void slotGamesListUpdated();

signals:
	void gamesListReady();
};

#endif // XF_GAMES_LIST_H
