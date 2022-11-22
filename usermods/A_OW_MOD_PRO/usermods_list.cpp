#include "wled.h"
/*
 * Register your v2 usermods here!
 */

#ifdef ANDON_MOD_PRO
#include "../usermods/ANDON_MOD_PRO/usermod_ANDON_MOD_PRO.h"
#endif

void registerUsermods()
{
#ifdef USERMOD_ANDON_MOD_PRO
  usermods.add(new Usermod_ANDON_MOD_PRO());
#endif
}