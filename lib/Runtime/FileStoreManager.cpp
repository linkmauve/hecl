#include "hecl/Runtime.hpp"

#include "hecl/hecl.hpp"

#include <logvisor/logvisor.hpp>

#if _WIN32
#include <ShlObj.h>
#endif

#if WINDOWS_STORE
using namespace Windows::Storage;
#endif

namespace hecl::Runtime {
static logvisor::Module Log("FileStoreManager");

FileStoreManager::FileStoreManager(SystemStringView domain) : m_domain(domain) {
#if _WIN32
#if !WINDOWS_STORE
  WCHAR home[MAX_PATH];
  if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, home)))
    Log.report(logvisor::Fatal, fmt(_SYS_STR("unable to locate profile for file store")));

  SystemString path(home);
#else
  StorageFolder ^ cacheFolder = ApplicationData::Current->LocalCacheFolder;
  SystemString path(cacheFolder->Path->Data());
#endif
  path += _SYS_STR("/.heclrun");

  hecl::MakeDir(path.c_str());
  path += _SYS_STR('/');
  path += domain.data();

  hecl::MakeDir(path.c_str());
  m_storeRoot = path;
#else
  const char* home = getenv("HOME");
  if (!home)
    Log.report(logvisor::Fatal, fmt("unable to locate $HOME for file store"));
  std::string path(home);
  path += "/.heclrun";
  if (mkdir(path.c_str(), 0755) && errno != EEXIST)
    Log.report(logvisor::Fatal, fmt("unable to mkdir at {}"), path);
  path += '/';
  path += domain.data();
  if (mkdir(path.c_str(), 0755) && errno != EEXIST)
    Log.report(logvisor::Fatal, fmt("unable to mkdir at {}"), path);
  m_storeRoot = path;
#endif
}

} // namespace hecl::Runtime
