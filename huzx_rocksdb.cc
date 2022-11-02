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

int main(int argc, char* argv[]) {

    if(argc < 3) {
        printf("Usage: huzx_rocksdb [db_path] [count]\n");
        exit(0);
    }

    std::string kDBPath = argv[1];
    long totalCnt = atoi(argv[2]);

    DB* db;

    /* ColumnFamilyOptions* cfOpts = ColumnFamilyOptions().OptimizeForPointLookup(128L); */
    /* DBOptions dbOptions; */
    /* dbOptions.create_if_missing = true; */
    /* Options* options = new Options(dbOptions, *cfOpts); */
    Options options;
    options.create_if_missing = true;
    options.compression = ROCKSDB_NAMESPACE::kNoCompression;
    /* options.OptimizeForSmallDb(); */
    /* options.OptimizeForPointLookup(10240); */

    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    long temp_time = getTimeInMill();

    char* buff = (char*)malloc(sizeof(char)*100*totalCnt);
    if(buff == NULL){
        printf("malloc error, size=%ld\n", sizeof(char)*100*totalCnt);
        exit(-1);
    }

    for(long i = 0; i < totalCnt; i++){
       sprintf((char*)(buff+i*100), "key%ld", i);
    }
    print_time("MALLOC TIME", temp_time);

    long temp_time2 = getTimeInMill();
    char temp_str2[64];
    for (long i = 0; i < totalCnt; i++) {
          char * temp_ptr = (char*)(buff+i*100);
          s = db->Put(WriteOptions(), temp_ptr, temp_ptr);
          assert(s.ok());

      if(i % SAMPLING_COUNT == 0){
          sprintf(temp_str2, "%s%d%s%16ld", "WRITE [", SAMPLING_COUNT, "]", i);
          print_time(temp_str2, temp_time2);
      }
    }
    print_time("WRITE TIME", temp_time);
    db->Flush(rocksdb::FlushOptions());
    print_time("FLUSH TIME", temp_time);

    /* SetPerfLevel(rocksdb::PerfLevel::kEnableTime); */
    /* rocksdb::get_perf_context()->EnablePerLevelPerfContext(); */
    /* rocksdb::get_perf_context()->Reset(); */

    temp_time2 = getTimeInMill();
    ReadOptions localReadOpts;
    std::string value;

    for (long i = 0; i < totalCnt; i++) {
      s = db->Get(localReadOpts, (char*)(buff+i*100), &value);
      assert(s.ok());

      if(i % SAMPLING_COUNT == 0){
          sprintf(temp_str2, "%s%d%s%16ld", "READ [", SAMPLING_COUNT, "]", i);
          print_time(temp_str2, temp_time2);
      }
    }
    print_time("READ TIME", temp_time);
    /* std::cout << rocksdb::get_perf_context()->ToString() << std::endl; */

    /* generate random key */
    for(long i = 0; i < totalCnt; i++){
       sprintf((char*)(buff+i*100), "key%ld", rand()%totalCnt);
    }
    print_time("GEN RAND KEY TIME", temp_time);

    temp_time2 = getTimeInMill();
    for (long i = 0; i < totalCnt; i++) {
      s = db->Get(localReadOpts, (char*)(buff+i*100), &value);
      assert(s.ok());

      if(i % SAMPLING_COUNT == 0){
          sprintf(temp_str2, "%s%d%s%16ld", "RAND READ [", SAMPLING_COUNT, "]", i);
          print_time(temp_str2, temp_time2);
      }
    }
    print_time("RAND READ TIME", temp_time);
    /* std::cout << rocksdb::get_perf_context()->ToString() << std::endl; */

    delete db;
    return 0;
}



