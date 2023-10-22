// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace TW::WalletConsole {

using namespace std;

class Coin {
public:
    int c;
    string id;
    string name;
    string symbol;
    int curve; // TWCurve
    int pubKeyType; // TWPublicKeyType
    string derivPath;
};

class Coins {
public:
    Coins(ostream& out) : _out(out) {}
    void coins() const;
    bool findCoin(const string& coin, Coin& coin_out) const;
    void init();
protected:
    void scanCoins();
    int findCoinId(const string& coin) const;
    static int pubKeyTypeFromCurve(int cc);
    ostream& _out;
    // Rather than four coin copies of
    // each coin, I added a vector here,
    // and now the maps are just indices into
    // the vector.
    //
    // The vector has a decault constructed
    // Coin object in position 0, so an index
    // of zero from the maps indicates a non-coin
    map<int, int> _coinsByNum;
    map<string, int> _coinsById;
    map<string, int> _coinsByName;
    map<string, int> _coinsBySymbol;
    vector<Coin> _coins;
};
ostream &operator<<(ostream &lhs, const Coin &rhs);

} // namespace TW::WalletConsole
