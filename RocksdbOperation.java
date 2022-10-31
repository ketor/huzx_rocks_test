// package org.rocksdb.benchmark;

import org.rocksdb.CompressionType;
import org.rocksdb.FlushOptions;
import org.rocksdb.Options;
import org.rocksdb.ReadOptions;
import org.rocksdb.RocksDB;
import org.rocksdb.Statistics;
import org.rocksdb.WriteOptions;

public class RocksdbOperation {
    static {
        RocksDB.loadLibrary();
    }

    public static void main(String[] args) throws Exception {

        if (args.length < 2) {
            System.out.println("\tPrint Usage: $1(totalWriteCnt) $2(rocksdbPath)");
            System.exit(-1);
        }

        Long totalCnt = Long.valueOf(args[0]);
        String dbPath = args[1];

        Options options = new Options();
        options.setCreateIfMissing(true);
        options.setStatistics(new Statistics());
        options.setCompressionType(CompressionType.NO_COMPRESSION);

        RocksDB rocksDB = RocksDB.open(options, dbPath);

        String[] keyArray = new String[totalCnt.intValue()];
        for (int i = 0; i < totalCnt.intValue(); i++) {
            keyArray[i] =  String.format("key%d", i);
        }

        long writeStartTime = System.nanoTime();
        for (int i = 0; i < totalCnt.intValue(); i++) {
            byte[] keyValueBytes = keyArray[i].getBytes();
            rocksDB.put(new WriteOptions(), keyValueBytes, keyValueBytes);
        }
        rocksDB.flush(new FlushOptions());
        long writeEndTime = System.nanoTime();

        for (int i = 0; i < totalCnt.intValue(); i++) {
            byte[] keyValueBytes = keyArray[i].getBytes();
            byte[] valueBytes = rocksDB.get(new ReadOptions(), keyValueBytes);
            if (valueBytes == null) {
                System.out.println("Invalid Key:" + i);
            }
        }

        long readEndTime = System.nanoTime();
        System.out.println("Time cost: Write:" + (writeEndTime - writeStartTime) / 1000 + "us, Read:" + (readEndTime - writeEndTime) / 1000 + "us.");
        rocksDB.close();
    }
}
