#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>  /* uprintf */
#include <sys/errno.h>
#include <sys/param.h>  /* defines used in kernel.h */
#include <sys/kernel.h> /* types used in module initialization */
#include <sys/jail.h>
#include <sys/proc.h>
#include <sys/lock.h>
#include <sys/sx.h>

static unsigned devctl_jail_osd_jail_slot;

/*
 * Jail OSD methods
 */
static int
devctl_jail_prison_create(void *obj, void *data __unused)
{
  //struct prison* pr = obj;
  uprintf("created jail");
  return (0);
}

static int
devctl_jail_prison_check(void *obj __unused, void *data __unused)
{
  return (0);
}

static int
devctl_jail_prison_set(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  uprintf("set jail %d", pr->pr_id);
  return (0);
}

static int
devctl_jail_prison_get(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  uprintf("get jail %d", pr->pr_id);
  return (0);
}

static int
devctl_jail_prison_remove(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  uprintf("remove jail %d", pr->pr_id);
  return (0);
}

static int
devctl_jail_prison_attach(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  struct thread* td = data;
  uprintf("attached PID %d to jail %d", td->td_proc->p_pid, pr->pr_id);
  return (0);
}

static void
devctl_jail_prison_destructor(void *data)
{
}

static void
devctl_jail_osd_jail_register(void)
{
	osd_method_t methods[PR_MAXMETHOD] = {
	    [PR_METHOD_CREATE] =	devctl_jail_prison_create,
	    [PR_METHOD_GET] =		devctl_jail_prison_get,
	    [PR_METHOD_SET] =		devctl_jail_prison_set,
	    [PR_METHOD_REMOVE] =	devctl_jail_prison_remove,
	    [PR_METHOD_ATTACH] =	devctl_jail_prison_attach,
	    [PR_METHOD_CHECK] =		devctl_jail_prison_check
	};

	devctl_jail_osd_jail_slot =
	    osd_jail_register(devctl_jail_prison_destructor, methods);
}

static void
devctl_jail_osd_jail_deregister(void)
{

	osd_jail_deregister(devctl_jail_osd_jail_slot);
}


static int
devctl_jail_loader(struct module *m, int what, void *arg)
{
  int err = 0;

  switch (what) {
  case MOD_LOAD:                /* kldload */
    devctl_jail_osd_jail_register();
    uprintf("devctl_jail KLD loaded.\n");
    break;
  case MOD_UNLOAD:
    devctl_jail_osd_jail_deregister();
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
