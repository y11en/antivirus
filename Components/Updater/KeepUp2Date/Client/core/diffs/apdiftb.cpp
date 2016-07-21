#include "../diffs.h"

#define inlinedMemcpy(destination, source, count)   \
{                                                   \
  unsigned char *dgmcdst = destination;             \
  const unsigned char *dgmcsrc = source;            \
  unsigned int dgmccnt = count;                     \
  while(dgmccnt--)                                  \
     *dgmcdst++ = *dgmcsrc++;                       \
}

// read block from memory stream
static unsigned long loopread(const unsigned char *inputBuffer,
                              size_t &offset,
                              const size_t bufferSize,
                              unsigned char *buf,
                              unsigned long cnt)
{
    if((offset + cnt) > bufferSize)
        cnt -= (cnt - (bufferSize - offset));
    
    inlinedMemcpy(buf, inputBuffer + offset, cnt);
    offset += cnt;
    return(cnt);
}

// decode big-endian signed dword
#define offtin(buf) (size_t)(((unsigned char*)(buf))[3] |               \
  (((unsigned char*)(buf))[2]<<8) | (((unsigned char*)(buf))[1]<<16) | \
  (((unsigned char*)(buf))[0]<<24))


bool applyDiffTable(const std::vector<unsigned char> &original, const std::vector<unsigned char> &difference,
                    std::vector<unsigned char> &result, KLUPD::Log *pLog)
{
    // check parameters
    if(original.empty() || difference.empty())
    {
        TRACE_MESSAGE3("Failed to apply difference table, invalid argument original size %d, difference size %d", original.empty(), difference.empty())
        return false;
    }

    size_t currentOffset = 0;
    
    // read difference file header
    unsigned char buf[16];
    const unsigned long loreadResult = loopread(&difference[0], currentOffset, difference.size(), buf, 16);
    if(16 != loreadResult)
    {
        TRACE_MESSAGE2("Failed to apply difference table, read difference size %d", loreadResult)
        return false;
    }
    
    /* diff table header format      */
    /* +00 [dword] : ctrl block len  */
    /* +04 [dword] : new file len    */
    /* +08 [dword] : diff block len  */
    /* +12 [dword] : extra block len */
    const unsigned long bzctrllen  = (unsigned long)offtin(buf);
    const unsigned long newsize = (unsigned long)offtin(buf + 4);
    const unsigned long bzdatalen  = (unsigned long)offtin(buf + 8);
    const unsigned long bzextralen = (unsigned long)offtin(buf + 12);
    
    // check total diff size
    if((bzctrllen + bzdatalen + bzextralen + 16) > difference.size())
    {
        TRACE_MESSAGE5("Failed to apply difference table, buffer size %d range check error bzctrllen=%d, bzdatalen=%d, bzextralen=%d",
            difference.size(), bzctrllen, bzdatalen, bzextralen);
        return false;
    }

    // init mem files
    const unsigned char *ctrlpipe = &difference[0] + 16;
    size_t ctrlpipeOffset = 0;
    const unsigned char *diffpipe = ctrlpipe + bzctrllen;
    size_t diffpipeOffset = 0;
    const unsigned char *extrpipe = diffpipe + bzdatalen;
    size_t extrpipeOffset = 0;

    result.resize(newsize + 16);
    
    // apply patch
    size_t oldpos = 0;
    size_t newpos = 0;
    size_t ctrl[3];
    size_t lenread = 0;
    while((unsigned long)newpos < newsize)
    {
        size_t i;
        for(i = 0; i < 3; ++i)
        {
            lenread = loopread(ctrlpipe, ctrlpipeOffset, bzctrllen, buf, 4);
            if(4 != lenread)
            {
                TRACE_MESSAGE2("Failed to apply difference table, read length %d", loreadResult)
                return false;
            }

            ctrl[i] = offtin(buf);
        }

        if(newsize < newpos + ctrl[0])
        {
            TRACE_MESSAGE4("Failed to apply difference table, size check error new size %d, new position %d, control %d", newsize, newpos, ctrl[0]);
            return false;
        }

        lenread = loopread(diffpipe, diffpipeOffset, bzdatalen, &result[newpos], ctrl[0]);
        if(ctrl[0] != lenread)
        {
            TRACE_MESSAGE3("Failed to apply difference table, read length %d, control %d", loreadResult, ctrl[0]);
            return false;
        }

        for(i = 0; i < ctrl[0]; ++i)
        {
            if((oldpos + i >= 0) && (oldpos + i < original.size()))
                result[newpos + i] += original[oldpos + i];
        }

        newpos += ctrl[0];
        oldpos += ctrl[0];
        
        if(static_cast<signed long>(newsize) < static_cast<signed long>(newpos + ctrl[1]))
        {
            TRACE_MESSAGE4("Failed to apply difference table, new size %d check failed, new position %d, control %d", newsize, newpos, ctrl[1]);
            return false;
        }

        lenread = loopread(extrpipe, extrpipeOffset, bzextralen, &result[newpos], ctrl[1]);
        if(ctrl[1] != lenread)
        {
            TRACE_MESSAGE3("Failed to apply difference table, read length %d differ from expected %d", lenread, ctrl[1]);
            return false;
        }
        
        newpos += ctrl[1];
        oldpos += ctrl[2];
    }

    result.resize(newsize);
    return true;
}

