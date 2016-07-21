class Platform :
    def __init__(self) :
        self.incpaths = []
        self.libpaths = []

    def builddir(self) :
        return 'build'
                
    def ccflags(self) :
        return ''

    def cxxflags(self) :
        return ''

    def linkflags(self) :
        return ''

    def custom(self, env) :
        pass

class MSVCWindows(Platform) :
    def __init__(self) :
        Platform.__init__(self)

    def ccflags(self) :
        return '/DWIN32 /wd4996 /wd4127'

    def cxxflags(self) :
        return '/EHsc /W4'

    def linkflags(self) :
        return '/SUBSYSTEM:CONSOLE'

    def custom(self, env) :
        env['WINDOWS_INSERT_MANIFEST'] = 1
        
class MSVCWindowsDebug(MSVCWindows) :
    def __init__(self) :
        MSVCWindows.__init__(self)

    def builddir(self) :
        return 'Debug'
                
    def ccflags(self) :
        return MSVCWindows.ccflags(self) + ' /Od /MDd /D_DEBUG /Z7'

    def linkflags(self) :
        return MSVCWindows.linkflags(self) + ' /DEBUG'

class MSVCWindowsRelease(MSVCWindows) :
    def __init__(self) :
        MSVCWindows.__init__(self)
        
    def builddir(self) :
        return 'Release'
                
    def ccflags(self) :
        return MSVCWindows.ccflags(self) + ' /Ox /MD /D_NDEBUG'

class LinuxGCC(Platform) :
    def __init__(self) :
        Platform.__init__(self)

class LinuxGCCDebug(LinuxGCC) :
    def __init__(self) :
        LinuxGCC.__init__(self)

    def builddir(self) :
        return 'debug'
                
    def ccflags(self) :
        return '-g -O0 -I.'

class LinuxGCCRelease(LinuxGCC) :
    def __init__(self) :
        LinuxGCC.__init__(self)

    def builddir(self) :
        return 'release'
                
    def ccflags(self) :
        return '-O2 -I.'

platforms = {}

platforms['windows-msvc-debug'] = MSVCWindowsDebug()
platforms['windows-msvc-release'] = MSVCWindowsRelease()

platforms['linux-gcc-debug'] = LinuxGCCDebug()
platforms['linux-gcc-release'] = LinuxGCCRelease()

env['PLATFORMS'] = platforms
