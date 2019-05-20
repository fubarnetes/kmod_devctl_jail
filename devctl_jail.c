#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>  /* uprintf */
#include <sys/errno.h>
#include <sys/param.h>  /* defines used in kernel.h */
#include <sys/kernel.h> /* types used in module initialization */


static int
devctl_jail_loader(struct module *m, int what, void *arg)
{
  int err = 0;

  switch (what) {
  case MOD_LOAD:                /* kldload */
    uprintf("devctl_jail KLD loaded.\n");
    break;
  case MOD_UNLOAD:
    uprintf("devctl_jail KLD unloaded.\n");
    break;
  default:
    err = EOPNOTSUPP;
    break;
  }
  return(err);
}

static moduledata_t devctl_jail_mod = {
  "devctl_jail",
  devctl_jail_loader,
  NULL
};

DECLARE_MODULE(devctl_jail, devctl_jail_mod, SI_SUB_KLD, SI_ORDER_ANY);
