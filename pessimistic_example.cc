// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include <cstdio>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include "rocksdb/perf_context.h"
#include "rocksdb/db.h"
#include "rocksdb/table.h"
#include "rocksdb/cache.h"
#include "rocksdb/slice.h"
#include "rocksdb/memtablerep.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/options.h"
#include "rocksdb/slice_transform.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"

#define SAMPLING_COUNT 100000

using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;
using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::Transaction;
using ROCKSDB_NAMESPACE::TransactionDB;
using ROCKSDB_NAMESPACE::TransactionDBOptions;
using ROCKSDB_NAMESPACE::TransactionOptions;

static long getTimeInMill() {
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    return start_time.tv_sec * 1000 + start_time.tv_usec / 1000.0;
}

void print_time(const char* msg, long& temp_time) {
    long local_temp_time = getTimeInMill();
    printf("%s time=[%ld]ms\n", msg, local_temp_time - temp_time);
    temp_time = local_temp_time;
}

void PrintStatus(const char* msg, Status s) {
    printf("%s Status=[%d]\n", msg, s.code());
}

int main(int argc, char* argv[]) {

    if(argc < 2) {
        printf("Usage: huzx_rocksdb [db_path]\n");
        exit(0);
    }

    std::string kDBPath = argv[1];

    long temp_time = getTimeInMill();

    TransactionDB* txn_db;
    TransactionDBOptions txndb_opts;
    TransactionOptions txn_opts;

    Options opts;
    std::string value;

    opts.create_if_missing = true;
    opts.compression = ROCKSDB_NAMESPACE::kNoCompression;

    Status s = TransactionDB::Open(opts, txndb_opts, kDBPath, &txn_db);
    PrintStatus("TransactionDB::Open", s);

    // initial value
    s = txn_db->Put(WriteOptions(), "key1", "value0");

    Transaction *txn1  = txn_db->BeginTransaction(WriteOptions());
    assert(txn1 != nullptr);
    Transaction *txn2  = txn_db->BeginTransaction(WriteOptions());
    assert(txn1 != nullptr);

    PrintStatus("txn1->GetForUpdate", txn1->GetForUpdate(ReadOptions(), "key1", &value));
    printf("value = %s\n", value.c_str());
    PrintStatus("txn1->Put", txn1->Put("key1", "value1"));
    PrintStatus("txn1->Get", txn1->Get(ReadOptions(), "key1", &value));
    printf("value = %s\n", value.c_str());

    PrintStatus("txn2->Get", txn2->Get(ReadOptions(), "key1", &value));
    printf("value = %s\n", value.c_str());
    PrintStatus("txn2->Put", txn2->Put("key1", "value2"));

    PrintStatus("txn1->Commit", txn1->Commit());
    PrintStatus("txn2->Commit", txn2->Commit());

    s = txn_db->Get(ReadOptions(), "key1", &value);
    printf("value = %s\n", value.c_str());

    delete txn_db;

    return 0;
}



