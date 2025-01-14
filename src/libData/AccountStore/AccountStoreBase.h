/*
 * Copyright (C) 2019 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZILLIQA_SRC_LIBDATA_ACCOUNTSTORE_ACCOUNTSTOREBASE_H_
#define ZILLIQA_SRC_LIBDATA_ACCOUNTSTORE_ACCOUNTSTOREBASE_H_

#include <Schnorr.h>
#include "common/Constants.h"
#include "common/Serializable.h"
#include "common/TxnStatus.h"
#include "depends/common/FixedHash.h"
#include "libData/AccountData/Account.h"
#include "libData/AccountData/Address.h"
#include "libData/AccountData/Transaction.h"
#include "libData/AccountData/TransactionReceipt.h"

class AccountStoreBase : public SerializableDataBlock {
 protected:
  std::shared_ptr<std::unordered_map<Address,Account>> m_addressToAccount;

  AccountStoreBase();

  bool CalculateGasRefund(const uint128_t& gasDeposit, const uint64_t& gasUnit,
                          const uint128_t& gasPrice, uint128_t& gasRefund);

  bool UpdateAccounts(const Transaction& transaction,
                      TransactionReceipt& receipt, TxnStatus& error_code);

 public:
  virtual void Init();

  /// Implements the Serialize function inherited from Serializable.
  bool Serialize(zbytes& dst, unsigned int offset) const;

  /// Implements the Deserialize function inherited from Serializable.
  virtual bool Deserialize(const zbytes& src, unsigned int offset);

  /// Implements the Deserialize function inherited from Serializable.
  virtual bool Deserialize(const std::string& src, unsigned int offset);

  virtual Account* GetAccount(const Address& address);

  /// Verifies existence of Account in the list.
  bool IsAccountExist(const Address& address);

  /// Adds an Account to the list.
  bool AddAccount(const Address& address, const Account& account,
                  bool toReplace = false);
  bool AddAccount(const PubKey& pubKey, const Account& account);

  void RemoveAccount(const Address& address);

  size_t GetNumOfAccounts() const;

  bool IncreaseBalance(const Address& address, const uint128_t& delta);
  bool DecreaseBalance(const Address& address, const uint128_t& delta);

  /// Updates the source and destination accounts included in the specified
  /// Transaction.
  bool TransferBalance(const Address& from, const Address& to,
                       const uint128_t& delta);
  uint128_t GetBalance(const Address& address);

  bool IncreaseNonce(const Address& address);
  uint64_t GetNonce(const Address& address);

  virtual void PrintAccountState();
};

#endif  // ZILLIQA_SRC_LIBDATA_ACCOUNTSTORE_ACCOUNTSTOREBASE_H_
