#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>

static struct net_device *net_xmpl_dev;

static int net_xmpl_open(struct net_device *dev){
  pr_info("net_xmpl: interface opened\n");
  netif_start_queue(dev);
  return 0;
}

static int net_xmpl_stop(struct net_device *dev){
  pr_info("net_xmpl: interface stopped\n");
  netif_stop_queue(dev);
  return 0;
}

static int net_xmpl_xmit(struct sk_buff *skb, struct net_device *dev){
  pr_info("net_xmpl: packet received in hard_start_xmit, len=%u\n", skb->len);
  print_hex_dump(KERN_INFO, "net_xmpl packet: ", DUMP_PREFIX_NONE, 16, 1, skb->data, skb->len, true);
  dev->stats.tx_packets++;
  dev->stats.tx_bytes += skb->len;
  dev_kfree_skb(skb);
  return NETDEV_TX_OK;
}

static int net_xmpl_init(struct net_device *dev){
  pr_info("net_xmpl: initialized\n");
  return 0;
}

static const struct net_device_ops net_xmpl_ops = {
  .ndo_init = net_xmpl_init,
  .ndo_open = net_xmpl_open,
  .ndo_stop = net_xmpl_stop,
  .ndo_start_xmit = net_xmpl_xmit,
};

static void net_xmpl_setup(struct net_device *dev){
  ether_setup(dev);
  dev->netdev_ops = &net_xmpl_ops;
  eth_hw_addr_random(dev);
  pr_info("net_xmpl: assigned random mac %pM\n", dev->dev_addr);
}

static int __init net_xmpl_module_init(void){
  int res;
  net_xmpl_dev = alloc_netdev(0, "netxmpl%d", NET_NAME_UNKNOWN, net_xmpl_setup);
  if(!net_xmpl_dev) return -ENOMEM;
  res = register_netdev(net_xmpl_dev);
  if(res){ free_netdev(net_xmpl_dev); return res; }
  pr_info("net_xmpl: module loaded\n");
  return 0;
}

static void __exit net_xmpl_module_exit(void){
  unregister_netdev(net_xmpl_dev);
  free_netdev(net_xmpl_dev);
  pr_info("net_xmpl: module unloaded\n");
}

module_init(net_xmpl_module_init);
module_exit(net_xmpl_module_exit);
MODULE_LICENSE("GPL");
