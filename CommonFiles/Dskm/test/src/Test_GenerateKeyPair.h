#if !defined(__KL_DSKMWRAP_TEST_GENERATEKEYPAIR_H)
#define __KL_DSKMWRAP_TEST_GENERATEKEYPAIR_H

#include <string>
#include <dskm/cpp_wrap/dskmwrap.h>

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

bool Test_GenerateKeyPair_SomeCase(std::vector<std::string>& errors);
bool Test_GenerateKeyPair_SignRegFile(std::vector<std::string>& errors);

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_TEST_GENERATEKEYPAIR_H)
