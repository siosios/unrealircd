/*
 *   IRC - Internet Relay Chat, src/modules/out.c
 *   (C) 2004 The UnrealIRCd Team
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "config.h"
#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "proto.h"
#include "channel.h"
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include "h.h"
#ifdef _WIN32
#include "version.h"
#endif

DLLFUNC int m_admin(aClient *cptr, aClient *sptr, int parc, char *parv[]);

#define MSG_ADMIN 	"ADMIN"	

ModuleHeader MOD_HEADER(m_admin)
  = {
	"m_admin",
	"$Id$",
	"command /admin", 
	"3.2-b8-1",
	NULL 
    };

MOD_INIT(m_admin)
{
	CommandAdd(modinfo->handle, MSG_ADMIN, m_admin, MAXPARA, M_UNREGISTERED|M_USER|M_SHUN|M_VIRUS);
	MARK_AS_OFFICIAL_MODULE(modinfo);
	return MOD_SUCCESS;
}

MOD_LOAD(m_admin)
{
	return MOD_SUCCESS;
}

MOD_UNLOAD(m_admin)
{
	return MOD_SUCCESS;
}

/*
** m_admin
**	parv[0] = sender prefix
**	parv[1] = servername
*/
DLLFUNC CMD_FUNC(m_admin)
{
	ConfigItem_admin *admin;
	/* Users may want to get the address in case k-lined, etc. -- Barubary

	   * Only allow remote ADMINs if registered -- Barubary */
	if (IsPerson(sptr) || IsServer(cptr))
		if (hunt_server(cptr, sptr, ":%s ADMIN :%s", 1, parc, parv) != HUNTED_ISME)
			return 0;

	if (!conf_admin_tail)
	{
		sendto_one(sptr, err_str(ERR_NOADMININFO),
		    me.name, parv[0], me.name);
		return 0;
	}

	sendto_one(sptr, rpl_str(RPL_ADMINME), me.name, parv[0], me.name);

	/* cycle through the list backwards */
	for (admin = conf_admin_tail; admin;
	    admin = (ConfigItem_admin *) admin->prev)
	{
		if (!admin->next)
			sendto_one(sptr, rpl_str(RPL_ADMINLOC1),
			    me.name, parv[0], admin->line);
		else if (!admin->next->next)
			sendto_one(sptr, rpl_str(RPL_ADMINLOC2),
			    me.name, parv[0], admin->line);
		else
			sendto_one(sptr, rpl_str(RPL_ADMINEMAIL),
			    me.name, parv[0], admin->line);
	}
	return 0;
}
