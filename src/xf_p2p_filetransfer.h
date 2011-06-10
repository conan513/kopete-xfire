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

#define XFIRE_P2P_FT_PRIVATE_FILEID_START   0x80000000
#define XF_P2P_FT_CHUNK_SIZE                0xC800 // 50 * 1024 bytes = 51200 bytes

#define XFIRE_P2P_FT_DATA_PACKET_SIZE       0x0400 // 1024 Byte
#define XFIRE_P2P_FT_MAX_REQUESTS   10

#include <QByteArray>
#include <QFile>

class XfireP2PFileChunk;
class XfireP2PSession;
namespace Kopete { class Transfer; class FileTransferInfo; }

class XfireP2PFileTransfer : public QObject
{
    Q_OBJECT;

public:
    XfireP2PFileTransfer(XfireP2PSession *parent, quint32 p_fileid, const QString p_filename, quint64 p_size);
    ~XfireP2PFileTransfer();

    XfireP2PSession *m_session;

    void handleChunkData(const QByteArray &p_data, quint64 p_offset, quint32 p_size);
    void start(quint64 p_offset, quint32 p_size, const QString& p_checksum);
    
    // Xfire transfer data
    quint32 m_fileid;
    quint32 m_msgid;
        
    // Chunks information
    XfireP2PFileChunk *m_chunk;
    quint64 m_chunksCount;
    quint64 m_chunksReceived;
    quint64 m_bytesTransferred;
        
    // File information
    QFile *m_file;
    quint64 m_size;

public slots:
    void slotChunkReady();
    void slotTransferAccepted(Kopete::Transfer *p_transfer, const QString &p_fileName);
    void slotTransferRefused(const Kopete::FileTransferInfo &p_transfer);

signals:
    void ready(XfireP2PFileTransfer *p_fileTransfer);
};

class XfireP2PFileChunk : public QObject
{
    Q_OBJECT

public:
    XfireP2PFileChunk(XfireP2PFileTransfer* p_fileTransfer, quint64 p_offset, quint32 p_size);
    ~XfireP2PFileChunk();

    void handleData(const QByteArray &p_data, quint64 p_offset, quint32 p_size);
    void start(quint64 p_offset, quint32 p_size, const QString &p_checksum);
    
    XfireP2PFileTransfer *m_fileTransfer;
    
    quint64 m_offset;
    quint32 m_size;

    quint32 m_packetsCount;
    quint32 m_packetsReceived;

    QByteArray m_data;
    QString m_checksum;
    
signals:
    void chunkReady();
    
};

#endif // XF_P2P_FILETRANSFER_H
