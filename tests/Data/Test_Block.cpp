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

#include <array>
#include <string>

#include "libCrypto/Sha2.h"
#include "libData/Block.h"
#include "libUtils/DataConversion.h"
#include "libUtils/Logger.h"

#define BOOST_TEST_MODULE blocktest
#include <boost/test/included/unit_test.hpp>

using namespace std;
using namespace boost::multiprecision;

DSBlockHeader dsHeader;

struct Fixture {
  Fixture() { INIT_STDOUT_LOGGER() }
};

BOOST_GLOBAL_FIXTURE(Fixture);

BOOST_AUTO_TEST_SUITE(blocktest)

BOOST_AUTO_TEST_CASE(DSBlock_test) {
  LOG_MARKER();

  std::array<unsigned char, BLOCK_HASH_SIZE> prevHash1;

  for (unsigned int i = 0; i < prevHash1.size(); i++) {
    prevHash1.at(i) = i + 1;
  }

  std::array<unsigned char, PUB_KEY_SIZE> pubKey1;

  for (unsigned int i = 0; i < pubKey1.size(); i++) {
    pubKey1.at(i) = i + 4;
  }

  std::array<unsigned char, BLOCK_SIG_SIZE> signature1;

  for (unsigned int i = 0; i < signature1.size(); i++) {
    signature1.at(i) = i + 8;
  }

  // FIXME: Handle exceptions.
  DSBlockHeader header1(20, prevHash1, 12345, pubKey1, pubKey1, 10, 789);

  DSBlock block1(header1, signature1);

  zbytes message1;
  block1.Serialize(message1, 0);
  LOG_PAYLOAD(INFO, "Block1 serialized", message1,
              Logger::MAX_BYTES_TO_DISPLAY);

  DSBlock block2(message1, 0);

  zbytes message2;
  block2.Serialize(message2, 0);
  dsHeader = block2.GetHeader();
  LOG_PAYLOAD(INFO, "Block2 serialized", message2,
              Logger::MAX_BYTES_TO_DISPLAY);
  DSBlockHeader header2 = block2.GetHeader();
  uint8_t diff2 = header2.GetDifficulty();
  const std::array<unsigned char, BLOCK_HASH_SIZE>& prevHash2 =
      header2.GetPrevHash();
  uint128_t nonce2 = header2.GetNonce();
  const std::array<unsigned char, PUB_KEY_SIZE>& pubKey2 =
      header2.GetMinerPubKey();
  uint128_t blockNum2 = header2.GetBlockNum();
  uint64_t timestamp2 = header2.GetTimestamp();
  const std::array<unsigned char, BLOCK_SIG_SIZE>& signature2 =
      block2.GetSignature();

  LOG_GENERAL(INFO, "Block 2 difficulty: " << to_string(diff2));
  BOOST_CHECK_MESSAGE(diff2 == 20,
                      "expected: " << 20 << " actual: " << diff2 << "\n");

  BOOST_CHECK_MESSAGE(
      prevHash2.at(31) == 32,
      "expected: " << 32 << " actual: " << prevHash2.at(31) << "\n");

  LOG_GENERAL(INFO, "Block 2 nonce: " << nonce2);
  BOOST_CHECK_MESSAGE(nonce2 == 12345,
                      "expected: " << 12345 << " actual: " << nonce2 << "\n");

  BOOST_CHECK_MESSAGE(
      pubKey2.at(32) == 36,
      "expected: " << 36 << " actual: " << pubKey2.at(32) << "\n");

  LOG_GENERAL(INFO, "Block 2 blockNum: " << blockNum2);
  BOOST_CHECK_MESSAGE(blockNum2 == 10,
                      "expected: " << 10 << " actual: " << blockNum2 << "\n");

  LOG_GENERAL(INFO, "Block 2 timestamp: " << timestamp2);
  BOOST_CHECK_MESSAGE(timestamp2 == 789,
                      "expected: " << 789 << " actual: " << timestamp2 << "\n");

  BOOST_CHECK_MESSAGE(
      signature2.at(63) == 71,
      "expected: " << 71 << " actual: " << signature2.at(63) << "\n");
}

Transaction CreateDummyTx1() {
  std::array<unsigned char, ACC_ADDR_SIZE> toAddr;

  for (unsigned int i = 0; i < toAddr.size(); i++) {
    toAddr.at(i) = i + 4;
  }

  std::array<unsigned char, ACC_ADDR_SIZE> fromAddr;

  for (unsigned int i = 0; i < fromAddr.size(); i++) {
    fromAddr.at(i) = i + 8;
  }

  std::array<unsigned char, TRAN_SIG_SIZE> signature;

  for (unsigned int i = 0; i < signature.size(); i++) {
    signature.at(i) = i + 16;
  }

  Predicate pred(3, fromAddr, 2, 1, toAddr, fromAddr, 33, 1);

  Transaction tx1(DataConversion::Pack(CHAIN_ID, 1), 5, toAddr, fromAddr, 55,
                  signature, pred);

  return tx1;
}

Transaction CreateDummyTx2() {
  std::array<unsigned char, ACC_ADDR_SIZE> toAddr;

  for (unsigned int i = 0; i < toAddr.size(); i++) {
    toAddr.at(i) = i + 1;
  }

  std::array<unsigned char, ACC_ADDR_SIZE> fromAddr;

  for (unsigned int i = 0; i < fromAddr.size(); i++) {
    fromAddr.at(i) = i + 3;
  }

  std::array<unsigned char, TRAN_SIG_SIZE> signature;

  for (unsigned int i = 0; i < signature.size(); i++) {
    signature.at(i) = i + 5;
  }

  Predicate pred(3, fromAddr, 2, 1, toAddr, fromAddr, 10, 2);

  Transaction tx1(DataConversion::Pack(CHAIN_ID, 1), 6, toAddr, fromAddr, 10,
                  signature, pred);

  return tx1;
}

BOOST_AUTO_TEST_CASE(TxBlock_test) {
  LOG_MARKER();

  Transaction tx1 = CreateDummyTx1();
  Transaction tx2 = CreateDummyTx2();

  // compute tx root hash
  zbytes vec;
  tx1.Serialize(vec, 0);
  SHA256Calculator sha2;
  sha2.Update(vec);
  zbytes tx1HashVec = sha2.Finalize();
  std::array<unsigned char, TRAN_HASH_SIZE> tx1Hash;
  copy(tx1HashVec.begin(), tx1HashVec.end(), tx1Hash.begin());

  vec.clear();
  sha2.Reset();
  tx2.Serialize(vec, 0);
  sha2.Update(vec);
  zbytes tx2HashVec = sha2.Finalize();
  std::array<unsigned char, TRAN_HASH_SIZE> tx2Hash;
  copy(tx2HashVec.begin(), tx2HashVec.end(), tx2Hash.begin());

  std::vector<std::array<unsigned char, TRAN_HASH_SIZE>> tranHashes1;
  tranHashes1.emplace_back(tx1Hash);
  tranHashes1.emplace_back(tx2Hash);

  std::vector<Transaction> tranData1;
  tranData1.emplace_back(tx1);
  tranData1.emplace_back(tx2);

  std::array<unsigned char, PUB_KEY_SIZE> pubKey1;

  for (unsigned int i = 0; i < pubKey1.size(); i++) {
    pubKey1.at(i) = i + 4;
  }

  std::array<unsigned char, BLOCK_SIG_SIZE> signature1;

  for (unsigned int i = 0; i < signature1.size(); i++) {
    signature1.at(i) = i + 8;
  }

  std::array<unsigned char, BLOCK_HASH_SIZE> zeroHeaderHash = {0};
  std::array<unsigned char, TRAN_HASH_SIZE> zeroTxHash = {0};
  std::vector<std::array<unsigned char, TRAN_HASH_SIZE>> emptyTranHashes;
  std::vector<Transaction> emptyTranData;
  TxBlockHeader header0(1, 1, 100, 50, zeroHeaderHash, 0, 12345, zeroTxHash, 0,
                        pubKey1, 0, zeroHeaderHash);
  TxBlock block0(header0, signature1, emptyTranHashes, 0, emptyTranData);

  // compute header hash
  vec.clear();
  header0.Serialize(vec, 0);
  sha2.Reset();
  sha2.Update(vec);
  zbytes prevHash1Vec = sha2.Finalize();
  std::array<unsigned char, BLOCK_HASH_SIZE> prevHash1;
  copy(prevHash1Vec.begin(), prevHash1Vec.end(), prevHash1.begin());

  vec.clear();
  copy(tx1Hash.begin(), tx1Hash.end(), back_inserter(vec));
  sha2.Reset();
  sha2.Update(vec);
  vec.clear();
  copy(tx2Hash.begin(), tx2Hash.end(), back_inserter(vec));
  sha2.Update(vec);
  zbytes txRootHash1Vec = sha2.Finalize();
  std::array<unsigned char, TRAN_HASH_SIZE> txRootHash1;
  copy(txRootHash1Vec.begin(), txRootHash1Vec.end(), txRootHash1.begin());

  sha2.Reset();
  zbytes headerSerialized;
  dsHeader.Serialize(headerSerialized, 0);
  sha2.Update(headerSerialized);
  zbytes headerHashVec = sha2.Finalize();
  std::array<unsigned char, BLOCK_HASH_SIZE> headerHash;
  copy(headerHashVec.begin(), headerHashVec.end(), headerHash.begin());
  TxBlockHeader header1(1, 1, 100, 50, prevHash1, 1, 23456, txRootHash1, 2,
                        pubKey1, dsHeader.GetBlockNum(), headerHash);
  TxBlock block1(header1, signature1, tranHashes1, 2, tranData1);

  zbytes message1;
  block1.Serialize(message1, 0);

  LOG_PAYLOAD(INFO, "Block1 serialized", message1,
              Logger::MAX_BYTES_TO_DISPLAY);

  TxBlock block2(message1, 0);

  zbytes message2;
  block2.Serialize(message2, 0);

  LOG_PAYLOAD(INFO, "Block2 serialized", message2,
              Logger::MAX_BYTES_TO_DISPLAY);

  for (unsigned int i = 0; i < message1.size(); i++) {
    if (message1.at(i) != message2.at(i)) {
      LOG_GENERAL(INFO, "message1[" << i << "]=" << std::hex << message1.at(i)
                                    << ", message2[" << i << "]=" << std::hex
                                    << message2.at(i));
    }
  }

  BOOST_CHECK_MESSAGE(message1 == message2,
                      "Block1 serialized != Block2 serialized!");

  TxBlockHeader header2 = block2.GetHeader();
  uint32_t version2 = header2.GetVersion();
  uint128_t gasLimit2 = header2.GetGasLimit();
  uint128_t gasUsed2 = header2.GetGasUsed();
  const std::array<unsigned char, BLOCK_HASH_SIZE>& prevHash2 =
      header2.GetPrevHash();
  uint128_t blockNum2 = header2.GetBlockNum();
  uint64_t timestamp2 = header2.GetTimestamp();
  const std::array<unsigned char, TRAN_HASH_SIZE>& txRootHash2 =
      header2.GetTxRootHash();
  uint32_t numTxs2 = header2.GetNumTxs();
  const std::array<unsigned char, PUB_KEY_SIZE>& pubKey2 =
      header2.GetMinerPubKey();
  uint128_t dsBlockNum2 = header2.GetDSBlockNum();
  const std::array<unsigned char, BLOCK_HASH_SIZE>& dsBlockHeader2 =
      header2.GetDSBlockHeader();

  const std::array<unsigned char, BLOCK_SIG_SIZE>& signature2 =
      block2.GetHeaderSig();
  uint32_t numTxData2 = block2.GetNumTxData();
  const std::vector<std::array<unsigned char, TRAN_HASH_SIZE>> tranHashes2 =
      block2.GetTranHashes();
  const std::vector<Transaction> tranData2 = block2.GetTranData();

  uint32_t type2_large = type2;
  LOG_GENERAL(INFO, "Block 2 type: " << type2_large);
  BOOST_CHECK_MESSAGE(type2 == 1,
                      "expected: " << 1 << " actual: " << type2 << "\n");

  LOG_GENERAL(INFO, "Block 2 version: " << version2);
  BOOST_CHECK_MESSAGE(version2 == 1,
                      "expected: " << 1 << " actual: " << version2 << "\n");

  LOG_GENERAL(INFO, "Block 2 gasLimit: " << gasLimit2);
  BOOST_CHECK_MESSAGE(gasLimit2 == 100,
                      "expected: " << 100 << " actual: " << gasLimit2 << "\n");

  LOG_GENERAL(INFO, "Block 2 gasUsed: " << gasUsed2);
  BOOST_CHECK_MESSAGE(gasUsed2 == 50,
                      "expected: " << 50 << " actual: " << gasUsed2 << "\n");

  zbytes byteVec;
  byteVec.resize(BLOCK_HASH_SIZE);
  copy(prevHash2.begin(), prevHash2.end(), byteVec.begin());
  LOG_PAYLOAD(INFO, "Block 2 prevHash", byteVec, Logger::MAX_BYTES_TO_DISPLAY);
  std::string expectedStr =
      "0D3979DA06841562C90DE5212BE5EFCF88FAEA17118945B6B49D304DE295E407";
  zbytes expectedVec;
  DataConversion::HexStrToUint8Vec(expectedStr, expectedVec);
  bool is_prevHash_equal = std::equal(byteVec.begin(), byteVec.end(),
                                      expectedVec.begin(), expectedVec.end());

  std::string actualStr;
  DataConversion::Uint8VecToHexStr(byteVec, actualStr))
  BOOST_CHECK_MESSAGE(
      is_prevHash_equal == true,
      "expected: " << expectedStr
                   << " actual: " << actualStr);

  LOG_GENERAL(INFO, "Block 2 blockNum: " << blockNum2);
  BOOST_CHECK_MESSAGE(blockNum2 == 1,
                      "expected: " << 1 << " actual: " << blockNum2 << "\n");

  LOG_GENERAL(INFO, "Block 2 timestamp: " << timestamp2);
  BOOST_CHECK_MESSAGE(timestamp2 == 23456, "expected: " << 23456 << " actual: "
                                                        << timestamp2 << "\n");

  byteVec.clear();
  byteVec.resize(TRAN_HASH_SIZE);
  copy(txRootHash2.begin(), txRootHash2.end(), byteVec.begin());
  LOG_PAYLOAD(INFO, "Block 2 txRootHash2", byteVec,
              Logger::MAX_BYTES_TO_DISPLAY);
  expectedStr =
      "4A740D0FA29B841C6D99B02892273F7D00518EF12DAFA2AD4D198E630789CF3B";
  expectedVec.clear();
  expectedVec = DataConversion::HexStrToUint8Vec(expectedStr);
  bool is_txRootHash_equal = std::equal(byteVec.begin(), byteVec.end(),
                                        expectedVec.begin(), expectedVec.end());

  DataConversion::Uint8VecToHexStr(byteVec), actualStr);
  BOOST_CHECK_MESSAGE(is_txRootHash_equal == true,
                      "expected: " << expectedStr << " actual: " << actualStr);

  LOG_GENERAL(INFO, "Block 2 numTxs2: " << numTxs2);
  BOOST_CHECK_MESSAGE(numTxs2 == 2,
                      "expected: " << 2 << " actual: " << numTxs2 << "\n");

  BOOST_CHECK_MESSAGE(
      pubKey2.at(32) == 36,
      "expected: " << 36 << " actual: " << pubKey2.at(32) << "\n");

  LOG_GENERAL(INFO, "Block 2 numTxData2: " << numTxData2);
  BOOST_CHECK_MESSAGE(numTxData2 == 2,
                      "expected: " << 2 << " actual: " << numTxData2 << "\n");

  bool allHashesEqual = true;
  if (tranHashes2.size() != numTxs2) {
    allHashesEqual = false;
  } else {
    for (unsigned int i = 0; i < numTxs2; i++) {
      if (tranHashes1.at(i) != tranHashes2.at(i)) {
        allHashesEqual = false;
        break;
      }
    }
  }
  BOOST_CHECK_MESSAGE(
      allHashesEqual == true,
      "Transaction hashes between Block1 and Block2 do not match!");

  bool allDataEqual = true;
  if (tranData2.size() != numTxData2) {
    allDataEqual = false;
  } else {
    for (unsigned int i = 0; i < numTxData2; i++) {
      if (tranData1.at(i).GetTranID() != tranData2.at(i).GetTranID()) {
        allDataEqual = false;
        break;
      }
    }
  }
  BOOST_CHECK_MESSAGE(
      allDataEqual == true,
      "Transaction data between Block1 and Block2 do not match!");

  BOOST_CHECK_MESSAGE(
      signature2.at(63) == 71,
      "expected: " << 71 << " actual: " << signature2.at(63) << "\n");

  BOOST_CHECK_MESSAGE(dsBlockNum2 == 10,
                      "expected: " << 10 << " actual: " << dsBlockNum2);

  zbytes dsBlockHeader2Vec(BLOCK_HASH_SIZE);
  copy(dsBlockHeader2.begin(), dsBlockHeader2.end(), dsBlockHeader2Vec.begin());

  std::string headerhashStr, dsblockheader2str;
  DataConversion::Uint8VecToHexStr(headerHashVec, headerhashStr);
  DataConversion::Uint8VecToHexStr(dsBlockHeader2Vec, dsblockheader2str);
  BOOST_CHECK_MESSAGE(
      dsBlockHeader2 == headerHash,
      "expected: " << headerhashStr << " actual: " << dsblockheader2str);
}

BOOST_AUTO_TEST_SUITE_END()
