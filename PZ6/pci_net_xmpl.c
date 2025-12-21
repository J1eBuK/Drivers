#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>

#define MY_VENDOR 0x1234
#define MY_DEVICE 0x5678

static int net_open(struct net_device *dev){
    pr_info("pci_net_xmpl: интерфейс открыт\n");
    netif_start_queue(dev);
    return 0;
}

static int net_stop(struct net_device *dev){
    pr_info("pci_net_xmpl: интерфейс остановлен\n");
    netif_stop_queue(dev);
    return 0;
}

static netdev_tx_t net_xmit(struct sk_buff *skb, struct net_device *dev){
    pr_info("pci_net_xmpl: пакет получен в hard_start_xmit, длина=%u\n", skb->len);
    print_hex_dump(KERN_INFO, "pci_net_xmpl пакет: ", DUMP_PREFIX_NONE, 16, 1,
                   skb->data, skb->len, true);
    dev->stats.tx_packets++;
    dev->stats.tx_bytes += skb->len;
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops net_ops = {
    .ndo_open = net_open,
    .ndo_stop = net_stop,
    .ndo_start_xmit = net_xmit,
};

static void net_setup(struct net_device *dev){
    ether_setup(dev);
    dev->netdev_ops = &net_ops;
    eth_hw_addr_random(dev);
    pr_info("pci_net_xmpl: MAC-адрес %pM\n", dev->dev_addr);
}

static const struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(MY_VENDOR, MY_DEVICE) }, {0}
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int pci_net_probe(struct pci_dev *pdev, const struct pci_device_id *id){
    int ret;
    struct net_device *ndev;

    pr_info("pci_net_xmpl: probe вызван для %04x:%04x\n", id->vendor, id->device);

    ret = pci_enable_device(pdev);
    if(ret) return ret;
    pci_set_master(pdev);

    ndev = alloc_netdev(0, "pci_xmpl%d", NET_NAME_UNKNOWN, net_setup);
    if(!ndev) return -ENOMEM;

    pci_set_drvdata(pdev, ndev);

    ret = register_netdev(ndev);
    if(ret){
        free_netdev(ndev);
        pci_disable_device(pdev);
        return ret;
    }

    pr_info("pci_net_xmpl: интерфейс зарегистрирован как %s\n", ndev->name);
    return 0;
}

static void pci_net_remove(struct pci_dev *pdev){
    struct net_device *ndev = pci_get_drvdata(pdev);

    unregister_netdev(ndev);
    free_netdev(ndev);
    pci_disable_device(pdev);

    pr_info("pci_net_xmpl: модуль выгружен\n");
}

static struct pci_driver pci_net_driver = {
    .name = "pci_net_xmpl",
    .id_table = pci_ids,
    .probe = pci_net_probe,
    .remove = pci_net_remove,
};

module_pci_driver(pci_net_driver);
MODULE_LICENSE("GPL");
