#include <string>
namespace TW::WalletConsole {
  using namespace std;
  class Command {
    public:
      string _group;
      string _name;
      string _help;
      bool _isExit;
      const char *complete(const char *, int i);
  };
}
