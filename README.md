# fast chunk allocator for STL containers(map/list/queue/...)

- 该分配器只适用于每个节size都固定的容器(map/list/queue/...)
- 该分配器的设计就是为了避免频繁的内存分配产生的巨大开销
- 该分配器在centos上使用过，其他系统上可以直接使用（只需修改一下#ifdef _STL_xxx_H 这几行)。 
- 该分配器设计要点是2级缓存，这也是其高效的根本原因。
   * 一级缓存为局部内存池不加锁零消耗速度超快
   * 二级缓存为全局内存池须加锁调用频度低一些

```
using stm::map;
using std::string;

map<int, int> rds;
for(int i = 0; i < 10000; i ++)
{
  rds.insert(std::make_pair(i,i));
}
```
