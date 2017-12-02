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

#ifndef IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP
#define IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP

#include "interfaces/transaction.hpp"

#include <boost/range/numeric.hpp>

#include "backend/protobuf/commands/proto_command.hpp"
#include "backend/protobuf/common_objects/signature.hpp"
#include "backend/protobuf/common_objects/trivial_proto.hpp"
#include "block.pb.h"
#include "utils/lazy_initializer.hpp"

namespace shared_model {
  namespace proto {
    class Transaction : public CopyableProto<interface::Transaction,
                                             iroha::protocol::Transaction,
                                             Transaction> {
     public:
      template <typename TransactionType>
      explicit Transaction(TransactionType &&transaction)
          : CopyableProto(std::forward<TransactionType>(transaction)),
            payload_(detail::makeReferenceGenerator(
                proto_, &iroha::protocol::Transaction::payload)),
            commands_([this] {
              return boost::accumulate(
                  payload_->commands(),
                  CommandsType{},
                  [](auto &&acc, const auto &cmd) {
                    acc.emplace_back(new Command(cmd));
                    return std::forward<decltype(acc)>(acc);
                  });
            }),
            blob_([this] { return BlobType(proto_->SerializeAsString()); }),
            signatures_([this] {
              return boost::accumulate(
                  proto_->signature(),
                  SignatureSetType{},
                  [](auto &&acc, const auto &sig) {
                    acc.emplace(new Signature(sig));
                    return std::forward<decltype(acc)>(acc);
                  });
            }) {}

      Transaction(const Transaction &o) : Transaction(o.proto_) {}

      Transaction(Transaction &&o) noexcept
          : Transaction(std::move(o.proto_)) {}

      const interface::types::AccountIdType &creatorAccountId() const override {
        return payload_->creator_account_id();
      }

      shared_model::interface::Transaction::TxCounterType transactionCounter()
          const override {
        return payload_->tx_counter();
      }

      const shared_model::interface::Transaction::CommandsType &commands()
          const override {
        return *commands_;
      }

      const shared_model::interface::Hashable<
          shared_model::interface::Transaction,
          iroha::model::Transaction>::BlobType &
      blob() const override {
        return *blob_;
      }

      const shared_model::interface::Signable<
          shared_model::interface::Transaction,
          iroha::model::Transaction>::SignatureSetType &
      signatures() const override {
        return *signatures_;
      }

      bool addSignature(
          const shared_model::interface::Signable<
              shared_model::interface::Transaction,
              iroha::model::Transaction>::SignatureType &signature) override {
        if (signatures_->count(signature) > 0) {
          return false;
        }
        auto sig = proto_->add_signature();
        sig->set_pubkey(signature->publicKey().blob());
        sig->set_signature(signature->signedData().blob());
        signatures_.invalidate();
        return true;
      }

      shared_model::interface::Signable<
              shared_model::interface::Transaction,
              iroha::model::Transaction>::TimestampType createdTime() const override {
        return payload_->created_time();
      }

     private:
      // lazy
      template <typename T>
      using Lazy = detail::LazyInitializer<T>;

      const Lazy<const iroha::protocol::Transaction::Payload &> payload_;

      const Lazy<CommandsType> commands_;

      const Lazy<BlobType> blob_;

      const Lazy<SignatureSetType> signatures_;
    };
  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP
