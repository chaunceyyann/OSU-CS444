/* Linked List in kernel
 */
struct node {
    struct node *next;
    struct node *prev;
    void *data;
}

struct list_head {
    struct list_head *next;
    struct list_head *prev;
}
/* handle of the head form <linux/list.h>
INIT_ LIST_HEAD
LIST_HEAD
*/
list_add(struct list_head *new, struct list_head *head);
list_add_tail(struct list_head *new, struct list_head *head);
/* moving
 */
lsit_move
list_move_tail
list_empty
list_splice->list=list1+list2
/* iteration
 */
list_for_each(p, list);
list_for_each_entry(p, head, list);
list_for_entry_reverse_safe(p, list);
/* Queues
 * Kfifos
 * <linux/kfifo.h>
 */
kfifo_alloc
kfifo_in    // for adding something
kfifo_out   // to remove something from the queue
kfifo_size  // size of the whole buffer
kfifo_len   // size of the data in bytes
kfifo_is_empty
kfifo_is_full
/* maps
 * key/value store
 * associative array / data structure
 * idr
 */

