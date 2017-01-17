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
    
    FC_ASSERT( o.buyback_price.base.asset_id == asset_id_type() ); // CORE/ASSET
    FC_ASSERT( o.buyback_price.quote.asset_id == o.asset_id );
    
    const auto& idx = d.get_index_type<asset_manager_index>().indices().get<by_account>();
    auto itr = idx.find(o.asset_manager_account);
    // The asset issuer must be an asset manager.
    FC_ASSERT( itr != idx.end() );
    
    // Remove the balance from the asset holders.
    const auto& bal_idx = d.get_index_type< account_balance_index >().indices().get< by_asset_balance >();
    auto range = bal_idx.equal_range( boost::make_tuple( o.asset_id ) );
    
    auto total_required = asset( 0, o.asset_id );
    share_type balance_sum = 0;
    
    auto account_balance_itr = range.first;
    auto account_balance_end = range.second;
    
    while (account_balance_itr != account_balance_end ) {
        auto old_account_balance_itr = account_balance_itr;
        ++ account_balance_itr;
        
        auto giveback_asset = old_account_balance_itr->get_balance() * o.buyback_price;
        total_required += giveback_asset;
        balance_sum += old_account_balance_itr->balance;
    }
    
    // the balance_sum should be the same with current_supply
    FC_ASSERT( balance_sum == asset_type.dynamic_data(d).current_supply );
    
    // using the statistic from balance rather than current_supply.
    // although the static balance summary should be equal to current supply.
    _required_asset = total_required;
    // _required_asset = asset( asset_type.dynamic_data(d).current_supply, asset_type.get_id() ) * o.buyback_price;
    
    FC_ASSERT( d.get_balance( o.asset_manager_account, _required_asset.asset_id ).amount >= _required_asset.amount,
              
              "", ("total_required",_required_asset)("balance",d.get_balance(o.asset_manager_account, _required_asset.asset_id)) );
    
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
    
    // Remove the balance from the asset holders.
    const auto& bal_idx = d.get_index_type< account_balance_index >().indices().get< by_asset_balance >();
    auto range = bal_idx.equal_range( boost::make_tuple( o.asset_id ) );
    
    // sub the asset from the asset manager
    d.adjust_balance( o.asset_manager_account, -_required_asset );
    // and give the asset to the account balance owners
    
    auto account_balance_itr = range.first;
    auto account_balance_end = range.second;
    
    while (account_balance_itr != account_balance_end ) {
        auto old_account_balance_itr = account_balance_itr;
        ++ account_balance_itr;
        
        auto giveback_asset = old_account_balance_itr->get_balance() * o.buyback_price;
        
        d.adjust_balance(old_account_balance_itr->owner, giveback_asset);
        
        d.remove(*old_account_balance_itr);
    }
    
    const asset_object&   asset_type      = o.asset_id(d);
    
    const auto& dynamic_data = asset_type.dynamic_data(d);
    
    // update the asset dynamic data such as current_supply etc.
    d.modify( dynamic_data, [&](asset_dynamic_data_object& obj){
        obj.current_supply = 0;
        // TODO:
        // confidential_supply; ///< total asset held in confidential balances
        // accumulated_fees; ///< fees accumulate to be paid out over time
        // fee_pool
    });
    
   return void_result();
} FC_CAPTURE_AND_RETHROW( (o) ) }

} } // graphene::chain
