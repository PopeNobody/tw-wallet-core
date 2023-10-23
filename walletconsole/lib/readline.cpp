#include <boost/iostreams/categories.hpp>
#include <array>
#include <iomanip>
#include <cstdlib>
#include <string.h>
#include <boost/iostreams/stream.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <functional>

namespace io = boost::iostreams;
using namespace std;
string last="*";
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

//rl_completion_entry_function = 
//my_source f;
//stream<my_source> is{f};

using namespace boost::iostreams;
