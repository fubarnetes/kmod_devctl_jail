# devctl_jail.ko
Kernel module providing devctl notifications for jails.

# Is it any good?
[Yes.](https://news.ycombinator.com/item?id=3067434)

# But how do I use it?

Build `devctl_jail.ko` with `make` and load the kernel module as root using `kldload ./devctl_jail.ko`.
If you want, you can install it by running `make install` as root.

This kernel module ships with an example devd.conf and syslogd.conf which add some logging functionality.
Install them and restart `devd` and `syslogd`:
```
cp syslogd.conf /usr/local/etc/syslog.d/jail.conf
touch /var/log/jail
systemctl restart syslogd
cp devd.conf /usr/local/etc/devd/jail.conf
systemctl restart devd
```

This will give you some logging that looks like this:
```
May 20 20:50:05 blackbox jail[50581]: Jail JID=1 created
May 20 20:50:08 blackbox jail[50592]: Process PID=50591 attached to Jail JID=1
May 20 20:50:54 blackbox jail[50626]: Jail JID=1 removed
```

If you are writing a jail manager however, you might be interested in getting the notifications yourself.
You can just connect to the `SOCK_SEQPACKET` socket at `/var/run/devd.seqpacket.pipe` to receive raw
notifications multiplexed by `devd`.g
