typedef const char* interface_t;
typedef const char* method_t;
typedef const char* method_t;

struct Request {
	interface_t	interface;
	method_t	method;
	const char*	inArgs;
	size_t		inArgsSize;
	char*		outArgs;
	size_t		outArgsSize;
	error_t		result;
};

template <typename T>
size_t demarshal(const char* data, MappedTypeTraits<T>::NativeTypeRef arg) {
}

template <>
size_t demarshal<Types::SInt32>(const char* data, int& arg) {
	arg = data[0];
	arg += (data[1] << 8);
	arg += (data[2] << 16);
	arg += (data[3] << 24);
	return MappedTypeTraits<Types::SInt32>::Size;
}

template <>
size_t marshal<Types::SInt32>(char* data, int arg) {
	data[0] = (char)(arg&0xff);
	data[1] = (char)((arg >> 8)&0xff);
	data[2] = (char)((arg >> 16)&0xff);
	data[3] = (char)((arg >> 24)&0xff);
	return MappedTypeTraits<Types::SInt32>::Size;
}


/*********** Mapping **********/

class I {
	virtual error_t m(arg0, ..., out0, ...) = 0;
}

class SkelI : public I {
	static void makeMethodTable() {
		_methodTable["m"] = &I::dispatcher_m;
	}

	static interface_t getInterfaceId() {
		return "I";
	}
	
	void dispatcher(Request& request) {
		request.result = this->(*_methodTable[request.method])(request);
	}

	error_t dispatcher_m(const std::vector<char>& in, std::vector<char>& out) {
		size_t offset = 0;
		offset += demarshal<type-in>(&in[offset], arg0);
		offset += demarshal<type-in>(&in[offset], arg0);
		offset += demarshal<type-in>(&in[offset], arg0);
		offset += demarshal<type-in>(&in[offset], arg0);

		error_t result = m(arg0, ..., out0, ...);

		offset = 0;
		offset += marshal<type-in>(&out[offset], arg0);
		offset += marshal<type-in>(&out[offset], arg1);
		offset += marshal<type-in>(&out[offset], arg2);
		offset += marshal<type-in>(&out[offset], arg3);
		
		return result;
	}
};


class InvokeFunctor {
  public:
  	virtual error_t operator(Request& request) = 0;
};

template <typename Skeleton>
class Invoke : InvokeFunctor {
  public:
  	Invoke(Skeleton* skeleton)
  
  	virtual error_t operator(Request& request) {
  		return _skeleton->dispatcher(request);
  	}

  private:
  	Skeleton* _skeleton;
};

class InterafceTable {
	_table["I"] = new Invoke(new I());
};


void call(const Request& request) {
	InterafceTable::findInterface(request.interface)(request);
}
