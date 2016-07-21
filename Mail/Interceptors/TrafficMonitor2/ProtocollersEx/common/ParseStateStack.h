#if !defined(__KL_PARSESTATESTACK_H)
#define __KL_PARSESTATESTACK_H

#include <deque>
#include <boost/shared_ptr.hpp>

namespace TrafficProtocoller
{
namespace Util
{

//////////////////////////////////////////////////////////////////////////
template <typename StateTypeEnum, typename IStateData>
class ParseStateStack
{
public:
	typedef boost::shared_ptr<IStateData> IStateDataPtr;
	struct StateInfo
	{
		StateInfo(StateTypeEnum new_type)
			: type(new_type)
		{
		}
		StateInfo(StateTypeEnum new_type, IStateDataPtr new_data)
			: type(new_type)
			, data(new_data)
		{
		}
		StateTypeEnum type;
		IStateDataPtr data;
	private:
		StateInfo();
	};
public:
	ParseStateStack()				
		{}
	bool Empty()
		{ return m_stack.empty(); }
	void Clear()
		{ m_stack.clear(); }
	void Push(StateTypeEnum type)
		{ m_stack.push_back(StateInfo(type)); }
	void Push(StateTypeEnum type, IStateDataPtr data)
		{ m_stack.push_back(StateInfo(type, data)); }
	void Push(const StateInfo& state_info)
		{ m_stack.push_back(state_info); }
	const StateInfo& Peek() const
		{ return m_stack.back(); }
	void Pop()
		{ m_stack.pop_back(); }
private:
	std::deque<StateInfo> m_stack;
};

}  // namespace Util
}  // namespace TrafficProtocoller
	
#endif  // !defined(__KL_PARSESTATESTACK_H)
