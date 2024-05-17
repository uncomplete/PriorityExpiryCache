# Priority-Expiry Cache

This was originally an interview project from a company that I applied to.

A general cache is often a fixed-size container for a set of key-value pairs.  
Pairs, or items, can be added to the cache, up to it's fixed size n, and the cache can be queried for values using keys.  
Once the n+1 item is added an existing item must be evicted in order to make room for the n+1 item.  
In an LRU cache the eviction policy is such that the item to be evicted is chosen as the item that was least recently added or queried.  
All cache's are very time-sensitive, expecting at most ln(x) worst-case performance on all operations.  
An LRU cache can be implemented using a hash-map and a doubly-linked-list, where the list stores the items themselves and the hash-map stores keys mapped to list-references.  

A Priority Expiry Cache (PEC) is also a fixed-size container for a set of key-value pairs however it differs in its eviction and retreival policies.  
Each item in a PEC is given a Priority and an Expiry which, all else being equal, should be used to choose which items to evict first.
Priority is simply a strict weak ordering placed on the items by the user, and is only used in eviction.  
The PEC maintains a time which is used to judge the expiry of an item (and may be maintained externally or internally).  
If an item is expired it should not be returned by the Cache if it's Key is queried, instead the PEC should raise an ItemExpiredException.  

The rules for evicting an item from a PEC can be summed as follows:
- Any item which is expired must be removed first.
- If no items are expired, the item with the lowest Priority must be evicted.
- If more than one item with the lowest Priority exists, the item which is LRU must be evicted.

We also add the ability to remove an arbitrary item, specified by its Key, from the Cache.

The API for a Priority Expiry Cache over items of type Item(K,V) where K is the type of the key, and V is the type of the value, is given below:
- SetMaxItems(size_t n)
- IncrementTime(Time t)
- SetItem(V value, P priority, T expiry, K? key) => K key
- GetItem(K key) => V value
- DeleteItem(K key)

Adding only a Priority (disregarding the expiry) to each item in an LRU Cache, requires only the replacement of the LinkedList with an OrderedMap<Priority, LinkedList> (along with some bookkeeping) in its implementation.  
However, the requirements imposed by the Expiry on each item create the need for an additional data structure.  
A heap, ordered on each item's Expiry, and storing each item's Key, may do the job.  
A heap allows efficient (ln(n)) max-removal and insertion however, in the case that no item is expired, and we must remove an item based on priority, a simple heap breaks down, affording only linear time removal of arbitrary elements.  

## Background: Priority Queues

In general, priority queues are characterized by the 'remove the maximum' and 'insert' operations.  
A first-order implementation of a Priority Queue may either maintain a fully-sorted or fully-unsorted collection of elements.  
However both such implementations have the property that either the 'insert' or the 'remove the maximum' operation take linear time in the worst case.  
In order to build a data structure which supports fast 'remove' and fast 'insert' we use partial-sorting, or a heap.

The C++ std::priority_queue container supports O(1) lookup of the min/max element and O(ln) insertion and extraction.  
Elements are stored in what is known as a binary heap, which guarantees that each key is larger than (or equal to) the keys at 2 other specific locations.  
A binary heap may be visualized as a complete binary tree which is heap-ordered, meaning that each node is guaranteed to be larger than (or equal to) the keys in that node's children.

The underlying container used to store a binary heap must support random-access iteration (array or vector, not a linked-list).  
We represent a heap of size n in array pq[] of length n + 1, with pq[0] unused and the heap in pq[1] through pq[n].  
Look-up of the max/min element is always a O(1) random access to position pq[1].  
Heap operations (insertion, removal of max/min) work by first making simple modifications that could violate the heap conditions then traveling through the heap, modifying as required to ensure that the heap condition is satisfied everywhere.  
We refer to this process as reheapifying, or restoring heap order.

A modification to a heap will result in one of two violations (difference is only one of perspective, or which element was added last):
- a node's key becomes larger than that node's parent or
- a node's key becomes smaller than one or both of that node's children's keys.

In a heap-based priority queue reheapifying is done in either a bottom-up (swim) or top-down (sink) manner:
- A swim operation is performed on a node with key larger than it's parent by exchanging the node with its parent, and is repeated until that node is smaller than its parent.
- A sink operation is performed on a node with key smaller than one or both of its children by exchanging that node with the larger of its two children, and is repeated until that node is larger than both its children.

These sink and swim operations provide the basis for an efficient implementation of the priority queue API.  
Insert is done by adding the new key to the end of the array and swimming it up to restore the heap condition.
Removing the maximum is done by removing pq[1], replacing it with pq[n], decrementing the size n, and sinking pq[1] down to restore the heap condition.  
In an n-item priority queue, the heap algorithms require no more than 1 + lg n compares for insert and no more than 2 lg n compares for remove the maximum.

Important Definitions:
- Binary Tree
- Heap Ordering
- Complete Tree

## Indexed Priority Queues

While priority queues afford efficient removal of the min/max element, the removal of any arbitrary element is still linear time in the worst case.
In order to achieve efficient removal of arbitrary elements we add 2 additional data structures, both HashMaps.  
One of these HashMaps maps Keys to indices into the heap-ordered array, and the other (its inverse) will map heap-ordered array indices to Keys.  
Both of these are required (as opposed to a single HashMap<Key, Index>) in order to support the efficient execution of both index-based (max-removal) and Key-based removal.  
Without the inverse, we would be required to search one linearly on either removal operation.  
The maintenance of these two data structures requires some careful bookkeeping but the constant-time speed of HashMap lookups ensures that it is efficient.  

`indexed_pq.hpp` is an implementation of an indexed priority queue data structure which affords ln(x) removal of arbitrary elements.

Reference: https://algs4.cs.princeton.edu/24pq/ 

## Complexity

(Average Case***) Complexity of each call - Get, Set, Evict - are as follows:

Get(key):
  The find() on an unordered_map is constant.
  The find() on a map is logarithmic.
  The splice (moving a node in the linked_list) is constant time.
  
  A Set on the cache is logarithmic over the number of unique priorities.

Set(key):
  A Set consists of a map lookup and insertion, an unordered_map insertion and a heap push.
  A map lookup/insertion is logarithmic in the number of unique priorities.
  An unordered_map insertion is constant time.
  A heap push() is up to logarithmic in the number of keys in the cache.
  
  A Set() is logarithmic in the number of unique priorities and/or keys.

EvictItem():
  Given the indexed priority queue the complexity of EvictItem does not depend on whether or not the Item to be evicted is expired or not (at the top of the heap).
  If there is an expired item present in the cache this operation consists of a heap pop(), an unordered_map erasure, and a map lookup.
  The heap pop() is logarithmic in the number of keys, an unordered_map erasure is constant, and a map lookup is logarithmic in the number of unique priorities.
  In this case the operation is logarithmic in the number of unique keys and/or priorities in the cache.
  If the item at the top of the heap is not expired we find the minimum priority in the ordered_map which is constant time and pop the LRU item which is also constant time.
  We then do a remove(k) from the IndexedPQ which is logarithmic.
  The unordered_map erasure is constant time on average, linear in the worst case.
  
  EvictItem() is logarithmic on average - whether or not the Item is expired does not affect the overall complexity of the call.

***unordered_map/vector operations are in fact linear in the worst case i.e. in the case that the items need to be rehashed/reallocated (resp.).
This however can be accounted for depending on the use case using the max_load_factor() and reserve() functions, or by using a custom hash function.

For reference - we performed a simple benchmark comparing the use of an indexed_priority_queue to the baseline use of an STL priority_queue (as commented below - custom_priority_queue).

We vary the number of cache entries, averaging over 10 runs.
Evict() is called once per call to SetMaxItems().
i.e.
c->SetMaxItems(MAXITEMS);
for(size_t j=0; j<cn; j++)
    c->Set(gen_random(12), 6, iarr[j%26], zarr[j%26]);
for(size_t b=cn; b>0; b--) {
    c->SetMaxItems(b);
}

Results:
left is cn
right is time (microseconds)

Baseline:
100  3282 
200  11517
300  17086
400  25790
500  39292
600  57531
700  74973
800  97650
900  122995
1000 152100

IndexedPQ:
100  1213
200  2580
300  3892
400  3594
500  4076
600  4098
700  4289
800  4866
900  4948
1000 6507


