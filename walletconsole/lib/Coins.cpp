
// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Coins.h"

#include "WalletConsole.h"
#include "Util.h"

#include <TrustWalletCore/TWCoinTypeConfiguration.h>
#include <TrustWalletCore/TWPublicKeyType.h>
#include "Data.h"
#include "Coin.h"

#include <iostream>
#include <vector>
#include <cassert>

#define WRAPS(x) std::shared_ptr<TWString>(x, TWStringDelete)

namespace TW::WalletConsole {

using namespace std;

static size_t symbolMax;
static size_t idMax;
static size_t nameMax;
ostream &operator<<(ostream &lhs, const Coin &rhs) {
  const string symbol="\""+rhs.symbol+"\"";
  const string id="\""+rhs.id+"\"";
  const string name="\""+rhs.name+"\"";
  return lhs
    << left << setw(int(symbolMax+3)) << symbol
    << left << setw(int(idMax+3)) << id
    << left << setw(int(nameMax+3)) << name;
}
void Coins::coins() const {
    for (auto c: _coinsById) {
      _out << _coins[c.second] << endl;
    }
    _out << _coinsById.size() << " coins listed." << endl;
}

int Coins::findCoinId(const string& coin) const {
  auto idItr = _coinsById.find(coin);
  auto nameItr = _coinsByName.find(coin);
  auto symbolItr = _coinsBySymbol.find(coin);

  int map = 
    (idItr==_coinsById.end()?0:1) |
    (nameItr==_coinsByName.end()?0:2) |
    (symbolItr==_coinsBySymbol.end()?0:4);
  

  switch(map) {
    // Zero matches
    case 0: return -1;
            // One Match
    case 1: return idItr->second;
    case 2: return nameItr->second;
    case 4: return symbolItr->second;
            // Two Matches
    case 3: 
            if(*idItr==*nameItr)
              return idItr->second;
            break;
    case 5:
            if(*idItr==*symbolItr)
              return idItr->second;
            break;
    case 6:
            if(*nameItr==*symbolItr)
              return nameItr->second;
            break;
            // Three Matches, good lord!
    case 7:
            if(*idItr!=*nameItr)
              break;
            if(*nameItr!=*symbolItr)
              break;
            return idItr->second;
  }
  cerr << "Ambiguoud Coin Selection." << endl;
  cerr << "  matches:" << endl;
  if(idItr!=_coinsById.end())
    cerr << "    " << _coins[idItr->second] << endl;
  if(nameItr!=_coinsByName.end())
    cerr << "    " << _coins[nameItr->second] << endl;
  if(symbolItr!=_coinsBySymbol.end())
    cerr << "    " << _coins[symbolItr->second] << endl;
  cerr << endl;
  return -1;
}

bool Coins::findCoin(const string& coin, Coin& coin_out) const {
  int c = findCoinId(coin);
  if (c < 0) {
    _out << "Error: No such coin '" << coin << "'" << endl;
    return false;
  }
  coin_out = *(_coins.begin()+c);
  return true;
}

void Coins::init() {
    // not very nice method: try each ID number, and record the ones
    // that are valid coins
    _out << "Loading coins ... ";
    scanCoins();
    _out << _coinsById.size() << " coins loaded." << endl;
}

void Coins::scanCoins() {
    const auto types = TW::getCoinTypes();
    cerr << types.size() << " coin types" << endl;
    _coins.clear();
    Coin dummy;
    _coins.push_back(dummy);
    // FIXME:  this should be part of the generation process
    for(auto c: types) {
      auto symbolTw = WRAPS(TWCoinTypeConfigurationGetSymbol(c));
      assert(TWStringSize(symbolTw.get()) != 0);
      string id = TWStringUTF8Bytes(WRAPS(TWCoinTypeConfigurationGetID(c)).get());
      Util::toLower(id);
      string symbol = TWStringUTF8Bytes(symbolTw.get());
      Util::toLower(symbol);
      string name = TWStringUTF8Bytes(WRAPS(TWCoinTypeConfigurationGetName(c)).get());
      Util::toLower(name);
      int curve = (int)TWCoinTypeCurve(c);
      int pubKeyType = (int)TW::publicKeyType(c);
      string derivPath = TW::derivationPath(c).string();
      Coin coin = Coin{c, id, name, symbol, curve, pubKeyType, derivPath};
      _coins.push_back(coin);
      idMax=max(id.length(),idMax);
      nameMax=max(name.length(),nameMax);
      symbolMax=max(symbol.length(),symbolMax);
    }
    cerr << _coins.size() << " coins" << endl;
    for (size_t index=1; index<_coins.size(); index++ ) {
      cerr << setw(4) << index << _coins[index] << endl;
    }
    for (size_t index=1; index<_coins.size(); index++ ) {
      const Coin &coin=_coins[index];
      cerr << coin << endl;
      auto idItr = _coinsById.find(coin.id);
      auto numItr = _coinsByNum.find(coin.c);
      auto nameItr = _coinsByName.find(coin.name);
      auto symbolItr = _coinsBySymbol.find(coin.symbol);
      if(
          (idItr!=_coinsById.end()) ||
          (numItr!=_coinsByNum.end()) ||
          (nameItr!=_coinsByName.end()) ||
          (symbolItr!=_coinsBySymbol.end())
        )
      {
        cerr << "Duplicate Coin: " << endl;
        if(numItr!=_coinsByNum.end())
         cerr << "#: " << _coins[numItr->second] << endl;
        if(idItr!=_coinsById.end())
          cerr << "I: " << _coins[idItr->second] << endl;
        if(nameItr!=_coinsByName.end())
          cerr << "N: " << _coins[nameItr->second] << endl;
        if(symbolItr!=_coinsBySymbol.end())
          cerr << "S: " << _coins[symbolItr->second] << endl;
        cerr << "*: " << coin << endl;
        cerr << endl;
      } else {
        _coinsByNum[coin.c] = (int)index;
        _coinsById[coin.id] = (int)index;
        _coinsByName[coin.name] = (int)index;
        _coinsBySymbol[coin.symbol] = (int)index;
        assert(_coins[index].id == coin.id);
        assert(_coins[index].name == coin.name);
        assert(_coins[index].symbol == coin.symbol);
      }
    }
}

} // namespace TW::WalletConsole
