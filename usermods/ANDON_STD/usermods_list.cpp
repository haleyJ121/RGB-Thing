#include "wled.h"
/*
 * Register your v2 usermods here!
 */
#ifdef ANDON_STD
#include "../usermods/ANDON_STD/usermod_ANDON_STD.h"
#endif

void registerUsermods()
{
#ifdef USERMOD_ANDON_STD
  usermods.add(new Usermod_ANDON_STD());
#endif
}