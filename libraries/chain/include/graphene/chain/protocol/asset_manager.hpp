/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/chain_parameters.hpp>

namespace graphene { namespace chain {

   /**
    * @brief Create a asset_manager object, as a bid to hold a asset_manager seat on the network.
    * @ingroup operations
    */
   struct asset_manager_create_operation : public base_operation
   {
      struct fee_parameters_type { uint64_t fee = 50000 * GRAPHENE_BLOCKCHAIN_PRECISION; };

      asset                                 fee;
      /// The account which owns the asset manager. This account pays the fee for this operation.
      account_id_type                       asset_manager_account;
      string                                url;

      account_id_type fee_payer()const { return asset_manager_account; }
      void            validate()const;
   };

   /**
    * @brief Update a asset_manager object.
    * @ingroup operations
    *
    * Currently the only field which can be updated is the `url`
    * field.
    */
   struct asset_manager_update_operation : public base_operation
   {
      struct fee_parameters_type { uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION; };

      asset                                 fee;
      /// The asset manager to update.
      asset_manager_id_type                 asset_manager;
      /// The account which owns the asset manager. This account pays the fee for this operation.
      account_id_type                       asset_manager_account;
      optional< string >                    new_url;

      account_id_type fee_payer()const { return asset_manager_account; }
      void            validate()const;
   };

   /**
    * @brief Used by asset manager account to force buyback assets which have support force buyback flags.
    * @ingroup operations
    *
    * This operation allows the asset manager account to force buyback assets which have support force buyback flags.
    *
    */
   struct asset_manager_force_buyback_asset_operation : public base_operation
   {
      struct fee_parameters_type { uint64_t fee = GRAPHENE_BLOCKCHAIN_PRECISION; };

      asset             fee;
      chain_parameters  new_parameters;
      
      asset_id_type     asset_id;
      account_id_type   asset_manager_account;
      // BASE/QUOTE: CORE/ASSET_SYMBOL
      price             buyback_price;

      account_id_type fee_payer()const { return asset_manager_account; }
      void            validate()const;
   };

   /// TODO: committee_member_resign_operation : public base_operation

} } // graphene::chain
FC_REFLECT( graphene::chain::asset_manager_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::asset_manager_update_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::asset_manager_force_buyback_asset_operation::fee_parameters_type, (fee) )


FC_REFLECT( graphene::chain::asset_manager_create_operation,
            (fee)(asset_manager_account)(url) )
FC_REFLECT( graphene::chain::asset_manager_update_operation,
            (fee)(asset_manager)(asset_manager_account)(new_url) )
FC_REFLECT( graphene::chain::asset_manager_force_buyback_asset_operation, (fee)(new_parameters)(asset_id)(asset_manager_account)(buyback_price) );
