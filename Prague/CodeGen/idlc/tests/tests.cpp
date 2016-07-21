#include <memory>
#include <sstream>
#include <iostream>
#include <fstream>

#include <cassert>
#include <cstdio>

#include <set>
#include <map>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

#include "Scaner.hpp"
#include "Parser.hpp"

#include "ErrorFactory.hpp"

#include "ValueFactory.hpp"
#include "Lexeme.hpp"
#include "LexemeValue.hpp"
#include "ErrorNode.hpp"

#include "StubNode.hpp"
#include "NameNode.hpp"
#include "OperationNode.hpp"

#include "ArgsLayout.hpp"

#include "namecontext/ConstEval.hpp"
#include "namecontext/GatherDeclaredNames.hpp"
#include "namecontext/NameResolver.hpp"
#include "namecontext/CheckAttrs.hpp"

#include "namecontext/Method.hpp"
#include "namecontext/Value.hpp"
#include "namecontext/ConstValue.hpp"
#include "namecontext/Type.hpp"
#include "namecontext/Struct.hpp"
#include "namecontext/Enum.hpp"
#include "namecontext/SequenceType.hpp"
#include "namecontext/Interface.hpp"
#include "namecontext/Typedef.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/Component.hpp"
#include "namecontext/Vendor.hpp"
#include "namecontext/Message.hpp"
#include "namecontext/MessageClass.hpp"
#include "namecontext/MessageClassExtend.hpp"
#include "namecontext/Property.hpp"
#include "namecontext/Implementation.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/framework.hpp>

#ifdef WIN32
#define UNLINKCALL  _unlink
#else
#include <fcntl.h>
#define UNLINKCALL  unlink
#endif

#ifndef WIN32
#ifdef BOOST_CHECK_EQUAL
#undef BOOST_CHECK_EQUAL
#endif

#define BOOST_CHECK_EQUAL(a,b) BOOST_CHECK((a) == (b))
#endif

using namespace idl::scaner;
using idl::parser::Value;
using idl::ErrorFactory;

using boost::dynamic_pointer_cast;

class ScanerTest {
  public:
    void prepare(const std::string& data) {
        _scaner = boost::shared_ptr<Scaner>(new Scaner());
        _stream = boost::shared_ptr<std::istringstream>(new std::istringstream(data));
        _scaner->setSource("<test stream>", _stream);
        //_scaner->switch_streams(_stream.get(), 0);
    }
    
    Value lexeme() {
    	int id = _scaner->yylex();
    	
    	Value value;
    	
		value._context = _scaner->getLexeme().getContext();
		value._lexeme = boost::shared_ptr<idl::scaner::LexemeValue>(new idl::scaner::LexemeValue(_scaner->getLexeme().getValue()));
		
		BOOST_CHECK_EQUAL(value._lexeme->getId(), id);
		
        return value;
    }
    
    Value lexeme(Lexeme::Id id) {
    	Value value = lexeme();
		BOOST_CHECK_EQUAL(value._lexeme->getId(), id);
        return value;
    }
    
    void test() {
        prepare("");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        Value value;
        
        prepare("interface");
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 9);
        BOOST_CHECK_EQUAL(value._context.getText(), "interface");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare(" interface");
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 10);
        BOOST_CHECK_EQUAL(value._context.getText(), "interface");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare(" interface ");
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 10);
        BOOST_CHECK_EQUAL(value._context.getText(), "interface");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare(" \tinterface ");
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 6);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 14);
        BOOST_CHECK_EQUAL(value._context.getText(), "interface");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare("interface \r\n interface ");
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 9);
        BOOST_CHECK_EQUAL(value._context.getText(), "interface");
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 2);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 10);
        BOOST_CHECK_EQUAL(value._context.getText(), "interface");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare("for \n if // comment interface\n while");
        value = lexeme(Lexeme::Ids::For);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 3);
        BOOST_CHECK_EQUAL(value._context.getText(), "for");
        value = lexeme(Lexeme::Ids::If);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 2);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 3);
        BOOST_CHECK_EQUAL(value._context.getText(), "if");
        value = lexeme(Lexeme::Ids::While);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 3);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 3);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 6);
        BOOST_CHECK_EQUAL(value._context.getText(), "while");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare("for \n /*if // comment*/ interface\n while");
        value = lexeme(Lexeme::Ids::For);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 1);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 3);
        value = lexeme(Lexeme::Ids::Interface);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 2);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 20);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 28);
        value = lexeme(Lexeme::Ids::While);
        BOOST_CHECK_EQUAL(value._context.getFirstLine(), 3);
        BOOST_CHECK_EQUAL(value._context.getFirstColon(), 2);
        BOOST_CHECK_EQUAL(value._context.getLastLine(), 3);
        BOOST_CHECK_EQUAL(value._context.getLastColon(), 6);
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        
        prepare("for @ interface`");
        value = lexeme(Lexeme::Ids::For);
        value = lexeme(Lexeme::Ids::ScanerError);
        BOOST_CHECK_EQUAL(value._context.getText(), "@");
        value = lexeme(Lexeme::Ids::Interface);
        value = lexeme(Lexeme::Ids::ScanerError);
        BOOST_CHECK_EQUAL(value._context.getText(), "`");
        BOOST_CHECK(lexeme()._lexeme->getId() == 0);
        
        prepare(" 1234 ");
        value = lexeme(Lexeme::Ids::NumberInt);
        BOOST_CHECK_EQUAL(value._lexeme->getInt(), 1234);
        BOOST_CHECK_EQUAL(value._context.getText(), "1234");

        prepare(" 0x1234 ");
        value = lexeme(Lexeme::Ids::NumberInt);
        BOOST_CHECK_EQUAL(value._lexeme->getInt(), 0x1234);
        BOOST_CHECK_EQUAL(value._context.getText(), "0x1234");

        prepare(" 0xab ");
        value = lexeme(Lexeme::Ids::NumberInt);
        BOOST_CHECK_EQUAL(value._lexeme->getInt(), 0xab);
        BOOST_CHECK_EQUAL(value._context.getText(), "0xab");

        prepare(" 1.200 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), 1.2);
        BOOST_CHECK_EQUAL(value._context.getText(), "1.200");

        prepare(" .2 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), .2);
        BOOST_CHECK_EQUAL(value._context.getText(), ".2");

        prepare(" 1. ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), 1.);
        BOOST_CHECK_EQUAL(value._context.getText(), "1.");

        prepare(" 1.2e2 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), 1.2e2);

        prepare(" .2e3 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), .2e3);

        prepare(" 2.e3 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), 2.e3);

        prepare(" 2.e+3 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), 2.e3);

        prepare(" 2.e-3 ");
        value = lexeme(Lexeme::Ids::NumberFloat);
        BOOST_CHECK_EQUAL(value._lexeme->getFloat(), 2.e-3);

        prepare(" cocos ");
        value = lexeme(Lexeme::Ids::Name);
        BOOST_CHECK_EQUAL(value._lexeme->getString(), "cocos");

        prepare(" \"cocos\" ");
        value = lexeme(Lexeme::Ids::QuotedString);
        BOOST_CHECK_EQUAL(value._lexeme->getString(), "cocos");
    }
    
  private:
    boost::shared_ptr<Scaner>               _scaner;

    boost::shared_ptr<std::istringstream>   _stream;
};

class ParsingSuite {
  public:
	template <class Type>
	class NodeCounter : public idl::parser::Visitor {
	  public:
	  	NodeCounter() : _count(0) {}
	  	
	  	virtual bool visitBefore(Type*) {
	  		++_count;
	  		return true;
	  	}
	  	
	  	int getCount() const {
	  		return _count;
	  	}
	  	
	  private:
	  	int	_count;
	};

	typedef NodeCounter<idl::parser::ErrorNode> ErrorCollector;
	
    static idl::parser::NodeInterface::Ptr parseSyntax(const std::string& data) {
        idl::parser::Parser parser;
        
        ErrorFactory::instance().clear();
        
        boost::shared_ptr<std::istream> stream = boost::shared_ptr<std::istringstream>(new std::istringstream(data));
        idl::parser::NodeInterface::Ptr result = parser.parse("<test stream>", stream);
        return result;
    }
    
    static idl::parser::NodeInterface::Ptr parse(const std::string& data, size_t expectedErrorsCount = 0) {
    	idl::parser::NodeInterface::Ptr result = parseSyntax(data);
        BOOST_CHECK(checkErrors(expectedErrorsCount));
        return result;
    }
    
    static bool checkErrors(size_t expectedCount = 0) {
    	return ErrorFactory::instance().getErrors().size() == expectedCount;
    }
};


class ParserTest {
  public:
    void prepare() {
    }

    bool check_syntax(const std::string& data) {
		//ParsingSuite::ErrorCollector counter;
        idl::parser::NodeInterface::Ptr root = ParsingSuite::parseSyntax(data);
		//root->visit(counter);
		return ParsingSuite::checkErrors();
    }
    
    void test() {
        prepare();
        
        BOOST_CHECK(check_syntax(""));
        
        // semicolon
        BOOST_CHECK(check_syntax(";"));
        BOOST_CHECK(check_syntax(";;"));
        
        // namespace
        BOOST_CHECK(check_syntax("namespace a {}"));
        BOOST_CHECK(check_syntax("namespace a {}"));
        BOOST_CHECK(check_syntax("namespace a { namespace b { namespace c {} } }"));
        BOOST_CHECK(check_syntax("namespace a {} namespace b { namespace c {} } ;"));
        
        // type definitions 
        BOOST_CHECK(check_syntax("typedef s32 Int32;"));
        
        BOOST_CHECK(check_syntax("typedef array<u32, 10> Array1;"));
        BOOST_CHECK(check_syntax("typedef string<12> String;"));
        BOOST_CHECK(check_syntax("typedef sequence<float, 12> Seq;"));
        BOOST_CHECK(check_syntax("enum E1 { a,b,c,d };"));
        BOOST_CHECK(check_syntax("enum E2 { a,b,c,d, };"));
        BOOST_CHECK(check_syntax("struct A { s8 a; s16 b; };"));
        BOOST_CHECK(check_syntax("typedef a.I xyz;"));
        BOOST_CHECK(check_syntax("namespace a { typedef a.I xyz; }"));
                    
        // Referencies test
        BOOST_CHECK(check_syntax("struct A { s8* a; s16 b; };"));
        BOOST_CHECK(check_syntax("struct A { const s8 a; s16 b; };"));
        BOOST_CHECK(check_syntax("struct A { const s8* a; s16 b; };"));
        BOOST_CHECK(check_syntax("struct A { const s8*** a; s16 b; };"));
        BOOST_CHECK(check_syntax("struct A { const x.y.z* a; s16 b; };"));
        BOOST_CHECK(check_syntax("typedef sequence<float, 12>* Seq;"));
        BOOST_CHECK(check_syntax("typedef const sequence<x.y.z, 12>* Seq;"));
        
        // const definition
        BOOST_CHECK(check_syntax("const s32 var = 1234;"));
        BOOST_CHECK(check_syntax("const float var = 1.234*.12+2.e3;"));
        BOOST_CHECK(check_syntax("const s32 var1 = 1234;"
        			"const float var2 = 1.234*.12+2.e3*var1;"));
        
        // interface definition
        BOOST_CHECK(check_syntax("interface I {}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	typedef int A;"
        	"	const float Pi = 3.14;"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	typedef int A;"
        	"	const float Pi = 3.14;"
        	"	method void call();"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	typedef int A;"
        	"	const float Pi = 3.14;"
        	"	method s8 call(string<10> arg1);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	[] method s16 call(int arg1);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	[ version(1) ] method string<10> call(int arg1);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	[ version(1) access(read) ] method void* call(int arg1);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	[ version(1) access(read, write) ] method const u8* call(int arg1);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	[ version(1) getter(sample) ] method void call(int arg1);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	prop s8 name;"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	message_cls clsname {}"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	message_cls clsname { message msgname; }"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"component C {}"));
        BOOST_CHECK(check_syntax(
        	"component C { implementation Impl {} }"));
        BOOST_CHECK(check_syntax(
        	"component C {"
        	"   implementation Impl {"
        	"		prop s8 name;"
        	"	}"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"component C {"
        	"   implementation Impl {"
        	"		extend message_cls cls{};"
        	"	}"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"component C {"
        	"   implementation Impl {"
        	"		add prop s8 name1;"
        	"	}"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"component C {"
        	"   implementation Impl {"
        	"		add message_cls cls1 {};"
        	"	}"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"component C {"
        	"   implementation Impl {"
        	"		prop s8 name;"
        	"		extend message_cls cls{};"
        	"		add prop s8 name1;"
        	"		add message_cls cls1 {};"
        	"	}"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"component C { implementation Impl {"
        	"	prop s8 name;"
        	"	extend message_cls cls{}"
        	"	add prop s8 name1;"
        	"	add message_cls cls1 { message msgname; }"
        	"}}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	typedef int A;"
        	"	const float Pi = 3.14;"
        	"	method s8 call(string<10> arg1, int arg2);"
        	"}"));
        BOOST_CHECK(check_syntax(
        	"interface I {"
        	"	"
        	"	typedef int A;"
        	"	const float Pi = 3.14;"
        	"	method void call1();"
        	"	method float call2(string<10> arg1);"
        	"	method A call3(string<10> arg1, int arg2);"
        	"	method A* call4(string<10> arg1, int arg2); "
        	"}"));
        	
        BOOST_CHECK(check_syntax(
        	"typedef s32 SInt32;"
        	"interface I {"
        	"	"
        	"	method I* call(SInt32 a);"
        	"}"));
        	
        BOOST_CHECK(check_syntax(
        	"typedef sequence<s32,10> SInt32;"
        	"interface I {"
        	"	"
        	"	method SInt32 call(SInt32 a);"
        	"}"));
        	
		BOOST_CHECK(check_syntax(
        	"namespace ns1 {interface I1 {}}"
        	));
        	
		BOOST_CHECK(check_syntax(
        	"namespace ns1 { interface I1 {  enum E {a, b, c}; } }"
        	));

        // casual test (provoked crash)
        BOOST_CHECK(!check_syntax(
        	"interface I {"
        	"	"
        	"	method void x() out"
        	"}"));
        	
        // component definition
        BOOST_CHECK(check_syntax(
        	"namespace ns1 { namespace ns2 {"
        	"interface I1 {}"
        	"}}"
        	"namespace ns1 {"
        	"interface I2 {}"
        	"}"
        	"component C1 {"
        	"}"
        	));
        	
        /*BOOST_CHECK(check_syntax(
        	"namespace ns1 {"
        	"interface I1 {}"
        	"namespace ns1 {"
        	"interface I2 {}"
        	"namespace ns3 {"
        	"component C1 {"
        	"	implements ns1.I2, ns1.ns2.I1;"
        	"	requires ns1.ns2.I1;"
        	"}"
        	"}}}"
        	));*/

		{        	
			ParsingSuite::ErrorCollector counter;
			idl::parser::NodeInterface::Ptr root = ParsingSuite::parse(
					"bred>;"
					"const s16 a = 10++;"
					"const s16 a = 10;"
					"const s16 a = 10;"
					"const s16 a = 10;"
					"bred1\nbred2\nbred3", 3);
			BOOST_CHECK(root);
			root->visit(counter);
			BOOST_CHECK_EQUAL(counter.getCount(), 3);
			BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors().size(), (size_t)3);
			BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::ParseError);
			BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[1].first, ErrorFactory::ParseError);
			BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[2].first, ErrorFactory::ParseError);
		}
		/*{        	
			idl::parser::NodeInterface::Ptr root = ParsingSuite::parse(
					"const u8 underscored_identifer = 0;", 1);
			BOOST_CHECK(root);
			BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::UnderscoresAreNotAllowed);
		}*/
		{
			ParsingSuite::NodeCounter<idl::parser::OperationNode<idl::parser::Operations::Interface> > counter;
			idl::parser::NodeInterface::Ptr root = ParsingSuite::parse(
				"namespace ns1 {interface I1 {}}"
				);
			BOOST_CHECK(root);
			root->visit(counter);
			BOOST_CHECK_EQUAL(counter.getCount(), 1);
		}
    }
    
  private:
};

class TransformTest {
    void prepare() {
    }

  public:
	template <class Type>
  	void testCount(idl::parser::NodeInterface::Ptr root, int expectedCount) {
			ParsingSuite::NodeCounter<Type> counter;
			root->visit(counter);
			BOOST_CHECK_EQUAL(counter.getCount(), expectedCount);
  	}
  
  	void test() {
		prepare();
		
        idl::parser::NodeInterface::Ptr root = ParsingSuite::parse(
        	"namespace ns1 {}"
        	"interface I1 {}"
        	);
        	
		testCount<idl::parser::NameNode>(root, 2);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Dot> >(root, 0);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Namespace> >(root, 1);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Interface> >(root, 1);

		class Unchanged : public idl::parser::Transformer {
		} unchanged;

		idl::parser::NodeInterface::Ptr sameTree = root->transform(unchanged);

		BOOST_CHECK(sameTree == root);
		testCount<idl::parser::StubNode>(sameTree, 0);
		testCount<idl::parser::NameNode>(sameTree, 2);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Dot> >(sameTree, 0);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Namespace> >(sameTree, 1);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Interface> >(sameTree, 1);

		class RemoveNameFromInterface : public idl::parser::Transformer {
			virtual idl::parser::NodeInterface::Ptr transform(idl::parser::OperationNode<idl::parser::Operations::Interface>::Ptr node) {
				idl::parser::NameNode::Ptr nameNode;
				node->getArg(idl::ArgsLayout::Interface::Name, nameNode);
				BOOST_CHECK(nameNode);
				BOOST_CHECK_EQUAL(nameNode->getName(), "I1");
				return node->replaceArg(idl::ArgsLayout::Interface::Name, new idl::parser::StubNode(nameNode->getContext()));
			}
		} removal;

		idl::parser::NodeInterface::Ptr stubbed = root->transform(removal);
		
		BOOST_CHECK(stubbed != root);
		testCount<idl::parser::StubNode>(stubbed, 1);
		testCount<idl::parser::NameNode>(stubbed, 1);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Dot> >(stubbed, 0);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Namespace> >(stubbed, 1);
		testCount<idl::parser::OperationNode<idl::parser::Operations::Interface> >(stubbed, 1);
  	}
    
  private:
};

class DeclarationGatheringTest {
    void prepare() {
    }

  public:
  	typedef std::set<std::string>	Names;
  
  	void collect(idl::NameContext::Ptr node, Names& names) {
  		BOOST_CHECK(names.insert(node->getNamePath().str()).second);
  		
  		idl::NameContext::Children::const_iterator i = node->getChildren().begin(), end = node->getChildren().end();
  		for(; i != end; ++i) {
  			collect(*i, names);
  		}
  	}
  
  	void find(const std::string& data, const std::string& name) {
        idl::parser::NodeInterface::Ptr node = ParsingSuite::parse(data);
		idl::NameContext::Ptr root = idl::gatherDeclaredNames(node);

		Names names;
		
		collect(root, names);
		
		BOOST_CHECK(names.find(name) != names.end());
  	}
  
  	void test() {
		prepare();
		
        find(
			"namespace ns {} namespace ns {}",
        	"ns");
        	
		BOOST_CHECK(ParsingSuite::checkErrors());
				
        find(
			"const s16 value = 12;",
        	"value");
        	
		BOOST_CHECK(ParsingSuite::checkErrors());
				
        find(
        	"namespace ns1 {}"
        	"interface I1 {}",
        	"ns1");
        	
        find(
        	"namespace ns1 {}"
        	"interface I1 {}",
        	"I1");
        	
        find(
        	"namespace ns1 { interface I1 {} }",
        	"ns1.I1");
        	
        find(
        	"namespace ns1 { interface I1 {  const s8 a = 0; } }",
        	"ns1.I1.a");
        	
        find(
        	"namespace ns1 { interface I1 {  enum E {a, b, c}; } }",
        	"ns1.I1.E.b");
        	
        find(
        	"namespace ns1 { interface I1 {  struct S {char a; float b; I1 c;}; } }",
        	"ns1.I1.S.c");
        	
        find(
        	"namespace ns1 { interface I1 {  typedef I1 IFace; } }",
        	"ns1.I1.IFace");
        	
        find(
        	"namespace ns1 { interface I1 {  method void func(); } }",
        	"ns1.I1.func");
        	
        find(
        	"namespace ns1 { interface I1 {  method u8 func(s16 arg0, s32 arg1); } }",
        	"ns1.I1.func.arg1");
  	}

  private:
};

class NameResolvingTest {
    void prepare() {
    }

  public:
  	typedef std::map<std::string, idl::NameContext::Ptr>	Names;
  	idl::RootNameContext::Ptr _root;
  	idl::parser::NodeInterface::Ptr _rootNode;
  
  	static void collect(idl::NameContext::Ptr node, Names& names) {
  		BOOST_CHECK(names.insert(Names::value_type(node->getNamePath().str(), node)).second);
  		
  		idl::NameContext::Children::const_iterator i = node->getChildren().begin(), end = node->getChildren().end();
  		for(; i != end; ++i) {
  			collect(*i, names);
  		}
  	}
  
  	void resolvedNames(const std::string& data, Names& names/*, size_t expectedErrorsCount = 0*/) {
        _rootNode = ParsingSuite::parse(data);
		_root = idl::gatherDeclaredNames(_rootNode);
		idl::resolveNames(_root, _rootNode);
		
		//BOOST_CHECK(ParsingSuite::checkErrors(expectedErrorsCount));
		collect(_root, names);
  	}
  
  	static idl::Struct::Ptr asStruct(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Struct>(nameContext);
  	}
  	
  	static idl::Enum::Ptr asEnum(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Enum>(nameContext);
  	}
  	
  	static idl::Type::Ptr asType(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Type>(nameContext);
  	}
  	
  	static idl::Value::Ptr asValue(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Value>(nameContext);
  	}
  	
  	static idl::ConstValue::Ptr asConstValue(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::ConstValue>(nameContext);
  	}
  	
  	static idl::SequenceType::Ptr asSequenceType(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::SequenceType>(nameContext);
  	}
  	
  	static idl::BasicType::Ptr asBasicType(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::BasicType>(nameContext);
  	}
  	
  	static idl::Interface::Ptr asInterface(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Interface>(nameContext);
  	}
  	
  	static idl::Implementation::Ptr asImplementation(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Implementation>(nameContext);
  	}
  	
  	static idl::Component::Ptr asComponent(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Component>(nameContext);
  	}
  	
  	static idl::Vendor::Ptr asVendor(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Vendor>(nameContext);
  	}
  	
  	static idl::RefType::Ptr asRef(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::RefType>(nameContext);
  	}
  	
  	static idl::Method::Ptr asMethod(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Method>(nameContext);
  	}
  	
  	static idl::Message::Ptr asMessage(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Message>(nameContext);
  	}
  	
  	static idl::MessageClass::Ptr asMessageClass(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::MessageClass>(nameContext);
  	}
  	
  	static idl::MessageClassExtend::Ptr asMessageClassExtend(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::MessageClassExtend>(nameContext);
  	}
  	
  	static idl::Property::Ptr asProperty(idl::NameContext::Ptr nameContext) {
  		return dynamic_pointer_cast<idl::Property>(nameContext);
  	}
  	
  	void test() {
		prepare();
		
		{
			Names names;
        	resolvedNames(
        			"namespace ns1 {}"
        			"interface I1 {}",
        			names);
        			
        	BOOST_CHECK(asType(names["I1"]));
        	BOOST_CHECK(asInterface(names["I1"]));
        	//BOOST_CHECK_EQUAL(asInterface(names["I1"])->getInterfaceId(), 8);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const s16 value = 12;",
        			names);
        			
        	BOOST_CHECK(asValue(names["value"]));
        	BOOST_CHECK(asBasicType(asValue(names["value"])->getType()));
        	BOOST_CHECK_EQUAL(asBasicType(asValue(names["value"])->getType())->getTypeId(), idl::parser::BasicTypeNode::SInt16);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const void* value = 12;",
        			names);
        			
        	BOOST_CHECK(asValue(names["value"]));
        	BOOST_CHECK(asRef(asValue(names["value"])->getType()));
        	BOOST_CHECK(asBasicType(asRef(asValue(names["value"])->getType())->getType()));
        	BOOST_CHECK_EQUAL(asBasicType(asRef(asValue(names["value"])->getType())->getType())->getTypeId(), idl::parser::BasicTypeNode::Void);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const void value = 12;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::VoidNotAllowed);
		}
		
		{
			Names names;
        	resolvedNames(
        			"struct A { void value; }",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::VoidNotAllowed);
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface A { method s8 f(void a); }",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::VoidNotAllowed);
		}
		
		{
			Names names;
        	resolvedNames(
        			"struct A { float s; };",
        			names);
        			
        	BOOST_CHECK(asStruct(names["A"]));
        	BOOST_CHECK(asValue(names["A.s"]));
        	BOOST_CHECK(asBasicType(asValue(names["A.s"])->getType()));
        	BOOST_CHECK_EQUAL(asBasicType(asValue(names["A.s"])->getType())->getTypeId(), idl::parser::BasicTypeNode::Float);
		}
		
		{
			Names names;
        	resolvedNames(
        			"struct A { sequence<s32, 10> s; };",
        			names);
        			
        	BOOST_CHECK(asStruct(names["A"]));
        	BOOST_CHECK(asValue(names["A.s"]));
        	BOOST_CHECK(asSequenceType(asValue(names["A.s"])->getType()));
        	BOOST_CHECK(asBasicType(asSequenceType(asValue(names["A.s"])->getType())->getType()));
        	BOOST_CHECK_EQUAL(asBasicType(asSequenceType(asValue(names["A.s"])->getType())->getType())->getTypeId(), idl::parser::BasicTypeNode::SInt32);
		}
		
		{
			Names names;
        	resolvedNames(
        			"struct A { sequence<sequence<s32, 9>, 10> s; };",
        			names);
        			
        	BOOST_CHECK(asStruct(names["A"]));
        	BOOST_CHECK(asValue(names["A.s"]));
        	BOOST_CHECK(asSequenceType(asValue(names["A.s"])->getType()));
        	BOOST_CHECK(asSequenceType(asSequenceType(asValue(names["A.s"])->getType())->getType()));
		}
		
		{
			Names names;
        	resolvedNames(
        			"namespace ns { struct A {s8 s;}; } "
        			"struct A { sequence<ns.A, 10> s; };",
        			names);
        			
        	BOOST_CHECK(asStruct(names["A"]));
        	BOOST_CHECK(asValue(names["A.s"]));
        	BOOST_CHECK(asSequenceType(asValue(names["A.s"])->getType()));
        	BOOST_CHECK(asStruct(asSequenceType(asValue(names["A.s"])->getType())->getType()));
        	BOOST_CHECK_EQUAL(names["ns.A"], asStruct(asSequenceType(asValue(names["A.s"])->getType())->getType()));
		}
		
		{
			Names names;
        	resolvedNames(
        			"enum A { a,b,c };",
        			names);
        			
        	BOOST_CHECK(asEnum(names["A"]));
        	BOOST_CHECK(asValue(names["A.b"]));
        	BOOST_CHECK_EQUAL(asValue(names["A.a"])->getType(), asType(names["A"]));
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface I1 {			"
        			"					"
        			"	method const I1* f(s8 arg);"
        			"}						",
        			names);
        			
        	BOOST_CHECK(asValue(names["I1.f.arg"]));
        	BOOST_CHECK(asBasicType(asValue(names["I1.f.arg"])->getType()));
        	BOOST_CHECK_EQUAL(asBasicType(asValue(names["I1.f.arg"])->getType())->getTypeId(), idl::parser::BasicTypeNode::SInt8);
		}
		{
			Names names;
        	resolvedNames(
        			"namespace ns1 {		"
        			"	interface I1 {		"
        			"					"
        			"	}					"
        			"	typedef I1 T1;		"
        			"	typedef ns1.I1 T2;	"
        			"}						",
        			names);
        			
        	BOOST_CHECK(asType(names["ns1.I1"]));
        	BOOST_CHECK(asType(names["ns1.T1"]));
        	BOOST_CHECK(asType(names["ns1.T2"]));
        	BOOST_CHECK_EQUAL(asType(names["ns1.T1"])->getType(), asType(names["ns1.I1"]));
        	BOOST_CHECK_EQUAL(asType(names["ns1.T1"])->getType(), asType(names["ns1.T2"])->getType());
		}
		/*!!! External!!!{
			Names names;
        	resolvedNames(
        			"typedef x16 value2;		",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::NameNotFound);
		}*/
		{
			Names names;
        	resolvedNames(
        			"const s32 value2 = 12;		"
        			"const s16 value2 = 12;		",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::NameAlreadyDefined);
		}
		{
			Names names;
        	resolvedNames(
        			"const s16 value1 = 12;	"
        			"typedef value1 value2;	",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::TypeExpected);
		}
		/*{
			Names names;
        	resolvedNames(
        			"typedef s16 type;		"
        			"const s16 value = type;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::ValueExpected);
		}*/
		
		{
			Names names;
        	resolvedNames(
					"typedef s32 SInt32;"
					"interface I {"
        			"	"
					"	method const SInt32* call(SInt32 a);"
					"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		
		{
			Names names;
        	resolvedNames(
					"typedef sequence<s32,10> SInt32;"
					"interface I {"
        			"	"
					"	method void call(SInt32 a);"
					"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}

		{
			Names names;
        	resolvedNames(
        			"struct A { s8* ptr;}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asValue(names["A.ptr"]));
        	BOOST_CHECK(asRef(asValue(names["A.ptr"])->getType()));
        	BOOST_CHECK_EQUAL(asRef(asValue(names["A.ptr"])->getType())->getCount(), 1);
        	BOOST_CHECK_EQUAL(asRef(asValue(names["A.ptr"])->getType())->isConst(), false);
		}
		
		{
			Names names;
        	resolvedNames(
        			"struct A { s8*** ptr;}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asValue(names["A.ptr"]));
        	BOOST_CHECK(asRef(asValue(names["A.ptr"])->getType()));
        	BOOST_CHECK_EQUAL(asRef(asValue(names["A.ptr"])->getType())->getCount(), 3);
        	BOOST_CHECK_EQUAL(asRef(asValue(names["A.ptr"])->getType())->isConst(), false);
		}
		
		{
			Names names;
        	resolvedNames(
        			"struct A { const s8* ptr;}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asValue(names["A.ptr"]));
        	BOOST_CHECK(asRef(asValue(names["A.ptr"])->getType()));
        	BOOST_CHECK_EQUAL(asRef(asValue(names["A.ptr"])->getType())->getCount(), 1);
        	BOOST_CHECK_EQUAL(asRef(asValue(names["A.ptr"])->getType())->isConst(), true);
		}
		
		{
			Names names;
        	resolvedNames(
        			"typedef s8* S8Ptr;"
        			"struct A { S8Ptr x; const S8Ptr* y; const s8** z; }",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asValue(names["A.x"]));
        	BOOST_CHECK(asValue(names["A.y"]));
        	BOOST_CHECK(asValue(names["A.z"]));
        	
			idl::Type::Ptr typeX = idl::reduceTypedef(_root, asValue(names["A.x"])->getType());
			BOOST_CHECK(asRef(typeX));
			BOOST_CHECK_EQUAL(asRef(typeX)->getCount(), 1);
			BOOST_CHECK_EQUAL(asRef(typeX)->isConst(), false);
        	
			idl::Type::Ptr typeY = idl::reduceTypedef(_root, asValue(names["A.y"])->getType());
			BOOST_CHECK(asRef(typeY));
			BOOST_CHECK_EQUAL(asRef(typeY)->getCount(), 2);
			BOOST_CHECK_EQUAL(asRef(typeY)->isConst(), true);
        	
			idl::Type::Ptr typeZ = idl::reduceTypedef(_root, asValue(names["A.z"])->getType());
			BOOST_CHECK(asRef(typeZ));
			BOOST_CHECK_EQUAL(asRef(typeZ)->getCount(), 2);
			BOOST_CHECK_EQUAL(asRef(typeZ)->isConst(), true);
			
			BOOST_CHECK(typeY == typeZ);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const s16 a = 8;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asConstValue(names["a"]));
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->type(), idl::Performer::TypeInt);
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->asInt(), 8);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const s16 b = 8;"
        			"const s16 a = b;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asConstValue(names["a"]));
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->type(), idl::Performer::TypeInt);
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->asInt(), 8);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const float a = 8.;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asConstValue(names["a"]));
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->type(), idl::Performer::TypeFloat);
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->asFloat(), 8.);
		}
		
		{
			Names names;
        	resolvedNames(
        			"const s16 c = 2;"
        			"const s16 b = 6;"
        			"const s16 a = b + c;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asConstValue(names["a"]));
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->type(), idl::Performer::TypeInt);
        	BOOST_CHECK_EQUAL(asConstValue(names["a"])->getPerformer()->asInt(), 8);
		}
		
		
		{
			Names names;
        	resolvedNames(
        			"const s16 c = 2;"
        			"const s16 b = 6;"
        			"typedef sequence<s16, c + b> S16Seq;",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asType(names["S16Seq"]));
        	idl::SequenceType::Ptr sequenceType = asSequenceType(asType(names["S16Seq"])->getType());
        	BOOST_CHECK(sequenceType);
        	BOOST_CHECK_EQUAL(sequenceType->getNumberOfItems(), (size_t)8);
		}
						
        // casual test (provoked crash)
		{
			Names names;
        	resolvedNames(
        			"namespace ns {			"
        			"}						"
        			"interface I {			"
        			"					"
        			"}						"
        			"component C {			"
        			"}						",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		
		
		// interface inheritance test
		{
			Names names;
        	resolvedNames(
        			"interface A { const s16 b = 8; }"
        			"interface B : A { ; } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	BOOST_CHECK(asConstValue(names["A.b"]));
        	
        	idl::Interface::Ptr interfaceA = asInterface(names["A"]);
        	BOOST_CHECK_EQUAL(interfaceA->getBases().size(), (size_t)0);
        	
        	idl::Interface::Ptr interfaceB = asInterface(names["B"]);
        	BOOST_CHECK_EQUAL(interfaceB->getBases().size(), (size_t)1);

        	BOOST_CHECK(interfaceB->getBases()[0] == interfaceA);

        	BOOST_CHECK(interfaceB->findWithinBases("b"));
        	BOOST_CHECK(interfaceB->findWithinBases("b") == names["A.b"]);
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface A { const s16 b = 8; }"
        			"interface B : A { const s16 c = b; } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	BOOST_CHECK(asConstValue(names["A.b"]));
        	
        	idl::Interface::Ptr interfaceA = asInterface(names["A"]);
        	BOOST_CHECK_EQUAL(interfaceA->getBases().size(), (size_t)0);
        	
        	idl::Interface::Ptr interfaceB = asInterface(names["B"]);
        	BOOST_CHECK_EQUAL(interfaceB->getBases().size(), (size_t)1);

        	BOOST_CHECK(interfaceB->getBases()[0] == interfaceA);

        	BOOST_CHECK(interfaceB->findWithinBases("b"));
        	BOOST_CHECK(interfaceB->findWithinBases("b") == names["A.b"]);
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface A { const s16 b = 8; message_cls msgclass { message msg0; message msg1; } }"
        			"interface B : A { extend message_cls msgclass { message msg2; message msg3; } } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	BOOST_CHECK(asConstValue(names["A.b"]));

        	BOOST_CHECK(asMessage(names["A.msgclass.msg0"]));
        	BOOST_CHECK(asMessage(names["B.msgclass.msg2"]));
        	
        	BOOST_CHECK(asMessageClass(names["A.msgclass"]));
        	BOOST_CHECK(asMessageClassExtend(names["B.msgclass"]));
        	        	
        	BOOST_CHECK(asMessageClassExtend(names["B.msgclass"])->getMessageClass() == asMessageClass(names["A.msgclass"]));
        	
        	BOOST_CHECK(asMessage(names["A.msgclass.msg0"])->getMessageClass() == asMessageClass(names["A.msgclass"]));
        	BOOST_CHECK(asMessage(names["B.msgclass.msg2"])->getMessageClass() == asMessageClass(names["B.msgclass"]));
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface A { typedef const s8* Str; prop const s8* prop0; }"
        			"interface B : A { prop Str prop1; } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	
        	BOOST_CHECK(asProperty(names["A.prop0"]));
        	BOOST_CHECK(asProperty(names["B.prop1"]));

        	BOOST_CHECK(asInterface(names["B"])->findWithinBases("prop0") == names["A.prop0"]);

        	BOOST_CHECK(asProperty(names["B.prop1"])->getType() == names["A.Str"]);
		}
		
		// recursive inheritance
		{
			Names names;
        	resolvedNames(
        			"interface A { const s16 b = 8; }"
        			"interface C : A {} "
        			"interface B : C { const s16 c = b; } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	BOOST_CHECK(asConstValue(names["A.b"]));
        	
        	idl::Interface::Ptr interfaceA = asInterface(names["A"]);
        	BOOST_CHECK_EQUAL(interfaceA->getBases().size(), (size_t)0);
        	
        	idl::Interface::Ptr interfaceB = asInterface(names["B"]);
        	BOOST_CHECK_EQUAL(interfaceB->getBases().size(), (size_t)1);

        	BOOST_CHECK_EQUAL(interfaceB->getBases()[0]->getBases().size(), (size_t)1);
        	BOOST_CHECK(interfaceB->getBases()[0]->getBases()[0] == interfaceA);

        	BOOST_CHECK(interfaceB->findWithinBases("b"));
        	BOOST_CHECK(interfaceB->findWithinBases("b") == names["A.b"]);
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface A { const s16 b = 8; message_cls msgclass { message msg0; message msg1; } }"
        			"interface C : A {} "
        			"interface B : C { extend message_cls msgclass { message msg2; message msg3; } } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	BOOST_CHECK(asConstValue(names["A.b"]));

        	BOOST_CHECK(asMessage(names["A.msgclass.msg0"]));
        	BOOST_CHECK(asMessage(names["B.msgclass.msg2"]));
        	
        	BOOST_CHECK(asMessageClass(names["A.msgclass"]));
        	BOOST_CHECK(asMessageClassExtend(names["B.msgclass"]));
        	        	
        	BOOST_CHECK(asMessageClassExtend(names["B.msgclass"])->getMessageClass() == asMessageClass(names["A.msgclass"]));
        	
        	BOOST_CHECK(asMessage(names["A.msgclass.msg0"])->getMessageClass() == asMessageClass(names["A.msgclass"]));
        	BOOST_CHECK(asMessage(names["B.msgclass.msg2"])->getMessageClass() == asMessageClass(names["B.msgclass"]));
		}
		
		{
			Names names;
        	resolvedNames(
        			"interface A { typedef const s8* Str; prop const s8* prop0; }"
        			"interface C : A {} "
        			"interface B : C { prop Str prop1; } ",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asInterface(names["B"]));
        	
        	BOOST_CHECK(asProperty(names["A.prop0"]));
        	BOOST_CHECK(asProperty(names["B.prop1"]));

        	BOOST_CHECK(asInterface(names["B"])->findWithinBases("prop0") == names["A.prop0"]);

        	BOOST_CHECK(asProperty(names["B.prop1"])->getType() == names["A.Str"]);
		}
		
		// implementation tests
		{
			Names names;
        	resolvedNames(
        			"interface A { prop s32 prop0; method void meth0(const s8* str); message_cls msgclass0 { message msg; } }"
        			"component Plugin { "
        			"	implementation B : A { "
        			"		[member_ref(xyz)] prop s32 prop0;"
        			"		[member_ref(xyz1)] add prop s16 prop1;"
        			"		add method A* meth1();"
        			"		extend message_cls msgclass0 { message msg; }"
        			"		add message_cls msgclass1 { message msg; }"
        			"	}"
        			"	method void meth1(u16 arg0);"
        			"}"	,
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asInterface(names["A"]));
        	BOOST_CHECK(asImplementation(names["B"]));
        	BOOST_CHECK(asMessageClass(names["A.msgclass0"]));
        	BOOST_CHECK(asMessage(names["A.msgclass0.msg"]));
        	BOOST_CHECK(asProperty(names["A.prop0"]));
        	BOOST_CHECK(asMethod(names["A.meth0"]));
        	BOOST_CHECK(asValue(names["A.meth0.str"]));
        	BOOST_CHECK(asProperty(names["B.prop1"]));
        	BOOST_CHECK(asMethod(names["B.meth1"]));
        	BOOST_CHECK(asMessageClassExtend(names["B.msgclass0"]));
        	BOOST_CHECK(asMessage(names["B.msgclass0.msg"]));
        	BOOST_CHECK(asMessageClass(names["B.msgclass1"]));
        	BOOST_CHECK(asMessage(names["B.msgclass1.msg"]));
        	BOOST_CHECK(asMethod(names["meth1"]));
		}
		
				
		// recursive type descent
		{
			Names names;
        	resolvedNames(
        			"interface I {  typedef s16 X; };	"
        			"typedef I T;						"
        			"struct S { T.X x; };				",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		
		{
			Names names;
        	resolvedNames(
        			"typedef X X;	",
        			//"typedef NotDeclaredTypeYet T;	"
        			//"typedef s8 NotDeclaredTypeYet;	",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
		}

		{
			Names names;
        	resolvedNames(
        			"interface I {  typedef s16 X; };	"
        			"struct S { T.X x; };				"
        			"typedef I T;						",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
		}

		// check cycles
		{
			Names names;
        	resolvedNames(
        			"typedef X.infinite X;	",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
		}
		{
			Names names;
        	resolvedNames(
        			"typedef X X;					"
        			"struct Z { X.infinite loop; };	",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(2));
		}
		{
			Names names;
        	resolvedNames(
        			"typedef X Y;					"
        			"typedef Y X;					"
        			"struct Z { X.infinite loop; };	",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(2));
		}
		
		// crash test
		{
			Names names;
        	resolvedNames(
					"typedef UnknownType T;\n		"
					"const T a = \"hi\";			",
					names);
			
        	BOOST_CHECK(ParsingSuite::checkErrors(3));
		}
		
		
		// var arg test
		{
			Names names;
        	resolvedNames(
        			"interface A { method void meth0(const s8* str, ...); } "
        			"component Plugin { "
        			"	implementation B : A { "
        			"		add method A* meth1();"
        			"	}"
        			"	method void meth2(...);"
        			"}"	,
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(asMethod(names["A.meth0"]));
        	BOOST_CHECK(asMethod(names["A.meth0"])->isVarArg());
        	BOOST_CHECK(asMethod(names["B.meth1"]));
        	BOOST_CHECK(!(asMethod(names["B.meth1"])->isVarArg()));
        	BOOST_CHECK(asMethod(names["meth2"]));
        	BOOST_CHECK(asMethod(names["meth2"])->isVarArg());
		}
  	}
};

class CheckAttributesTest : public NameResolvingTest {
  public:
  	void checkAttrs(const std::string& data, Names& names) {
  		resolvedNames(data, names);
  		idl::checkAttrs(_root, _rootNode);
  	}
  
  	void test() {
		{
			Names names;
        	checkAttrs("", names);
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		
		{
			Names names;
        	checkAttrs(
        			"namespace ns1 {}"
        			"interface I1 {}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		
		{
			Names names;
        	checkAttrs(
        			"namespace ns1 {}"
        			"[id(0x100)] interface I1 {}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		
		{
			Names names;
        	checkAttrs(
        			"namespace ns1 {}"
        			"[id(0x100) id(0x01)] interface I1 {}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::AttributeDuplicated);
		}
		
		{
			Names names;
        	checkAttrs(
        			"namespace ns1 {}"
        			"[getter(fig)] interface I1 {}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(1));
        	BOOST_CHECK_EQUAL(ErrorFactory::instance().getErrors()[0].first, ErrorFactory::AttributeNotAllowed);
		}
		
		{
			Names names;
        	checkAttrs(
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"id(23)"
						//"version(1)"
						"mnemonic(ID_I1)"
						"c_type_name(hI1)"
						"cpp_type_name(cI1)"
						"output_header_file(i1)"
						"output_source_file(i1)"
        			"] interface I1 {}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"version(1)"
						"autostart(true)"
        			"] component C1 {}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 {}"
        			"component C1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						//"id(23)"
						"version(1)"
						"mnemonic(ID_I1)"
						"c_type_name(hI1)"
						"cpp_type_name(hI1)"
						"output_header_file(i1)"
						"output_source_file(i1)"
        			"] implementation Impl1 : I1 {}"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"id(23)"
        			"] prop s32 Prop;"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
        			"] method void* f(const s32* buffer);"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"warning(false)"
        			"] error Err;"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"id(12)"
        			"] message_cls M1 {};"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I0 {"
        			"	message_cls M1 {}"
        			"}"
        			"interface I1 : I0 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
        			"] extend message_cls M1 {};"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 {"
        			"message_cls M1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"id(12)"
					"] message msg;"	
        			"}"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 { prop s8 p0; }"
        			"component C1 {"
        			"implementation Impl1 : I1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"shared(false)"
						"access(read, write)"
						"setter(setfunc)"
						"getter(getfunc)"
        			"] prop s8 p0;"
        			"}"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
		{
			Names names;
        	checkAttrs(
        			"interface I1 { prop s8 p0; }"
        			"component C1 {"
        			"implementation Impl1 : I1 {"
        			"["
						"comment(\"xxx\")"
						"comment_ex(\"xxx\")"
						"shared(false)"
						"id(23)"
						"access(read, write)"
						"member_ref(field)"
        			"] add prop s8 p1;"
        			"}"
        			"}",
        			names);
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
		}
  	}
};


class CasualTest {
    void prepare() {
    }

  public:
  	void test() {
		prepare();
  	}
};
		
/*class ConstEvalTest {
    void prepare() {
    }

  public:
  	typedef std::map<std::string, idl::NameContext::Ptr>	Names;
  
  	static void evalConsts(const std::string& data, idl::EvaledConsts& consts) {
        idl::parser::NodeInterface::Ptr node = ParsingSuite::parse(data);
		idl::RootNameContext::Ptr root = idl::gatherDeclaredNames(node);
		idl::resolveNames(root, node);
		idl::evalConsts(node, consts);
		//BOOST_CHECK(ParsingSuite::checkErrors(expectedErrorsCount));
  	}

  	void test() {
		prepare();
		
 		{
			idl::EvaledConsts consts;
        	idl::Perfromer performer = evalConsts(
					"const s32 value = 10;"
					,
        			consts);

        	BOOST_CHECK(ParsingSuite::checkErrors(0));
			BOOST_CHECK(performer);
        	BOOST_CHECK_EQUAL(performer->type(), idl::Performer::TypeInt);
        	BOOST_CHECK_EQUAL(performer->asInt(), 10);
		}
		
 		{
			idl::EvaledConsts consts;
        	idl::Perfromer performer = evalConsts(
					"const s32 a = 10;"
					"const s32 value = a;"
					,
        			consts);
        	idl::NamePath value = idl::NamePath::make("value");
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(consts.find(value) != consts.end());
        	BOOST_CHECK_EQUAL(consts[value]->type(), idl::Performer::TypeInt);
        	BOOST_CHECK_EQUAL(consts[value]->asInt(), 10);
		}
		
 		{
			idl::EvaledConsts consts;
        	evalConsts(
					"const s32 a = 3;"
					"const s32 b = 7;"
					"const s32 value = a + b;"
					,
        			consts);

        	idl::NamePath value = idl::NamePath::make("value");
        			
        	BOOST_CHECK(ParsingSuite::checkErrors(0));
        	BOOST_CHECK(consts.find(value) != consts.end());
        	BOOST_CHECK_EQUAL(consts[value]->type(), idl::Performer::TypeInt);
        	BOOST_CHECK_EQUAL(consts[value]->asInt(), 10);
		}
  	}
};*/

#define NAMED_CLASS_TEST_CASE( function, tc_instance, name ) \
boost::unit_test::make_test_case((function), name, tc_instance )

#define TEST_GENERATOR(test) new SuiteGenerator<test>(#test)

using boost::unit_test::test_suite;
using boost::unit_test::test_case;

template <class Test>
struct SuiteGenerator : public test_suite {
    SuiteGenerator(const char* name) {
        boost::shared_ptr<Test> instance(new Test());

        test_case* test_case = NAMED_CLASS_TEST_CASE(&Test::test, instance, name);    
        add(test_case);
    }
};

extern void init();
extern void term();

void init() {
}

void term() {
}

test_suite* init_unit_test_suite(int /*argc*/, char*[] /*argv*/ ) {
    test_suite* test = BOOST_TEST_SUITE("Kak IDL Test Suite");
    test->add(TEST_GENERATOR(ScanerTest));
    test->add(TEST_GENERATOR(ParserTest));
    test->add(TEST_GENERATOR(TransformTest));
    test->add(TEST_GENERATOR(DeclarationGatheringTest));
    test->add(TEST_GENERATOR(NameResolvingTest));
    test->add(TEST_GENERATOR(CheckAttributesTest));
    
    //test->add(TEST_GENERATOR(ConstEvalTest));
    return test;
}
