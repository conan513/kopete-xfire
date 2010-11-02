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

#include <QDir>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>

#include <KDebug>
#include <KStandardDirs>

#include "xf_account.h"
#include "xf_game_detection.h"

XfireGameDetection::XfireGameDetection(XfireAccount *pAccount)
{
	mAccount = pAccount;

	// Initialize and start game detection timer
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(slotUpdateRunningProcesses()));
	mTimer->start(8000);
}

XfireGameDetection::~XfireGameDetection()
{
}

quint32 XfireGameDetection::isGameRunning(QString pExecutable)
{
	for(int i = 0; i < mProcessesList.size(); i++)
	{
		processInfo proc = mProcessesList.at(i);
		if(proc.executable == pExecutable)
			return proc.pid;
	}

	return -1;
}

void XfireGameDetection::checkRunningGames()
{
	quint32 detectedGameID = 0;
	
	QDomNodeList configuredGames = mAccount->mGamesList->mConfiguredGamesList->elementsByTagName("game");
	for(int i = 0; i < configuredGames.count(); i++)
	{
		quint32 gameID = mAccount->mGamesList->getGameIDFromName(configuredGames.at(i).attributes().namedItem("name").nodeValue());
		QString executable = configuredGames.at(i).firstChildElement("command").firstChildElement("detect").text();

		quint32 running = isGameRunning(executable);
		if(running != -1)
		{
			detectedGameID = (quint32)gameID;
			break;
		}
	}

	if(detectedGameID != mCurrentGame.id)
	{
		mCurrentGame.id = detectedGameID;
		mCurrentGame.ip = 0;
		mCurrentGame.port = 0;

		emit gameRunning();
	}
}

QString
XfireGameDetection::getWinePath(QStringList pEnviron, QString pPath)
{
	QStringList args;
	args.append("-u");
	args.append(pPath);

	QProcess *proc = new QProcess(this);
	proc->setEnvironment(pEnviron);
	proc->start("winepath", args);
	proc->waitForFinished(1000);

	if(proc->exitStatus() == QProcess::NormalExit)
		return QString(proc->readAllStandardOutput()).trimmed();

	return "";
}

QHash<QString, QString>
XfireGameDetection::getProcessEnviron(QString pProcessPath)
{
	QHash<QString, QString> environ;

	QFile file(pProcessPath + "/environ");
	if(!file.open(QIODevice::ReadOnly))
		return environ;

	QByteArray content = file.readAll();
	QList<QByteArray> split = content.split('\0');
	file.close();

	for(int i = 0; i < split.size(); i++)
	{
		QString element = QString(split.at(i));
		QStringList elementSplit = element.split("=");

		if(elementSplit.size() != 2)
			continue;
		else
			environ.insert(elementSplit.at(0), elementSplit.at(1));
	}

	return environ;
}

void
XfireGameDetection::slotUpdateRunningProcesses()
{
	kDebug() << "Updating running processes.";

	mProcessesList.clear(); // Clear processes list

	QDir dir("/proc");
	QFileInfoList processes = dir.entryInfoList();

	// Iterate through running processes
	for(int i = 0; i < processes.size(); ++i)
	{
		QFileInfo process = processes.at(i);

		// Check process name
		foreach(QChar c, process.fileName().toAscii())
		{
			if(!c.isDigit())
				continue;
		}

		// Check process ownership
		if(process.ownerId() != geteuid())
			continue;

		processInfo proc; // Create process info

		// Get process pid
		uint pid = process.fileName().toUInt();
		if(pid == 0)
			continue;

		// Get process cmdline
		QStringList cmdline;

		QFile file(process.filePath() + "/cmdline");
		if(file.open(QIODevice::ReadOnly))
		{
			QByteArray content = file.readAll();
			QList<QByteArray> contentSplit =  content.split('\0');

			file.close();

			for(int i = 0; i < contentSplit.size(); ++i)
				cmdline.append(contentSplit.at(i));
		}

		if(cmdline.size() < 1)
			continue;

		// Set the process executable (canonicalized)
		QString executable;

		QFileInfo exe(process.filePath() + "/exe");
		QString exeCanonicalized = exe.canonicalFilePath();

		if(exeCanonicalized == "/usr/bin/wine-preloader") // Wine support
		{
			QHash<QString, QString> environ = getProcessEnviron(process.filePath()); // Get environ

			// Build environment in order to call winepath
			QStringList foo;

			QString prefix = environ.value("WINEPREFIX", "");
			if(prefix != "")
				foo.append("WINEPREFIX=" + prefix);

			// Get unix path using winepath
			QString unixPath = getWinePath(foo, cmdline.at(0)).trimmed();
			QFileInfo unixPathTest(unixPath);

			if(unixPathTest.canonicalFilePath() == "") // We have only the executable name
			{
				// Guess using CWD
				unixPathTest.setFile(process.filePath() + "/cwd");
				if(unixPathTest.canonicalFilePath() == "")
					continue;

				unixPathTest.setFile(unixPathTest.canonicalFilePath() + "/" + cmdline.at(0));
				if(unixPathTest.canonicalFilePath() == "")
					continue;

				executable = unixPathTest.filePath();
			}
			else
				executable = unixPathTest.canonicalFilePath();
		}
		else // Default behaviour
			executable = exeCanonicalized;

		proc.pid = pid;
		proc.cmdline = cmdline;
		proc.executable = executable;

		mProcessesList.append(proc); // Add the process to the process list
	}

	checkRunningGames(); // Check for running games
}
