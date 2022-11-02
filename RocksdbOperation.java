// package org.rocksdb.benchmark;

import org.rocksdb.CompressionType;
import org.rocksdb.FlushOptions;
import org.rocksdb.Options;
import org.rocksdb.ReadOptions;
import org.rocksdb.RocksDB;
import org.rocksdb.Statistics;
import org.rocksdb.WriteOptions;
import java.util.Random;

public class RocksdbOperation {
    static {
        RocksDB.loadLibrary();
    }

    static long get_time_us(){
        return System.nanoTime() / 1000000;
    }

    static long print_time(String msg, long temp_time){
        long local_temp_time = get_time_us();
        if(temp_time == 0) {
            temp_time = local_temp_time;
        }
        // System.out.println("MSG=[" + msg + "], time = [" + (local_temp_time - temp_time) + "]");
        System.out.println(String.format("%s time=[%16d]", msg,  local_temp_time - temp_time));

        return local_temp_time;
    }

    public static void main(String[] args) throws Exception {

        if (args.length < 2) {
            System.out.println("\tPrint Usage: [db_path] [count]");
            System.exit(-1);
        }

        System.out.println(args[0]);
        System.out.println(args[1]);

        String dbPath = args[0];
        Long totalCnt = Long.valueOf(args[1]);

        long temp_time = get_time_us();

        Options options = new Options();
        options.setCreateIfMissing(true);
        options.setStatistics(new Statistics());
        options.setCompressionType(CompressionType.NO_COMPRESSION);

        temp_time = print_time("OPEN RocksDB", temp_time);

        RocksDB rocksDB = RocksDB.open(options, dbPath);

        temp_time = print_time("OPEN RocksDB OK", temp_time);

        long temp_time2 = get_time_us();

        // String[] keyArray = new String[totalCnt.intValue()];
        // for (int i = 0; i < totalCnt.intValue(); i++) {
        //     keyArray[i] =  String.format("key%d", i);

        //     if (i % 100000 == 0) {
        //         temp_time2 = print_time(String.format("MALLOC[%16d]", i), temp_time2);
        //     }
        // }
        // temp_time = print_time("MALLOC TIME", temp_time);

        byte[][] keyArray = new byte[totalCnt.intValue()][];
        for (int i = 0; i < totalCnt.intValue(); i++) {
            keyArray[i] =  String.format("key%d", i).getBytes();

            if (i % 100000 == 0) {
                temp_time2 = print_time(String.format("MALLOC[%16d]", i), temp_time2);
            }
        }
        temp_time = print_time("MALLOC TIME", temp_time);

        temp_time2 = get_time_us();

        for (int i = 0; i < totalCnt.intValue(); i++) {
            // byte[] keyValueBytes = keyArray[i].getBytes();
            byte[] keyValueBytes = keyArray[i];
            rocksDB.put(new WriteOptions(), keyValueBytes, keyValueBytes);

            if (i % 100000 == 0) {
                temp_time2 = print_time(String.format("WRITE[%16d]", i), temp_time2);
            }
        }
        temp_time = print_time("WRITE TIME", temp_time);
        rocksDB.flush(new FlushOptions());
        temp_time = print_time("FLUSH TIME", temp_time);

        temp_time2 = get_time_us();

        for (int i = 0; i < totalCnt.intValue(); i++) {
            // byte[] keyValueBytes = keyArray[i].getBytes();
            byte[] keyValueBytes = keyArray[i];
            byte[] valueBytes = rocksDB.get(new ReadOptions(), keyValueBytes);
            if (valueBytes == null) {
                System.out.println("Invalid Key:" + i);
            }

            if (i % 100000 == 0) {
                temp_time2 = print_time(String.format("READ[%16d]", i), temp_time2);
            }
        }
        temp_time = print_time("READ TIME", temp_time);

        // gen random key
        Random rand = new Random();
        for (int i = 0; i < totalCnt.intValue(); i++) {
            keyArray[i] =  String.format("key%d", rand.nextInt(totalCnt.intValue())).getBytes();

            if (i % 100000 == 0) {
                temp_time2 = print_time(String.format("MALLOC[%16d]", i), temp_time2);
            }
        }
        temp_time = print_time("GEN RAND KEY TIME", temp_time);

        temp_time2 = get_time_us();

        for (int i = 0; i < totalCnt.intValue(); i++) {
            // byte[] keyValueBytes = keyArray[i].getBytes();
            byte[] keyValueBytes = keyArray[i];
            byte[] valueBytes = rocksDB.get(new ReadOptions(), keyValueBytes);
            if (valueBytes == null) {
                System.out.println("Invalid Key:" + i);
            }

            if (i % 100000 == 0) {
                temp_time2 = print_time(String.format("RAND READ[%16d]", i), temp_time2);
            }
        }
        temp_time = print_time("RAND READ TIME", temp_time);

        rocksDB.close();
    }
}
