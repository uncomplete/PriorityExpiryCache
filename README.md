# indexed_priority_queue
An implementation of an indexed priority queue data structure (for use in a priority-expiry-cache)

First we implement a custom Indexed Minimum Priority Queue data structure - index_min_pq - which consists of a vector holding the values and 2 unordered_maps which are the inverse of each other (keys[_keys[i]] = i && _keys[keys[s]] = s).
The indexed priority queue implements the usual PQ operations - pop(), push() - in logarithmic time.
push() operations take both a value to push onto the PQ and a key which the user can use to delete non-minimum elements in logarithmic time using the remove(k) operation.
Reference: https://algs4.cs.princeton.edu/24pq/ 

In the Cache we use three data structures - an unordered_map, an (ordered) map, and an indexed_priority_queue.

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


