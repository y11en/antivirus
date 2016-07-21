#include <iostream>

#include "../metadata/metaloader.h"
#include "../metadata/metasaver.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc != 3){
		cout << "Metadata byteorder convertor" << endl;
		cout << "Usage: meta2atem <metadata filename > <converted metadata file name>" << endl;
		return -1;
	}

	PragueMetadata metadata;
	MetaLoader<GetLEValue> loader;
	
	if(!loader.load(argv[1], metadata)){
		cerr << "Can't load metadata from " << argv[1] << endl;
		return -1;
	}
	MetaSaver<SaveBEValue> saver;
	if(!saver.open(argv[2])){
		cerr << "Can't open " << argv[2] << endl;
		return -1;
	}
		
	if(!saver.saveOne(metadata)){
		cerr << "Can't save metadata to " << argv[2] << endl;
		return -1;
	}
  
	cout << "Metadata byteorder has been converted successfully." << endl;
	return 0;
}
