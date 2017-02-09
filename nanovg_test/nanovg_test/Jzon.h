/*
Copyright (c) 2015 Johannes Häggqvist

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef Jzon_h__
#define Jzon_h__

#include <string>
#include <vector>
#include <queue>
#include <iterator>
#include <istream>
#include <ostream>

#ifndef JZON_API
#	ifdef JZON_DLL
#		if defined _WIN32 || defined __CYGWIN__
#			define JZON_API __declspec(dllimport)
#			define JZON_STL_EXTERN extern
#		elif __GNUC__ >= 4
#			define JZON_API __attribute__ ((visibility ("default")))
#		else
#			define JZON_API
#		endif
#	else
#		define JZON_API
#	endif
#endif

#ifdef JZON_STL_EXTERN
JZON_STL_EXTERN template class JZON_API std::basic_string<char>;
#undef JZON_STL_EXTERN
#endif

namespace Jzon
{
	namespace Version
	{
		const int MAJOR = 2;
		const int MINOR = 1;
	}

	class Node;
	typedef std::pair<std::string, Node> NamedNode;

	class JZON_API Node
	{
	public:
		class iterator : public std::iterator<std::input_iterator_tag, NamedNode>
		{
		public:
            iterator() : p(0) {}
			iterator(NamedNode *o) : p(o) {}
			iterator(const iterator &it) : p(it.p) {}

			iterator &operator++() { ++p; return *this; }
			iterator operator++(int) { iterator tmp(*this); operator++(); return tmp; }

			bool operator==(const iterator &rhs) { return p == rhs.p; }
			bool operator!=(const iterator &rhs) { return p != rhs.p; }

			NamedNode &operator*() { return *p; }
			NamedNode *operator->() { return p; }

		private:
			NamedNode *p;
		};
		class const_iterator : public std::iterator<std::input_iterator_tag, const NamedNode>
		{
		public:
			const_iterator() : p(0) {}
			const_iterator(const NamedNode *o) : p(o) {}
			const_iterator(const const_iterator &it) : p(it.p) {}

			const_iterator &operator++() { ++p; return *this; }
			const_iterator operator++(int) { const_iterator tmp(*this); operator++(); return tmp; }

			bool operator==(const const_iterator &rhs) { return p == rhs.p; }
			bool operator!=(const const_iterator &rhs) { return p != rhs.p; }

			const NamedNode &operator*() { return *p; }
			const NamedNode *operator->() { return p; }

		private:
			const NamedNode *p;
		};

		enum Type
		{
			T_INVALID,
			T_OBJECT,
			T_ARRAY,
			T_NULL,
			T_STRING,
			T_NUMBER,
			T_BOOL
		};

		Node();
		explicit Node(Type type);
		Node(const Node &other);
		Node(Type type, const std::string &value);
		Node(const std::string &value);
		Node(const char *value);
		Node(int value);
		Node(unsigned int value);
		Node(long long value);
		Node(unsigned long long value);
		Node(float value);
		Node(double value);
		Node(bool value);
		~Node();

		void detach();

		inline Type getType() const { return (data == NULL ? T_INVALID : data->type); };

		inline bool isValid()  const { return (getType() != T_INVALID); }
		inline bool isObject() const { return (getType() == T_OBJECT);  }
		inline bool isArray()  const { return (getType() == T_ARRAY);   }
		inline bool isNull()   const { return (getType() == T_NULL);    }
		inline bool isString() const { return (getType() == T_STRING);  }
		inline bool isNumber() const { return (getType() == T_NUMBER);  }
		inline bool isBool()   const { return (getType() == T_BOOL);    }

		inline bool isContainer() const { return (isObject() || isArray()); }
		inline bool isValue() const { return (isNull() || isString() || isNumber() || isBool()); }

		std::string toString(const std::string &def = std::string()) const;
		int toInt(int def = 0) const;
		float toFloat(float def = 0.f) const;
		double toDouble(double def = 0.0) const;
		bool toBool(bool def = false) const;

		void setNull();
		void set(Type type, const std::string &value);
		void set(const std::string &value);
		void set(const char *value);
		void set(int value);
		void set(unsigned int value);
		void set(long long value);
		void set(unsigned long long value);
		void set(float value);
		void set(double value);
		void set(bool value);

		Node &operator=(const Node &rhs);
		Node &operator=(const std::string &rhs);
		Node &operator=(const char *rhs);
		Node &operator=(int rhs);
		Node &operator=(unsigned int rhs);
		Node &operator=(long long rhs);
		Node &operator=(unsigned long long rhs);
		Node &operator=(float rhs);
		Node &operator=(double rhs);
		Node &operator=(bool rhs);

		void add(const Node &node);
		void add(const std::string &name, const Node &node);
		void append(const Node &node);
		void remove(size_t index);
		void remove(const std::string &name);
		void clear();

		bool has(const std::string &name) const;
		size_t getCount() const;
		Node get(const std::string &name) const;
		Node get(size_t index) const;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		bool operator==(const Node &other) const;
		bool operator!=(const Node &other) const;
		inline operator bool() const { return isValid(); }

	private:
		typedef std::vector<NamedNode> NamedNodeList;
		struct Data
		{
			explicit Data(Type type);
			Data(const Data &other);
			~Data();
			void addRef();
			bool release();
			int refCount;

			Type type;
			std::string valueStr;
			NamedNodeList children;
		} *data;
	};

	JZON_API std::string escapeString(const std::string &value);
	JZON_API std::string unescapeString(const std::string &value);

	JZON_API Node invalid();
	JZON_API Node null();
	JZON_API Node object();
	JZON_API Node array();

	struct JZON_API Format
	{
		bool newline;
		bool spacing;
		bool useTabs;
		unsigned int indentSize;
	};
	const Format StandardFormat = { true, true, true, 1 };
	const Format NoFormat = { false, false, false, 0 };

	class JZON_API Writer
	{
	public:
		explicit Writer(const Format &format = NoFormat);
		~Writer();

		void setFormat(const Format &format);

		void writeStream(const Node &node, std::ostream &stream) const;
		void writeString(const Node &node, std::string &json) const;
		void writeFile(const Node &node, const std::string &filename) const;

	private:
		void writeNode(const Node &node, unsigned int level, std::ostream &stream) const;
		void writeObject(const Node &node, unsigned int level, std::ostream &stream) const;
		void writeArray(const Node &node, unsigned int level, std::ostream &stream) const;
		void writeValue(const Node &node, std::ostream &stream) const;

		std::string getIndentation(unsigned int level) const;

		Format format;
		char indentationChar;
		const char *newline;
		const char *spacing;
	};

	class JZON_API Parser
	{
	public:
		Parser();
		~Parser();

		Node parseStream(std::istream &stream);
		Node parseString(const std::string &json);
		Node parseFile(const std::string &filename);

		const std::string &getError() const;

	private:
		enum Token
		{
			T_UNKNOWN,
			T_OBJ_BEGIN,
			T_OBJ_END,
			T_ARRAY_BEGIN,
			T_ARRAY_END,
			T_SEPARATOR_NODE,
			T_SEPARATOR_NAME,
			T_VALUE
		};
		typedef std::queue<Token> TokenQueue;
		typedef std::queue<std::pair<Node::Type, std::string> > DataQueue;

		void tokenize(std::istream &stream, TokenQueue &tokens, DataQueue &data);
		Node assemble(TokenQueue &tokens, DataQueue &data);

		void jumpToNext(char c, std::istream &stream);
		void jumpToCommentEnd(std::istream &stream);

		void readString(std::istream &stream, DataQueue &data);
		bool interpretValue(const std::string &value, DataQueue &data);

		std::string error;
	};
}

#endif // Jzon_h__
