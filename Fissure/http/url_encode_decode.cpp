

#include <string>
#include <ostream>
#include <sstream>

using namespace std;

inline unsigned char      to_hex(unsigned char   x)  
{
  return x + (x > 9 ? ('A'-10) : '0');
}
inline unsigned char    from_hex(unsigned char  ch) 
{
  if (ch <= '9' && ch >= '0')
    ch -= '0';
  else if (ch <= 'f' && ch >= 'a')
    ch -= 'a' - 10;
  else if (ch <= 'F' && ch >= 'A')
    ch -= 'A' - 10;
  else 
    ch = 0;
  return ch;
}
const string           urlencode(const string&   s)  
{
  ostringstream os;

  for(string::const_iterator ci = s.begin(); ci != s.end(); ++ci )
  {
    if ( (*ci >= 'a' && *ci <= 'z') ||
      (*ci >= 'A' && *ci <= 'Z') ||
      (*ci >= '0' && *ci <= '9') )
    { // allowed
      os << *ci;
    }
    else if ( *ci == ' ')
    {
      os << '+';
    }
    else
    {
      os << '%' << to_hex(*ci >> 4) << to_hex(*ci % 16);
    }
  }

  return os.str();
}
const string           urldecode(const string& str) 
{
  using namespace std;
  string result;
  string::size_type i;
  for (i = 0; i < str.size(); ++i)
  {
    if (str[i] == '+')
    {
      result += ' ';
    }
    else if (str[i] == '%' && str.size() > i+2)
    {
      const unsigned char ch1 = from_hex(str[i+1]);
      const unsigned char ch2 = from_hex(str[i+2]);
      const unsigned char ch = (ch1 << 4) | ch2;
      result += ch;
      i += 2;
    }
    else
    {
      result += str[i];
    }
  }
  return result;
}


//void parse_url(
//  string word, 
//  key_value_map& queries
//)
//  /*!
//  Parses the query string of a URL.  word should be the stuff that comes
//  after the ? in the query URL.
//  !*/
//{
//  string::size_type pos;
//
//  for (pos = 0; pos < word.size(); ++pos)
//  {
//    if (word[pos] == '&')
//      word[pos] = ' ';
//  }
//
//  istringstream sin(word);
//  sin >> word;
//  while (sin)
//  {
//    pos = word.find_first_of("=");
//    if (pos != string::npos)
//    {
//      string key = urldecode(word.substr(0,pos));
//      string value = urldecode(word.substr(pos+1));
//
//      queries[key] = value;
//    }
//    sin >> word;
//  }
//}
//
//void read_with_limit(
//  istream& in, 
//  string& buffer, 
//  int delim = '\n'
//) 
//{
//  using namespace std;
//  const size_t max = 64*1024;
//  buffer.clear();
//  buffer.reserve(300);
//
//  while (in.peek() != delim && in.peek() != '\n' && in.peek() != EOF && buffer.size() < max)
//  {
//    buffer += (char)in.get();
//  }
//
//  // if we quit the loop because the data is longer than expected or we hit EOF
//  if (in.peek() == EOF)
//    throw http_parse_error("HTTP field from client terminated incorrectly", 414);
//  if (buffer.size() == max)
//    throw http_parse_error("HTTP field from client is too long", 414);
//
//  in.get();
//  // eat any remaining whitespace
//  if (delim == ' ')
//  {
//    while (in.peek() == ' ')
//      in.get();
//  }
//}
//
