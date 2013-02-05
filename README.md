timeout_cache
=============
时间总是总第一个数据进入开始的时间作为起始点 first-time。
之后数据的timeou :now-first-time+timout.
这样无需更新每项的时间。
cache 的超时处理使用一个链表实现的优先级队列。
还有种方案是对时间进行time->array<item> 进行map,
对应时间到了就删除内部对应的项找出对应时间


cache 的查询时使用hash表对队列的节点指针进行缓存。以达到外部的告诉查询，删除。