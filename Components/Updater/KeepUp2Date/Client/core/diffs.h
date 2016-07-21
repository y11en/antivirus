#ifndef _DIFFS_H_INCLUDED_07BDDB32_A7B1_4b30_AF7C_E866F07E7506
#define _DIFFS_H_INCLUDED_07BDDB32_A7B1_4b30_AF7C_E866F07E7506

#include "../helper/stdinc.h"
#include "../helper/path.h"

extern bool diffsApplySingleDiff(const KLUPD::Path &originalFullFileName,
                                 const KLUPD::Path &differenceFullFileName,
                                 const KLUPD::Path &resultFullFileName,
                                 KLUPD::Log *);
extern bool applyDifferenceOnIndexFile(const KLUPD::Path &originalFullFileName,
                                       const KLUPD::Path &differenceFullFileName,
                                       const KLUPD::Path &resultFullFileName,
                                       KLUPD::Log *);

extern bool packSingleKLZ(const KLUPD::Path &originalFullFileName, const KLUPD::Path &resultFullFileName, KLUPD::Log *);
extern bool unpackSingleKLZ(const KLUPD::Path &originalFullFileName, const KLUPD::Path &resultFullFileName, KLUPD::Log *);

// internal function
bool applyDiffTable(const std::vector<unsigned char> &original, const std::vector<unsigned char> &difference,
                    std::vector<unsigned char> &result, KLUPD::Log *);


#endif  // #ifndef _DIFFS_H_INCLUDED_07BDDB32_A7B1_4b30_AF7C_E866F07E7506
