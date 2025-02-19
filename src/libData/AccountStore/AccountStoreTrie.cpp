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

#include "libData/AccountStore/AccountStoreTrie.h"
#include "libMessage/MessengerAccountStoreTrie.h"
#include "libUtils/DataConversion.h"


AccountStoreTrie::AccountStoreTrie() : m_db("state"), m_state(&m_db) {}


void AccountStoreTrie::Init() {
  AccountStoreSC::Init();
  InitTrie();
}


void AccountStoreTrie::InitTrie() {
  std::lock_guard<std::mutex> g(m_mutexTrie);
  m_state.init();
  m_prevRoot = m_state.root();
}


bool AccountStoreTrie::Serialize(zbytes& dst, unsigned int offset) const {
  std::lock_guard<std::mutex> g(m_mutexTrie);
  if (LOOKUP_NODE_MODE) {
    if (m_prevRoot != dev::h256()) {
      try {
        m_state.setRoot(m_prevRoot);
      } catch (...) {
        return false;
      }
    }
  }
  if (!MessengerAccountStoreTrie::SetAccountStoreTrie(
          dst, offset, m_state, this->m_addressToAccount)) {
    LOG_GENERAL(WARNING, "Messenger::SetAccountStoreTrie failed.");
    return false;
  }

  return true;
}


Account* AccountStoreTrie::GetAccount(const Address& address) {
  return this->GetAccount(address, false);
}


Account* AccountStoreTrie::GetAccount(const Address& address,
                                           bool resetRoot) {
  // LOG_MARKER();
  using namespace boost::multiprecision;

  Account* account = AccountStoreBase::GetAccount(address);
  if (account != nullptr) {
    return account;
  }

  std::string rawAccountBase;

  {
    std::lock(m_mutexTrie, m_mutexDB);
    std::lock_guard<std::mutex> lock1(m_mutexTrie, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(m_mutexDB, std::adopt_lock);

    if (LOOKUP_NODE_MODE && resetRoot) {
      if (m_prevRoot != dev::h256()) {
        try {
          auto t_state = m_state;
          t_state.setRoot(m_prevRoot);
          rawAccountBase =
              t_state.at(DataConversion::StringToCharArray(address.hex()));
        } catch (std::exception& e) {
          LOG_GENERAL(WARNING, "setRoot for " << m_prevRoot.hex() << " failed, "
                                              << e.what());
          return nullptr;
        }
      }
    } else {
      rawAccountBase =
          m_state.at(DataConversion::StringToCharArray(address.hex()));
    }
  }
  if (rawAccountBase.empty()) {
    return nullptr;
  }

  account = new Account();
  if (!account->DeserializeBase(
          zbytes(rawAccountBase.begin(), rawAccountBase.end()), 0)) {
    LOG_GENERAL(WARNING, "Account::DeserializeBase failed");
    delete account;
    return nullptr;
  }

  if (account->isContract()) {
    account->SetAddress(address);
  }

  auto it2 = this->m_addressToAccount->emplace(address, *account);

  delete account;

  return &it2.first->second;
}


bool AccountStoreTrie::GetProof(const Address& address,
                                     const dev::h256& rootHash,
                                     Account& account,
                                     std::set<std::string>& nodes) {
  if (!LOOKUP_NODE_MODE) {
    LOG_GENERAL(WARNING, "not lookup node");
    return false;
  }

  std::string rawAccountBase;

  dev::h256 t_rootHash = (rootHash == dev::h256()) ? m_prevRoot : rootHash;

  LOG_GENERAL(INFO, "RootHash " << t_rootHash.hex());

  {
    std::lock(m_mutexTrie, m_mutexDB);
    std::lock_guard<std::mutex> lock1(m_mutexTrie, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(m_mutexDB, std::adopt_lock);

    auto t_state = m_state;

    if (t_rootHash != dev::h256()) {
      try {
        t_state.setRoot(t_rootHash);
      } catch (std::exception& e) {
        LOG_GENERAL(WARNING, "setRoot for " << t_rootHash.hex() << " failed "
                                            << e.what());
        return false;
      }
    }

    rawAccountBase = t_state.getProof(
        DataConversion::StringToCharArray(address.hex()), nodes);
  }

  if (rawAccountBase.empty()) {
    return false;
  }

  Account t_account;
  if (!t_account.DeserializeBase(
          zbytes(rawAccountBase.begin(), rawAccountBase.end()), 0)) {
    LOG_GENERAL(WARNING, "Account::DeserializeBase failed");
    return false;
  }

  if (t_account.isContract()) {
    t_account.SetAddress(address);
  }

  account = std::move(t_account);

  return true;
}


bool AccountStoreTrie::UpdateStateTrie(const Address& address,
                                            const Account& account) {
  zbytes rawBytes;
  if (!account.SerializeBase(rawBytes, 0)) {
    LOG_GENERAL(WARNING, "Messenger::SetAccountBase failed");
    return false;
  }

  std::lock_guard<std::mutex> g(m_mutexTrie);
  m_state.insert(DataConversion::StringToCharArray(address.hex()), rawBytes);

  return true;
}


bool AccountStoreTrie::RemoveFromTrie(const Address& address) {
  // LOG_MARKER();
  std::lock_guard<std::mutex> g(m_mutexTrie);

  m_state.remove(DataConversion::StringToCharArray(address.hex()));

  return true;
}


dev::h256 AccountStoreTrie::GetStateRootHash() const {
  std::lock_guard<std::mutex> g(m_mutexTrie);

  return m_state.root();
}


dev::h256 AccountStoreTrie::GetPrevRootHash() const {
  std::lock_guard<std::mutex> g(m_mutexTrie);

  return m_prevRoot;
}


bool AccountStoreTrie::UpdateStateTrieAll() {
  std::lock_guard<std::mutex> g(m_mutexTrie);
  if (m_prevRoot != dev::h256()) {
    try {
      m_state.setRoot(m_prevRoot);
    } catch (...) {
      LOG_GENERAL(WARNING, "setRoot for " << m_prevRoot.hex() << " failed");
      return false;
    }
  }
  for (auto const& entry : *(this->m_addressToAccount)) {
    zbytes rawBytes;
    if (!entry.second.SerializeBase(rawBytes, 0)) {
      LOG_GENERAL(WARNING, "Messenger::SetAccountBase failed");
      return false;
    }
    m_state.insert(DataConversion::StringToCharArray(entry.first.hex()),
                   rawBytes);
  }

  m_prevRoot = m_state.root();

  return true;
}


void AccountStoreTrie::PrintAccountState() {
  AccountStoreBase::PrintAccountState();
  LOG_GENERAL(INFO, "State Root: " << GetStateRootHash());
}
