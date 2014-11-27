#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/seq_file.h>
#include <linux/rtc.h>

MODULE_LICENSE("GPL");


static int clock_seq_show(struct seq_file *sf, void *v)
{
	struct timeval tv;
	static char *buf;
	struct rtc_time result;

	buf = kmalloc(4096, GFP_KERNEL);
	do_gettimeofday(&tv);
	/* According to POSIX definition time_t
	 * should be signed int
	 * coerce to the form of int should also work
	 * in kernel space
	 */
	sprintf(buf, "Seconds since Epoch: %d\n"
			"Microseconds: %d\n",
			(int)tv.tv_sec, (int)tv.tv_usec);

	seq_printf(sf, "%s", buf);

	rtc_time_to_tm(tv.tv_sec, &result);
	sprintf(buf, "%d:%d:%d %d.%d.%d\n", result.tm_hour -
			(sys_tz.tz_minuteswest / 60) + sys_tz.tz_dsttime,
			result.tm_min, result.tm_sec, result.tm_mday,
			result.tm_mon,
			/* The tm_year is the number since 1900 */
			result.tm_year + 1900);
	seq_printf(sf, "%s", buf);

	kfree(buf);
	return 0;
}

static int clock_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, clock_seq_show, NULL);
}

static const struct file_operations clock_proc_ops = {
	.owner = THIS_MODULE,
	.open = clock_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};

static int __init proc_clock_init(void)
{
	struct proc_dir_entry *entry;

	entry = proc_create("clock", 0, NULL, &clock_proc_ops);
	if (!entry) {
		pr_debug("Error occured when creating proc entry");
		return -1;
	}

	return 0;
}

static void __exit proc_clock_exit(void)
{
	remove_proc_entry("clock", NULL);
}

module_init(proc_clock_init);
module_exit(proc_clock_exit);
