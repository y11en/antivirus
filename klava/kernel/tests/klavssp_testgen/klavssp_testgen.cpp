// klavssp_testgen.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"

typedef unsigned char byte;

enum region_matching_option
{
	always,
	good_type,
	bad_type,
	good_type_good_id,
	good_type_bad_id,
	bad_type_good_id,
	bad_type_bad_id,
	good_class_1,
	good_class_2,
	bad_class,
	region_matching_option_top
};

inline bool matchable(region_matching_option rmo)
{
	switch (rmo)
	{
	case always:
	case good_type:
	case good_type_good_id:
	case good_class_1:
	case good_class_2:
		return true;
	default:
		return false;
	}
}

inline void write(ostream& o, region_matching_option rmo)
{
	switch (rmo)
	{
	case always:
		break;
	case good_type:
		o << ", TEST_TYPE_1";
		break;
	case bad_type:
		o << ", TEST_TYPE_2";
		break;
	case good_type_good_id:
		o << ", TEST_TYPE_1, TEST_ID_1";
		break;
	case good_type_bad_id:
		o << ", TEST_TYPE_1, TEST_ID_2";
		break;
	case bad_type_good_id:
		o << ", TEST_TYPE_2, TEST_ID_1";
		break;
	case bad_type_bad_id:
		o << ", TEST_TYPE_2, TEST_ID_2";
		break;
	case good_class_1:
		o << ", TEST_CLASS_1";
		break;
	case good_class_2:
		o << ", TEST_CLASS_3";
		break;
	case bad_class:
		o << ", TEST_CLASS_2";
		break;
        default:
//         case region_matching_option_top :
                break;
	}
}

enum length_option
{
	fixed_length_1,
	fixed_length_2,
	fixed_length_3,
	fixed_length_4,
	fixed_length_5,
	fixed_length_6,
	fixed_length_7,
	fixed_length_8,
	fixed_length_9,
	length_minus_5,
	length_minus_4,
	length_minus_3,
	length_minus_2,
	length_minus_1,
	whole_length,
	length_option_top
};

unsigned make_length(length_option lo, unsigned whole)
{
	switch (lo)
	{
	case fixed_length_1:
	case fixed_length_2:
	case fixed_length_3:
	case fixed_length_4:
	case fixed_length_5:
	case fixed_length_6:
	case fixed_length_7:
	case fixed_length_8:
	case fixed_length_9:
		return 1 + lo - fixed_length_1;
	default:
		return whole - (whole_length - lo);
	}
}

enum position_option
{
	fixed_pos_0,
	fixed_pos_1,
	fixed_pos_3,
	fixed_pos_7,
	fixed_pos_12,
	middle,
	end_minus_length,
	position_option_top
};

unsigned make_position(position_option po, unsigned whole, unsigned len)
{
	switch (po)
	{
	case fixed_pos_0:
		return 0;
	case fixed_pos_1:
		return 1;
	case fixed_pos_3:
		return 3;
	case fixed_pos_7:
		return 7;
	case fixed_pos_12:
		return 12;
	case middle:
		return whole / 2;
	default:
		return whole - len;
	}
}

enum body_option
{
	correct,
	shifted_forth_1,
	shifted_forth_4,
	shifted_back_1,
	shifted_back_4,
	damaged_0,
	damaged_1,
	damaged_2,
	damaged_3,
	damaged_4,
	damaged_5,
	damaged_6,
	damaged_7,
	damaged_8,
	damaged_last,
	damaged_last_minus_1,
	damaged_last_minus_2,
	damaged_last_minus_3,
	damaged_last_minus_4,
	randomize,
	body_option_top
};

inline bool possible(body_option bo, unsigned whole, unsigned len, unsigned pos)
{
	if (pos + len > whole)
		return false;
	switch (bo)
	{
	case shifted_forth_1:
		return pos + len + 1 <= whole;
	case shifted_forth_4:
		return pos + len + 4 <= whole;
	case shifted_back_1:
		return pos > 1;
	case shifted_back_4:
		return pos > 4;
	case damaged_1:
		return len > 1;
	case damaged_2:
		return len > 2;
	case damaged_3:
		return len > 3;
	case damaged_4:
		return len > 4;
	case damaged_5:
		return len > 5;
	case damaged_6:
		return len > 6;
	case damaged_7:
		return len > 7;
	case damaged_8:
		return len > 8;
	case damaged_last:
		return len > 9;
	case damaged_last_minus_1:
		return len > 10;
	case damaged_last_minus_2:
		return len > 11;
	case damaged_last_minus_3:
		return len > 12;
	case damaged_last_minus_4:
		return len > 13;
	default:
		return true;
	}
}

inline bool is_correct(body_option bo)
{
	return bo == correct;
}

void make_sample(vector<byte> const& src, unsigned len, unsigned pos, body_option bo, vector<byte>& result)
{
	result.resize(len);
	if (bo == randomize)
	{
		for (unsigned i = 0; i < len; ++i)
			result[i] = rand() % (UCHAR_MAX + 1);
	} else
	{
		bool damage = false;
		unsigned begin = pos;
		unsigned damage_pos;
		switch (bo)
		{
		case shifted_forth_1:
			begin += 1;
			break;
		case shifted_forth_4:
			begin += 4;
			break;
		case shifted_back_1:
			begin -= 1;
			break;
		case shifted_back_4:
			begin -= 4;
			break;
		case damaged_0:
			damage = true;
			damage_pos = 0;
			break;
		case damaged_1:
			damage = true;
			damage_pos = 1;
			break;
		case damaged_2:
			damage = true;
			damage_pos = 2;
			break;
		case damaged_3:
			damage = true;
			damage_pos = 3;
			break;
		case damaged_4:
			damage = true;
			damage_pos = 4;
			break;
		case damaged_5:
			damage = true;
			damage_pos = 5;
			break;
		case damaged_6:
			damage = true;
			damage_pos = 6;
			break;
		case damaged_7:
			damage = true;
			damage_pos = 7;
			break;
		case damaged_8:
			damage = true;
			damage_pos = 8;
			break;
		case damaged_last:
			damage = true;
			damage_pos = len - 1;
			break;
		case damaged_last_minus_1:
			damage = true;
			damage_pos = len - 2;
			break;
		case damaged_last_minus_2:
			damage = true;
			damage_pos = len - 3;
			break;
		case damaged_last_minus_3:
			damage = true;
			damage_pos = len - 4;
			break;
		case damaged_last_minus_4:
			damage = true;
			damage_pos = len - 5;
			break;
                default:
//                 case correct:
//                 case randomize:
//                 case body_option_top:
                  break;
		}

		for (unsigned i = 0; i < len; ++i)
			result[i] = src[begin + i];

		if (damage)
			++result[damage_pos];
	}
}

bool findable_by_fact(vector<byte> const& src, unsigned len, unsigned pos, vector<byte> const& result)
{
	byte const* pb = &src[pos];
	return equal(pb, pb + len, result.begin());
}

void write_hex_signature(ostream& o, vector<byte> const& sig, unsigned len)
{
	static char digits[] = "0123456789ABCDEF";

	o << "\"";

	for (unsigned i = 0; i < len; ++i)
	{
		unsigned d = sig[i];
		unsigned d1 = d / 16;
		unsigned d2 = d % 16;

		o << digits[d1] << digits[d2];
	}

	o << "\"";
}

string make_signature_name(bool findable)
{
	static unsigned n1 = 0;
	static unsigned n2 = 0;
	stringstream s;
	if (findable)
		s << "s" << n1++ << "_y";
	else
		s << "s" << n2++ << "_n";
	return s.str();
}

int process(istream& src, ostream& out)
{
	unsigned s = src.seekg(0, ios::end).tellg();
	unsigned sh = s - s / 2;
	src.seekg(0);

	vector<byte> v(s);
	byte* data = &v.front();

	src.read(reinterpret_cast<char*>(data), s);

	out << "$on (LOAD) SimpleLoadProc();" << endl;
	out << "$on (PROCESS_OBJECT) KlavsspTestProc();" << endl;
	out << "$rgn_type TEST_TYPE_1(1);" << endl;
	out << "$rgn_type TEST_TYPE_2(2);" << endl;
	out << "$rgn_type TEST_TYPE_3(3);" << endl;
	out << "$rgn_id TEST_ID_1(1);" << endl;
	out << "$rgn_id TEST_ID_2(2);" << endl;
	out << "$rgn_id TEST_ID_3(3);" << endl;
	out << "$rgn_class TEST_CLASS_1(TEST_TYPE_1, TEST_TYPE_2);" << endl;
	out << "$rgn_class TEST_CLASS_2(TEST_TYPE_2, TEST_TYPE_3);" << endl;
	out << "$rgn_class TEST_CLASS_3(TEST_TYPE_3, TEST_TYPE_1);" << endl;

	vector<byte> sig;
	sig.reserve(s);

	vector<string> findables;
	vector<string> nonfindables;

	set<unsigned> lengths;
	for (unsigned lov = 0; lov < length_option_top; ++lov)
	{
		length_option lo = length_option(lov);

		unsigned l = make_length(lo, s);
		if (l > s || lengths.find(l) != lengths.end())
			continue;
		lengths.insert(l);

		set<unsigned> positions;
		for (unsigned pov = 0; pov < position_option_top; ++pov)
		{
			position_option po = position_option(pov);

			unsigned p = make_position(po, s, l);
			if (p + l > s || positions.find(p) != positions.end())
				continue;
			positions.insert(p);

			int rp = p - sh;
			
			for (unsigned rmov = 0; rmov < region_matching_option_top; ++rmov)
			{
				region_matching_option rmo = region_matching_option(rmov);

				bool f1 = matchable(rmo);

				for (unsigned bov = 0; bov < body_option_top; ++bov)
				{
					body_option bo = body_option(bov);

					if (!possible(bo, s, l, p))
						continue;
					bool findable = f1 && is_correct(bo);
					
					make_sample(v, l, p, bo, sig);

					if (!findable && findable_by_fact(v, l, p, sig))
						continue;
					
					string name = make_signature_name(findable);

					out << "$sig_pos " << name << "(";
					write_hex_signature(out, sig, l);
					write(out, rmo);
					out << ", " << rp << ");" << endl;

					(findable ? findables : nonfindables).push_back(name);
				}
			}
		}
	}

	for (vector<string>::const_iterator it1 = findables.begin(); it1 != findables.end(); ++it1)
		out << "$on (!" << *it1 << ") $verdict(\"!" << *it1 << "\");" << endl;
	
	for (vector<string>::const_iterator it2 = nonfindables.begin(); it2 != nonfindables.end(); ++it2)
		out << "$on (" << *it2 << ") $verdict(\"" << *it2 << "\");" << endl;
	
	out << "$on (PROCESS_OBJECT_DONE) RMDump ();" << endl;

	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cerr << endl << "Usage:" << endl;
		cerr << "\tklavssp_testgen <eicar-file> <resulting-test.kt>" << endl << endl;
		return 1;
	}

	char const* sourceFileName = argv[1];
	char const* resultFileName = argv[2];

	ifstream sourceFile(sourceFileName, ios::in | ios::binary);
	ofstream resultFile(resultFileName, ios::out);

	return process(sourceFile, resultFile);
}
