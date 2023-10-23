#include "WalletConsole.h"
#include <boost/iostreams/categories.hpp>
#include <array>
#include <iomanip>
#include <cstdlib>
#include <string.h>
#include <boost/iostreams/stream.hpp>
#include <readline/readline.h>
#include <readline/history.h>
using namespace boost::iostreams;
using namespace boost::iostreams;

using namespace std;
namespace io = boost::iostreams;

// Fixme:  This and the help text should probably live in
//         a nice map some place.  
const std::array<string,27> words = {
  "addr",
  "addrDefault",
  "addrDP",
  "addrPri",
  "addrPub",
  "addrXpub",
  "base64Decode",
  "base64Encode",
  "buffer",
  "coin",
  "coins",
  "dumpDP",
  "dumpMnemonic",
  "dumpSeed",
  "dumpXpub",
  "exit",
  "fileR",
  "fileW",
  "help",
  "hex",
  "newKey",
  "newMnemonic",
  "priDP",
  "priPub",
  "pubPri",
  "quit",
  "setMnemonic"
};

string last="*";
auto b(words.begin()), e(words.end());
char *completion_matches(const char *text, int state) {
  if(!state) {
    last=text;
    b=words.begin(); e=words.end();
    while(b!=e && b->substr(0,last.length())!=last)
      b++;
    if(b==e)
      return 0;
    auto p(b);
    while(p!=e && p->substr(0,last.length())==last)
      p++;
    e=p;
    p=b;
  }
  auto p(b);
  for(int i=0;i<state;i++){
    if(++p==e)
      return 0;
  };
  return strdup(p->c_str());
}
class my_source {
public:
    typedef char        char_type;
    typedef io::source_tag  category;
    char *line;
    char *pos;
    my_source()
      : line(0), pos(0)
    {
    }
    std::streamsize read(char* s, std::streamsize n)
    {
      memset(s,0,n);
      if(!line) {
        pos = line = readline("> ");
        if(!pos)
          return 0;
        add_history(pos);
      }
      for(int i=0;i<n-1;i++){
        if((s[i]=*pos++)==0){
          free(line);
          pos=line=0;
          s[i++]='\n';
          return i;
        }
      }
      return n;
    }
};


using namespace boost::iostreams;


int main(int, char**)
{
  dup2(2,1);
  rl_completion_entry_function = completion_matches;
  my_source f;
  stream<my_source> is{f};
  TW::WalletConsole::WalletConsole console;
  console.loop();
  return 0;
}
