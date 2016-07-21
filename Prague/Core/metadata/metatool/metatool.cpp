#include <iostream>
#include <string>
#include <vector>
#include "../metadata/metaloader.h"
#include "../metadata/metasaver.h"

using namespace std;

typedef vector<string> Files;

int main(int argc, char** argv)
{
	if(argc < 3){
		cout << "Prague metadata tool" << endl;
		cout << "Usage: " << argv[0] <<" [-s] [-c] [-o  <output file>] <metadata filename1>...<metadata filenameN>" << endl;
		cout << "\t-s\t\t" << "change byteorder of metadata from little-endian to big-endian" << endl;
		cout << "\t-c\t\t" << "save in metacache format" << endl;
		cout << "\t-m\t\t" << "save in multimeta format" << endl;
		cout << "\t-o <filename>\t" << "name of output file [default is out.meta]" << endl;
		return -1;
	}
	bool swap = false;
	bool cache = false;
	bool multi = false;
	Files files;
	int i = 1;
	string outfile("out.meta");
	while(i < argc){
		if(argv[i][0] == '-'){
			switch(argv[i++][1]){
			case 's': swap = true; break;
			case 'c': cache = true; break;
			case 'm': multi = true; break;	
			case 'o': 
				if(i < argc) 
					outfile = argv[i++]; 
				else 
					cerr << "an output file is not specified" << endl;
				break;					
			default: cerr << "unknown option '" << argv[i][1] << "'" << endl;
			}
		}
		else
			files.push_back(argv[i++]);
	}
	PragueMetadata metadata;
	MetaLoader<GetLEValue> loader;
	for(Files::const_iterator i = files.begin(); i != files.end(); ++i)
		if(!loader.load(i->c_str(), metadata)){
			cerr << "Can't load metadata from " << *i << endl;
			continue;
		}
	bool result = false;
	if(swap){
		MetaSaver<SaveBEValue> saver;
		if(!saver.open(outfile.c_str())){
			cerr << "can't open " << outfile << endl;
			return -1;
		}
		if(cache)
			result = saver.saveCache(metadata);
		else if(multi || (metadata.count() > 1))
			result = saver.saveMulti(metadata);
		else 
			result = saver.saveOne(metadata);
	}
	else{
		MetaSaver<SaveValue> saver;
		if(!saver.open(outfile.c_str())){
			cerr << "can't open " << outfile << endl;
			return -1;
		}
		if(cache)
			result = saver.saveCache(metadata);
		else if(multi || (metadata.count() > 1))
			result = saver.saveMulti(metadata);
		else 
			result = saver.saveOne(metadata);
	}
	if(!result){
		cerr << "Can't save metadata to " << outfile << endl;
		return -1;
	}

	return 0;
}
