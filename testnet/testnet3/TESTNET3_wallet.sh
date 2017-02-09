#!/usr/bin/env bash

./cli_wallet \
  --chain-id="33a5ccbfcb93dc3283c96734706823690dbd0b07301792aa3a2d9cfd0269e9f8" \
  --wallet-file="./wallet_data/wallet_private_TESTNET3.json" \
  --server-rpc-endpoint="ws://127.0.0.1:8091" -u user -p pass --rpc-endpoint="127.0.0.1:8094" --rpc-http-endpoint="127.0.0.1:8093"

