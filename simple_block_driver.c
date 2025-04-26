#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "simple_block"
#define DEVICE_CAPACITY 1024 // Capacity in sectors
#define SECTOR_SIZE 512

static int major_num;
static struct gendisk *block_disk;
static struct request_queue *block_queue;
static spinlock_t lock;
static char *device_data;

static void process_request(struct request_queue *q)
{
    struct request *req;
    while ((req = blk_fetch_request(q)) != NULL) {
        if (req_op(req) == REQ_OP_READ || req_op(req) == REQ_OP_WRITE) {
            pr_info(DEVICE_NAME ": Request processed\n");
            blk_end_request_all(req, 0);
        } else {
            pr_err(DEVICE_NAME ": Unknown request\n");
            blk_end_request_all(req, -EIO);
        }
    }
}

static int __init simple_block_init(void)
{
    major_num = register_blkdev(0, DEVICE_NAME);
    if (major_num < 0) {
        pr_err(DEVICE_NAME ": Failed to register block device\n");
        return major_num;
    }

    block_queue = blk_init_queue(process_request, &lock);
    if (!block_queue) {
        unregister_blkdev(major_num, DEVICE_NAME);
        return -ENOMEM;
    }

    block_disk = alloc_disk(1);
    if (!block_disk) {
        blk_cleanup_queue(block_queue);
        unregister_blkdev(major_num, DEVICE_NAME);
        return -ENOMEM;
    }

    device_data = vmalloc(DEVICE_CAPACITY * SECTOR_SIZE);
    if (!device_data) {
        put_disk(block_disk);
        blk_cleanup_queue(block_queue);
        unregister_blkdev(major_num, DEVICE_NAME);
        return -ENOMEM;
    }

    block_disk->major = major_num;
    block_disk->first_minor = 0;
    block_disk->fops = NULL; // Placeholder for file operations
    block_disk->private_data = NULL;
    snprintf(block_disk->disk_name, 32, DEVICE_NAME);
    set_capacity(block_disk, DEVICE_CAPACITY);
    add_disk(block_disk);

    pr_info(DEVICE_NAME ": Initialized successfully\n");
    return 0;
}

static void __exit simple_block_exit(void)
{
    del_gendisk(block_disk);
    put_disk(block_disk);
    blk_cleanup_queue(block_queue);
    vfree(device_data);
    unregister_blkdev(major_num, DEVICE_NAME);
    pr_info(DEVICE_NAME ": Unloaded successfully\n");
}

module_init(simple_block_init);
module_exit(simple_block_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple Linux Block Device Driver");