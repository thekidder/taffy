/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <cassert>
#include <string.h>
#include <zlib.h>

#include <sore_logger.h>
#include <sore_network_buffers.h>

namespace SORE_Network
{
    static const unsigned int CHUNK = 16384;

    unsigned int SendBuffer::totalBytesSent = 0;

    unsigned int SendBuffer::GetTotalBytes()
    {
        return totalBytesSent;
    }

    void SendBuffer::ResetTotalBytes()
    {
        totalBytesSent = 0;
    }

    SendBuffer::SendBuffer()
    {
    }

    SendBuffer& SendBuffer::operator<<(const ubyte& data)
    {
        buf.push_back(data);
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const sbyte& data)
    {
        ubyte copy =  static_cast<ubyte>(data);
        *this << copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const ubyte2& data)
    {
        ubyte2 copy = htons(data);
        size_t len = buf.size();
        buf.resize(len+2);
        ubyte2* ptr = reinterpret_cast<ubyte2*>(&buf[len]);
        *ptr = copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const sbyte2& data)
    {
        ubyte2 copy = static_cast<ubyte2>(data);
        *this << copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const ubyte4& data)
    {
        ubyte4 copy = htonl(data);
        size_t len = buf.size();
        buf.resize(len+4);
        ubyte4* ptr = reinterpret_cast<ubyte4*>(&buf[len]);
        *ptr = copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const sbyte4& data)
    {
        ubyte4 copy = static_cast<ubyte4>(data);
        *this << copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const ubyte8& data)
    {
        ubyte8 copy = htonll(data);
        size_t len = buf.size();
        buf.resize(len+8);
        ubyte8* ptr = reinterpret_cast<ubyte8*>(&buf[len]);
        *ptr = copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const sbyte8& data)
    {
        ubyte8 copy = static_cast<ubyte8>(data);
        *this << copy;
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const std::string& data)
    {
        size_t len = buf.size();
        buf.resize(len + data.length() + 1);
        //std::copy(data.c_str()[0], data.c_str()[data.length()-1], &buf[len]);
        memcpy(&buf[len], data.c_str(), data.length());
        buf[len + data.length()] = '\0';
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const float1& data)
    {
        AddFloat(data, 4);
        return *this;
    }

    SendBuffer& SendBuffer::operator<<(const float2& data)
    {
        AddFloat(data, 8);
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const ubyte & data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const sbyte & data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const ubyte2& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const sbyte2& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const ubyte4& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const sbyte4& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const ubyte8& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const sbyte8& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const std::string& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const float1& data)
    {
        *this << data;
        return *this;
    }

    SendBuffer& SendBuffer::operator&(const float2& data)
    {
        *this << data;
        return *this;
    }

    void SendBuffer::AddFloat(float2 f, size_t numBytes)
    {
        unsigned int bits, expbits;
        switch(numBytes)
        {
            case 1:
            {
                bits = 8;
                expbits = 2;
                ubyte packed = static_cast<ubyte>(pack754(f, bits, expbits));
                *this << packed;
                break;
            }
            case 2:
            {
                bits = 16;
                expbits = 4;
                ubyte2 packed = static_cast<ubyte2>(pack754(f, bits, expbits));
                *this << packed;
                break;
            }
            case 4:
            {
                /*bits = 32;
                expbits = 8;
                ubyte4 packed = pack754(f, bits, expbits);
                AddUByte4(packed);*/

                size_t len = buf.size();
                buf.resize(len+4);

                float1* ptr = reinterpret_cast<float1*>(&buf[len]);
                *ptr = static_cast<float1>(f);
                break;
            }
            case 8:
            {
                /*bits = 64;
                expbits = 11;
                ubyte8 packed = pack754(f, bits, expbits);
                //ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("adding encoded double: 0x%016llX") % packed);
                AddUByte8(packed);*/

                size_t len = buf.size();
                buf.resize(len+8);

                float2* ptr = reinterpret_cast<float2*>(&buf[len]);
                *ptr = f;
                break;
            }
            default:
                ENGINE_LOG(SORE_Logging::LVL_WARNING, "Unsupported double size");
        }

    }

    SendBuffer& SendBuffer::operator+=(SendBuffer& b)
    {
        buf.insert(buf.end(), b.buf.begin(), b.buf.end());
        return *this;
    }

    unsigned int SendBuffer::size() const
    {
        return buf.size();
    }

    void SendBuffer::Compress(SendBuffer& compressed)
    {
        ubyte out[CHUNK];
        z_stream strm;
        strm.zalloc   = Z_NULL;
        strm.zfree    = Z_NULL;
        strm.opaque   = Z_NULL;
        int ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
        if(ret != Z_OK)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Compressing failed - could not init zlib");
            return;
        }

        unsigned int read = 0, have;
        unsigned int written = 0;
        int flush;

        do
        {
            if(size() - read < CHUNK)
                strm.avail_in = size() - read;
            else
                strm.avail_in = CHUNK;
            flush = size() - read == 0 ? Z_FINISH : Z_NO_FLUSH;
            //ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Compressing - starting at %u") % static_cast<unsigned int>(buf[read]));
            if(strm.avail_in)
                strm.next_in = &(buf[read]);
            read += strm.avail_in;
            do
            {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = deflate(&strm, flush);
                assert(ret != Z_STREAM_ERROR);
                have = CHUNK - strm.avail_out;
                written += have;
                for(unsigned int i=0;i<have;++i)
                {
                    compressed << out[i];
                    //compressed.AddUByte(out[i]);
                }
            } while (strm.avail_out == 0);
            assert(strm.avail_in == 0);
        } while(flush != Z_FINISH);
        assert(ret == Z_STREAM_END);
        deflateEnd(&strm);
        //ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Compressed %d bytes to %d bytes") % read % written);
    }

    ENetPacket* SendBuffer::GetPacket(enet_uint32 flags)
    {
        ENetPacket* temp;
        ubyte* begin = &buf[0];
        void* vdata = static_cast<void*>(begin);
        temp = enet_packet_create(vdata, buf.size(), flags);
        return temp;
    }

    void SendBuffer::Send(ENetPeer* peer, enet_uint8 channelID, enet_uint32 flags)
    {
        ENetPacket* temp = GetPacket(flags);
        enet_peer_send(peer, channelID, temp);
        totalBytesSent += buf.size();
    }

    void SendBuffer::Broadcast(ENetHost* host, enet_uint8 channelID, enet_uint32 flags)
    {
        ENetPacket* temp = GetPacket(flags);
        enet_host_broadcast(host, channelID, temp);
    }

    void SendBuffer::Clear()
    {
        buf.clear();
    }

    ReceiveBuffer::ReceiveBuffer(ENetPacket& packet)
    {
        buf = static_cast<ubyte*>(packet.data);
        length = static_cast<size_t>(packet.dataLength);
        remaining = length;
    }

    ReceiveBuffer::ReceiveBuffer(ReceiveBuffer& b, bool compressed)
    {
        if(compressed)
        {
            ubyte out[CHUNK];
            unsigned int written = 0;
            unsigned int read    = 0;

            z_stream strm;
            strm.zalloc   = Z_NULL;
            strm.zfree    = Z_NULL;
            strm.opaque   = Z_NULL;
            strm.avail_in = 0;
            strm.next_in  = Z_NULL;
            int ret = inflateInit(&strm);
            if (ret != Z_OK)
            {
                ENGINE_LOG(SORE_Logging::LVL_ERROR, "Decompressing failed - could not init zlib");
                return;
            }
            do
            {
                if(b.remaining > CHUNK)
                    strm.avail_in = CHUNK;
                else
                    strm.avail_in = b.remaining;
                read += strm.avail_in;
                strm.next_in = (b.buf + (b.length - b.remaining) );
                b.remaining -= strm.avail_in;

                do
                {
                    strm.avail_out = CHUNK;
                    strm.next_out  = out;
                    ret = inflate(&strm, Z_NO_FLUSH);
                    assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                    switch (ret)
                    {
                        case Z_NEED_DICT:
                            ret = Z_DATA_ERROR;     /* and fall through */
                        case Z_DATA_ERROR:
                        case Z_MEM_ERROR:
                            inflateEnd(&strm);
                            return;
                    }
                    unsigned int have = CHUNK - strm.avail_out;
                    for(unsigned int i = 0; i < have; ++i)
                    {
                        //ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("out[%d] = %u") % i % static_cast<unsigned int>(out[i]));
                        ownData.push_back(out[i]);
                    }
                    written += have;
                } while(strm.avail_out == 0);
            } while(ret != Z_STREAM_END);
            buf = &(ownData[0]);
            length = written;
            remaining = length;
            //ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("Decompressed %d bytes to %d bytes") % read % written);
        }
        else
        {
            buf = b.buf;
            length = b.length;
            remaining = b.remaining;
        }
    }

    ReceiveBuffer::~ReceiveBuffer()
    {
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(ubyte& data)
    {
        assert(remaining>=1);
        data = buf[length-remaining];
        remaining--;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(sbyte& data)
    {
        ubyte copy = static_cast<ubyte>(data);
        *this >> copy;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(ubyte2& data)
    {
        assert(remaining>=2);
        ubyte2* pos = reinterpret_cast<ubyte2*>(&buf[length-remaining]);
        data = *pos;
        remaining-=2;
        data = ntohs(data);
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(sbyte2& data)
    {
        ubyte2 copy = static_cast<ubyte2>(data);
        *this >> copy;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(ubyte4& data)
    {
        assert(remaining>=4);
        ubyte4* pos = reinterpret_cast<ubyte4*>(&buf[length-remaining]);
        data = *pos;
        remaining-=4;
        data = ntohl(data);
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(sbyte4& data)
    {
        ubyte4 copy = static_cast<ubyte4>(data);
        *this >> copy;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(ubyte8& data)
    {
        assert(remaining>=8);
        ubyte8* pos = reinterpret_cast<ubyte8*>(&buf[length-remaining]);
        data = *pos;
        remaining-=8;
        data = ntohll(data);
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(sbyte8& data)
    {
        ubyte8 copy = static_cast<ubyte8>(data);
        *this >> copy;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(std::string& data)
    {
        data.clear();
        ubyte c;
        *this >> c;
        while(c != '\0')
        {
            data += static_cast<char>(c);
            *this >> c;
        }
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(float1& data)
    {
        data = static_cast<float1>(GetFloat(4));
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator>>(float2& data)
    {
        data = GetFloat(8);
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(ubyte & data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(sbyte & data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(ubyte2& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(sbyte2& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(ubyte4& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(sbyte4& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(ubyte8& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(sbyte8& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(std::string& data)
    {
        *this >> data;
        return *this;
    }


    ReceiveBuffer& ReceiveBuffer::operator&(float1& data)
    {
        *this >> data;
        return *this;
    }

    ReceiveBuffer& ReceiveBuffer::operator&(float2& data)
    {
        *this >> data;
        return *this;
    }

    float2 ReceiveBuffer::GetFloat(size_t numBytes)
    {
        assert(numBytes==1 || numBytes==2 || numBytes==4 || numBytes==8);
        ubyte8 num;
        unsigned int bits, expbits;
        switch(numBytes)
        {
            case 1:
            {
                ubyte temp;
                *this >> temp;
                num = static_cast<ubyte8>(temp);
                bits = 8;
                expbits = 2;
                break;
            }
            case 2:
                ubyte2 temp;
                *this >> temp;
                num = static_cast<ubyte8>(temp);
                bits = 16;
                expbits = 4;
                break;
            case 4:
            {
                /*num = static_cast<ubyte8>(GetUByte4());
                bits = 32;
                expbits = 8;*/
                assert(remaining>=4);
                float1* pos = reinterpret_cast<float1*>(&buf[length-remaining]);
                float1 i = *pos;
                remaining-=4;
                return static_cast<float2>(i);
                break;
            }
            case 8:
            {
                /*num = static_cast<ubyte8>(GetUByte8());
                //ENGINE_LOG(SORE_Logging::LVL_DEBUG3, boost::format("receiving encoded double: 0x%016llX") % num);
                bits = 64;
                expbits = 11;*/
                assert(remaining>=8);
                float2* pos = reinterpret_cast<float2*>(&buf[length-remaining]);
                float2 i = *pos;
                remaining-=8;
                return i;
                break;
            }
            default:
                ENGINE_LOG(SORE_Logging::LVL_WARNING, "Unsupported double size");
        }
        return static_cast<double>(unpack754(num, bits, expbits));
    }

    size_t ReceiveBuffer::Remaining() const
    {
        return remaining;
    }
}
