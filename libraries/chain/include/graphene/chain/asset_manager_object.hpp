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
#include <graphene/chain/protocol/types.hpp>
#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene { namespace chain {
   using namespace graphene::db;

   class account_object;

   /**
    *  @brief tracks information about a asset manager account.
    *  @ingroup object
    *
    *  A asset manager is an kind of account able to create assets which can be forced buy back.
    */
   class asset_manager_object : public abstract_object<asset_manager_object>
   {
      public:
         static const uint8_t space_id = protocol_ids;
         static const uint8_t type_id  = asset_manager_object_type;

         account_id_type  asset_manager_account;
         string           url;
   };

   struct by_account;
   using asset_manager_multi_index_type = multi_index_container<
      asset_manager_object,
      indexed_by<
         ordered_unique< tag<by_id>,
            member<object, object_id_type, &object::id>
         >,
         ordered_unique< tag<by_account>,
            member<asset_manager_object, account_id_type, &asset_manager_object::asset_manager_account>
         >
      >
   >;
   using asset_manager_index = generic_index<asset_manager_object, asset_manager_multi_index_type>;
} } // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::asset_manager_object, (graphene::db::object),
                    (asset_manager_account)(url) )
