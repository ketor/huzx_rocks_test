ROCKSDB_BASE_PATH=/root/mengsz/facebook/rocksdb

ROCKSDB_JAR=${ROCKSDB_BASE_PATH}/java/target/rocksdbjni-7.9.0-linux64.jar
ROCKSDB_LIBPATH=${ROCKSDB_BASE_PATH}/java/target
ROCKSDB_CLASSPATH=${ROCKSDB_BASE_PATH}/java/benchmark/target/classes

TERARKDB_BASE_PATH=/root/mengsz/bytedance/terarkdb/build

all: terark rocks rocksjni
	@echo "Make all finished"

help:
	@echo "make [all|terark|rocks|rocksjni]"

terark:
	/usr/bin/c++ -W -Wextra -Wall -Wno-sign-compare -Wno-shadow -Wno-unused-parameter -Wno-unused-variable -Woverloaded-virtual -Wnon-virtual-dtor -Wno-missing-field-initializers -Wno-strict-aliasing -std=c++14 -DBUILD_BY_CMAKE -fPIC -fno-omit-frame-pointer -momit-leaf-frame-pointer -march=native -fno-builtin-memcmp -O2 -g -DNDEBUG -c huzx_terarkdb.cc -o huzx_terarkdb.o -I ${TERARKDB_BASE_PATH}/build/include
	/usr/bin/c++  -W -Wextra -Wall -Wno-sign-compare -Wno-shadow -Wno-unused-parameter -Wno-unused-variable -Woverloaded-virtual -Wnon-virtual-dtor -Wno-missing-field-initializers -Wno-strict-aliasing -std=c++14 -DBUILD_BY_CMAKE -fPIC -fno-omit-frame-pointer -momit-leaf-frame-pointer -march=native -fno-builtin-memcmp -O2 -g -DNDEBUG huzx_terarkdb.o ${TERARKDB_BASE_PATH}/CMakeFiles/db_bench.dir/tools/db_bench_tool.cc.o ${TERARKDB_BASE_PATH}/CMakeFiles/testharness.dir/util/testharness.cc.o -o huzx_terarkdb ${TERARKDB_BASE_PATH}/third-party/gtest-1.8.1/fused-src/gtest/libgtest.a ${TERARKDB_BASE_PATH}/libterarkdb.a -lpthread ${TERARKDB_BASE_PATH}/lib/libjemalloc.a ${TERARKDB_BASE_PATH}/lib/libgflags.a ${TERARKDB_BASE_PATH}/lib/libsnappy.a ${TERARKDB_BASE_PATH}/lib/libz.a ${TERARKDB_BASE_PATH}/lib/libbz2.a ${TERARKDB_BASE_PATH}/lib/liblz4.a ${TERARKDB_BASE_PATH}/lib/libzstd.a -lpthread -ldl -lgomp -laio -lrt -lpthread -ldl -lgomp -laio -lrt


rocks:
	g++  -fno-rtti   -g -W -Wextra -Wall -Wsign-compare -Wshadow -Wunused-parameter -Werror -I${ROCKSDB_BASE_PATH} -I${ROCKSDB_BASE_PATH}/include -std=c++17  -faligned-new -DHAVE_ALIGNED_NEW -DROCKSDB_PLATFORM_POSIX -DROCKSDB_LIB_IO_POSIX  -DOS_LINUX -fno-builtin-memcmp -DROCKSDB_FALLOCATE_PRESENT -DSNAPPY -DGFLAGS=1 -DZLIB -DBZIP2 -DLZ4 -DROCKSDB_MALLOC_USABLE_SIZE -DROCKSDB_PTHREAD_ADAPTIVE_MUTEX -DROCKSDB_BACKTRACE -DROCKSDB_RANGESYNC_PRESENT -DROCKSDB_SCHED_GETCPU_PRESENT -DROCKSDB_AUXV_GETAUXVAL_PRESENT -march=native   -DHAVE_SSE42  -DHAVE_PCLMUL  -DHAVE_AVX2  -DHAVE_BMI  -DHAVE_LZCNT -DHAVE_UINT128_EXTENSION  -isystem ${ROCKSDB_BASE_PATH}/third-party/gtest-1.8.1/fused-src -O2 -fno-omit-frame-pointer -momit-leaf-frame-pointer -DNDEBUG -Woverloaded-virtual -Wnon-virtual-dtor -Wno-missing-field-initializers -Wno-invalid-offsetof -c huzx_rocksdb.cc -o huzx_rocksdb.o
	g++ -L${ROCKSDB_BASE_PATH} huzx_rocksdb.o ${ROCKSDB_BASE_PATH}/tools/db_bench_tool.o ${ROCKSDB_BASE_PATH}/tools/simulated_hybrid_file_system.o ${ROCKSDB_BASE_PATH}/test_util/testutil.o  -lrocksdb  -ldl -o huzx_rocksdb -lpthread -lrt -ldl -lsnappy -lgflags -lz -lbz2 -llz4

a='$$'
b='@'
c='java -Xms32g -Xmx32g -XX:+AlwaysPreTouch -XX:+UseG1GC -XX:+ScavengeBeforeFullGC -XX:+DisableExplicitGC -XX:+HeapDumpOnOutOfMemoryError -Djava.library.path=${ROCKSDB_LIBPATH} -cp ${ROCKSDB_JAR}:${ROCKSDB_CLASSPATH}:./ RocksdbOperation '

rocksjni:
	javac -server -cp "${ROCKSDB_JAR}:${ROCKSDB_CLASSPATH}":./ RocksdbOperation.java
	echo $(c)$(a)$(b) > rocksjni.sh
	chmod a+x rocksjni.sh

clean:
	rm -f huzx_rocksdb huzx_terarkdb rocksjni.sh *.class *.o


