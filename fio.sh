#fio test script

if [ $# -ne 1 ]
then
	echo "fio.sh [your disk, for instance: sdd]"
	exit
fi

TEST_TIME=30
TEST_DEVICE=/dev/$1

fio -direct=1 -iodepth=128 -rw=randwrite -ioengine=libaio -bs=4k -size=32G -numjobs=1 -runtime=${TEST_TIME} -group_reporting -filename=${TEST_DEVICE} -name=Rand_Write_Testing
fio -direct=1 -iodepth=128 -rw=randread -ioengine=libaio -bs=4k -size=32G -numjobs=1 -runtime=${TEST_TIME} -group_reporting -filename=${TEST_DEVICE} -name=Rand_Read_Testing
fio -direct=1 -iodepth=64 -rw=write -ioengine=libaio -bs=1024k -size=32G -numjobs=1 -runtime=${TEST_TIME} -group_reporting -filename=${TEST_DEVICE} -name=Write_PPS_Testing
fio -direct=1 -iodepth=64 -rw=read -ioengine=libaio -bs=1024k -size=32G -numjobs=1 -runtime=${TEST_TIME} -group_reporting -filename=${TEST_DEVICE} -name=Read_PPS_Testing
#fio -direct=1 -iodepth=1 -rw=randwrite -ioengine=libaio -bs=4k -size=32G -numjobs=1 -group_reporting -filename=${TEST_DEVICE} -name=Rand_Write_Latency_Testing
#fio -direct=1 -iodepth=1 -rw=randread -ioengine=libaio -bs=4k -size=32G -numjobs=1 -group_reporting -filename=${TEST_DEVICE} -name=Rand_Read_Latency_Testing

