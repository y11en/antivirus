#ifndef compiler_h_INCLUDED
#define compiler_h_INCLUDED

struct KLAV_Bin_Writer;

bool compileLbt(char const* buffer, size_t bufferSize, uint32_t vendorId, uint32_t blockId, uint32_t blockSeq, KLAV_Bin_Writer* writer);

class LbtCompiler
{
public:
	LbtCompiler() :
		m_lbVarNo(0)
		{}

	typedef std::vector<uint8_t> Buffer;
	typedef KLAV_Buffer_Writer<Buffer> BufferWriter;

	void writeSections(BufferWriter& writer);

	size_t getStringLiteralIdx(char const* str, size_t len)
		{ return getStringIdx(str, len, m_stringLiteralMap, m_stringLiteralTable); }
	std::string const& getStringLiteral(size_t id)
		{ return getString(id, m_stringLiteralTable); }

	size_t getIdentIdx(char const* str, size_t len)
		{ return getStringIdx(str, len, m_identMap, m_identTable); }
	std::string const& getIdent(size_t id)
		{ return getString(id, m_identTable); }

	void declareWord(uint64_t wordStringId);
	void declareVar(uint64_t varNameId, uint64_t size, uint64_t alignment);
	void declareLabel(uint64_t labelNameId);
	void referLb();
	void referLabel(uint64_t labelNameId, uint64_t inputNo);
	void referVerdict(uint64_t verdictNameId);
	void checkSequenceFinalization();
	void setUpLinks();

	void emitTrap();
	void emitHub();
	void emitFrame(uint64_t depth);
	void emitCall(uint64_t procNameId);
	void emitCallN(uint8_t n, uint64_t procNameId);
	void emitAnd(uint64_t n);
	void emitSeq(uint64_t n);
	void emitPush1(uint64_t n1);
	void emitPush2(uint64_t n1, uint64_t n2);
	void emitPush3(uint64_t n1, uint64_t n2, uint64_t n3);
	void emitPush(uint64_t mask);
	void emitStkop(uint64_t op1, uint64_t op2 = 0, uint64_t op3 = 0);
	void emitCmp0(uint64_t elt, uint64_t m64);
	void emitCmp16(uint64_t elt, uint64_t m64, uint64_t val);
	void emitCmp32(uint64_t cmpOp, uint64_t m64, uint64_t val);
	void emitCmp64(uint64_t cmpOp, uint64_t val);
	void emitCmp(uint64_t cmpOp, uint64_t m64);
	void emitRd(uint64_t elt, uint64_t varId, uint64_t offset);
	void emitRdx(uint64_t elt, uint64_t slot, uint64_t offset);
	void emitRdslot(uint64_t slot);
	void emitRdcx(uint64_t slot, uint64_t offset);

	void setFinal(uint8_t final)
		{ m_final = final != 0; }

	void addOutput();
	void addValue(uint64_t val);
	void addVarRef(uint64_t varNameId);
	void addVarRef(uint64_t varNameId, uint64_t offset);
	void addStrRef(uint64_t strId);
	void addPushed();

private:
	enum Section
	{
		sec_words,
		sec_verdicts,
		sec_pubnames,
		sec_strings,
		sec_vars,
		sec_lb,
	};

	struct Ref
	{
		Section section;
		uint32_t address;

		bool operator<(Ref const& other) const
		{
			if (section != other.section)
				return section < other.section;
			return address < other.address;
		}
	};

	Ref m_linkPlace;

	typedef std::map<std::string, size_t> StringMap;
	typedef std::vector<StringMap::const_iterator> StringTable;

	StringMap m_stringLiteralMap;
	StringTable m_stringLiteralTable;

	StringMap m_identMap;
	StringTable m_identTable;

	static size_t getStringIdx(char const* str, size_t len, StringMap& strMap, StringTable& strTable);
	static std::string const& getString(size_t idx, StringTable& strTable)
		{ return strTable[idx]->first; }

	typedef std::map<size_t, uint32_t> StringSectionMap;
	typedef std::vector<size_t> StringSectionTable;

	StringSectionMap m_stringSectionMap;
	StringSectionTable m_stringSectionTable;

	std::vector<uint32_t> m_lb;
	bool m_final;

	typedef std::map<size_t, uint32_t> WordMap;
	typedef std::vector<size_t> WordTable;

	WordMap m_wordMap;
	WordTable m_wordTable;

	struct Var
	{
		size_t varNameId;
		size_t size;
		size_t alignment;
	};

	typedef std::map<size_t, uint32_t> VarMap;
	typedef std::vector<Var> VarTable;
	typedef std::multimap<size_t, Ref> VarLinkMap;

	VarMap m_varMap;
	VarTable m_varTable;
	VarLinkMap m_varLinkMap;

	typedef std::map<size_t, uint32_t> LabelMap;
	typedef std::multimap<uint8_t, Ref> LabelRefs;
	typedef std::map<size_t, LabelRefs> LabelLinkMap;

	LabelMap m_labelMap;
	LabelLinkMap m_labelLinkMap;

	typedef std::map<size_t, uint32_t> VerdictMap;
	typedef std::vector<size_t> VerdictTable;

	VerdictMap m_verdictMap;
	VerdictTable m_verdictTable;

	typedef std::map<size_t, uint32_t> ProcMap;
	typedef std::vector<size_t> ProcTable;

	ProcMap m_procMap;
	ProcTable m_procTable;

	void registerProcRef(size_t procNameId);

	typedef std::set<Ref> LinkTargets;
	typedef std::map<uint8_t, LinkTargets> ModeLinkTargets;
	typedef std::map<Ref, ModeLinkTargets> Links;

	Links m_links;

	size_t m_outputCounterPlace;
	uint32_t m_outputCounterMode;
	size_t m_valueCounterPlace;

	unsigned m_lbVarNo;

	std::string getLbVarName();
	static void incrementByMask(uint32_t& value, uint32_t mask, char const* errorMsg);
};

#endif // compiler_h_INCLUDED
