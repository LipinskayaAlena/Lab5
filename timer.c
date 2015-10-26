#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static int delay = -1;
struct timer_list timer;

static struct kobject *kobj;

static void hello(unsigned long arg);
ssize_t show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
ssize_t store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
static int my_init(void);
static void my_exit(void);

static void hello(unsigned long arg)
{
	printk(KERN_INFO "Hello world!\n");
	timer.expires = jiffies + HZ * delay;
	//add_timer(&timer);
	if (delay == 0)
		del_timer(&timer);
}

ssize_t show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", delay);
}

ssize_t store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int tmp = 0;
	sscanf(buf, "%du", &tmp);
	if (tmp > 0) {
		if (delay < 0) {
			init_timer(&timer);
			timer.function = hello;
			timer.data = 0;
		}
		delay = tmp;
		add_timer(&timer);
	}
	return count;
}

static struct kobj_attribute sc_attrb = { 
	.attr = {
		.name = "HelloWorld",
		.mode = S_IWUSR | S_IRUGO,
	},
	.show = show,
	.store = store
};

static int my_init(void) 
{
	kobj = kobject_create_and_add("hello", NULL);
	if (!kobj)
		return -ENOMEM;
	if (sysfs_create_file(kobj, &sc_attrb.attr))
		kobject_put(kobj);
	return 0;
}

static void my_exit(void) 
{
	del_timer(&timer);
	kobject_put(kobj);
	sysfs_remove_file(kobj, &sc_attrb.attr);
}

module_init(my_init);
module_exit(my_exit);


