#include <cstdio>
#include "FileSystem.h"
#include <cstdio>
using namespace native::libhdfs;

int main()
{

	FileSystem fs("127.0.0.1", 9000);
	//fs.rename("/xx1", "/xx2");
	//fs.mkdir("/asd123");
	char s[1000];
	puts("测试判断文件是否存在,请输入文件名");
	scanf("%s", s);
	if (fs.exists(s))
		printf("文件%s 存在\n", s);
	else printf("文件%s 不存在\n", s);

	puts("------------------------");
	puts("测试mkdir,请输入绝对路径");
	while (1)
	{
		scanf("%s", s);
		if (s[0] != '/')
			puts("输入数据不合法");
		else if (fs.exists(s))
			puts("路径已经存在");
		else {
			fs.mkdir(s);
			break;
		}
	}

	puts("------------------------");
	puts("测试remove,请输入绝对路径");
	while (1)
	{
		scanf("%s", s);
		if (s[0] != '/')
			puts("输入数据不合法");
		else if (!fs.exists(s))
			puts("路径不存在");
		else {
			fs.remove(s);
			break;
		}
	}

	puts("------------------------");
	puts("测试copyToLocal,请输入src和dest，其中src是hdfs中的绝对路径，dest为本地路径");
	char dest[1000];
	while (1)
	{
		scanf("%s", s);
		if (s[0] != '/')
			puts("输入数据不合法");
		else if (!fs.exists(s))
			puts("路径不存在");
		else {
			scanf("%s", dest);
			fs.copyToLocalFile(s, dest);
			break;
		}
	}
	//bool xxx = fs.exists("/xx2");
	//if (xxx) printf("exist");
	//else printf("not exist");

	//fs.copyToLocalFile("/bb", "xx");
	//fs.copyToLocalFile("/user/hadoop/input.txt", "xx1");
	//fs.remove("/asd123", true);
	//client.read("/bb", NULL, 0);
	//client.read("/user/hadoop/input.txt", NULL, 0);

	return 0;
}
