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

using TERARKDB_NAMESPACE::DB;
using TERARKDB_NAMESPACE::Options;
//using TERARKDB_NAMESPACE::PinnableSlice;
using TERARKDB_NAMESPACE::ReadOptions;
using TERARKDB_NAMESPACE::Status;
using TERARKDB_NAMESPACE::WriteBatch;
using TERARKDB_NAMESPACE::WriteOptions;

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
	    printf("Usage: huzx_terarkdb [db_path] [count]\n");
	    exit(0);
    }

    std::string kDBPath = argv[1];
    long totalCnt = atoi(argv[2]);

    DB* db;
    Options options;
    // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
    //options.IncreaseParallelism();
    //options.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present
    options.create_if_missing = true;
    options.statistics = TERARKDB_NAMESPACE::CreateDBStatistics();
    options.compression = TERARKDB_NAMESPACE::kNoCompression;

    //TERARKDB_NAMESPACE::BlockBasedTableOptions tableOpts;
    //tableOpts.filter_policy.reset(TERARKDB_NAMESPACE::NewBloomFilterPolicy(16, false));

    //tableOpts.whole_key_filtering = true;
    //tableOpts.cache_index_and_filter_blocks = true;

    //options.table_factory.reset(TERARKDB_NAMESPACE::NewBlockBasedTableFactory(tableOpts));

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
    print_time("PREPARE DATA", temp_time);

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
    db->Flush(TERARKDB_NAMESPACE::FlushOptions());
    print_time("WRITE TTIME", temp_time);

    SetPerfLevel(TERARKDB_NAMESPACE::PerfLevel::kEnableTime);
    TERARKDB_NAMESPACE::get_perf_context()->EnablePerLevelPerfContext();
    TERARKDB_NAMESPACE::get_perf_context()->Reset();


    temp_time2 = getTimeInMill();
    ReadOptions localReadOpts;
    std::string value;
    // localReadOpts.auto_prefix_mode = true;
    for (long i = 0; i < totalCnt; i++) {
        s = db->Get(localReadOpts, (char*)(buff+i*100), &value);
        assert(s.ok());

	if(i % SAMPLING_COUNT == 0){
	    sprintf(temp_str2, "%s%d%s%16ld", "READ [", SAMPLING_COUNT, "]", i);
	    print_time(temp_str2, temp_time2);
	}
    }
    print_time("READ TIME", temp_time);
    std::cout << TERARKDB_NAMESPACE::get_perf_context()->ToString() << std::endl;

    delete db;
    return 0;
}


