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
#include <graphene/chain/asset_manager_evaluator.hpp>
#include <graphene/chain/asset_manager_object.hpp>
#include <graphene/chain/database.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/protocol/fee_schedule.hpp>
#include <graphene/chain/transaction_evaluation_state.hpp>
#include <graphene/chain/market_object.hpp>

#include <fc/smart_ref_impl.hpp>

namespace graphene { namespace chain {

void_result asset_manager_create_evaluator::do_evaluate( const asset_manager_create_operation& op )
{ try {
   FC_ASSERT(db().get(op.asset_manager_account).is_lifetime_member());
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

object_id_type asset_manager_create_evaluator::do_apply( const asset_manager_create_operation& op )
{ try {
   const auto& new_del_object = db().create<asset_manager_object>( [&]( asset_manager_object& obj ){
         obj.asset_manager_account   = op.asset_manager_account;
         obj.url                = op.url;
   });
   return new_del_object.id;
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result asset_manager_update_evaluator::do_evaluate( const asset_manager_update_operation& op )
{ try {
   FC_ASSERT(db().get(op.asset_manager).asset_manager_account == op.asset_manager_account);
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result asset_manager_update_evaluator::do_apply( const asset_manager_update_operation& op )
{ try {
   database& _db = db();
   _db.modify(
      _db.get(op.asset_manager),
      [&]( asset_manager_object& amo )
      {
         if( op.new_url.valid() )
            amo.url = *op.new_url;
      });
   return void_result();
} FC_CAPTURE_AND_RETHROW( (op) ) }

void_result asset_manager_force_buyback_asset_evaluator::do_evaluate(const asset_manager_force_buyback_asset_operation& o)
{ try {
    const database& d = db();
    
    const asset_object&   asset_type      = o.asset_id(d);
    GRAPHENE_ASSERT(
                    asset_type.can_force_buyback(),
                    asset_manager_force_buyback_asset_not_permitted,
                    "force buyback not permitted for asset ${asset}",
                    ("asset", o.asset_id)
                    );
    
    FC_ASSERT( asset_type.issuer == o.asset_manager_account );
    
    const auto& idx = d.get_index_type<asset_manager_index>().indices().get<by_account>();
    auto itr = idx.find(o.asset_manager_account);
    // The asset issuer must be an asset manager.
    FC_ASSERT( itr != idx.end() );
    
    // TODO:
   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

void_result asset_manager_force_buyback_asset_evaluator::do_apply(const asset_manager_force_buyback_asset_operation& o)
{ try {
    database& d = db();
    const auto& limit_order_idx = d.get_index_type<limit_order_index>().indices().get<by_id>();
    
    // TODO: it should be possible to simply check the NEXT/PREV iterator after new_order_object to
    // determine whether or not this order has "changed the book" in a way that requires us to
    // check orders. For now I just lookup the lower bound and check for equality... this is log(n) vs
    // constant time check. Potential optimization.
    
    auto limit_itr = limit_order_idx.begin();
    auto limit_end = limit_order_idx.end();
    
    bool finished = false;
    while( !finished && limit_itr != limit_end )
    {
        auto old_limit_itr = limit_itr;
        ++limit_itr;
        
        if( old_limit_itr->sell_price.base.asset_id == o.asset_id
           || old_limit_itr->sell_price.quote.asset_id == o.asset_id )
        {
            d.cancel_order(*old_limit_itr);
        }
    }
    
    // TODO: Do we need to cancel force_settlement_object?

   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

} } // graphene::chain
