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

#define XF_GAMES_VERSION_URL "http://gfireproject.org/files/gfire_version.xml"
#define XF_GAMES_LIST_URL "http://gfireproject.org/files/gfire_games_v2.xml"

class XfireGamesList : public QObject
{
    Q_OBJECT;

public:
    // Constructor and destructor
    XfireGamesList();
    ~XfireGamesList();

    // Configured games list
    QList<QString> configuredGames();
    void updateConfiguredGame(QString p_name, QString pLaunchExe, QString p_detectExe);
    bool gameIsConfigured(QString p_name);

    QString getGameNameFromID(quint32 p_gameId);

    QList<QString> getGamesList();
    QDomElement getConfiguredGame(QString p_name);

    void removeConfiguredGame(QString p_name);

    QDomDocument *mConfiguredGamesList; // FIXME: Make private
    void saveConfiguredGamesList();
    void saveGamesList();

    QDomDocument *mGamesList;
    quint32 getGameIDFromName(QString p_name);

    void slotUpdate();

private:

    QNetworkAccessManager *m_manager;
    void initConfiguredGamesList();
    void initGamesList();

    quint32 mLocalGamesListVersion;
    quint32 mRemoteGamesListVersion;

private slots:
    void slotReceivedGamesList(QNetworkReply *p_reply);
    void slotGamesListUpdated();

signals:
    void gamesListReady();
};

#endif // XF_GAMES_LIST_H
