/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_PROTO_TRANSACTION_BUILDER_HPP
#define IROHA_PROTO_TRANSACTION_BUILDER_HPP

#include "amount/amount.hpp"
#include "backend/protobuf/transaction.hpp"
#include "block.pb.h"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  namespace proto {
    class TransactionBuilder {
     private:
      iroha::protocol::Transaction transaction_;

     public:
      TransactionBuilder() = default;

      TransactionBuilder creatorAccountId(
          const interface::types::AccountIdType &account_id) {
        transaction_.mutable_payload()->set_creator_account_id(account_id);
        return *this;
      }

      TransactionBuilder txCounter(Transaction::TxCounterType tx_counter) {
        transaction_.mutable_payload()->set_tx_counter(tx_counter);
        return *this;
      }

      TransactionBuilder addAssetQuantity(
          const interface::types::AccountIdType &account_id,
          const interface::types::AssetIdType &asset_id,
          const std::string &amount) {
        auto command = transaction_.mutable_payload()
                           ->add_commands()
                           ->mutable_add_asset_quantity();
        command->set_account_id(account_id);
        command->set_asset_id(asset_id);
        iroha::Amount::createFromString(amount) | [&](auto &&amount) {
          auto proto_amount = command->mutable_amount();
          auto proto_value = proto_amount->mutable_value();
          auto uint64s = amount.to_uint64s();
          proto_value->set_first(uint64s.at(0));
          proto_value->set_second(uint64s.at(1));
          proto_value->set_third(uint64s.at(2));
          proto_value->set_fourth(uint64s.at(3));
          proto_amount->set_precision(amount.getPrecision());
        };
        return *this;
      }

      Transaction build() {
        return Transaction(std::move(transaction_));
      }
    };
  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_PROTO_TRANSACTION_BUILDER_HPP
