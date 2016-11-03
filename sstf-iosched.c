//Tao Chen
//SSTF shortest seek time first algorithm for kernel I/O scheduler

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

sector_t curSector = -1;

struct sstf_data{
	struct list_head queue;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq, struct request *next){
	list_del_init(&next->queuelist);
	printk(KERN_DEBUG "Request merged.\n");
}

static int sstf_dispatch(struct request_queue *q, int force){
	struct sstf_data *nd = q->elevator->elevator_data;

	if(!list_empty(&nd->queue)){
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		curSector = blk_rq_pos(rq);
		return 1;
	}
	printk(KERN_DEBUG "Queue dispatched.\n");
	return 0;
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e){
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if(!eq){
		return -ENOMEM;
	}
	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if(!nd){
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	printk(KERN_DEBUG "SSTF INIT.\n");
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;
	struct list_head *cur = NULL;
	struct list_head *reqOnQueue = NULL;

	if(list_empty(&nd->queue)){
		printk(KERN_DEBUG "Queue is empty.\n");
	}

	list_for_each(cur, &nd->queue){
		struct request *c = list_entry(cur, struct request, queuelist);
		printk(KERN_DEBUG "Going through the queue.\n");
		if(blk_rq_pos(rq) < curSector){
			if(blk_rq_pos(rq) > blk_rq_pos(c)){
				break;
			}
			if(blk_rq_pos(c) > curSector){
				break;
			}
		}else{
			if(blk_rq_pos(c) > curSector && blk_rq_pos(rq) > blk_rq_pos(c)){
				break;
			}
		}
	}

	if(cur != NULL){
		printk(KERN_DEBUG "Cur is not NULL!!");
	}

	//list_add_tail(new request, head of the queue)
	//adds the new request immediately before the head node
	list_add_tail(&rq->queuelist, cur);

	//print the queue on the console for debugging
	printk(KERN_DEBUG "This is current sector: %llu.\n", (unsigned long long)curSector);
	printk(KERN_DEBUG "This is the sector of the new request: %llu.\n", (unsigned long long)blk_rq_pos(rq));
	list_for_each(reqOnQueue, &nd->queue){
		struct request *secNum = list_entry(reqOnQueue, struct request, queuelist);
		printk(KERN_DEBUG "%llu.\n", (unsigned long long)blk_rq_pos(secNum));
	}
	printk(KERN_DEBUG "Request added.\n");
}

static struct request* sstf_latter_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;

	if(rq->queuelist.next == &nd->queue){
		return NULL;
	}
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static struct request* sstf_former_request(struct request_queue *q, struct request *rq){
	struct sstf_data *nd = q->elevator->elevator_data;

	if(rq->queuelist.prev == &nd->queue){
		return NULL;
	}
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static void sstf_exit_queue(struct elevator_queue *e){
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void){
	return elv_register(&elevator_sstf);
}

static void __exit sstf_exit(void){
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);

MODULE_AUTHOR("Tao Chen");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("C-Look IO Scheduler");
