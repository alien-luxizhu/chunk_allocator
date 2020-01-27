# fast chunk allocator
## fast chunk allocator for STL containers(map/list/queue/...).


- 有一类容器(map/list/queue/...)每次都分配固定大小的内存块，该allocator的设计即是为了避免这类容器频繁的内存分配产生的巨大开销。
- 该分配器设计要点是2级缓存，一级缓存为局部内存池不加锁零消耗速度超快，二级缓存为全局内存池须加锁调用频度低一些。

```
using stm::map;
using std::string;

map<int, int> rds;
for(int i = 0; i < 10000; i ++)
{
  rds.insert(std::make_pair(i,i));
}
```
