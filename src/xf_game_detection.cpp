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
#include "xf_games_list.h"

XfireGameDetection::XfireGameDetection(XfireAccount *p_account)
    : QThread(p_account),
    m_account(p_account),
    m_timer(new QTimer(this))
{
}

void XfireGameDetection::run()
{
    // Start game detection timer
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotUpdateRunningProcesses()));
    m_timer->start(8000);
    
    exec();
}

quint32 XfireGameDetection::isGameRunning(QString p_executable)
{
    for(int i = 0; i < m_processesList.size(); i++)
    {
        processInfo proc = m_processesList.at(i);
        if(proc.executable == p_executable)
            return proc.pid;
    }

    return 0;
}

void XfireGameDetection::checkRunningGames()
{
    quint32 detectedGameID = 0;

    QDomNodeList configuredGames = m_account->m_gamesList->m_configuredGamesList->elementsByTagName("game");
    for(int i = 0; i < configuredGames.count(); i++)
    {
        quint32 gameID = m_account->m_gamesList->getGameIDFromName(configuredGames.at(i).attributes().namedItem("name").nodeValue());
        QString executable = configuredGames.at(i).firstChildElement("command").firstChildElement("detect").text();

        quint32 running = isGameRunning(executable);
        if(running != 0)
        {
            detectedGameID = (quint32 )gameID;
            break;
        }
    }

    if(detectedGameID != m_currentGame.id)
    {
        m_currentGame.id = detectedGameID;
        m_currentGame.ip = 0;
        m_currentGame.port = 0;

        emit gameRunning();
    }
}

QString XfireGameDetection::getWinePath(QStringList p_environ, QString p_path)
{
    QStringList args;
    args.append("-u");
    args.append(p_path);

    QProcess *proc = new QProcess(this);
    proc->setEnvironment(p_environ);
    proc->start("winepath", args);
    proc->waitForFinished(1000);

    if(proc->exitStatus() == QProcess::NormalExit)
        return QString(proc->readAllStandardOutput()).trimmed();

    return "";
}

QHash<QString, QString> XfireGameDetection::getProcessEnviron(QString p_processPath)
{
    QHash<QString, QString> environ;

    QFile file(p_processPath + "/environ");
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

void XfireGameDetection::slotUpdateRunningProcesses()
{
    kDebug() << "Updating running processes";

    m_processesList.clear(); // Clear processes list

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

        m_processesList.append(proc); // Add the process to the process list
    }

    checkRunningGames(); // Check for running games
}
