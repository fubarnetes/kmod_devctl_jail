#include <sys/cdefs.h>
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
#include <sys/malloc.h>
#include <sys/sbuf.h>
#include "devctl.h"

#define DEVCTL_JAIL_LOGSIZE 128

static unsigned devctl_jail_osd_jail_slot;

static MALLOC_DEFINE(M_DEVCTL_JAIL, "devctl_jail", "Devctl jail notifications");

/*
 * Jail OSD methods
 */
static int
devctl_jail_prison_create(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  struct sbuf sb;
  char *buf;

  buf = malloc(DEVCTL_JAIL_LOGSIZE, M_DEVCTL_JAIL, M_NOWAIT);
  if (buf == NULL) {
    printf("devctl_jail_prison_create: out of memory\n");
    return (0);
  }
  sbuf_new(&sb, buf, DEVCTL_JAIL_LOGSIZE, SBUF_FIXEDLEN);
  sbuf_printf(&sb, "jid=%d", pr->pr_id);
  sbuf_finish(&sb);
  devctl_notify_f("kernel", "jail", "created", sbuf_data(&sb), M_NOWAIT);
  sbuf_delete(&sb);
  free(buf, M_DEVCTL_JAIL);
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
  struct sbuf sb;
  char *buf;

  buf = malloc(DEVCTL_JAIL_LOGSIZE, M_DEVCTL_JAIL, M_NOWAIT);
  if (buf == NULL) {
    printf("devctl_jail_prison_set: out of memory\n");
    return (0);
  }
  sbuf_new(&sb, buf, DEVCTL_JAIL_LOGSIZE, SBUF_FIXEDLEN);
  sbuf_printf(&sb, "jid=%d", pr->pr_id);
  sbuf_finish(&sb);
  devctl_notify_f("kernel", "jail", "updated", sbuf_data(&sb), M_NOWAIT);
  sbuf_delete(&sb);
  free(buf, M_DEVCTL_JAIL);
  return (0);
}

static int
devctl_jail_prison_get(void *obj, void *data __unused)
{
  return (0);
}

static int
devctl_jail_prison_remove(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  struct sbuf sb;
  char *buf;

  buf = malloc(DEVCTL_JAIL_LOGSIZE, M_DEVCTL_JAIL, M_NOWAIT);
  if (buf == NULL) {
    return (0);
  }
  sbuf_new(&sb, buf, DEVCTL_JAIL_LOGSIZE, SBUF_FIXEDLEN);
  sbuf_printf(&sb, "jid=%d", pr->pr_id);
  sbuf_finish(&sb);
  devctl_notify_f("kernel", "jail", "removed", sbuf_data(&sb), M_NOWAIT);
  sbuf_delete(&sb);
  free(buf, M_DEVCTL_JAIL);
  return (0);
}

static int
devctl_jail_prison_attach(void *obj, void *data __unused)
{
  struct prison* pr = obj;
  struct thread* td = data;
  struct sbuf sb;
  char *buf;

  buf = malloc(DEVCTL_JAIL_LOGSIZE, M_DEVCTL_JAIL, M_NOWAIT);
  if (buf == NULL) {
    printf("devctl_jail_prison_attach: out of memory\n");
    return (0);
  }
  sbuf_new(&sb, buf, DEVCTL_JAIL_LOGSIZE, SBUF_FIXEDLEN);
  sbuf_printf(&sb, "jid=%d pid=%d", pr->pr_id, td->td_proc->p_pid);
  sbuf_finish(&sb);
  devctl_notify_f("kernel", "jail", "attached", sbuf_data(&sb), M_NOWAIT);
  sbuf_delete(&sb);
  free(buf, M_DEVCTL_JAIL);
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
