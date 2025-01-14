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

#ifndef ZILLIQA_SRC_LIBDATA_BLOCKDATA_BLOCK_BLOCKBASE_H_
#define ZILLIQA_SRC_LIBDATA_BLOCKDATA_BLOCK_BLOCKBASE_H_

#include <Schnorr.h>
#include "libData/BlockData/BlockHeader/BlockHeaderBase.h"

struct CoSignatures {
  Signature m_CS1;
  std::vector<bool> m_B1;
  Signature m_CS2;
  std::vector<bool> m_B2;

  CoSignatures(unsigned int bitmaplen = 1) : m_B1(bitmaplen), m_B2(bitmaplen) {}
  CoSignatures(const Signature& CS1, const std::vector<bool>& B1,
               const Signature& CS2, const std::vector<bool>& B2)
      : m_CS1(CS1), m_B1(B1), m_CS2(CS2), m_B2(B2) {}
};

/// [TODO] Base class for all supported block data types
class BlockBase : public SerializableDataBlock {
  // TODO: pull out all common code from ds, micro and tx block
 protected:
  BlockHash m_blockHash;
  CoSignatures m_cosigs;
  uint64_t m_timestamp;

 public:
  /// Default constructor.
  BlockBase() : m_timestamp(0) {}

  /// Implements the Serialize function inherited from Serializable.
  virtual bool Serialize(zbytes& /*dst*/,
                         unsigned int /*offset*/) const override {
    return true;
  }

  /// Implements the Deserialize function inherited from Serializable.
  virtual bool Deserialize(const zbytes& /*src*/,
                           unsigned int /*offset*/) override {
    return true;
  }

  /// Implements the Deserialize function inherited from Serializable.
  virtual bool Deserialize(const std::string& /*src*/,
                           unsigned int /*offset*/) override {
    return true;
  }

  /// Returns the block hash
  const BlockHash& GetBlockHash() const { return m_blockHash; }

  /// Returns the timestamp
  const uint64_t& GetTimestamp() const { return m_timestamp; }

  /// Set the timestamp
  void SetTimestamp(const uint64_t& timestamp) { m_timestamp = timestamp; }

  /// Set the block hash
  void SetBlockHash(const BlockHash& blockHash) { m_blockHash = blockHash; }

  /// Returns the co-sig for first round.
  const Signature& GetCS1() const { return m_cosigs.m_CS1; }

  /// Returns the co-sig bitmap for first round.
  const std::vector<bool>& GetB1() const { return m_cosigs.m_B1; }

  /// Returns the co-sig for second round.
  const Signature& GetCS2() const { return m_cosigs.m_CS2; }

  /// Returns the co-sig bitmap for second round.
  const std::vector<bool>& GetB2() const { return m_cosigs.m_B2; }

  /// Sets the co-sig members.
  void SetCoSignatures(CoSignatures& cosigs) { m_cosigs = cosigs; }
  void SetCoSignatures(CoSignatures&& cosigs) { m_cosigs = std::move(cosigs); }

  friend std::ostream& operator<<(std::ostream& os, const BlockBase& t);
};

inline std::ostream& operator<<(std::ostream& os, const BlockBase& t) {
  os << "<BlockBase>" << std::endl
     << " m_blockHash = " << t.GetBlockHash() << std::endl
     << " m_timestamp = " << t.GetTimestamp();
  return os;
}

#endif  // ZILLIQA_SRC_LIBDATA_BLOCKDATA_BLOCK_BLOCKBASE_H_
