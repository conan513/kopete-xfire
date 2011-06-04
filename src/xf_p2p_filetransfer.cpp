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

XfireP2PFileTransfer::XfireP2PFileTransfer(XfireP2PSession *parent, quint32 p_fileid, const QString p_filename, quint64 p_size) : m_session(parent), m_fileid(p_fileid), m_size(p_size)
{
    m_file = new QFile(p_filename);
    if(m_file->open(QIODevice::ReadWrite))
    {
        kDebug() << "File allocated:" << "filename:" << p_filename << "size:" << p_size;
        m_file->resize(p_size);

        // Send reply and request first chunk
        m_session->sendFileRequestReply(p_fileid, TRUE);
        m_session->sendFileChunkInfoRequest(p_fileid, 0, XF_P2P_FT_CHUNK_SIZE, 0, m_session->m_p2p->m_messageId++);
    }
}

XfireP2PFileTransfer::~XfireP2PFileTransfer()
{
    m_file->close();
}

void XfireP2PFileTransfer::createNewChunk(quint64 p_offset, quint32 p_size)
{
    m_currentChunk = new XfireP2PFileChunk(this, p_offset, p_size);
}

XfireP2PFileChunk::XfireP2PFileChunk(XfireP2PFileTransfer* p_fileTransfer, quint64 p_offset, quint32 p_size) : m_fileTransfer(p_fileTransfer), m_offset(p_offset), m_size(p_size)
{
    // Allocate chunk
    m_data = new QByteArray();
    m_data->resize(XF_P2P_FT_CHUNK_SIZE);

    m_packetsCount = p_size / XFIRE_P2P_FT_DATA_PACKET_SIZE; // Calculate needed packets to obtain one chunk
    kDebug() << "File chunk parts to request:" << m_packetsCount;

    // Request chunk parts
    for(quint32 i = 0; i < m_packetsCount; i++)
    {
        quint64 offset = p_offset + i * XFIRE_P2P_FT_DATA_PACKET_SIZE;
        m_fileTransfer->m_session->sendFileDataPacketRequest(m_fileTransfer->m_fileid, offset, XFIRE_P2P_FT_DATA_PACKET_SIZE, m_fileTransfer->m_msgid++); // FIXME: XFIRE_P2P_FT_DATA_PACKET_SIZE is wrong for last chunk part!
    }
}

XfireP2PFileChunk::~XfireP2PFileChunk()
{
}
