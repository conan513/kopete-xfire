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

#ifndef XF_GAME_DETECTION_H
#define XF_GAME_DETECTION_H

#include <QByteArray>
#include <QDomDocument>
#include <QHash>
#include <QTimer>

class XfireAccount;

class XfireGameDetection : public QObject
{
	Q_OBJECT;

	struct processInfo
	{
		quint32 pid;
		QString executable;
		QStringList cmdline;
	};

	struct gameInfo
	{
		quint32 id;
		quint32 ip;
		quint32 port;
	};

public:
	// Constructor & destructor
	XfireGameDetection(XfireAccount *pAccount);
	~XfireGameDetection();

	quint32 isGameRunning(QString pExecutable);
	gameInfo mCurrentGame; // FIXME: Make private?

private:
	XfireAccount *mAccount;
	QList <processInfo> mProcessesList;
	QTimer *mTimer;

	QString getWinePath(QStringList pEnviron, QString pPath);
	void checkRunningGames();

	QHash<QString, QString> getProcessEnviron(QString pProcessPath);

private slots:
	void slotUpdateRunningProcesses();

signals:
	void gameRunning();
};


#endif // XF_GAME_DETECTION_H
