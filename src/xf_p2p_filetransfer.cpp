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

#include "xf_contact.h"
#include "xf_p2p.h"
#include "xf_p2p_filetransfer.h"
#include "xf_p2p_session.h"

#include <KDebug>
#include <QCryptographicHash>

XfireP2PFileTransfer::XfireP2PFileTransfer(XfireP2PSession *parent, quint32 p_fileid, const QString p_filename, quint64 p_size) :
    m_session(parent), m_fileid(p_fileid), m_size(p_size), m_bytesTransferred(0), m_currentChunk(0), m_chunkCount(0)
{
    // Allocate file
    m_file = new QFile(p_filename);
    if(m_file->open(QIODevice::ReadWrite))
    {
        kDebug() << "File allocated:" << "filename:" << p_filename << "size:" << p_size;
        m_file->resize(p_size);

        // Send reply and request first chunk
        m_session->sendFileRequestReply(p_fileid, TRUE);
        m_session->sendFileChunkInfoRequest(p_fileid, 0, XF_P2P_FT_CHUNK_SIZE, m_chunkCount++, m_session->m_p2p->m_messageId++);
    }
}

XfireP2PFileTransfer::~XfireP2PFileTransfer()
{
    m_file->close();
}

void XfireP2PFileTransfer::createNewChunk(quint64 p_offset, quint32 p_size)
{
    delete m_currentChunk;

    m_currentChunk = new XfireP2PFileChunk(this, p_offset, p_size);
    connect(m_currentChunk, SIGNAL(chunkReady()), this, SLOT(slotChunkReady()));
}

void XfireP2PFileTransfer::slotChunkReady()
{
    m_file->write(m_currentChunk->m_data->constData(), m_currentChunk->m_size);
    kDebug() << "Receiving file chunk done, requesting next chunk";
    
    if((m_size - m_bytesTransferred) < XFIRE_P2P_FT_DATA_PACKET_SIZE)
    {
        kDebug() << "LAST CHUNK";

        // Calculate resting size
        quint32 rest = m_size - m_bytesTransferred;
        m_session->sendFileChunkInfoRequest(m_fileid, m_currentChunk->m_offset + XF_P2P_FT_CHUNK_SIZE, rest, m_chunkCount++, m_session->m_p2p->m_messageId++);
    }
    else
    {
        kDebug() << "NORMAL CHUNK";
        m_session->sendFileChunkInfoRequest(m_fileid, m_currentChunk->m_offset + XF_P2P_FT_CHUNK_SIZE, XF_P2P_FT_CHUNK_SIZE, m_chunkCount++, m_session->m_p2p->m_messageId++);
    }
}

XfireP2PFileChunk::XfireP2PFileChunk(XfireP2PFileTransfer* p_fileTransfer, quint64 p_offset, quint32 p_size) : m_fileTransfer(p_fileTransfer), m_offset(p_offset), m_size(p_size), m_packetsReceived(0)
{
    // Allocate chunk
    m_data = new QByteArray();
    m_data->resize(XF_P2P_FT_CHUNK_SIZE);

    // Calculate needed packets to obtain one chunk
    m_packetsCount = p_size / XFIRE_P2P_FT_DATA_PACKET_SIZE;
    if((p_size % XFIRE_P2P_FT_DATA_PACKET_SIZE) != 0)
        m_packetsCount++;
      
    kDebug() << "File chunk parts to request:" << m_packetsCount;

    quint32 size = ((m_packetsReceived == m_packetsCount - 1) && ((m_size % XFIRE_P2P_FT_DATA_PACKET_SIZE) != 0)) ?
        m_size % XFIRE_P2P_FT_DATA_PACKET_SIZE : XFIRE_P2P_FT_DATA_PACKET_SIZE;
    
    // Request chunk parts
    for(quint32 i = 0; i <= m_packetsCount; i++)
    {
        quint64 offset = p_offset + i * XFIRE_P2P_FT_DATA_PACKET_SIZE;
        m_fileTransfer->m_session->sendFileDataPacketRequest(m_fileTransfer->m_fileid, offset, size, m_fileTransfer->m_msgid++);
    }
}

XfireP2PFileChunk::~XfireP2PFileChunk()
{
    delete m_data;
}

void XfireP2PFileChunk::writeData(const QByteArray &p_data, quint64 p_offset, quint32 p_size)
{
    // Write data to chunk
    m_data->replace(p_offset, p_size, p_data);
    m_fileTransfer->m_bytesTransferred += p_size;

    // Check if chunk is complete
    if(m_packetsCount == ++m_packetsReceived)
    {
        kDebug() << "Chunk completed, writing to file";
        kDebug() << m_data->toHex();
        
        // FIXME: Control checksum

        // Seek and write chunk
        m_fileTransfer->m_file->seek(m_offset);
        m_fileTransfer->m_file->write(m_data->constData());

        emit chunkReady();
    }
    else
        kDebug() << m_packetsCount - m_packetsReceived << "packets resting";
}
