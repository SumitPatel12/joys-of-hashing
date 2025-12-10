## Intrusive Linked List
Intrusive linked lists as the name suggests are intrusive structures. A normal list would own the data, but not an intrusive linked list.
An intrusive linked list just has a structure that'd let you maintain the links but has no direct control over the data that it links. So, you could do cool stuff like linking one type of struct with another but, I don't konw if that's a good thing (:shrug:)

The structure of the node of an intrusive linked list is directly embedded into the struct of the object you want to make a linked list of. Something like the code snippet below. The hlist_node is embedded into the struct and not the other way around, and that node will keep track of linkage. Additionally, if the same node is a part of multiple lists, then more than one `hlist_nodes` can be embedded in it's struct, and it will be a part of multiple lists.
```c
struct my_linked_list_struct {
    unsigned int my_data;
    ...
    struct hlist_node* node_list_1;
    struct hlist_node* node_list_2;
}
```

### List Structure
We've got two structures for the hlist:
1. **hlist_head**: This structrue keeps track of the first node of the list. `first` is NULL if the list is empty.
2. **hlist_node**: The actual constituent node of the list. It stores two things:
   - **next** : Pointer to the next node in the list, NULL if it's the last node of the list.
   - **pprev**: Pointer to the pointer that points to this node. It will point to the `next` pointer of the previous node if the node is not the first node in the list. If it's the first node it'll point to the `first` of the head node.

```c
struct hlist_head {
  struct hlist_node* first;
};

struct hlist_node {
  struct hlist_node* next;
  struct hlist_node** pprev;
};
```

Image showig deletion. The image might still not make complete sense in that case you can run the accompanying file [hlist_deletion_with_address.c](../assets/hlist_deletion_with_address.c). It's buit to run independently and prints the address of relevant pointers, it's a big help along with the visualizations. Don't ask me how long it took me to understand why the deletion worked (:woozy_face:).
![Hlist Deletion](../assets/hlist_deletion.png)
