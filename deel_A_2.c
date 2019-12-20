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
static int   button     = 4;
static int   button_irq = -1;
int          counter    = 0;

module_param(button, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(button, "Input button");

/*
 * The interrupt service routine called on button presses
 */
static irqreturn_t button_isr(int irq, void *data)
{
	// Counter optellen als er een interupt is op de knop
	if (irq == button_irq)
	{
		counter++;
	}
	
	printk(KERN_INFO "Current counter value: %d\n", counter);
	return IRQ_HANDLED;
}

/*
 * Module init function
 */
static int __init deel_A_2_init(void)
{
	int ret = 0;
	
	printk(KERN_INFO "%s\n", __func__);
	
	if (button > 0)
	{
		// Register button GPIO
		ret = gpio_request_one(button, GPIOF_IN, "BUTTON");
		
		if (ret)
		{
			printk(KERN_ERR "Unable to request GPIO: %d\n", ret);
			return 0;
		}
		
		// Register IRQ on button
		printk(KERN_INFO "Current button value: %d\n", gpio_get_value(button));
		
		ret = gpio_to_irq(button);
		
		if (ret < 0)
		{
			printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
			goto fail1;
		}
		
		button_irq = ret;
		
		printk(KERN_INFO "Successfully requested BUTTON IRQ # %d\n", button_irq);
		
		// Set up IRQ method for input button
		ret = request_irq(button_irq, button_isr, IRQF_TRIGGER_RISING /*| IRQF_DISABLED*/, "gpiomod#BUTTON", NULL);
		
		if (ret)
		{
			printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
			goto fail1;
		}
		
		// cleanup what has been setup so far
		fail1:
			gpio_free(button);
	}
	
	return ret;
}

/*
 * Module exit function
 */
static void __exit deel_A_2_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
	
	// free irqs
	free_irq(button_irq, NULL);
	
	// unregister
	gpio_free(button);
	
	printk(KERN_INFO "Giving button freedom back!\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fishezzz");
MODULE_DESCRIPTION("Basic Linux Kernel module for toggling 2 IO's and counting IO edges");

module_init(deel_A_2_init);
module_exit(deel_A_2_exit);
