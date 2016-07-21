#include "../diffs.h"
#include "../diftypes.h"

#include "base.h"
#include "calcsum.h"
#include "sq_su.h"
#include "records.h"

#include "md5.h"
#include "crc32.h"
#include "sqze.h"


#include <lzmalib/lzma_decode.h>
#include <lzmalib/lzma_encode.h>

#include <diffs/klava/klavpacklib/klavpack_encode.h>
#include <diffs/klava/klavpacklib/klavpack_decode.h>

enum DifferenceFormat
{
    plainDifferenceFormat,
    avcDifferenceFormat,
    sqzeDifferenceFormat,
    klavaDifferenceFormat,
    klavaKfbDifferenceFormat,
};


// structure declared internally, because of std::set<> usage
struct DIFF_Data
{
    typedef std::vector<unsigned char> Entry;

    // original buffer, use to apply difference on
    std::vector <unsigned char> m_originalBuffer;
    // base data hash
    unsigned char BaseHash[8];

    std::map<size_t, Entry> m_entryes;
};


static bool transformBase(const std::vector<unsigned char> &mfInBase, std::vector<unsigned char> &destination, const bool compress, KLUPD::Log *pLog)
{
    AVP_BaseHeader baseHeader;
    if(mfInBase.size() < sizeof(baseHeader))
    {
        TRACE_MESSAGE4("Failed to %s AVC base, buffer size %d is not enough to fit AVP Base header %d",
            compress ? "compress" : "uncompress", mfInBase.size(), sizeof(baseHeader));
        return false;
    }
    memcpy(&baseHeader, &mfInBase[0], sizeof baseHeader);

    // if AVP_MAGIC is found, then reading file once again and put into structure with offset
    if(*(reinterpret_cast<unsigned long *>(&(baseHeader.Authenticity))) == AVP_MAGIC)
        memcpy(&baseHeader.Authenticity, &mfInBase[0], sizeof(baseHeader) - 0x40);

    if(*(reinterpret_cast<unsigned long *>(&baseHeader.Magic)) != AVP_MAGIC)
    {
        // magic check failed (not fatal error, because other un-compress methods will be tried
        return false;
    }

    bool baseCompressed = false;
    size_t currentOffset = baseHeader.BlockHeaderTableFO;

    std::vector<AVP_BlockHeader> blockHeaderArray;
    for(unsigned int headerTableIndex = 0; headerTableIndex < baseHeader.BlockHeaderTableSize; ++headerTableIndex)
    {
        AVP_BlockHeader blockHeader;

        // buffer range check
        if(mfInBase.size() < currentOffset + sizeof(blockHeader))
        {
            TRACE_MESSAGE5("Failed to %s AVC base, buffer size %d is not enough to fit AVP header block number %d (size %d)",
                compress ? "compress" : "uncompress", mfInBase.size(), headerTableIndex, sizeof(blockHeader));
            return false;
        }

        memcpy(&blockHeader, &mfInBase[0] + currentOffset, sizeof(blockHeader));

        currentOffset += sizeof(AVP_BlockHeader);

        if(blockHeader.Compression)
            baseCompressed = true;
        blockHeaderArray.push_back(blockHeader);
    }

    // check compression status
    if(baseCompressed == compress)
    {
        TRACE_MESSAGE2("Failed to %s AVC base, compressed flag check failed", compress ? "compress" : "uncompress");
        return false;
    }

    ////////////////////////////
    // writing base header
    destination.insert(destination.end(), reinterpret_cast<unsigned char *>(&baseHeader),
        reinterpret_cast<unsigned char *>(&baseHeader + 1));


    ////////////////////////////
    // BlockTableHeader write
    baseHeader.BlockHeaderTableFO = destination.size();
    // reserve enough space for headers
    destination.insert(destination.end(), baseHeader.BlockHeaderTableSize * sizeof(AVP_BlockHeader), 0);

    ////////////////////////////
    // Records & Links write
    for(unsigned int baseHeaderIndex = 0; baseHeaderIndex < baseHeader.BlockHeaderTableSize; baseHeaderIndex++)
    {
        if(!blockHeaderArray[baseHeaderIndex].CompressedSize && !blockHeaderArray[baseHeaderIndex].UncompressedSize)
            continue;
        // badly wiped 16-bit link
        if(!blockHeaderArray[baseHeaderIndex].CompressedSize || !blockHeaderArray[baseHeaderIndex].UncompressedSize)
        {
            TRACE_MESSAGE4("Failed to %s AVC base, empty check failed. Compressed size %d, uncompressed size %d",
                compress ? "compress" : "uncompress", blockHeaderArray[baseHeaderIndex].CompressedSize, blockHeaderArray[baseHeaderIndex].UncompressedSize);
            return false;
        }

        ////////////////////////////
        // reading block
        if(mfInBase.size() < blockHeaderArray[baseHeaderIndex].BlockFO + blockHeaderArray[baseHeaderIndex].CompressedSize)
        {
            TRACE_MESSAGE5("Failed to %s AVC base, failed to read block, because buffer is not enough. Total size %d, block offset %d, compressed size %d",
                compress ? "compress" : "uncompress", mfInBase.size(), blockHeaderArray[baseHeaderIndex].BlockFO, blockHeaderArray[baseHeaderIndex].CompressedSize);
            return false;
        }

        std::vector<unsigned char> buf(blockHeaderArray[baseHeaderIndex].CompressedSize);
        memcpy(&buf[0], &mfInBase[0] + blockHeaderArray[baseHeaderIndex].BlockFO, blockHeaderArray[baseHeaderIndex].CompressedSize);

        const unsigned long crc = CalcSum(&buf[0], blockHeaderArray[baseHeaderIndex].CompressedSize);
        if(blockHeaderArray[baseHeaderIndex].CRC != crc)
        {
            TRACE_MESSAGE4("Failed to %s AVC base, CRC check failed. Block header CRC %d, compressed CRC %d",
                compress ? "compress" : "uncompress", blockHeaderArray[baseHeaderIndex].CRC, crc);
            return false;
        }

        ////////////////////////////
        // decompress block
        if(blockHeaderArray[baseHeaderIndex].Compression)
        {
            std::vector<unsigned char> out_buf(blockHeaderArray[baseHeaderIndex].UncompressedSize + 16);
            for(unsigned int ib = 0; ib < blockHeaderArray[baseHeaderIndex].CompressedSize; ++ib)
                buf[ib] ^= static_cast<unsigned char>(ib);

            const size_t bytesRead = unsqu(&buf[0], &out_buf[0]);
            if(bytesRead != blockHeaderArray[baseHeaderIndex].UncompressedSize)
            {
                TRACE_MESSAGE4("Failed to %s AVC base, failed to decompress block. Bytes read %d, expected %d",
                    compress ? "compress" : "uncompress", bytesRead, blockHeaderArray[baseHeaderIndex].UncompressedSize);
                return false;
            }

            buf.swap(out_buf);

            blockHeaderArray[baseHeaderIndex].Compression = 0;
            blockHeaderArray[baseHeaderIndex].CompressedSize = blockHeaderArray[baseHeaderIndex].UncompressedSize;
        }


        ////////////////////////////
        // compress block
        if(compress)
        {
            std::vector<unsigned char> mfOutBlock;
            blockHeaderArray[baseHeaderIndex].Compression = 1;
            blockHeaderArray[baseHeaderIndex].CompressedSize = squeeze(&buf[0], mfOutBlock, blockHeaderArray[baseHeaderIndex].UncompressedSize);

            // incorrect size
            if(mfOutBlock.size() != blockHeaderArray[baseHeaderIndex].CompressedSize)
            {
                TRACE_MESSAGE4("Failed to %s AVC base, failed to decompress block. Block size %d, expected %d",
                    compress ? "compress" : "uncompress", mfOutBlock.size(), blockHeaderArray[baseHeaderIndex].CompressedSize);
                return false;
            }

            buf = mfOutBlock;

            for(unsigned int ib = 0; ib < blockHeaderArray[baseHeaderIndex].CompressedSize; ++ib)
                buf[ib] ^= static_cast<unsigned char>(ib);
        }

        // ---------------------------------------------------------------------------------
        blockHeaderArray[baseHeaderIndex].BlockFO = destination.size();
        blockHeaderArray[baseHeaderIndex].CRC  = CalcSum(&buf[0], blockHeaderArray[baseHeaderIndex].CompressedSize);

        destination.insert(destination.end(), &buf[0], &buf[0] + blockHeaderArray[baseHeaderIndex].CompressedSize);
    }

    ////////////////////////////
    // again write base header
    baseHeader.FileSize = destination.size() + (compress ? 64 : 0);
    baseHeader.BlockHeaderTableSize = blockHeaderArray.size();
    baseHeader.AuthenticityCRC = (CalcSum(baseHeader.Authenticity, 0x40));
    baseHeader.HeaderCRC=(CalcSum(reinterpret_cast<unsigned char *>(&baseHeader.Magic), sizeof(baseHeader) - 0x84));

    ////////////////////////////
    // checking array bounds
    if(destination.size() < sizeof(baseHeader))
    {
        TRACE_MESSAGE4("Failed to %s AVC base, buffer size %d is not enough to fit base header %d",
            compress ? "compress" : "uncompress", destination.size(), sizeof(baseHeader));
        return false;
    }
    memcpy(&destination[0], &baseHeader, sizeof(baseHeader));


    // write again block table header
    unsigned long offset = baseHeader.BlockHeaderTableFO;
    for(unsigned int blockTableHeaderIndex = 0; blockTableHeaderIndex < baseHeader.BlockHeaderTableSize; ++blockTableHeaderIndex)
    {
        // checking destination bounds
        if(destination.size() < offset + sizeof(AVP_BlockHeader))
        {
            TRACE_MESSAGE5("Failed to %s AVC base, buffer size %d is not enough to %d-block table header %d",
                compress ? "compress" : "uncompress", destination.size(), blockTableHeaderIndex, sizeof(AVP_BlockHeader));
            return false;
        }

        // checking source bounds
        if(blockHeaderArray.size() < blockTableHeaderIndex)
        {
            TRACE_MESSAGE4("Failed to %s AVC base, failed to write %d-block table header, because only %d blocks exist",
                compress ? "compress" : "uncompress", blockTableHeaderIndex, blockHeaderArray.size());
            return false;
        }

        memcpy(&destination[offset], &blockHeaderArray[blockTableHeaderIndex], sizeof(AVP_BlockHeader));
        offset += sizeof(AVP_BlockHeader);
    }

    return true;
}

static void offset2Len(unsigned long *pdwOffset, unsigned long *pdwLastOffset, const bool restore)
{
    if(!restore)
    {
        unsigned long dwOffset = *pdwOffset;
        *pdwOffset -= *pdwLastOffset;
        *pdwLastOffset = dwOffset;
    }
    else
        *pdwLastOffset = *pdwOffset = *pdwOffset + *pdwLastOffset;
}


bool prepBase4Diff(std::vector<unsigned char> &base, const bool restore, KLUPD::Log *pLog)
{
    AVP_BaseHeader baseHeader;
    if(base.size() < sizeof(baseHeader))
    {
        TRACE_MESSAGE4("Failed to %s base for difference, base buffer size %d is not enough to fit base header %d",
            restore ? "restore" : "prepare", base.size(), sizeof(baseHeader));
        return false;
    }

    memcpy(&baseHeader, &base[0], sizeof(baseHeader));
    if(baseHeader.Magic != AVP_MAGIC)
    {
        // not fatal error (no need to trace), because other compression method is going to be used
        return false;
    }

    const AVP_BlockHeader *blockHeaderArray = reinterpret_cast<const AVP_BlockHeader *>(&base[0] + baseHeader.BlockHeaderTableFO);

    bool result = false;
    for(unsigned int i = 0; i < baseHeader.BlockHeaderTableSize; ++i)
    {
        const AVP_BlockHeader *bhp = blockHeaderArray + i;
        // range check
        if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(bhp + 1))
        {
            TRACE_MESSAGE5("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block header %d",
                restore ? "restore" : "prepare", base.size(), i, sizeof(AVP_BlockHeader));
            return false;
        }

        // can not prepare compressed data
        if(bhp->Compression)
        {
            TRACE_MESSAGE2("Failed to %s base for difference, can not prepare compressed data", restore ? "restore" : "prepare");
            return false;
        }

        unsigned long npos = 0;
        switch(bhp->BlockType)
        {
        case BT_NAME:
            break;
        case BT_RECORD:
            if(bhp->NumRecords > 0)
                result = true;

            switch(bhp->RecordType)
            {
            case RT_MEMORY:
                {
                    R_Memory *p = reinterpret_cast<R_Memory *>(&base[0] + bhp->BlockFO);

                    // bounds check
                    if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(p + bhp->NumRecords))
                    {
                        TRACE_MESSAGE6("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block %d RMemory record(s) of %d bytes",
                            restore ? "restore" : "prepare", base.size(), i, bhp->NumRecords, sizeof(R_Memory));
                        return false;
                    }

                    for(unsigned int j = 0; j < bhp->NumRecords; ++j, ++p)
                        offset2Len(&p->NameIdx, &npos, restore);

                    break;
                }
            case RT_SECTOR:
                {
                    R_Sector *p = reinterpret_cast<R_Sector *>(&base[0] + bhp->BlockFO);

                    // bounds check
                    if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(p + bhp->NumRecords))
                    {
                        TRACE_MESSAGE6("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block %d RSector record(s) of %d bytes",
                            restore ? "restore" : "prepare", base.size(), i, bhp->NumRecords, sizeof(R_Sector));
                        return false;
                    }

                    for(unsigned int j = 0; j < bhp->NumRecords; ++j, ++p)
                        offset2Len(&p->NameIdx, &npos, restore);

                    break;
                }
            case RT_FILE:
                {
                    R_File *p = reinterpret_cast<R_File *>(&base[0] + bhp->BlockFO);

                    // bounds check
                    if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(p + bhp->NumRecords))
                    {
                        TRACE_MESSAGE6("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block %d RFile record(s) of %d bytes",
                            restore ? "restore" : "prepare", base.size(), i, bhp->NumRecords, sizeof(R_File));
                        return false;
                    }

                    for(unsigned int j = 0; j < bhp->NumRecords; ++j, ++p)
                        offset2Len(&p->NameIdx, &npos, restore);

                    break;
                }
            case RT_CA:
                {
                    R_CA *p = reinterpret_cast<R_CA *>(&base[0] + bhp->BlockFO);

                    // bounds check
                    if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(p + bhp->NumRecords))
                    {
                        TRACE_MESSAGE6("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block %d RCA record(s) of %d bytes",
                            restore ? "restore" : "prepare", base.size(), i, bhp->NumRecords, sizeof(R_CA));
                        return false;
                    }

                    for(unsigned int j = 0; j < bhp->NumRecords; ++j, ++p)
                        offset2Len(&p->NameIdx, &npos, restore);

                    break;
                }
            case RT_UNPACK:
                {
                    R_Unpack *p = reinterpret_cast<R_Unpack *>(&base[0] + bhp->BlockFO);

                    // bounds check
                    if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(p + bhp->NumRecords))
                    {
                        TRACE_MESSAGE6("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block %d RUnpack record(s) of %d bytes",
                            restore ? "restore" : "prepare", base.size(), i, bhp->NumRecords, sizeof(R_Unpack));
                        return false;
                    }

                    for(unsigned int j = 0; j < bhp->NumRecords; ++j, ++p)
                        offset2Len(&p->NameIdx, &npos, restore);

                    break;
                }
            case RT_EXTRACT:
                {
                    R_Extract *p = reinterpret_cast<R_Extract *>(&base[0] + bhp->BlockFO);

                    // bounds check
                    if(&base[0] + base.size() < reinterpret_cast<const unsigned char *>(p + bhp->NumRecords))
                    {
                        TRACE_MESSAGE6("Failed to %s base for difference, base buffer size %d is not enough to fit %d-block %d RExtract record(s) of %d bytes",
                            restore ? "restore" : "prepare", base.size(), i, bhp->NumRecords, sizeof(RT_EXTRACT));
                        return false;
                    }

                    for(unsigned int j = 0; j < bhp->NumRecords; ++j, ++p)
                        offset2Len(&p->NameIdx, &npos, restore);

                    break;
                }
            default:
                break;
            }
            break;

        case BT_LINK16:
            break;

        case BT_LINK32:
            break;
        default:
            break;
        }
    }
    if(!result)
    {
        TRACE_MESSAGE2("Failed to %s base for difference, no record found", restore ? "restore" : "prepare");
        return false;
    }
    if(base.empty())
    {
        TRACE_MESSAGE2("Failed to %s base for difference, result is empty", restore ? "restore" : "prepare");
        return false;
    }

    return true;
}

// uncompress file in AVC format
static bool unpackAVC(const std::vector<unsigned char> &avcBuffer, std::vector<unsigned char> &unpacked, KLUPD::Log *pLog)
{
    if(!transformBase(avcBuffer, unpacked, false, pLog))
        return false;

    return prepBase4Diff(unpacked, false, pLog);
}

// uncompress file in SQZE format
static bool uncompressSQZE(const std::vector<unsigned char> &sqzeBuffer, std::vector<unsigned char> &unpacked, KLUPD::Log *pLog)
{
    if(sqzeBuffer.size() < sizeof(CSqueezeHeader))
    {
        TRACE_MESSAGE3("Failed to uncompress SQZE, buffer size %d is not enough to fit SQZE header %d", sqzeBuffer.size(), sizeof(CSqueezeHeader));
        return false;
    }

    const CSqueezeHeader *pSQZE = reinterpret_cast<const CSqueezeHeader *>(&sqzeBuffer[0]);
    // check signature
    if((pSQZE->dwSignature != SQZE_HEADER_SIGNATURE) || (pSQZE->dwVersion != 1))
    {
        // not SQZE format, not fatal error (no trace needed), because other compression type will be tried
        return false;
    }

    // SQZE format, incorrect checksum
    unsigned long crc = ~CRC32((unsigned char *)pSQZE + pSQZE->dwHeaderSize, pSQZE->dwCompressedSize, 0xffffffff); 
    if(pSQZE->dwCrc != crc)
    {
        TRACE_MESSAGE3("Failed to uncompress SQZE, CRC %d check failed, expected %d", crc, pSQZE->dwCrc);
        return false;
    }

    // SQZE format, unpacking
    unpacked.resize(pSQZE->dwHeaderSize + pSQZE->dwOriginalSize);
    CSqueezeHeader *pSQZU = reinterpret_cast<CSqueezeHeader *>(&unpacked[0]);

    memcpy(&unpacked[0], pSQZE, pSQZE->dwHeaderSize);
    pSQZU->dwSignature = SQZU_HEADER_SIGNATURE;

    const size_t unpackedSize = unsqu(&sqzeBuffer[0] + pSQZE->dwHeaderSize, &unpacked[0] + pSQZU->dwHeaderSize);

    // check if corrupted
    if(pSQZU->dwOriginalSize != unpackedSize)
    {
        TRACE_MESSAGE3("Failed to uncompress SQZE, size %d check failed, expected %d", unpackedSize, pSQZU->dwOriginalSize);
        return false;
    }
    return true;
}

bool loadDiff(DIFF_Data &data, const unsigned int index, const std::vector<unsigned char> &differenceBuffer, KLUPD::Log *pLog)
{
    if(differenceBuffer.size() < sizeof(tDIFF_HDR))
    {
        TRACE_MESSAGE3("Failed to load difference, buffer size %d is not enought to fit difference header %d", differenceBuffer.size(), sizeof(tDIFF_HDR));
        return false;
    }

    // check header
    const tDIFF_HDR *MHdr = reinterpret_cast<const tDIFF_HDR *>(&differenceBuffer[0]);
    if(memcmp(MHdr->Sign, &DiffMark[0], 4))
    {
        // not 'DIFF' format, not fatal error (trace is not need to be written), because other compression type will be tried
        return false;
    }
    if(MHdr->Version != DIFF_VERSION)
    {
        TRACE_MESSAGE3("Failed to load difference version %d, expected version %d", MHdr->Version, DIFF_VERSION);
        return false;
    }
    if(differenceBuffer.size() < MHdr->DiffSize)
    {
        TRACE_MESSAGE3("Failed to load difference, difference buffer size %d, is not expected. Expected %d", differenceBuffer.size(), MHdr->DiffSize);
        return false;
    }
    const unsigned long crc = ~CRC32(&differenceBuffer[8], MHdr->DiffSize - 8, 0xffffffff);
    if(MHdr->DiffCrc32 != crc)
    {
        TRACE_MESSAGE3("Failed to load difference, CRC %d check failed, expected %d", crc, MHdr->DiffCrc32);
        return false;
    }

    // save diff pointer
    data.m_entryes[index] = differenceBuffer;
    return true;
}

bool preloadDiffs(DIFF_Data &data, const std::vector<unsigned char> &diffBuffer, KLUPD::Log *pLog)
{
    // check buffer range
    if((diffBuffer.size() <= sizeof(tDIFF_BIND_MHDR)) || (diffBuffer.size() <= sizeof(tDIFF_HDR)))
    {
        TRACE_MESSAGE4("Failed to preload difference, buffer size %d is not enough to fit binary difference header (sizes %d, %d)",
            diffBuffer.size(), sizeof(tDIFF_BIND_MHDR), sizeof(tDIFF_HDR));
        return false;
    }

    // check signature if 'DIFF' difference format
    if(memcmp(&diffBuffer[0], &DiffMark[0], 4) == 0)
        return loadDiff(data, 0, diffBuffer, pLog);

    // check signature if 'BDIF' binary difference format
    if(memcmp(&diffBuffer[0], &DiffBDIFMark[0], 4))
    {
        // not fatal error (no need to trace error), because other difference type will be tried
        return false;
    }

    // process binded diff
    const tDIFF_BIND_MHDR *MHdr = reinterpret_cast<const tDIFF_BIND_MHDR *>(&diffBuffer[0]);

    // check binded header
    if(MHdr->Version != DIFF_BIND_VERSION)
    {
        TRACE_MESSAGE3("Failed to preload difference version %d, expected version %d", MHdr->Version, DIFF_BIND_VERSION);
        return false;
    }
    if(MHdr->BDifSize > diffBuffer.size())
    {
        TRACE_MESSAGE3("Failed to preload difference, buffer size %d is not enough to fit loaded size %d", diffBuffer.size(), MHdr->BDifSize);
        return false;
    }
    const unsigned long crc = ~CRC32(&diffBuffer[8], MHdr->BDifSize-8, 0xffffffff);
    if(MHdr->BDifCrc32 != crc)
    {
        TRACE_MESSAGE3("Failed to preload difference, CRC %d check failed, expected %d", crc, MHdr->BDifCrc32);
        return false;
    }
    if(MHdr->ItemSize[0] == DIFF_BIND_LASTENTRY)
    {
        TRACE_MESSAGE3("Failed to preload difference, bind latency check failed %d, expected %d", MHdr->ItemSize[0], DIFF_BIND_LASTENTRY);
        return false;
    }

    // calculate entry number
    unsigned int Idx = 0;
    while(MHdr->ItemSize[Idx] != DIFF_BIND_LASTENTRY)
        ++Idx;

    // prosess entryes
    const unsigned char *DPtr = reinterpret_cast<const unsigned char *>(MHdr) + sizeof(tDIFF_BIND_MHDR)
        + (Idx * sizeof(MHdr->ItemSize[0]));

    const size_t ESize = MHdr->ItemSize[0];
    for(Idx = 0; DIFF_BIND_LASTENTRY != ESize; ++Idx)
    {
        if(!loadDiff(data, Idx, std::vector<unsigned char>(DPtr, DPtr + ESize), pLog))
            return false;

        DPtr += ESize;
    }
    return true;
}

static int getCompatibleDiffIdx(DIFF_Data &data)
{
    int compatibleIndex = 0;
    for(std::map<size_t, DIFF_Data::Entry>::iterator entryIter = data.m_entryes.begin(); entryIter != data.m_entryes.end(); ++entryIter, ++compatibleIndex)
    {
        if(entryIter->second.empty())
            continue;

        tDIFF_HDR *entry = reinterpret_cast<tDIFF_HDR *>(&(entryIter->second)[0]);
        if(entry->BaseSize != data.m_originalBuffer.size())
            continue;

        if(memcmp(entry->BaseHash, &data.BaseHash[0], 8))
            continue;

        return compatibleIndex;
    }

    // not found
    return -1;
}

bool packInAvc(const std::vector<unsigned char> &aveBuffer, std::vector<unsigned char> &avcBuffer, KLUPD::Log *pLog)
{
    std::vector<unsigned char> buffer;
    if(!transformBase(aveBuffer, buffer, true, pLog))
        return false;

    avcBuffer.insert(avcBuffer.end(), buffer.begin(), buffer.end());
    return true;
}

bool applyDiff(DIFF_Data &data, const unsigned int DIdx, std::vector<unsigned char> &hFileIO, KLUPD::Log *pLog)
{
    if(static_cast<signed int>(data.m_entryes.size()) < static_cast<signed int>(DIdx))
    {
        TRACE_MESSAGE3("Failed to apply difference, index %d out of range 0 - %d", DIdx, data.m_entryes.size());
        return false;
    }

    // alloc buffer for unpacked diff
    tDIFF_HDR *MHdr = reinterpret_cast<tDIFF_HDR *>(&(data.m_entryes[DIdx])[0]);

    std::vector<unsigned char> UDiff(MHdr->TblUSize, 0);

    // setup pointers
    unsigned char *DifData = (unsigned char *)(MHdr) + sizeof(tDIFF_HDR);
    unsigned char Sign[SIGN_SIZE];
    if(MHdr->Flags & DIFF_FLG_SIGN30)
    {
        memcpy(Sign, DifData, SIGN_SIZE);
        DifData += SIGN_SIZE;
    }

    if(MHdr->Flags & DIFF_FLG_TBLPACK)
    {
        // calc packed table size
        unsigned int PSize = MHdr->DiffSize - sizeof(tDIFF_HDR);
        if(MHdr->Flags & DIFF_FLG_SIGN30)
            PSize -= SIGN_SIZE;

        // unpack layer-2 diff data
        if(!UDiff.empty()
            && (LzmaDecodeMem2Mem(DifData, PSize, &UDiff[0], UDiff.size()) != LZMA_DECODE_ERR_OK))
        {
            TRACE_MESSAGE3("Failed to apply difference, lzma unpack error (size %d, unpack difference size %d)", PSize, UDiff.size());
            return false;
        }
    }
    // use unpacked diff
    else
        memcpy(&UDiff[0], DifData, MHdr->TblUSize);

    // check crc again (unpacked)
    const unsigned long crc = ~CRC32(&UDiff[0], MHdr->TblUSize, 0xFFFFFFFFL);
    if(MHdr->TblUCrc32 != crc)
    {
        TRACE_MESSAGE3("Failed to apply difference, CRC %d check error, expected %d", crc, MHdr->TblUCrc32);
        return false;
    }

    // place diff
    std::vector<unsigned char> RBuf;
    if(!applyDiffTable(data.m_originalBuffer, UDiff, RBuf, pLog))
        return false;

    // check size and hash
    unsigned char diffHash[8];
    calcDiffMD5HashFirst8Bytes(RBuf, diffHash);
    if((RBuf.size() != MHdr->CodeSize) || (memcmp(MHdr->CodeHash, diffHash, sizeof(diffHash)) != 0))
    {
        TRACE_MESSAGE3("Failed to apply difference, buffer size %d, code size %d", RBuf.size(), MHdr->CodeSize);
        return false;
    }

    // apply filter
    if(MHdr->Flags & DIFF_FLG_NAMEOFF2LEN)
    {
        if(!prepBase4Diff(RBuf, true, pLog))
            return false;
    }

    // pack SQZE format
    if(RBuf.size() > sizeof(CSqueezeHeader))
    {
        CSqueezeHeader *pSQZU = reinterpret_cast<CSqueezeHeader *>(&RBuf[0]);
        if((pSQZU->dwSignature == SQZU_HEADER_SIGNATURE) && pSQZU->dwVersion == 1)
        {
            std::vector<unsigned char> SQZE(pSQZU->dwHeaderSize);

            // put signature
            memcpy(&SQZE[0], pSQZU, pSQZU->dwHeaderSize);
            (reinterpret_cast<CSqueezeHeader *>(&SQZE[0]))->dwSignature = SQZE_HEADER_SIGNATURE;

            // compressing
            std::vector<unsigned char> compressedBuffer;
            const unsigned int compressedSize = squeeze(reinterpret_cast<unsigned char *>(pSQZU) + pSQZU->dwHeaderSize, compressedBuffer, pSQZU->dwOriginalSize);
            if(compressedSize != pSQZU->dwCompressedSize
                || compressedSize != compressedBuffer.size())
            {
                TRACE_MESSAGE4("Failed to apply difference, difference file corrupted, compressed size %d, size from header %d, result size %d",
                    compressedSize, pSQZU->dwCompressedSize, compressedBuffer.size());
                return false;
            }

            SQZE.insert(SQZE.end(), compressedBuffer.begin(), compressedBuffer.end());

            // validate checksum
            CSqueezeHeader *pSQZE = reinterpret_cast<CSqueezeHeader *>(&SQZE[0]);
            const unsigned long crc = ~CRC32((unsigned char *)pSQZE + pSQZE->dwHeaderSize, pSQZE->dwCompressedSize, 0xffffffff);
            if(pSQZE->dwCrc != crc)
            {
                TRACE_MESSAGE3("Failed to apply difference, CRC %d check failed, expected %d", pSQZE->dwCrc, crc);
                return false;
            }

            // write result to supplied buffer
            RBuf.assign(reinterpret_cast<unsigned char *>(pSQZE), reinterpret_cast<unsigned char *>(pSQZE) + pSQZU->dwHeaderSize + pSQZU->dwCompressedSize);
        }
    }

    // pack file to avc if needed
    if(0 != (MHdr->Flags & DIFF_FLG_PACK))
    {
        if(!packInAvc(RBuf, hFileIO, pLog))
            return false;

        // get packed data size
        RBuf.resize(hFileIO.size());
    }
    // write result
    else
        hFileIO.insert(hFileIO.end(), RBuf.begin(), RBuf.end());

    // write digital sign
    if(MHdr->Flags & DIFF_FLG_SIGN30)
        hFileIO.insert(hFileIO.end(), Sign, Sign + SIGN_SIZE);

    return true;
}

bool applyBinDiff(const std::vector<unsigned char> &originalBuffer, const std::vector<unsigned char> &diffFile, std::vector<unsigned char> &resultFile, KLUPD::Log *pLog)
{
    if(originalBuffer.empty() || diffFile.empty())
    {
        TRACE_MESSAGE3("Failed to apply binary difference, invalid parameter original size %d, difference size %d", originalBuffer.size(), diffFile.size());
        return false;
    }

    DIFF_Data data;

    DifferenceFormat differenceFormat = plainDifferenceFormat;

    // try AVC difference format
    if(unpackAVC(originalBuffer, data.m_originalBuffer, pLog))
        differenceFormat = avcDifferenceFormat;
    else if(uncompressSQZE(originalBuffer, data.m_originalBuffer, pLog))
        differenceFormat = sqzeDifferenceFormat;
    else if(KlavPack_IsPacked(originalBuffer))
    {
        differenceFormat = klavaDifferenceFormat;
        if(!KlavPack_DecodeVector(originalBuffer, data.m_originalBuffer))
        {
            TRACE_MESSAGE("Warning: KLAVA format detected, but failed to decode vector. Try plain difference file format");
            differenceFormat = plainDifferenceFormat;
        }
    }
    else if(KlavPack_KFB_IsPacked(originalBuffer))
    {
        differenceFormat = klavaKfbDifferenceFormat;

        if(!KlavPack_KFB_DecodeVector(originalBuffer, data.m_originalBuffer))
        {
            TRACE_MESSAGE("Warning: KLAVA KFB format detected, but failed to decode vector. Try plain difference file format");
            differenceFormat = plainDifferenceFormat;
        }
    }

    // use original file as input (binary difference file format)
    if(differenceFormat == plainDifferenceFormat)
        data.m_originalBuffer = originalBuffer;


    calcDiffMD5HashFirst8Bytes(data.m_originalBuffer, data.BaseHash);

    if(!preloadDiffs(data, diffFile, pLog))
        return false;

    const int differenceIndex = getCompatibleDiffIdx(data);
    if(differenceIndex == -1)
    {
        TRACE_MESSAGE("Failed to apply binary difference, compatible index is not found");
        return false;
    }

    if(!applyDiff(data, differenceIndex, resultFile, pLog))
        return false;

    if(differenceFormat == klavaDifferenceFormat)
    {
        std::vector<unsigned char> resultKlavaFile;
        if(!KlavPack_EncodeVector(resultFile, resultKlavaFile))
        {
            TRACE_MESSAGE("Failed to encode KLAVA buffer");
            return false;
        }
        resultKlavaFile.swap(resultFile);
    }
    else if(differenceFormat == klavaKfbDifferenceFormat)
    {
        std::vector<unsigned char> resultKlavaKfbFile;
        if(!KlavPack_KFB_EncodeVector(resultFile, resultKlavaKfbFile))
        {
            TRACE_MESSAGE("Failed to encode KLAVA Kfb buffer");
            return false;
        }
        resultKlavaKfbFile.swap(resultFile);
    }
    return true;
}

bool packSingleKLZ_Mem(const std::vector<unsigned char> &originalBuffer, std::vector<unsigned char> &resultFile, KLUPD::Log *pLog)
{
    if(originalBuffer.empty())
    {
        TRACE_MESSAGE("Failed to pack KLZ, empty buffer pack is not supported");
        return false;
    }

    // reserve memory in advance
    unsigned int resultSize = originalBuffer.size() + 64 * 1024;
    resultFile.resize(resultSize);
    if((LzmaEncodeMem2Mem(&originalBuffer[0], originalBuffer.size(), &resultFile[0], &resultSize) != LZMA_DECODE_ERR_OK)
        || resultFile.empty())
    {
        TRACE_MESSAGE("Failed to pack KLZ, lzma pack error");
        return false;
    }
    resultFile.resize(resultSize);


    // append KLZ header
    KLZ_HDR hdr;
    // write magic number signature
    memcpy(hdr.m_magicNumberSign, g_bKLZSign, 4);
    hdr.m_packedSize = resultSize;
    hdr.m_unpackedSize = originalBuffer.size();
    hdr.m_headerSize = sizeof(hdr);
    hdr.m_packedBufferCRC = CRC32(&resultFile[0], resultFile.size(), 0);
    resultFile.insert(resultFile.begin(), reinterpret_cast<unsigned char *>(&hdr), reinterpret_cast<unsigned char *>(&hdr) + sizeof(hdr));
    return true;
}

bool unpackSingleKLZ_Mem(const std::vector<unsigned char> &originalBuffer, std::vector<unsigned char> &resultFile, KLUPD::Log *pLog)
{
    if(originalBuffer.size() < sizeof(KLZ_HDR))
    {
        TRACE_MESSAGE3("Failed to unpack KLZ, buffer size %d is not enough to fit KLZ header %d", originalBuffer.size(), sizeof(KLZ_HDR));
        return false;
    }
    const KLZ_HDR *hdr = reinterpret_cast<const KLZ_HDR *>(&originalBuffer[0]);

    // check magic number signature
    if(memcmp(hdr->m_magicNumberSign, g_bKLZSign, 4) != 0)
    {
        TRACE_MESSAGE("Failed to unpack KLZ, signature check failed");
        return false;
    }

    if((static_cast<signed int>(hdr->m_packedSize) < 1) || (static_cast<signed int>(hdr->m_unpackedSize) < 1))
    {
        TRACE_MESSAGE3("Failed to unpack KLZ, header is corrupted, packed size %d, unpacked size %d",
            hdr->m_packedSize, hdr->m_unpackedSize);
        return false;
    }
    if(originalBuffer.size() < hdr->m_headerSize + hdr->m_packedSize)
    {
        TRACE_MESSAGE4("Failed to unpack KLZ, header is corrupted, original size %d, header size %d, packed size %d",
            originalBuffer.size(), hdr->m_headerSize, hdr->m_packedSize);
        return false;
    }

    // packed data, size of data = hdr.m_packedSize
    const unsigned char *packedData = &originalBuffer[0] + hdr->m_headerSize;


    const unsigned long crc = CRC32(packedData, hdr->m_packedSize, 0);
    if(crc != hdr->m_packedBufferCRC)
    {
        TRACE_MESSAGE3("Failed to unpack KLZ, CRC %d check failed, expected %d", crc, hdr->m_packedBufferCRC);
        return false;
    }

    resultFile.resize(hdr->m_unpackedSize);
    memset(&resultFile[0], 0, resultFile.size());

    if(!resultFile.empty()
        && (LzmaDecodeMem2Mem(packedData, hdr->m_packedSize, &resultFile[0], resultFile.size()) != LZMA_DECODE_ERR_OK))
    {
        TRACE_MESSAGE3("Failed to unpack KLZ, lzma unpack error, header compressed size %d, header decompressed size %d",
            hdr->m_packedSize, resultFile.size());
        return false;
    }
    return true;
}

bool diffsApplySingleDiff(const KLUPD::Path &originalFullFileName, const KLUPD::Path &differenceFullFileName,
                          const KLUPD::Path &resultFullFileName, KLUPD::Log *pLog)
{
    KLUPD::LocalFile originalFile(originalFullFileName);
    std::vector<unsigned char> originalBuffer;
    if(!KLUPD::isSuccess(originalFile.read(originalBuffer)))
        return false;

    KLUPD::LocalFile differenceFile(differenceFullFileName);
    std::vector<unsigned char> differenceBuffer;
    if(!KLUPD::isSuccess(differenceFile.read(differenceBuffer)))
        return false;

    std::vector<unsigned char> resultBuffer;
    if(!applyBinDiff(originalBuffer, differenceBuffer, resultBuffer, pLog))
        return false;

    KLUPD::LocalFile resultFile(resultFullFileName);
    const KLUPD::CoreError writeResult = resultFile.write(resultBuffer);
    if(!KLUPD::isSuccess(writeResult))
    {
        TRACE_MESSAGE5("Failed to apply difference on '%S', difference file '%S', unable to write to result file '%S', result '%S'",
            originalFullFileName.toWideChar(), differenceFullFileName.toWideChar(),
            resultFullFileName.toWideChar(), KLUPD::toString(writeResult).toWideChar());
        return false;
    }
    return true;
}


//////////////////////////////////////////////////////////////////////////
///////// BEGIN: this code works with difference files for index /////////
//////////////////////////////////////////////////////////////////////////

static int numberFromAscii(const char *s, const size_t size)
{
    size_t n = 0;
    for(size_t i = 0; i < size; ++i)
    {
        if((s[i] < '0') || (s[i] > '9'))
            return 0;
        n *= 10;
        n += s[i] - '0';
    }
    return n;
}

time_t tm2time(struct tm *ptm)
{
    unsigned long time = 0;
    time =  static_cast<unsigned long>(ptm->tm_year) * 60 * 60 * 24 * 31 * 12;
    time += static_cast<unsigned long>(ptm->tm_mon)  * 60 * 60 * 24 * 31;
    time += static_cast<unsigned long>(ptm->tm_mday) * 60 * 60 * 24;
    time += static_cast<unsigned long>(ptm->tm_hour) * 60 * 60;
    time += static_cast<unsigned long>(ptm->tm_min)  * 60;
    time += static_cast<unsigned long>(ptm->tm_sec);
    return (time_t)time;
}

static time_t getXMLTimeStamp(const std::vector<unsigned char> &buffer)
{
    char buff[0x200];
    memset(buff, 0, sizeof(buff));
    // find minimum in such way to avoid compilation problem with macro min() and std::min on different platforms
    const size_t bytes = (sizeof(buff) - 1 < buffer.size())
        ? (sizeof(buff) - 1) : buffer.size();
    memcpy(buff, &buffer[0], bytes);

    buff[bytes] = 0; // zero-terminate string
    char *timestamp = strstr(buff, "UpdateDate=\"");
    if(!timestamp)
        return 0;
    timestamp += 12;
    if(strlen(timestamp) < 13)
        return 0;
    if(timestamp[13] != '\"')
        return 0;
    if(timestamp[8] != ' ')
        return 0;

    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_mday = numberFromAscii(timestamp + 0, 2);
    t.tm_mon  = numberFromAscii(timestamp + 2, 2) - 1;
    t.tm_year = numberFromAscii(timestamp + 4, 4) - 1970;
    t.tm_hour = numberFromAscii(timestamp + 9, 2);
    t.tm_min  = numberFromAscii(timestamp + 11, 2);
    const time_t _time = tm2time(&t);
    if(_time == (time_t) - 1)
        return 0;
    return static_cast<long>(_time);
}

static bool GetNum(const unsigned char *sourceIn, const unsigned long sourceSizeIn, unsigned long &pnNum, unsigned long &bytesRead)
{
    if(static_cast<const signed int>(sourceSizeIn) < 0)
        return false;

    const unsigned char *source = sourceIn;
    unsigned long sourceSize = sourceSizeIn;

    unsigned long num = 0;
    unsigned char b = 0;
    size_t shift = 0;
    bytesRead = 0;
    do
    {
        if(!sourceSize)
            return false;

        b = *reinterpret_cast<const unsigned char *>(source);
        ++source;
        ++bytesRead;
        --sourceSize;

        num |= (b & 0x7F) << shift;
        shift += 7;
    } while (b & 0x80);
    pnNum = num;
    return true;
}

static bool readDifferenceForXmlHeader(const unsigned char *pFile, const size_t bufferSize, tXML_DIFF_HDR &phdr, unsigned long &bytes_read, KLUPD::Log *pLog)
{
    if(bufferSize < sizeof(tXML_DIFF_HDR))
    {
        TRACE_MESSAGE3("Failed to read difference for XML header, buffer size %d is not enought to fit XML difference header %d bytes", bufferSize, sizeof(tXML_DIFF_HDR));
        return false;
    }

    const tXML_DIFF_HDR *p = reinterpret_cast<const tXML_DIFF_HDR *>(pFile);
    const unsigned long size = sizeof(p->size);

    phdr.from_ver = p->from_ver;
    phdr.to_ver = p->to_ver;

    const unsigned char *x = reinterpret_cast<const unsigned char *>(&(p->size));
    unsigned long bytesRead = 0;
    if(!GetNum(x, size, phdr.size, bytesRead))
    {
        TRACE_MESSAGE("Failed to read difference for XML header, failed to get header size");
        return false;
    }

    x += bytesRead;

    // x incremented and points to next field
    phdr.incremental = *x;
    bytes_read = sizeof(p->from_ver) + sizeof(p->from_ver) + bytesRead + 1;
    return true;
}

static bool applySingleXMLDifferenceFile(const std::vector<unsigned char> &originalBuffer,
                                         // pointer + size used, not std::vector<> to avoid memory coping
                                         const unsigned char *differenceBuffer, const size_t differenceSize,
                                         std::vector<unsigned char> &resultBuffer, KLUPD::Log *pLog)
{
    size_t originalBufferOffset = 0;
    size_t differenceBufferOffset = 0;

    while(true)
    {
        // unsigned long copy = 8, cut = 8, insert = 8;
        unsigned long num1 = 0;
        unsigned long bytesRead1 = 0;

        if(!GetNum(differenceBuffer + differenceBufferOffset, differenceSize - differenceBufferOffset, num1, bytesRead1))
        {
            TRACE_MESSAGE("Failed to read single difference for XML header, failed to get header first number");
            return false;
        }
        differenceBufferOffset += bytesRead1;

        unsigned long num2 = 0;
        unsigned long bytesRead2 = 0;
        if(!GetNum(differenceBuffer + differenceBufferOffset, differenceSize - differenceBufferOffset, num2, bytesRead2))
        {
            TRACE_MESSAGE("Failed to read single difference for XML header, failed to get header second number");
            return false;
        }
        differenceBufferOffset += bytesRead2;

        unsigned long num3 = 0;
        unsigned long bytesRead3 = 0;
        if(!GetNum(differenceBuffer + differenceBufferOffset, differenceSize - differenceBufferOffset, num3, bytesRead3))
        {
            TRACE_MESSAGE("Failed to read single difference for XML header, failed to get header third number");
            return false;
        }
        differenceBufferOffset += bytesRead3;

        if(num1)
        {
            if(originalBuffer.size() < num1 + originalBufferOffset)
            {
                TRACE_MESSAGE4("Failed to read single difference for XML header, original buffer %d is not enought to fit first chunk %d bytes, current offset %d", originalBuffer.size(), num1, originalBufferOffset);
                return false;
            }
            resultBuffer.insert(resultBuffer.end(), &originalBuffer[0] + originalBufferOffset, &originalBuffer[0] + originalBufferOffset + num1);
            originalBufferOffset += num1;
        }
        if(num2)
            originalBufferOffset += num2;
        if(num3)
        {
            if(differenceSize < differenceBufferOffset + num3)
            {
                TRACE_MESSAGE4("Failed to read single difference for XML header, original buffer %d is not enought to fit third chunk %d bytes, current offset %d", originalBuffer.size(), num3, originalBufferOffset);
                return false;
            }
            resultBuffer.insert(resultBuffer.end(), differenceBuffer + differenceBufferOffset,
                differenceBuffer + differenceBufferOffset + num3);
            differenceBufferOffset += num3;
        }

        if((!num1 && !num2 && !num3) || (differenceSize <= differenceBufferOffset))
        {
            if(resultBuffer.empty())
            {
                TRACE_MESSAGE("Failed to read single difference for XML header, result buffer is empty");
                return false;
            }
            return true;
        }
    }
}


static bool applyXmlDiffSet(const std::vector<unsigned char> &originalFileBuffer,
                            const std::vector<unsigned char> &differenceFileBuffer, std::vector<unsigned char> &resultBuffer, KLUPD::Log *pLog)
{
    tXML_DIFF_HDR hdr;
    unsigned long nLastNonIncrDiff = 0;
    const time_t old_ver = getXMLTimeStamp(originalFileBuffer);
    if(!old_ver)
    {
        TRACE_MESSAGE("Failed apply XML difference set, old version is not parsed");
        return false;
    }

    // check magic number
    long signature = 0;
    if(differenceFileBuffer.size() < sizeof(signature))
    {
        TRACE_MESSAGE2("Failed apply XML difference set, buffer is not enough %d to fit signature", differenceFileBuffer.size());
        return false;
    }
    memcpy(&signature, &differenceFileBuffer[0], sizeof(signature));
    if((signature != DIF_SIGNATURE)
        && (signature != DIF_SIGNATURE2))
    {
        TRACE_MESSAGE4("Failed apply XML difference set, signature does not suit %d, expected %d or %d", signature, DIF_SIGNATURE, DIF_SIGNATURE2);
        return false;
    }

    unsigned long nCurPos = sizeof(signature);

    // index file version
    if(differenceFileBuffer.size() < nCurPos + 2 * sizeof(long))
    {
        TRACE_MESSAGE2("Failed apply XML difference set, buffer size %d is not enough to get index file version", differenceFileBuffer.size());
        return false;
    }
    const long dest_ver = *(long *)(&differenceFileBuffer[0] + nCurPos);
    nCurPos += sizeof(dest_ver);


    // extended header
    std::vector<unsigned char> md5FromHeader;
    const size_t md5Size = 16;
    if(signature == DIF_SIGNATURE2)
    {
        unsigned long ext_hdr_len = 0;
        unsigned long bytesRead = 0;
        if(!GetNum(&differenceFileBuffer[0] + nCurPos, differenceFileBuffer.size() - nCurPos, ext_hdr_len, bytesRead))
        {
            TRACE_MESSAGE("Failed apply XML difference set, failed to get extended header size");
            return false;
        }
        nCurPos += bytesRead;

        if(differenceFileBuffer.size() < nCurPos + md5Size)
        {
            TRACE_MESSAGE3("Failed apply XML difference set, buffer size %d is not enough to get md5 size, current position %d", differenceFileBuffer.size(), nCurPos);
            return false;
        }
        md5FromHeader.assign(&differenceFileBuffer[0] + nCurPos, &differenceFileBuffer[0] + nCurPos + md5Size);

        // extended header contains hash and other information
        nCurPos += ext_hdr_len;
    }


    // already latest version
    if(old_ver == dest_ver)
    {
        resultBuffer = originalFileBuffer;
        return true;
    }

    while(true)
    {
        // no more records
        if(differenceFileBuffer.size() < nCurPos)
        {
            TRACE_MESSAGE("Warining: unable to apply XML difference set, no more records. Original file may be too much obsolete");
            return false;
        }

        unsigned long bytes_read = 0;
        if(!readDifferenceForXmlHeader(&differenceFileBuffer[0] + nCurPos, differenceFileBuffer.size() - nCurPos, hdr, bytes_read, pLog))
        {
            TRACE_MESSAGE("Unable to apply XML difference set, failed to read XML header");
            return false;
        }
        if(!hdr.size)
        {
            TRACE_MESSAGE("Warining: unable to apply XML difference set, record is empty. Original file may be too much obsolete");
            return false;
        }

        if(!hdr.incremental)
            nLastNonIncrDiff = nCurPos;

        if(hdr.from_ver > hdr.to_ver)
        {
            unsigned long tmp = hdr.from_ver;
            hdr.from_ver = hdr.to_ver;
            hdr.to_ver = tmp;
        }

        nCurPos += bytes_read;

        // diff found
        if(old_ver >= hdr.from_ver && old_ver <= hdr.to_ver)
            break;

        nCurPos += hdr.size;
    }

    // diff found
    if(hdr.incremental)
    {
        const unsigned long nLastPos = nCurPos;
        if(!nLastNonIncrDiff)
        {
            TRACE_MESSAGE("Failed apply XML difference set, non last incremental difference");
            return false;
        }

        nCurPos = nLastNonIncrDiff;
        unsigned long bytesRead = 0;
        if(!readDifferenceForXmlHeader(&differenceFileBuffer[0] + nCurPos, differenceFileBuffer.size() - nCurPos, hdr, bytesRead, pLog))
        {
            TRACE_MESSAGE("Failed apply XML difference set, unable to read incremental XML header");
            return false;
        }
        nCurPos += bytesRead;


        // coping difference into temporary buffer
        if(differenceFileBuffer.size() < nCurPos + hdr.size)
        {
            TRACE_MESSAGE4("Failed apply XML difference set, unable to fit incremental header, difference size %d, current position %d, header size %d",
                differenceFileBuffer.size(), nCurPos, hdr.size);
            return false;
        }
        std::vector<unsigned char> tempFile(&differenceFileBuffer[0] + nCurPos, &differenceFileBuffer[0] + nCurPos + hdr.size);
        nCurPos += hdr.size;

        do 
        {
            if(nLastPos <= nCurPos)
            {
                TRACE_MESSAGE3("Failed apply XML incremental difference set, current position %d exceeded difference buffer size %d", nLastPos, nCurPos);
                return false;
            }

            unsigned long bytesReadHeader = 0;
            if(!readDifferenceForXmlHeader(&differenceFileBuffer[0] + nCurPos, differenceFileBuffer.size() - nCurPos, hdr, bytesReadHeader, pLog))
            {
                TRACE_MESSAGE("Failed apply XML incremental difference set, unable to read XML header");
                return false;
            }
            nCurPos += bytesReadHeader;

            // apply difference on difference
            std::vector<unsigned char> tempFile2;
            if(!applySingleXMLDifferenceFile(tempFile, &differenceFileBuffer[0] + nCurPos, differenceFileBuffer.size() - nCurPos, tempFile2, pLog))
                return false;

            // swap temp files
            tempFile = tempFile2;
        } while(nCurPos < nLastPos);

        // full difference now in tempFile
        if(!applySingleXMLDifferenceFile(originalFileBuffer, &tempFile[0], tempFile.size(), resultBuffer, pLog))
            return false;
    }
    else
    {
        if(!applySingleXMLDifferenceFile(originalFileBuffer, &differenceFileBuffer[0] + nCurPos, differenceFileBuffer.size() - nCurPos, resultBuffer, pLog))
            return false;
    }

    // MD5 check for DIF_SIGNATURE2 compression
    if(signature == DIF_SIGNATURE2)
    {
        std::vector<unsigned char> md5FromFile(16);
        calcMD5Hash(&resultBuffer[0], resultBuffer.size(), &md5FromFile[0]);
        if(md5FromFile != md5FromHeader)
        {
            TRACE_MESSAGE("Failed apply XML difference, result signature check failed");
            return false;
        }
    }
    return true;
}

bool applyDifferenceOnIndexFile(const KLUPD::Path &originalFullFileName, const KLUPD::Path &differenceFullFileName,
                                const KLUPD::Path &resultFullFileName, KLUPD::Log *pLog)
{
    // read original file content
    std::vector<unsigned char> originalFileBuffer;
    KLUPD::LocalFile originalFile(originalFullFileName, 0);
    if(!KLUPD::isSuccess(originalFile.read(originalFileBuffer)))
        return false;

    // read difference file content
    std::vector<unsigned char> differenceFileBuffer;
    KLUPD::LocalFile differenceFile(differenceFullFileName, 0);
    if(!KLUPD::isSuccess(differenceFile.read(differenceFileBuffer)))
        return false;

    // check to avoid dereference 0-pointer
    if(originalFileBuffer.empty() || differenceFileBuffer.empty())
        return false;

    // apply difference
    std::vector<unsigned char> resultBuffer;
    if(!applyXmlDiffSet(originalFileBuffer, differenceFileBuffer, resultBuffer, pLog))
        return false;

    // write result to file
    const KLUPD::CoreError writeResult = KLUPD::LocalFile(resultFullFileName, pLog).write(resultBuffer);
    if(!KLUPD::isSuccess(writeResult))
    {
        TRACE_MESSAGE3("Failed to apply difference file on index, failed to write to '%S', result '%S'",
            resultFullFileName.toWideChar(), KLUPD::toString(writeResult).toWideChar());
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
////////// END: this code works with difference files for index //////////
//////////////////////////////////////////////////////////////////////////

bool packSingleKLZ(const KLUPD::Path &originalFullFileName, const KLUPD::Path &resultFullFileName, KLUPD::Log *pLog)
{
    std::vector<unsigned char> originalBuffer;
    if(!KLUPD::isSuccess(KLUPD::LocalFile(originalFullFileName).read(originalBuffer))
        || originalBuffer.empty())
    {
        return false;
    }

    std::vector<unsigned char> resultFile;
    if(!packSingleKLZ_Mem(originalBuffer, resultFile, pLog))
        return false;

    const KLUPD::CoreError writeResult = KLUPD::LocalFile(resultFullFileName).write(resultFile);
    if(!KLUPD::isSuccess(writeResult))
    {
        TRACE_MESSAGE3("Failed to unpack single KLZ, write to result file '%S', error '%S'",
            resultFullFileName.toWideChar(), KLUPD::toString(writeResult).toWideChar());
        return false;
    }
    return true;
}

bool unpackSingleKLZ(const KLUPD::Path &originalFullFileName, const KLUPD::Path &resultFullFileName, KLUPD::Log *pLog)
{
    std::vector<unsigned char> originalBuffer;
    if(!KLUPD::isSuccess(KLUPD::LocalFile(originalFullFileName).read(originalBuffer))
        || originalBuffer.empty())
    {
        return false;
    }

    std::vector<unsigned char> resultFile;
    if(!unpackSingleKLZ_Mem(originalBuffer, resultFile, pLog))
        return false;

    const KLUPD::CoreError writeResult = KLUPD::LocalFile(resultFullFileName).write(resultFile);
    if(!KLUPD::isSuccess(writeResult))
    {
        TRACE_MESSAGE3("Failed to unpack single KLZ, write to result file '%S', error '%S'",
            resultFullFileName.toWideChar(), KLUPD::toString(writeResult).toWideChar());
        return false;
    }
    return true;
}
