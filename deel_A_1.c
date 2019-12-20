#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h> 

/*
 * The module commandline arguments ...
 */
static int       data           = 0;
static int       toggleSpeed    = 500;
static int       ioNummers[2]   = { -1, -1 };
static int       arr_argc 		= 0;
static struct timer_list blink_timer;

module_param(data, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(data, "The state to be set for the IO's, 0 or 1");
module_param(toggleSpeed, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(toggleSpeed, "The toggling speed for the IO's, in ms");
module_param_array(ioNummers, int, &arr_argc, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(ioNummers, "An array of IO numbers to toggle, max 2");

/*
 * Timer function called periodically
 */
static void blink_timer_func(struct timer_list* t)
{
    int i = 0;
    
	printk(KERN_INFO "%s\n", __func__);

    // Set RELAIS GPIOs output to data
    for(i = 0; i < ARRAY_SIZE(ioNummers); i++)
    {
        if (ioNummers[i] > 0)
        {
            gpio_set_value(ioNummers[i], data);
        }
    }

	data = !data; 
	
	/* schedule next execution */
	blink_timer.expires = jiffies + ((toggleSpeed / 1000) * HZ);
	add_timer(&blink_timer);
}

/*
 * Module init function
 */
static int __init deel_A_1_init(void)
{
    int i = 0, ret = 0;

    printk(KERN_INFO "%s\n=============\n", __func__);
    printk(KERN_INFO "toggleSpeed is the toggling speed for the IO's, in ms: %i\n", toggleSpeed);

    for (i = 0; i < ARRAY_SIZE(ioNummers); i++)
    {
        printk(KERN_INFO "ioNummers[%i] = %hi\n", i, ioNummers[i]);
    }

    // register RELAIS GPIOs, turn RELAIS on
    for (i = 0; i < ARRAY_SIZE(ioNummers); i++)
    {
        if (ioNummers[i] > 0)
        {
	        ret = gpio_request_one(ioNummers[i], GPIOF_OUT_INIT_LOW, "RELAIS");
            if (ret)
            {
                printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
                return ret;
            }
        }
    }

	/* init timer, add timer function */
	timer_setup(&blink_timer, blink_timer_func, 0);
	blink_timer.function = blink_timer_func;
	blink_timer.expires = jiffies + ((toggleSpeed / 1000) * HZ);
	add_timer(&blink_timer);

    return ret;
}

/*
 * Module exit function
 */
static void __exit deel_A_1_exit(void)
{
    int i = 0;

    printk(KERN_INFO "%s\n", __func__);

	// deactivate timer if running
	del_timer_sync(&blink_timer);

    // turn RELAIS off
    for (i = 0; i < ARRAY_SIZE(ioNummers); i++)
    {
        if (ioNummers[i] > 0)
        {
            gpio_set_value(ioNummers[i], 0);
        }
    }

    // unregister GPIO
    for (i = 0; i < ARRAY_SIZE(ioNummers); i++)
    {
        if (ioNummers[i] > 0)
        {
            gpio_free(ioNummers[i]);
        }
    }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fishezzz");
MODULE_DESCRIPTION("Basic Linux Kernel module for toggling 2 IO's and counting IO edges");

module_init(deel_A_1_init);
module_exit(deel_A_1_exit);
