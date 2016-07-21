#ifndef WRITESS_H
#define WRITESS_H

void ReadSSInstallXML(const char *szSourceDir, char *props, bool& uninstall);

void WriteSetupResult(int   res1,
                      int   res2,
                      const char *resstr,
                      bool  silent,
                      const char *taskid, 
                      const char *resfiledir, 
                      const char *zombie
                     );

#endif //WRITESS_H