timeout_cache
=============
make
./test-release 1000000
1:时间总是总第一个数据进入开始的时间作为起始点 firstTime。
之后数据的timeou :now-firstTime+timout.
这样无需更新每项的时间。


2:cache 的超时处理使用一个链表实现的优先级队列。这个时候的插入耗时会比较久。最好是有链表的快速删除，插入和二分的快速查找。
平衡二叉树是个不错的选择。高效的二分。快速插入，快速删除。
跳跃表也不错。可以由不错的二分性能。
还有种方案是对时间进行time->array<item> 进行map,
对应时间到了就删除内部对应的项找出对应时间
这样做hash以后数据插入会快很多。但相对数据在删除的时候会慢点。


3:cache 的查询时使用hash表对队列的节点指针进行缓存。以达到外部的告诉查询，删除。


链表+hash（代码实现了）
1：对timeout 使用插入排序。
2：key->listnode 外部查询，删除。

跳跃表+hash （之后实现）
1：使用timeout建立跳跃表。
2：key-> treenode 外部查询

树+hash （之后实现）
1：使用timeout建立二叉树。
2：key-> treenode 外部查询

双hash（代码实现了）
1：key -> item 外部查询 删除 用。
2：timeout->item 内部超时使用。使用就近原则的hash用于分摊 优先队列的长度。