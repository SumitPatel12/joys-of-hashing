## Hash Tables
Hash tables are constructed from two fundamental ideas:
1. Reducing application keys to a hash key - a number ranging from 0 - N-1, and
2. Mapping these keys to a smaller range: 0 - m-1, m << N.

The typical solution is to keep N large and have a second step that reduces/maps these N keys to a smaller bin-size say **m**. If you keep **m** small, you can allocate and initialize it in linear time and get any bin in it in constant time. For inserting, checks, and/or deletions for elements in the hash table, you map the application value to its hahs key and map the hash key to it's bin index.

Also, in most production cases you'll see that the application value is converted to an integer using some hash functions that give out values that would then facilitate a good distribution among the bins. An even distribution directly affects the performance of the hash.

Since m << N it's likely that we'll have collissions (i.e. two values match to the same bin). Handling collissions is also a big part of hash tables and functions.

![Hash Key Mappings](../assets/hash_keys_mapping.png)
