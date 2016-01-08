/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Any modified source or binaries are used only with the BitShares network.
 *
 * 2. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "BlockChain.hpp"

#include <graphene/app/application.hpp>
#include <graphene/account_history/account_history_plugin.hpp>
#include <graphene/market_history/market_history_plugin.hpp>

#include <fc/thread/thread.hpp>

#include <boost/program_options.hpp>

#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

BlockChain::BlockChain()
   : chainThread(new fc::thread("chainThread")),
     grapheneApp(new graphene::app::application),
     webUsername(QStringLiteral("webui")),
     webPassword(QString::fromStdString(fc::sha256::hash(fc::ecc::private_key::generate())))
{}

BlockChain::~BlockChain() {
   startFuture.cancel_and_wait(__FUNCTION__);
   chainThread->async([this] {
      grapheneApp->shutdown_plugins();
      delete grapheneApp;
   }).wait();
   delete chainThread;
}

void BlockChain::start()
{
   startFuture = chainThread->async([this] {
      try {
         QSettings settings;
         rpcEndpoint = settings.value( "rpc-endpoint", "127.0.0.1:8091" ).value<QString>();
         grapheneApp->register_plugin<graphene::account_history::account_history_plugin>();
         grapheneApp->register_plugin<graphene::market_history::market_history_plugin>();

         QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
         QDir(dataDir).mkpath(".");
         idump((dataDir.toStdString()));
         boost::program_options::variables_map map;
         map.insert({"rpc-endpoint",boost::program_options::variable_value(rpcEndpoint.toStdString(), false)});
         grapheneApp->initialize(dataDir.toStdString(), map);
         grapheneApp->initialize_plugins(map);
         grapheneApp->startup();
         grapheneApp->startup_plugins();

         graphene::app::api_access_info webPermissions;
         auto passHash = fc::sha256::hash(webPassword.toStdString());
         webPermissions.password_hash_b64 = fc::base64_encode(passHash.data(), passHash.data_size());
         webPermissions.password_salt_b64 = fc::base64_encode("");
         webPermissions.allowed_apis = {"database_api", "network_broadcast_api", "network_node_api", "history_api"};
         grapheneApp->set_api_access_info(webUsername.toStdString(), std::move(webPermissions) );
      } catch (const fc::exception& e) {
         elog("Crap went wrong: ${e}", ("e", e.to_detail_string()));
      }
      QMetaObject::invokeMethod(this, "started");
   });
}
