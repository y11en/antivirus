#include "precompile.h"

#include "compiler.h"
#include "parser.h"

#if _MSC_VER >= 1300
#	pragma warning(disable: 4267 4146)
#endif

size_t LbtCompiler::getStringIdx(char const* str, size_t len, StringMap& strMap, StringTable& strTable)
{
	std::string s(str, len);
	StringMap::const_iterator it = strMap.find(s);
	if (it != strMap.end())
		return it->second;
	size_t r = strTable.size();
	std::pair<StringMap::iterator, bool> pib = strMap.insert(std::make_pair(s, r));
	strTable.push_back(pib.first);
	return r;
}

void LbtCompiler::declareWord(uint64_t wordStringId)
{
	size_t wsi = size_t(wordStringId);

	m_linkPlace.section = sec_words;

	WordMap::const_iterator it = m_wordMap.find(wsi);
	if (it != m_wordMap.end())
	{
		m_linkPlace.address = it->second;
		return;
	}
	m_linkPlace.address = uint32_t(m_wordTable.size());
	m_wordMap.insert(std::make_pair(wsi, m_linkPlace.address));
	m_wordTable.push_back(wsi);
}

void LbtCompiler::declareVar(uint64_t varNameId, uint64_t size, uint64_t alignment)
{
	size_t vni = size_t(varNameId);
	if (size == 0)
		throw std::runtime_error("Variable declared to have zero size: " + getIdent(vni));
	if (size >= 0x1000000)
		throw std::runtime_error("Variable declared to have too big size: " + getIdent(vni));
	if (alignment > 0x10 || (alignment & (alignment - 1)) != 0)
		throw std::runtime_error("Variable declared to have invalid alignment: " + getIdent(vni));
	VarMap::const_iterator it = m_varMap.find(vni);
	if (it != m_varMap.end())
		throw std::runtime_error("Duplicate declaration for variable: " + getIdent(vni));
	m_varMap.insert(std::make_pair(vni, uint32_t(m_varTable.size())));
	Var var;
	var.varNameId = vni;
	var.size = size_t(size);
	var.alignment = size_t(alignment);
	m_varTable.push_back(var);
}

void LbtCompiler::declareLabel(uint64_t labelNameId)
{
	size_t lni = size_t(labelNameId);

	LabelMap::const_iterator it = m_labelMap.find(lni);
	if (it != m_labelMap.end())
		throw std::runtime_error("Duplicate label name: " + getIdent(lni));
	m_labelMap.insert(std::make_pair(lni, uint32_t(m_lb.size())));
}

void LbtCompiler::referLb()
{
	Ref current;
	current.section = sec_lb;
	current.address = m_lb.size();
	m_links[current][0].insert(m_linkPlace);
}

void LbtCompiler::referLabel(uint64_t labelNameId, uint64_t inputNo)
{
	size_t lni = size_t(labelNameId);
	uint8_t i = uint8_t(inputNo);
	if (i != inputNo)
	{
		std::ostringstream ss;
		ss << "Input number too great: " << getIdent(lni) << " (" << inputNo << ")";
		throw std::runtime_error(ss.str());
	}
	m_labelLinkMap[lni].insert(std::make_pair(i, m_linkPlace));
}

void LbtCompiler::referVerdict(uint64_t verdictNameId)
{
	size_t vni = size_t(verdictNameId);
	VerdictMap::const_iterator it = m_verdictMap.find(vni);
	uint32_t n;
	if (it != m_verdictMap.end())
		n = it->second;
	else
	{
		n = uint32_t(m_verdictTable.size());
		m_verdictMap.insert(std::make_pair(vni, n));
		m_verdictTable.push_back(vni);
	}
	Ref vdctRef;
	vdctRef.section = sec_verdicts;
	vdctRef.address = n;
	m_links[vdctRef][0x80].insert(m_linkPlace);
}

void LbtCompiler::checkSequenceFinalization()
{
	if (!m_final)
		throw std::runtime_error("Logical block fragment not finalized");
}

void LbtCompiler::setUpLinks()
{
	VarMap::const_iterator const varEnd = m_varMap.end();
	VarLinkMap::const_iterator const varLinkEnd = m_varLinkMap.end();
	for (VarLinkMap::const_iterator it = m_varLinkMap.begin(); it != varLinkEnd; ++it)
	{
		size_t varNameId = it->first;
		Ref const& ref = it->second;

		VarMap::const_iterator jt = m_varMap.find(varNameId);

		if (jt == varEnd)
			throw std::runtime_error("Variable used but not declared: " + getIdent(varNameId));

		Ref varRef;
		varRef.section = sec_vars;
		varRef.address = jt->second;

		m_links[varRef][0].insert(ref);
	}
	m_varLinkMap.clear();

	LabelMap::const_iterator const labelEnd = m_labelMap.end();
	LabelLinkMap::const_iterator const labelLinkEnd = m_labelLinkMap.end();
	for (LabelLinkMap::const_iterator it = m_labelLinkMap.begin(); it != labelLinkEnd; ++it)
	{
		size_t labelId = it->first;
		LabelRefs const& refs = it->second;

		LabelMap::const_iterator jt = m_labelMap.find(labelId);
		if (jt == labelEnd)
			throw std::runtime_error("Label used but not declared: " + getIdent(labelId));

		Ref labelRef;
		labelRef.section = sec_lb;
		labelRef.address = jt->second;

		LabelRefs::const_iterator const refsEnd = refs.end();
		for (LabelRefs::const_iterator kt = refs.begin(); kt != refsEnd; ++kt)
		{
			uint8_t mode = kt->first;
			Ref const& ref = kt->second;
			m_links[labelRef][mode].insert(ref);
		}
	}
	m_labelLinkMap.clear();

	Links::const_iterator const linkEnd = m_links.end();
	for (Links::iterator it = m_links.begin(); it != linkEnd;)
	{
		Ref const& src = it->first;
		ModeLinkTargets& modeTargets = it->second;
		ModeLinkTargets::iterator zeroMode = modeTargets.find(0);
		ModeLinkTargets::const_iterator const modeTargetEnd = modeTargets.end();
		for (ModeLinkTargets::iterator jt = modeTargets.begin(); jt != modeTargetEnd;)
		{
			uint8_t mode = jt->first;
			LinkTargets& targets = jt->second;
			if (mode || src.section == sec_lb)
			{
				LinkTargets::const_iterator const tgtEnd = targets.end();
				for (LinkTargets::iterator kt = targets.begin(); kt != tgtEnd;)
				{
					Ref const& tgt = *kt;
					if (tgt.section == sec_lb)
					{
						uint32_t& lbWord = m_lb[tgt.address];
						if (src.section == sec_lb)
							lbWord = src.address;
						else
						{
							if (zeroMode == modeTargetEnd)
								zeroMode = modeTargets.insert(std::make_pair(0, LinkTargets())).first;
							zeroMode->second.insert(tgt);

							lbWord &= ~0xFF000000;
						}
						lbWord |= mode << 24;

						targets.erase(kt++);
						continue;
					}
					++kt;
				}

				if (targets.empty())
				{
					modeTargets.erase(jt++);
					continue;
				}
			}
			++jt;
		}

		if (modeTargets.empty())
		{
			m_links.erase(it++);
			continue;
		}
		++it;
	}
}

void LbtCompiler::emitTrap()
{
	uint32_t w1 = MAKE_LBELT(LB2_ELT_TRAP, !m_final);
	m_lb.push_back(w1);
}

void LbtCompiler::emitHub()
{
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_HUB, !m_final, 0);
	m_outputCounterPlace = m_lb.size();
	m_outputCounterMode = LBELT_MASK_DATA;
	m_lb.push_back(w1);
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitFrame(uint64_t depth)
{
	if (depth >= 0x1000000)
		throw std::runtime_error("Depth too big");
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_FRAME, !m_final, uint32_t(depth));
	m_lb.push_back(w1);
}

void LbtCompiler::emitCall(uint64_t procNameId)
{
	size_t pni = size_t(procNameId);
	registerProcRef(pni);
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_CALL, !m_final, 0);
	m_lb.push_back(w1);
}

void LbtCompiler::emitCallN(uint8_t n, uint64_t procNameId)
{
	size_t pni = size_t(procNameId);
	registerProcRef(pni);
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_CALL0 + n, !m_final, 0);
	m_lb.push_back(w1);
}

void LbtCompiler::emitAnd(uint64_t n)
{
	m_outputCounterPlace = -1;

	std::string varName = getLbVarName();
	size_t varNameIdx = getIdentIdx(varName.data(), varName.size());

	uint8_t elt;

	switch (n)
	{
	case -2:
		elt = LB2_ELT_AND2;
		break;
	case -3:
		elt = LB2_ELT_AND3;
		break;
	case -4:
		elt = LB2_ELT_AND4;
		break;
	default:
		if (n >= 0x100)
			throw std::runtime_error("Too many inputs for logical element");
		{
			uint32_t m = uint32_t(n);
			uint32_t s = (m + 31) / 32;
			declareVar(varNameIdx, s * 4, 4);
			addVarRef(varNameIdx);
			uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_ANDN, !m_final, 0);
			m_lb.push_back(w1);
			m_lb.push_back(m);
		}
		goto setupLinkPlace;
	}

	declareVar(varNameIdx, 1, 0);
	addVarRef(varNameIdx);
	uint32_t w1 = MAKE_LBELT_DATA(elt, !m_final, 0);
	m_lb.push_back(w1);

setupLinkPlace:
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitSeq(uint64_t n)
{
	m_outputCounterPlace = -1;

	std::string varName = getLbVarName();
	size_t varNameIdx = getIdentIdx(varName.data(), varName.size());

	uint8_t elt;

	switch (n)
	{
	case -2:
		elt = LB2_ELT_SEQ2;
		break;
	case -3:
		elt = LB2_ELT_SEQ3;
		break;
	case -4:
		elt = LB2_ELT_SEQ4;
		break;
	default:
		if (n >= 0x100)
			throw std::runtime_error("Too many inputs for logical element");
		{
			uint32_t m = uint32_t(n);
			declareVar(varNameIdx, 4, 4);
			addVarRef(varNameIdx);
			uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_SEQN, !m_final, 0);
			m_lb.push_back(w1);
			m_lb.push_back(m);
			goto setupLinkPlace;
		}
		return;
	}

	declareVar(varNameIdx, 1, 0);
	addVarRef(varNameIdx);
	uint32_t w1 = MAKE_LBELT_DATA(elt, !m_final, 0);
	m_lb.push_back(w1);

setupLinkPlace:
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitPush1(uint64_t n1)
{
	uint64_t t1 = n1 & 0xFFFFFF;
	t1 |= -(t1 & 0x800000);
	if (t1 != n1)
		throw std::runtime_error("Inappropriate value for PUSH1");
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_PUSH1, !m_final, uint32_t(n1 & 0xFFFFFF));
	m_lb.push_back(w1);
}

void LbtCompiler::emitPush2(uint64_t n1, uint64_t n2)
{
	uint64_t t1 = n1 & 0xFF;
	t1 |= -(t1 & 0x80);
	uint64_t t2 = n2 & 0xFFFF;
	t2 |= -(t2 & 0x8000);
	if (t1 != n1 || t2 != n2)
		throw std::runtime_error("Inappropriate value for PUSH2");
	uint32_t w1 = MAKE_LBELT_DATA_16(LB2_ELT_PUSH2, !m_final, uint32_t(n1 & 0xFF), uint32_t(n2 & 0xFFFF));
	m_lb.push_back(w1);
}

void LbtCompiler::emitPush3(uint64_t n1, uint64_t n2, uint64_t n3)
{
	uint64_t t1 = n1 & 0xFF;
	t1 |= -(t1 & 0x80);
	uint64_t t2 = n2 & 0xFF;
	t2 |= -(t2 & 0x80);
	uint64_t t3 = n3 & 0xFF;
	t3 |= -(t3 & 0x80);
	if (t1 != n1 || t2 != n2 || t3 != n3)
		throw std::runtime_error("Inappropriate value for PUSH3");
	uint32_t w1 = MAKE_LBELT_DATA_8(LB2_ELT_PUSH3, !m_final, uint32_t(n1 & 0xFF), uint32_t(n2 & 0xFF), uint32_t(n3 & 0xFF));
	m_lb.push_back(w1);
}

void LbtCompiler::emitPush(uint64_t mask)
{
	if (mask >= 0x10000)
		throw std::runtime_error("Mask value too large");
	m_valueCounterPlace = m_lb.size();
	uint32_t w1 = MAKE_LBELT_DATA_16(LB2_ELT_PUSHN, !m_final, 0, uint32_t(mask));
	m_lb.push_back(w1);
}

void LbtCompiler::emitStkop(uint64_t op1, uint64_t op2, uint64_t op3)
{
	if (op1 >= 0x100 || op2 >= 0x100 || op3 >= 0x100)
		throw std::runtime_error("Inline stack operand too large");
	uint32_t w1 = MAKE_LBELT_DATA_8(LB2_ELT_STKOP, !m_final, uint32_t(op3), uint32_t(op2), uint32_t(op1));
	m_lb.push_back(w1);
}

void LbtCompiler::emitCmp0(uint64_t elt, uint64_t m64)
{
	uint32_t w1 = MAKE_LBELT_DATA_22(uint32_t(elt), !m_final, m64 ? LB_ELT_CMP_N2_64BIT : 0, 0);
	m_outputCounterPlace = m_lb.size();
	m_outputCounterMode = LBELT_MASK_DATA_22;
	m_lb.push_back(w1);
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitCmp16(uint64_t elt, uint64_t m64, uint64_t val)
{
	uint64_t t = val & 0xFFFF;
	t |= -(t & 0x8000);
	if (t != val)
		throw std::runtime_error("Comparison operand too large");
	uint32_t w1 = MAKE_LBELT_DATA_6_16(uint32_t(elt), !m_final, m64 ? LB_ELT_CMP_N2_64BIT : 0, 0, uint32_t(val));
	m_outputCounterPlace = m_lb.size();
	m_outputCounterMode = LBELT_MASK_DATA_B2_6;
	m_lb.push_back(w1);
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitCmp32(uint64_t cmpOp, uint64_t m64, uint64_t val)
{
	uint64_t t = val & 0xFFFFFFFF;
	t |= -(t & 0x80000000);
	if (t != val)
		throw std::runtime_error("Comparison operand too large");
	bool sgn = false;
	uint32_t co = uint32_t(cmpOp);
	switch (co)
	{
	case ~LB_ELT_CMP_OP_LT:
	case ~LB_ELT_CMP_OP_GT:
	case ~LB_ELT_CMP_OP_LE:
	case ~LB_ELT_CMP_OP_GE:
		sgn = true;
		co = ~co;
	}
	uint32_t w1 = MAKE_LBELT_DATA_6_16(LB2_ELT_CMP32, !m_final, (m64 ? LB_ELT_CMP_N2_64BIT : 0) | (sgn ? LB_ELT_CMP_N2_SIGNED : 0), co, 0);
	m_outputCounterPlace = m_lb.size();
	m_outputCounterMode = LBELT_MASK_DATA_W0;
	m_lb.push_back(w1);
	m_lb.push_back(uint32_t(val));
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitCmp64(uint64_t cmpOp, uint64_t val)
{
	bool sgn = false;
	uint32_t co = uint32_t(cmpOp);
	switch (co)
	{
	case ~LB_ELT_CMP_OP_LT:
	case ~LB_ELT_CMP_OP_GT:
	case ~LB_ELT_CMP_OP_LE:
	case ~LB_ELT_CMP_OP_GE:
		sgn = true;
		co = ~co;
	}
	uint32_t w1 = MAKE_LBELT_DATA_6_16(LB2_ELT_CMP64, !m_final, sgn ? LB_ELT_CMP_N2_SIGNED : 0, co, 0);
	m_outputCounterPlace = m_lb.size();
	m_outputCounterMode = LBELT_MASK_DATA_W0;
	m_lb.push_back(w1);
	m_lb.push_back(uint32_t(val & 0xFFFFFFFF));
	m_lb.push_back(uint32_t(val >> 32));
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitCmp(uint64_t cmpOp, uint64_t m64)
{
	bool sgn = false;
	uint32_t co = uint32_t(cmpOp);
	switch (co)
	{
	case ~LB_ELT_CMP_OP_LT:
	case ~LB_ELT_CMP_OP_GT:
	case ~LB_ELT_CMP_OP_LE:
	case ~LB_ELT_CMP_OP_GE:
		sgn = true;
		co = ~co;
	}
	uint32_t w1 = MAKE_LBELT_DATA_6_16(LB2_ELT_CMP, !m_final, (m64 ? LB_ELT_CMP_N2_64BIT : 0) | (sgn ? LB_ELT_CMP_N2_SIGNED : 0), co, 0);
	m_outputCounterPlace = m_lb.size();
	m_outputCounterMode = LBELT_MASK_DATA_W0;
	m_lb.push_back(w1);
	m_linkPlace.section = sec_lb;
	m_linkPlace.address = m_lb.size();
}

void LbtCompiler::emitRd(uint64_t elt, uint64_t varNameId, uint64_t offset)
{
	size_t vni = size_t(varNameId);
	if (offset >= 0x1000000)
		throw std::runtime_error("Offset too large");
	uint32_t w1 = MAKE_LBELT_DATA(uint32_t(elt), !m_final, uint32_t(offset));
	addVarRef(vni);
	m_lb.push_back(w1);
}

void LbtCompiler::emitRdx(uint64_t elt, uint64_t slot, uint64_t offset)
{
	if (slot >= 0x1000 || offset >= 0x1000)
		throw std::runtime_error("Slot or offset too large");
	uint32_t w1 = MAKE_LBELT_DATA_12(uint32_t(elt), !m_final, uint32_t(slot), uint32_t(offset));
	m_lb.push_back(w1);
}

void LbtCompiler::emitRdslot(uint64_t slot)
{
	if (slot >= 0x1000000)
		throw std::runtime_error("Slot too large");
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_RDSLOT, !m_final, uint32_t(slot));
	m_lb.push_back(w1);
}

void LbtCompiler::emitRdcx(uint64_t slot, uint64_t offset)
{
	if (slot >= 0x1000000 || offset >= 0x100000000)
		throw std::runtime_error("Slot or offset too large");
	uint32_t w1 = MAKE_LBELT_DATA(LB2_ELT_RDSLOT, !m_final, uint32_t(slot));
	m_lb.push_back(w1);
	m_lb.push_back(uint32_t(offset));
}

void LbtCompiler::addOutput()
{
	if (m_outputCounterPlace != -1)
		incrementByMask(m_lb[m_outputCounterPlace], m_outputCounterMode, "Too many outputs");
	m_lb.push_back(0);
	m_linkPlace.address = uint32_t(m_lb.size());
}

void LbtCompiler::addValue(uint64_t val)
{
	if (val >= 0x100000000)
		throw std::runtime_error("Value too large");
	m_lb.push_back(uint32_t(val));
}

void LbtCompiler::addVarRef(uint64_t varNameId)
{
	size_t vni = size_t(varNameId);

	Ref ref;
	ref.section = sec_lb;
	ref.address = uint32_t(m_lb.size());
	m_varLinkMap.insert(std::make_pair(vni, ref));
}

void LbtCompiler::addVarRef(uint64_t varNameId, uint64_t offset)
{
	if (offset > 0x1000000)
		throw std::runtime_error("Offset too large");
	addVarRef(varNameId);
	m_lb.push_back(uint32_t(offset));
}

void LbtCompiler::addStrRef(uint64_t strId)
{
	size_t si = size_t(strId);

	uint32_t n;
	StringSectionMap::const_iterator it = m_stringSectionMap.find(si);

	if (it == m_stringSectionMap.end())
	{
		n = uint32_t(m_stringSectionTable.size());
		m_stringSectionMap.insert(std::make_pair(si, n));
		m_stringSectionTable.push_back(si);
	} else
		n = it->second;

	Ref strRef;
	strRef.section = sec_strings;
	strRef.address = n;

	Ref lbRef;
	lbRef.section = sec_lb;
	lbRef.address = uint32_t(m_lb.size());

	m_links[strRef][0].insert(lbRef);

	m_lb.push_back(0);
}

void LbtCompiler::addPushed()
{
	incrementByMask(m_lb[m_valueCounterPlace], LBELT_MASK_DATA_B2, "Too many pushed values");
}

void LbtCompiler::registerProcRef(size_t procNameId)
{
	size_t pni = size_t(procNameId);
	ProcMap::const_iterator it = m_procMap.find(pni);
	uint32_t n;
	if (it != m_procMap.end())
		n = it->second;
	else
	{
		n = uint32_t(m_procTable.size());
		m_procMap.insert(std::make_pair(pni, n));
		m_procTable.push_back(pni);
	}
	Ref procRef;
	procRef.section = sec_pubnames;
	procRef.address = n;
	Ref lbRef;
	lbRef.section = sec_lb;
	lbRef.address = uint32_t(m_lb.size());
	m_links[procRef][0].insert(lbRef);
}

std::string LbtCompiler::getLbVarName()
{
	std::ostringstream ss;
	ss << "%LB:" << m_lbVarNo++;
	return ss.str();
}

void LbtCompiler::incrementByMask(uint32_t& value, uint32_t mask, char const* errorMsg)
{
	uint32_t one = 1;
	while ((mask & one) == 0)
		one <<= 1;
	uint32_t n = value & mask;
	value &= ~mask;
	n += one;
	if ((n & mask) != n)
		throw std::runtime_error(errorMsg);
	value |= n;
}

void LbtCompiler::writeSections(BufferWriter& writer)
{
	typedef std::map<Section, uint32_t> SectionMap;
	SectionMap sectionMap;
	uint32_t sectionNo = 0;

	if (!m_procTable.empty())
	{
		sectionMap[sec_pubnames] = sectionNo++;

		Buffer buf;
		BufferWriter dataWriter(buf);

		size_t s = m_procTable.size();
		for (size_t i = 0; i < s; ++i)
		{
			std::string const& procName = getIdent(m_procTable[i]);
			KDUWriteVectorAttr(&dataWriter, KDU_ATTR_TYPE_STRING, procName.data(), procName.length(), KDU_ATTR_F_EXB | KDU_ATTR_F_SOR, KDU_PUBNAME_TYPE_PROC);
		}

		KDUSectionInfo sectionInfo;
		sectionInfo.sectionType = KDBID_PUBNAMES;
		sectionInfo.fragmentID = 0;
		sectionInfo.data = &buf.front();
		sectionInfo.size = buf.size();
		sectionInfo.raw = 0;
		KDUWriteSection(&writer, sectionInfo);
	}

	if (!m_varTable.empty())
	{
		sectionMap[sec_vars] = sectionNo++;

		Buffer buf;
		BufferWriter dataWriter(buf);

		size_t s = m_varTable.size();
		for (size_t i = 0; i < s; ++i)
		{
			Var const& var = m_varTable[i];
			if (var.alignment)
				KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_UINT, var.size, KDU_ATTR_F_SOR | KDU_ATTR_F_EXB, var.alignment);
			else
				KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_UINT, var.size, KDU_ATTR_F_SOR);
			std::string const& varName = getIdent(var.varNameId);
			KDUWriteVectorAttr(&dataWriter, KDU_ATTR_TYPE_STRING, varName.data(), varName.length());
		}

		KDUSectionInfo sectionInfo;
		sectionInfo.sectionType = KDBID_VARIABLES;
		sectionInfo.fragmentID = 0;
		sectionInfo.data = &buf.front();
		sectionInfo.size = buf.size();
		sectionInfo.raw = 0;
		KDUWriteSection(&writer, sectionInfo);
	}

	if (!m_verdictTable.empty())
	{
		sectionMap[sec_verdicts] = sectionNo++;

		Buffer buf;
		BufferWriter dataWriter(buf);

		size_t s = m_verdictTable.size();
		for (size_t i = 0; i < s; ++i)
		{
			uint32_t fmtFlags = KDU_VERDICT_FMT_VERSION_1;
			KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_UINT, fmtFlags, KDU_ATTR_F_SOR);
			std::string const& vdctName = getStringLiteral(m_verdictTable[i]);
			KDUWriteVectorAttr(&dataWriter, KDU_ATTR_TYPE_STRING, vdctName.data(), vdctName.length());
		}

		KDUSectionInfo sectionInfo;
		sectionInfo.sectionType = KDBID_VERDICTS;
		sectionInfo.fragmentID = 0;
		sectionInfo.data = &buf.front();
		sectionInfo.size = buf.size();
		sectionInfo.raw = 0;
		KDUWriteSection(&writer, sectionInfo);
	}

	if (!m_stringSectionTable.empty())
	{
		sectionMap[sec_strings] = sectionNo++;

		Buffer buf;
		BufferWriter dataWriter(buf);

		size_t s = m_stringSectionTable.size();
		for (size_t i = 0; i < s; ++i)
		{
			std::string const& str = getStringLiteral(m_stringSectionTable[i]);
			KDUWriteVectorAttr(&dataWriter, KDU_ATTR_TYPE_STRING, str.data(), str.length(), KDU_ATTR_F_SOR);
		}

		KDUSectionInfo sectionInfo;
		sectionInfo.sectionType = KDBID_STRINGTABLE;
		sectionInfo.fragmentID = 0;
		sectionInfo.data = &buf.front();
		sectionInfo.size = buf.size();
		sectionInfo.raw = 0;
		KDUWriteSection(&writer, sectionInfo);
	}

	if (!m_wordTable.empty())
	{
		sectionMap[sec_words] = sectionNo++;

		Buffer buf;
		BufferWriter dataWriter(buf);

		size_t s = m_wordTable.size();
		for (size_t i = 0; i < s; ++i)
		{
			std::string const& str = getStringLiteral(m_wordTable[i]);
			KDUWriteVectorAttr(&dataWriter, KDU_ATTR_TYPE_STRING, str.data(), str.length(), KDU_ATTR_F_SOR);
		}

		KDUSectionInfo sectionInfo;
		sectionInfo.sectionType = KDBID_TEST_WORDS;
		sectionInfo.fragmentID = 0;
		sectionInfo.data = &buf.front();
		sectionInfo.size = buf.size();
		sectionInfo.raw = 0;
		KDUWriteSection(&writer, sectionInfo);
	}

	if (!m_lb.empty())
	{
		sectionMap[sec_lb] = sectionNo++;

		KDUSectionInfo sectionInfo;
		sectionInfo.sectionType = KDBID_LB_CONFIG_V2;
		sectionInfo.fragmentID = 0;
		sectionInfo.data = reinterpret_cast<uint8_t const*>(&m_lb.front());
		sectionInfo.size = m_lb.size() * sizeof (uint32_t);
		sectionInfo.raw = 1;
		KDUWriteSection(&writer, sectionInfo);
	}

	Buffer buf;
	BufferWriter dataWriter(buf);

	Links::const_iterator const linkEnd = m_links.end();
	for (Links::const_iterator it = m_links.begin(); it != linkEnd; ++it)
	{
		Ref const& src = it->first;
		ModeLinkTargets const& modeTargets = it->second;

		ModeLinkTargets::const_iterator const modeEnd = modeTargets.end();
		for (ModeLinkTargets::const_iterator jt = modeTargets.begin(); jt != modeEnd; ++jt)
		{
			uint8_t flag = jt->first;
			LinkTargets const& targets = jt->second;
			if (flag)
				KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_ID, src.address, KDU_ATTR_F_SOR | KDU_ATTR_F_EXB, flag);
			else
				KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_ID, src.address, KDU_ATTR_F_SOR);
			KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_SECT, sectionMap[src.section]);

			LinkTargets::const_iterator const targetEnd = targets.end();
			for (LinkTargets::const_iterator kt = targets.begin(); kt != targetEnd; ++kt)
			{
				Ref const& tgt = *kt;

				uint8_t mode = 0;
				if (tgt.section == sec_lb)
				{
					switch (src.section)
					{
					case sec_pubnames:
					case sec_verdicts:
					case sec_vars:
					case sec_lb:
						mode = LBOFF_MODE_MASK_24;
						break;
					default:
						mode = LBOFF_MODE_MASK_32;
					}
				}
				if (mode)
					KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_REL, tgt.address, KDU_ATTR_F_EXB, mode);
				else
					KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_REL, tgt.address);
				KDUWriteAttr(&dataWriter, KDU_ATTR_TYPE_SECT, sectionMap[tgt.section]);
			}
		}
	}

	KDUSectionInfo sectionInfo;
	sectionInfo.sectionType = KDBID_LINKTABLE;
	sectionInfo.fragmentID = 0;
	sectionInfo.data = &buf.front();
	sectionInfo.size = buf.size();
	sectionInfo.raw = 0;
	KDUWriteSection(&writer, sectionInfo);
}

bool compileLbt(char const* buffer, size_t bufferSize, uint32_t vendorId, uint32_t blockId, uint32_t blockSeq, KLAV_Bin_Writer* writer)
{
	LbtCompiler compiler;
	LbtParser parser(compiler);
	try
	{
		if (!parser.parse(buffer, bufferSize))
			throw std::runtime_error(parser.errorMsg());

		LbtCompiler::Buffer sectionBuf;
		LbtCompiler::BufferWriter sectionWriter(sectionBuf);
		compiler.writeSections(sectionWriter);

		KDUBlockInfo blockInfo;
		blockInfo.data = &sectionBuf.front();
		blockInfo.size = sectionBuf.size();
		blockInfo.type = KDU_BLOCK_TYPE_LINK;
		KDU_SUB_HEADER_LINK_V2 subhdr;
		subhdr.kdu_arch_id = 0;
		subhdr.kdu_block_id = blockId;
		memset(subhdr.kdu_block_long_id, 0, sizeof subhdr.kdu_block_long_id);
		subhdr.kdu_vendor_id = vendorId;
		subhdr.kdu_group_id = 0;
		subhdr.kdu_block_seq = blockSeq;
		blockInfo.subhdr = reinterpret_cast<uint8_t const*>(&subhdr);
		blockInfo.subhdr_len = sizeof subhdr;
		KDUErr err = KDUWriteBlock(writer, blockInfo);
		if (err != KDU_E_OK)
			throw std::runtime_error("KDU write failed");
	} catch (std::exception& e)
	{
		fprintf(stderr, "Compile error: %s, @ line %u\n", e.what(), parser.lastLineNo());
		return false;
	}

	return true;
}
