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

#ifndef XF_P2P_FILETRANSFER_H
#define XF_P2P_FILETRANSFER_H

#include <QByteArray>
#include <QFile>

class XfireP2PFileChunk;
class XfireP2PSession;

class XfireP2PFileTransfer : public QObject
{
    Q_OBJECT;

public:
    XfireP2PFileTransfer(XfireP2PSession *parent, quint32 p_fileid, const QString p_filename, quint64 p_size);
    ~XfireP2PFileTransfer();

    XfireP2PSession *m_session;

    void createNewChunk(quint64 p_offset, quint32 p_size);
    
    // Xfire transfer data
    quint32 m_fileid;
    quint32 m_msgid;
    bool m_aborted;
        
    // Chunks
    quint64 m_chunkCount;
    XfireP2PFileChunk *m_currentChunk;
    quint64 m_bytesTransferred;
        
    // File
    QFile *m_file;
    quint64 m_size;

public slots: // FIXME: really?
    void slotChunkReady();
};

class XfireP2PFileChunk : public QObject
{
    Q_OBJECT

public:
    XfireP2PFileChunk(XfireP2PFileTransfer* p_fileTransfer, quint64 p_offset, quint32 p_size);
    ~XfireP2PFileChunk();

    XfireP2PFileTransfer *m_fileTransfer;

    quint64 m_offset;
    quint32 m_size;

    quint32 m_packetsCount;
    quint32 m_packetsReceived;
    quint32 m_lastPacketRequested;
    quint32 m_requested;

    QString *m_checksum;
    QByteArray *m_data;

    bool m_done; // FIXME: get rid of this using slots
    
    void writeData(const QByteArray &p_data, quint64 p_offset, quint32 p_size);
    
signals:
    void chunkReady();
    
};

#endif // XF_P2P_FILETRANSFER_H
