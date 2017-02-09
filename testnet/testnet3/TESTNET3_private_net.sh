#!/usr/bin/env bash

json_file="/Users/AChien/Projects/artwook/repo/servers/bootstrap/./output//TESTNET3_genesis.json"

./witness_node \
  --genesis-json="$json_file" \
  --api-access api-access.json \
  --p2p-endpoint=0.0.0.0:1776 \
  --rpc-endpoint=127.0.0.1:8091 \
  -d $(pwd)/data_private_net_TESTNET3 \
  --replay-blockchain \
  --enable-stale-production \
  --witness-id '"1.6.1"' \
  --private-key '["AWK52PYmS9aoKJAjzXuLXgk4DyZGGuYNQRq8zaWj4sRdnKhsReyBd","5KDhcxqn6DoZ4FvDGWaBS6wJx2jYwKPM1oy7BZLnXxga3groqdV"]'  \
  --witness-id '"1.6.2"' \
  --private-key '["AWK5ZECi8uvNmRgL8bLuw3UnQ1VhaoMxnfRsJf5m9iJS5dXMgyNrz","5J9pBWFZK626B8n9XfjVKWJimwAP82ohhdvx5UZf7hryycnVN3v"]'  \
  --witness-id '"1.6.3"' \
  --private-key '["AWK5xckCim7RpKDVNLhTNHdn98ws98uQM7AEH8UEarsCPPXRo1eFG","5Jhe9iqF1XXxsjphaxuApA3YAKnViBhroqoXukMrNt9GRsAr2cU"]'  \
  --witness-id '"1.6.4"' \
  --private-key '["AWK8d15ykhFjxuSTBAbCWpRYN6hvcRwdMeQwwKWqcJXpMTxCi9vWD","5J1FXG5AZCumTTcDhHCMBcHz5xoaESQ2hYAzS3knW6JWADkVXdB"]'  \
  --witness-id '"1.6.5"' \
  --private-key '["AWK64Te2W7jKA65NtzYvVfEhtPo5cokg13SyeY7NnFjLR4vN73Dvh","5J7qCYTfGwV7YnX4AqBCoQWpSgchZLHYoQjsaHxx2YtVNcYtU78"]'  \
  --witness-id '"1.6.6"' \
  --private-key '["AWK64eZiJVNZqYiVjGUtvzQwHmLq9c73VxA5xyDEDSHkwVW5cGM3Y","5KFaG6ZG1GtFDg78nNLEgQT12BukmjdCaxGgadUrZT1hr5a8s8w"]'  \
  --witness-id '"1.6.7"' \
  --private-key '["AWK5ZbcAfKDAhudJeGp6b9wGrE9SZdMAHdPgpptZ7G1Moj7QtDrX1","5JTyJRzGpX168gsJiZhWoeyxcFxxmoJEUVkp63vpdqqJTHTSyEa"]'  \
  --witness-id '"1.6.8"' \
  --private-key '["AWK5PBMAnX8zBNnqtZrXmupnmAbc5gFAgLy9yBZLpx2eCpDdU7kwK","5JvnUe4fHYA5tbb2aL3qsBxL4yY6RFVww2wmb5XHTin2v5ner8h"]'  \
  --witness-id '"1.6.9"' \
  --private-key '["AWK5L6ChkHCiaG6AwfBsL3hDRbaLwh95VH6rrHzAjkAVEgWCAwnCk","5J9GbLmVYfZQhocmvRZdoRnC1EJE2gNQt4gru3kp5GaUfEYJFJz"]'  \
  --witness-id '"1.6.10"' \
  --private-key '["AWK7nPQiaPYxvTP3p1t5NtqbHpS1Ziskon8uqxLJiAue98o975kNF","5Js3nFmz492Urgavyr5oRagGSvZNuxtvd9io1UFvqYMXymSbo9X"]'  \
  --witness-id '"1.6.11"' \
  --private-key '["AWK7axa69yXuavKPvkZqgAAWrByk8f2PZDjfziWhJdy9H5mLbac1w","5JuVgVhkmamM5o56kerBMbnbb2RHLgvxWjpTgk7YvLQzE6SPhZZ"]'  \
