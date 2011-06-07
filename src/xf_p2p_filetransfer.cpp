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

#include "xf_p2p.h"
#include "xf_p2p_filetransfer.h"
#include "xf_p2p_session.h"

#include <KDebug>
#include <QCryptographicHash>

XfireP2PFileTransfer::XfireP2PFileTransfer(XfireP2PSession *parent, quint32 p_fileid, const QString p_filename, quint64 p_size) :
    m_session(parent), m_fileid(p_fileid), m_size(p_size), m_bytesTransferred(0), m_chunk(0), m_chunksReceived(0), m_chunksCount(0)
{
    // Calculate needed chunks to obtain the file
    m_chunksCount = p_size / XF_P2P_FT_CHUNK_SIZE;
    if((p_size % XF_P2P_FT_CHUNK_SIZE) != 0)
        m_chunksCount++;

    // Allocate file
    m_file = new QFile(p_filename);
    if(m_file->open(QIODevice::ReadWrite))
    {
        kDebug() << "File allocated:" << "filename:" << p_filename << "size:" << p_size;
        m_file->resize(p_size);
    }

    // Allocate chunk
    m_chunk = new XfireP2PFileChunk(this, 0, 0);
    connect(m_chunk, SIGNAL(chunkReady()), this, SLOT(slotChunkReady()));
    
    // Reply to file transfer request
    m_session->sendFileRequestReply(m_fileid, TRUE);
    
    // Request first chunk FIXME: Calculate size here too for -50KB files
    m_session->sendFileChunkInfoRequest(m_fileid, 0, XF_P2P_FT_CHUNK_SIZE, m_chunksReceived + 1, m_session->m_p2p->m_messageId++);
}

XfireP2PFileTransfer::~XfireP2PFileTransfer()
{
    delete m_file;
}

void XfireP2PFileTransfer::start(quint64 p_offset, quint32 p_size, const QString& p_checksum)
{
    m_chunk->start(p_offset, p_size, p_checksum);
}

void XfireP2PFileTransfer::handleChunkData(const QByteArray &p_data, quint64 p_offset, quint32 p_size)
{
    m_chunk->handleData(p_data, p_offset, p_size);
    m_chunksReceived++;
}

void XfireP2PFileTransfer::slotChunkReady()
{   
    // Write chunk to file
    kDebug() << "Chunk completed, writing data to file";
    
    m_file->seek(m_chunk->m_offset);
    m_file->write(m_chunk->m_data);

    // Check if file transfer is finished
    if(m_chunksReceived == m_chunksCount)
    {
        emit ready(this);
        return;
    }
    
    // Request next chunk
    kDebug() << "Receiving file chunk done, requesting next chunk";
    quint32 size = (m_chunksReceived == m_chunksCount - 1) ? (m_size - m_bytesTransferred) : XF_P2P_FT_CHUNK_SIZE;
    m_session->sendFileChunkInfoRequest(m_fileid, m_chunk->m_offset + XF_P2P_FT_CHUNK_SIZE, size, m_chunksReceived + 1, m_session->m_p2p->m_messageId++);
}

XfireP2PFileChunk::XfireP2PFileChunk(XfireP2PFileTransfer* p_fileTransfer, quint64 p_offset, quint32 p_size) :
    m_fileTransfer(p_fileTransfer), m_offset(p_offset), m_size(p_size), m_packetsReceived(0)
{
    // Allocate chunk
    m_data = QByteArray();
}

XfireP2PFileChunk::~XfireP2PFileChunk()
{
}

void XfireP2PFileChunk::handleData(const QByteArray &p_data, quint64 p_offset, quint32 p_size)
{
    kDebug() << "Writing chunk part" << p_data.toHex();
    kDebug() << "Writing chunk part:" << p_offset << p_size << p_data.toHex();

    // Write data to chunk
    m_data.replace(p_offset - m_offset, p_size, p_data);
    m_fileTransfer->m_bytesTransferred += p_size;

    // Check if chunk is complete
    if(m_packetsCount == ++m_packetsReceived)
    {
        // Checksum
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        hasher.addData(m_data);
      
        // FIXME: request chunk again if checksum didn't match
        kDebug() << "SHA1:" << hasher.result().toHex();
        if(QString(hasher.result().toHex()).compare(m_checksum) != 0)
            kDebug() << "CHECKSUM NOT OK!";
    
        emit chunkReady();
    }
    else
        kDebug() << m_packetsCount - m_packetsReceived << "packets resting";
}

void XfireP2PFileChunk::start(quint64 p_offset, quint32 p_size, const QString &p_checksum)
{
    // Resize chunk
    m_data.resize(m_size);
    
    // Reset packets count/received
    m_packetsCount = 0;
    m_packetsReceived = 0;

    m_offset = p_offset;
    m_size = p_size;
    m_checksum = p_checksum;
    
    // Calculate needed packets to obtain one chunk
    m_packetsCount = p_size / XFIRE_P2P_FT_DATA_PACKET_SIZE;
    if((p_size % XFIRE_P2P_FT_DATA_PACKET_SIZE) != 0)
        m_packetsCount++;
      
    kDebug() << "File chunk parts to request:" << m_packetsCount;
    
    // Request chunk parts
    for(quint32 i = 0; i < m_packetsCount; i++)
    {
        quint32 size = (i == m_packetsCount - 1) ? (((m_size % XFIRE_P2P_FT_DATA_PACKET_SIZE) == 0) ? XFIRE_P2P_FT_DATA_PACKET_SIZE :m_size % XFIRE_P2P_FT_DATA_PACKET_SIZE) : XFIRE_P2P_FT_DATA_PACKET_SIZE;
        quint64 offset = p_offset + i * XFIRE_P2P_FT_DATA_PACKET_SIZE;

        m_fileTransfer->m_session->sendFileDataPacketRequest(m_fileTransfer->m_fileid, offset, size, m_fileTransfer->m_msgid++);
    }
}