#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h> 

/*
 * The module commandline arguments ...
 */
static short int     ioNummers[2]   = { 0, 0 };
static int           toggleSpeed    = 500;

module_param(ioNummers, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(ioNummers, "An array of IO numbers to toggle, max 2");
module_param(toggleSpeed, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(toggleSpeed, "The toggling speed for the IO's, in ms");

/*
 * Module init function
 */
static int __init opdrachtmod_init(void)
{
    int i;

    printk(KERN_INFO "%s\n=============\n", __func__);
    printk(KERN_INFO "ioNummers is an array of IO numbers to toggle, max 2: %d\n", ioNummers);
    printk(KERN_INFO "toggleSpeed is the toggling speed for the IO's, in ms: %hd\n", toggleSpeed);

    for (i = 0; i < (sizeof ioNummers / sizeof (int)); i++)
    {
        printk(KERN_INFO "ioNummers[%d] = %d\n", i, ioNummers[i]);
    }

    return 0;
}

/*
 * Exit function
 */
static void __exit opdrachtmod_exit(void)
{
    printk(KERN_INFO "%s\n", __func__);
}

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Fishezzz");
MODULE_DESCRIPTION("Basic Linux Kernel module for toggling 2 IO's and counting IO edges");

module_init(opdrachtmod_init);
module_exit(opdrachtmod_exit);
