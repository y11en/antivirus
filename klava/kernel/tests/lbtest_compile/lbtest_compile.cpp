#include "precompile.h"

bool compileLbt(char const* buffer, size_t bufferSize, uint32_t vendorId, uint32_t blockId, uint32_t blockSeq, KLAV_Bin_Writer* writer);

class KDUFileWriter : public KLAV_Bin_Writer
{
public:
	KDUFileWriter(FILE* file, bool closeOnDestroy = false) : 
		m_file(file), m_closeOnDestroy(closeOnDestroy) 
		{}

	~KDUFileWriter()
	{
		if (m_closeOnDestroy && m_file != 0)
		{
			fclose (m_file);
			m_file = 0;
		}
	}

	klav_bool_t KL_CDECL write(void const* data, size_t size)
	{ 
		return fwrite(data, size, 1, m_file) == 1;
	}

private:
	FILE * m_file;
	bool   m_closeOnDestroy;
};

void usage()
{
	fprintf(stderr, "Use: lbtest_compile option <lbt file name> [<kdu file name>]\n");
	fprintf(stderr, "options are:\n");
	fprintf(stderr, " [-b <blockId>]\n");
	fprintf(stderr, " [-s <blockSeq>]\n");
}

int main(int argc, char* argv[])
{
	char const* lbtPath = NULL;
	char const* kduPath = NULL;

	bool blockIdSpecified = false;
	unsigned blockId = 0;
	bool blockSeqSpecified = false;
	unsigned blockSeq = 0;

	int a = 1;

	for (; a < argc; ++a)
	{
		char const* arg = argv[a];
		if (arg[0] != '-')
			break;

		if (arg[1] == 0 || arg[2] != 0)
		{
			usage();
			return 1;
		}

		switch (arg[1])
		{
		case 'b':
			{
				arg = argv[++a];

				char* end;
				blockId = strtoul(arg, &end, 0);
				if (blockIdSpecified || *end != 0)
				{
					usage();
					return 1;
				}
				blockIdSpecified = true;
			} break;
		case 's':
			{
				arg = argv[++a];

				char* end;
				blockSeq = strtoul(arg, &end, 0);
				if (blockSeqSpecified || *end != 0)
				{
					usage();
					return 1;
				}
				blockSeqSpecified = true;
			} break;
		default:
			fprintf(stderr, "error: invalid option: %s\n\n", arg);
			usage();
			return 1;
		}
	}

	if (a >= argc)
	{
		fprintf(stderr, "error: missing source file name\n\n");
		usage();
		return 1;
	}
	lbtPath = argv[a++];

	if (a < argc)
	{
		kduPath = argv[a++];

		if (a < argc)
		{
			fprintf(stderr, "error: too many parameters\n\n");
			usage();
			return 1;
		}
	}

	FileMapping input;
	if (input.create(lbtPath, KL_FILEMAP_COPY_ON_WRITE, KLSYS_MMAP_HINT_DEFAULT))
	{
		fprintf(stderr, "can't open the file %s\n", lbtPath);
		return 2;
	}
	FileMapping::kl_size_t mappingSize = 0;
	char const* buffer = reinterpret_cast<const char*>(input.map(0, 0, &mappingSize));
	if (!buffer)
	{
		fprintf(stderr, "can't open the file %s\n", lbtPath);
		return 1;
	}

	std::string kduName;
	if (kduPath)
		kduName = kduPath;
	else
	{
		kduName = lbtPath;
		size_t dotPos = kduName.find_last_of('.');
		if (dotPos != std::string::npos && dotPos != 0)
			kduName.resize(dotPos);
		kduName += ".kdu";
	}

	FILE* kduFile = fopen(kduName.c_str(), "wb");
	if (!kduFile) 
	{
		fprintf(stderr, "can't open the file %s\n", kduName.c_str());
		return 2;
	}
	KDUFileWriter writer(kduFile, true); 

	uint32_t vendorID = 0;	
	if (!compileLbt(buffer, mappingSize, vendorID, blockId, blockSeq, &writer))
	{
		fprintf(stderr, "compilation failed\n");
		return 3;
	}

	return 0;
}
