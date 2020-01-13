# chunk_allocator
chunk allocator for stl

在使用std::map/list/queue/...时，其频繁的内存分配将产生巨大的开销。
这类容器的显著特点是，他们每次都分配固定大小的内存块。
我针对这一特征，设计了该内存池。
分2级缓存，一级缓存不加锁，二级缓存为全局内存池须加锁。


using stm::map;
using std::string;

map<int, int> rds;
for(int i = 0; i < 10000; i ++)
{
  rds.insert(std::make_pair(i,i));
}
