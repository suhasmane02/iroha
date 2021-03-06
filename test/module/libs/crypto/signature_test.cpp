/*
Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "common/byteutils.hpp"
#include "common/types.hpp"
#include "crypto/base64.hpp"
#include "crypto/crypto.hpp"

#include <gtest/gtest.h>

using iroha::create_seed;
using iroha::create_keypair;
using iroha::sign;
using iroha::verify;
using iroha::stringToBlob;

TEST(Signature, sign_data_size) {
  auto keypair = iroha::create_keypair();

  std::string nonce =
      "c0a5cca43b8aa79eb50e3464bc839dd6fd414fae0ddf928ca23dcebf8a8b8dd0";
  auto signature = sign((const unsigned char*)nonce.c_str(), nonce.size(),
                        keypair.pubkey, keypair.privkey);

  ASSERT_TRUE(verify((const unsigned char*)nonce.c_str(), nonce.size(),
                     keypair.pubkey, signature));
}

TEST(Signature, PrintkeyPair) {
  auto keypair = iroha::create_keypair();
  ASSERT_NO_THROW({ std::cout << keypair.pubkey.to_base64() << std::endl; });
  ASSERT_NO_THROW({ std::cout << keypair.privkey.to_base64() << std::endl; });
}
