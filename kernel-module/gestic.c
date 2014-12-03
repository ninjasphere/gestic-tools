/*
J13,2  SPI1_SCLK  SOM-167   TS (GPIO3_0)

J13,3  I2C1_SDA  SOM-104  SDA
J13,5  I2C1_SCL  SOM-106  SCL
*/

#define GESTIC_GPIO_TS ((3*32) + 10) // 3_0 on EVK
#define GESTIC_GPIO_TS_NAME "mii1_rxclk" // mii1_col on EVK

#define GESTIC_GPIO_MCLR ((3*32) + 4) // 3_0 on EVK
#define GESTIC_GPIO_MCLR_NAME "mii1_rxdv"

// this is "2" on 3.2 and "1" on 3.12
#define GESTIC_I2C_BUS_NUM 1
#define GESTIC_I2C_ADDRESS 0x42


#define GESTIC_DEBUG (spammy_debug)


#define MAX_MESSAGE_LEN 255
#define BUFFER_SIZE (MAX_MESSAGE_LEN + 3)


#define OMAP_MUX_OUTPUT 0x07
#define OMAP_MUX_INPUT 0x3f
#define OMAP_MUX_INPUT_PULLUP 0x37
#define OMAP_MUX_INPUT_PULLDOWN 0x27


#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <../arch/arm/mach-omap2/mux.h>
#include <asm/io.h>

static volatile unsigned char *ts_mux_setting;
static volatile unsigned char *mclr_mux_setting;
static volatile int is_writing = 0;

static dev_t first_dev;
static struct cdev c_dev;
static struct class *cls;
static int ts_irq_num;
static wait_queue_head_t read_queue, write_queue;
static struct i2c_client *gestic_client;

static char msg_buffer[BUFFER_SIZE] = { 0xfe, 0xff };
static volatile size_t msg_length = 0;
static volatile size_t msg_offset = 0;

static char xmit_buffer[BUFFER_SIZE];
static volatile size_t xmit_len = 0;

static volatile int waiting_ts_release = 0;

static bool bridge_spam_reads = 0;   
module_param(bridge_spam_reads, bool, 0);   
MODULE_PARM_DESC(bridge_spam_reads, "Bridge mode, reading regardless of TS line");

static bool spammy_debug = 0;   
module_param(spammy_debug, bool, 0);
MODULE_PARM_DESC(spammy_debug, "Enables an extremely verbose spammy debug mode");

static int i2c_delay_read = 5;
static int i2c_delay_write = 0;
#define post_i2c_delay(delay) msleep((delay) << 1)

static int gestic_fill_buffer(void);

struct gestic_message_header
{
  uint8_t size;
  uint8_t flags;
  uint8_t seq;
  uint8_t id;
} __attribute__( ( packed ) );


static struct i2c_device_id gestic_idtable[] = {
  { "gestic", GESTIC_I2C_ADDRESS },
  { }
};

MODULE_DEVICE_TABLE(i2c, gestic_idtable);

static struct i2c_board_info gestic_boardinfo = {
  I2C_BOARD_INFO("gestic", 0x42)
};


static void gestic_reset(void)
{
  gpio_direction_output(GESTIC_GPIO_MCLR, 0);
  msleep(5);
  gpio_direction_output(GESTIC_GPIO_MCLR, 1);
  msleep(20);
}


static int gestic_open(struct inode *i, struct file *f)
{
  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: open()\n");
  xmit_len = 0;
  waiting_ts_release = 0;
  
  // gestic_reset();
  // just make sure we're not in reset state.
  gpio_direction_output(GESTIC_GPIO_MCLR, 1);
  msleep(20);

  return 0;
}

static int gestic_close(struct inode *i, struct file *f)
{
  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: close()\n");
  return 0;
}

/**
 * i2c_master_recv_len - issue a single I2C message in master receive mode
 * while also setting the I2C_M_RECV_LEN flag, meaning the first byte
 * specifies the amount of data to read in total, bounded by count.
 *
 * @client: Handle to slave device
 * @buf: Where to store data read from slave
 * @count: Maximum bytes to read, must be less than 64k since msg.len is u16
 *
 * Returns negative errno, or else the number of bytes read.
 */
int i2c_master_recv_len(const struct i2c_client *client, char *buf, int count)
{
  struct i2c_adapter *adap = client->adapter;
  struct i2c_msg msg[2];
  int ret;

  msg[0].addr = client->addr;
  msg[0].flags = client->flags & I2C_M_TEN;
  msg[0].flags |= I2C_M_RECV_LEN;
  msg[0].flags |= I2C_M_RD;
  msg[0].len = count;
  msg[0].buf = buf;

  /*
  msg[1].addr = client->addr;
  msg[1].flags = client->flags & I2C_M_TEN;
  // msg.flags |= I2C_M_RECV_LEN;
  msg[1].flags |= I2C_M_RD;
  msg[1].len = count - 1;
  msg[1].buf = buf + 1;
  */

  ret = i2c_transfer(adap, msg, 1);

  /* If everything went ok (i.e. 1 msg transmitted), return #bytes
     actually received, else error code. */
  return (ret == 1) ? (msg[0].len /*+ msg[1].len*/) : ret;
}

static int gestic_fill_buffer() {
  char ktmp[512];
  struct gestic_message_header *hdr = (struct gestic_message_header *)ktmp;
  int bytes_read = 0;

  // asserted: now we assert too, so the data doesn't change
  // gpio_direction_output(GESTIC_GPIO_TS, 0);
  // is_writing = 1;
  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: <<< RE-asserting TS (in fill_buffer)\n");
  __raw_writew(OMAP_MUX_OUTPUT, ts_mux_setting);
  gpio_direction_output(GESTIC_GPIO_TS, 0);

  // perform the i2c transactions
  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: fill(): ready to read (client=%p, addr=%d)\n", gestic_client, gestic_client->addr);

  memset(ktmp, 0, 255);
  bytes_read = i2c_master_recv(gestic_client, ktmp, 138); // 4 is legth of headrer, min, which holds the length field
//  bytes_read = i2c_master_recv(gestic_client, ktmp, 140); // len will be += the length byte, which could be up to 140

  if (bytes_read > 0) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: fill(): read %d bytes, len %d, flags %04x\n", bytes_read, hdr->size, gestic_client->flags);
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: fill(): read msg[size=%d, flags=%d, seq=%d, id=%d]\n", hdr->size, hdr->flags, hdr->seq, hdr->id);

    if (GESTIC_DEBUG) print_hex_dump(KERN_DEBUG, "i2c_master_recv <<< ", DUMP_PREFIX_OFFSET, 16, 1, ktmp, bytes_read, true);
  }

  if (bytes_read > 0 && hdr->size > 4) {
    // cap the bytes to the amount in the actual GestIC payload
    if ( bytes_read > hdr->size )
      bytes_read = hdr->size;

    // cap the bytes at what the buffer can hold
    if ( bytes_read > MAX_MESSAGE_LEN )
      bytes_read = MAX_MESSAGE_LEN;

    // copy from the temporary buffer to our main buffer, after the prefix.
    memcpy(msg_buffer + 2, ktmp, bytes_read);
    msg_length = 2 + bytes_read;
    msg_offset = 0;

    bytes_read = msg_length;
  } else {
    if (bytes_read > 0) {
      if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: fill(): read FAILED with hdr->size=%d\n", hdr->size);
    } else {
      if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: fill(): read FAILED with error=%d\n", -bytes_read);
    }
    bytes_read = -EWOULDBLOCK;
  }

  // stop asserting the TS line
  // gpio_direction_input(GESTIC_GPIO_TS);
  // is_writing = 0;
  waiting_ts_release = 1;
  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: <<< releasing TS\n");
  gpio_direction_input(GESTIC_GPIO_TS);
  __raw_writew(OMAP_MUX_INPUT_PULLUP, ts_mux_setting);
  post_i2c_delay(i2c_delay_read);

  return bytes_read;
}

static ssize_t gestic_read(struct file *f, char __user *buf, size_t
  len, loff_t *off)
{
  int bytes_read;
  size_t to_send;

  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC INFO: gestic_read(len=%d)\n", len);

  if (len == 0) {
    printk(KERN_INFO "GestIC WARN: gestic_read() WARN: called with empty buffer, returning 0 bytes\n");
    return 0; // can't read 0 bytes
  }

  // if (waiting_ts_release) {
  //   if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: waiting for TS release from MGC3130...\n");
  //   if (wait_event_interruptible(write_queue, (waiting_ts_release == 0)) < 0) {
  //     return -ERESTARTSYS;
  //   }
  //   if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: released by MGC3130 [waiting_ts_release=%d]!\n", waiting_ts_release);
  // }

  // if we have data available in our buffer, push it out
  to_send = msg_length - msg_offset;
  if (to_send > len) {
    to_send = len;
  }
  if (to_send > 0) {
    // copy bytes, noting for the user how many actually arrived (copy_to_user returns bytes NOT copied)
    to_send -= copy_to_user(buf, msg_buffer + msg_offset, to_send);
    msg_offset += to_send;
    return to_send;
  }

  if (bridge_spam_reads) {
    // don't wait or be polite, act like the PIC USB bridge
    goto force_read_attempt;
  }

  // we're blocking for input, so make sure we wait for the TS line to be asserted
  if (gpio_get_value(GESTIC_GPIO_TS) != 0)
  {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: waiting for data...\n");

    // not asserted: this means there is no data to return at the moment, so we will block.
    if (wait_event_interruptible(read_queue, (gpio_get_value(GESTIC_GPIO_TS) == 0)) < 0) {
      return -ERESTARTSYS;
    }

    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: received TS assertion\n");
  }

  if (gpio_get_value(GESTIC_GPIO_TS) != 0)
  {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: was woken by read queue but TS not asserted!\n");

    return -EAGAIN;
  }

  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: TS in state: %d\n", gpio_get_value(GESTIC_GPIO_TS));

  __raw_writew(OMAP_MUX_OUTPUT, ts_mux_setting);
  gpio_direction_output(GESTIC_GPIO_TS, 0);

  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: now holding TS low\n");

  force_read_attempt:
  bytes_read = gestic_fill_buffer();

  if (bytes_read > 0) {
    // if we successfully received new data from the i2c chip, call ourselves to buffer them out to userspace
    return gestic_read(f, buf, len, off);
  }

  return bytes_read;
}

static void gestic_write_byte(char b)
{
  struct gestic_message_header *hdr = (struct gestic_message_header *)&xmit_buffer[2];
  int bytes_sent;

  if (xmit_len >= BUFFER_SIZE) {
    return;
  }

  if (xmit_len == 0 && b != '\xfe') return;
  if (xmit_len == 1 && b != '\xff') {
    xmit_len = 0;
    return;
  }

  // if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: xmit[%d] = %02x\n", xmit_len, b);
  xmit_buffer[xmit_len] = b;
  xmit_len++;

  if (xmit_len == 8 && hdr->size == 0x00) {
    // we've received a control message, exactly 8 bytes read.

    if (xmit_buffer[3] == 0x11) {
      // RESET cmd
      gestic_reset();
    } else {
      // set delay cmd
      i2c_delay_read = xmit_buffer[4];
      i2c_delay_write = xmit_buffer[5];
    }

    xmit_len = 0; // done, ready for next message
    return;
  }

  // if we have a valid length > 0, and our bytes read (not including feff) matches the len, process msg
  if (xmit_len > 3 && hdr->size > 0 && xmit_len-2 == hdr->size) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: write msg[size=%d, flags=%d, seq=%d, id=%d], xmit_len=%d\n", hdr->size, hdr->flags, hdr->seq, hdr->id, xmit_len);

    // wait for TS line to be *released*
    // if (gpio_get_value(GESTIC_GPIO_TS) == 0) {
    //   wait_event_interruptible(write_queue, (gpio_get_value(GESTIC_GPIO_TS) != 0));
    // }

    if (GESTIC_DEBUG) print_hex_dump(KERN_DEBUG, "i2c_master_send >>> ", DUMP_PREFIX_OFFSET, 16, 1, xmit_buffer + 2, xmit_len - 2, true);
    bytes_sent = i2c_master_send(gestic_client, xmit_buffer + 2, xmit_len - 2);
    xmit_len = 0; // reset for next message

    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: i2c_master_send returned %d\n", bytes_sent);

    post_i2c_delay(i2c_delay_write);
  }
}

static ssize_t gestic_write(struct file *f, const char __user *buf,
  size_t len, loff_t *off)
{
  char ktmp[512];
  size_t bytes = len;
  int i;

  /*
  if (waiting_ts_release) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: waiting for TS release from MGC3130...\n");
    if (wait_event_interruptible(write_queue, (waiting_ts_release == 0)) < 0) {
      return -ERESTARTSYS;
    }
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: released by MGC3130 [waiting_ts_release=%d]!\n", waiting_ts_release);
  }
  */

  if ( bytes > 255 ) {
    bytes = 255;
  }

  bytes -= copy_from_user(ktmp, buf, bytes);

  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: write( %d bytes )\n", bytes);

  for (i = 0; i < bytes; i++) {
    gestic_write_byte(ktmp[i]);
  }

  return bytes;
}

static unsigned int gestic_poll(struct file *f, poll_table *pt)
{
  if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: polled\n");

  // if we have data waiting in our buffer, then we should signal data available
  if (msg_offset < msg_length) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: poll() found data ready in buffer\n");
    goto data_already_available; // immediate data in buffer
  }

  if (bridge_spam_reads) {
    // don't wait or be polite, act like the PIC USB bridge
    // just say there's always data, and assume userspace doesn't mind us 
    // returning -EWOULDBLOCK on the read if nothing's actually available
    return (POLLIN | POLLRDNORM) | (POLLOUT | POLLWRNORM);
  }

  // GESTIC_GPIO_TS pulled low (asserted) when data *available* for reading
  if (gpio_get_value(GESTIC_GPIO_TS) == 0) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: poll() found TS was asserted\n");
    goto data_ready_to_read; // data can be read immediately, but not yet in buffer
  }

  // otherwise we have no data in our buffer *and* no data ready to read from i2c
  poll_wait(f, &read_queue, pt);

  // now we check to confirm that TS is now asserted
  if (gpio_get_value(GESTIC_GPIO_TS) == 0) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: poll() woken when TS asserted\n");
    goto data_ready_to_read; // data can be read immediately, but not yet in buffer
  } else {
    return POLLOUT;
  }

  data_ready_to_read:

  gestic_fill_buffer();

  if (msg_offset == msg_length) {
    // read didn't produce new data in the buffer, so nothing to read
    return (POLLOUT | POLLWRNORM);
  }

  data_already_available:
  return (POLLIN | POLLRDNORM) | (POLLOUT | POLLWRNORM);
}

static struct file_operations pugs_fops =
{
  .owner = THIS_MODULE,
  .open = gestic_open,
  .release = gestic_close,
  .read = gestic_read,
  .write = gestic_write,
  .poll = gestic_poll
};

static void _do_ts_change(int value) {
  if (value == 0) {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: >>> TS asserted\n");

    wake_up_all(&read_queue);
  } else {
    if (GESTIC_DEBUG) printk(KERN_INFO "GestIC: ~~~ TS released\n");

    waiting_ts_release = 0;
    wake_up_all(&write_queue);
  }
}

static irqreturn_t data_incoming_ready(int irq, void *dev_id)
{
  static int last_value = 1;
  int new_value = !!gpio_get_value(GESTIC_GPIO_TS);

  if ( irq != ts_irq_num )
  {
    return IRQ_NONE;
  }

  // if ( is_writing ) {
  //   return IRQ_HANDLED;
  // }
  
  if (new_value != last_value) {
    // simple change
    _do_ts_change(!!new_value);
  } else {
    // toggle and back again
    _do_ts_change(!new_value);
    _do_ts_change(!!new_value);
  }

  last_value = new_value;

  return IRQ_HANDLED;
}

static int __init gestic_init(void)
{
  int err;
  struct i2c_adapter *i2c_adap;

  ts_mux_setting = ioremap(0x44E10930, PAGE_SIZE);
  mclr_mux_setting = ioremap(0x44E10918, PAGE_SIZE);

  printk(KERN_INFO "GestIC: registered; TS pinmux = %04x; MCLR pinmux = %04x\n", __raw_readw(ts_mux_setting), __raw_readw(mclr_mux_setting));

  if (bridge_spam_reads) {
    printk(KERN_INFO "GestIC: bridge_spam_reads enabled, gestic driver will use more CPU and flood I2C like the reference implementation\n");
  }

  init_waitqueue_head(&read_queue);
  init_waitqueue_head(&write_queue);

  if (alloc_chrdev_region(&first_dev, 0, 1, "GestIC") < 0)
  {
    return -1;
  }

  if ((cls = class_create(THIS_MODULE, "chardrv")) == NULL)
  {
    unregister_chrdev_region(first_dev, 1);
    return -1;
  }

  if (device_create(cls, NULL, first_dev, NULL, "gestic") == NULL)
  {
    class_destroy(cls);
    unregister_chrdev_region(first_dev, 1);
    return -1;
  }

  cdev_init(&c_dev, &pugs_fops);
  if (cdev_add(&c_dev, first_dev, 1) == -1)
  {
    device_destroy(cls, first_dev);
    class_destroy(cls);
    unregister_chrdev_region(first_dev, 1);
    return -1;
  }

  i2c_adap = i2c_get_adapter(GESTIC_I2C_BUS_NUM);
  gestic_client = i2c_new_device(i2c_adap, &gestic_boardinfo);

  // prep the GPIO for the TS line
  //omap_mux_init_gpio(GESTIC_GPIO_TS, OMAP_PIN_INPUT);
  err = gpio_request(GESTIC_GPIO_TS, "mgc3130_TS");
  gpio_direction_input(GESTIC_GPIO_TS);
  __raw_writew(OMAP_MUX_INPUT_PULLUP, ts_mux_setting);
  ts_irq_num = gpio_to_irq(GESTIC_GPIO_TS);
  if (err == -1 || ts_irq_num == -1)
  {
    i2c_unregister_device(gestic_client);
    cdev_del(&c_dev);
    device_destroy(cls, first_dev);
    class_destroy(cls);
    unregister_chrdev_region(first_dev, 1);
    return -1;
  }

  if (request_irq(ts_irq_num, data_incoming_ready, 0, "mgc3130_TS_R", NULL) == -1)
  {
    gpio_free(GESTIC_GPIO_TS);
    i2c_unregister_device(gestic_client);
    cdev_del(&c_dev);
    device_destroy(cls, first_dev);
    class_destroy(cls);
    unregister_chrdev_region(first_dev, 1);
    return -1;
  }

  irq_set_irq_type(ts_irq_num, IRQ_TYPE_EDGE_BOTH);

  // get RST IO as well
  err = gpio_request(GESTIC_GPIO_MCLR, "mgc3130_MCLR");
  __raw_writew(OMAP_MUX_OUTPUT, mclr_mux_setting);
  gestic_reset();

  return 0;
}
 
static void __exit gestic_exit(void)
{
  free_irq(ts_irq_num, NULL);
  gpio_free(GESTIC_GPIO_TS);
  gpio_free(GESTIC_GPIO_MCLR);

  i2c_unregister_device(gestic_client);

  cdev_del(&c_dev);
  device_destroy(cls, first_dev);
  class_destroy(cls);
  unregister_chrdev_region(first_dev, 1);

  printk(KERN_INFO "GestIC: unregistered");
}
 
module_init(gestic_init);
module_exit(gestic_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Theo Julienne <theo@ninjablocks.com>");
MODULE_DESCRIPTION("MGC3130 GestIC Driver");
