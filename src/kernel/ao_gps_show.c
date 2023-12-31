/*
 * Copyright © 2013 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef AO_GPS_TEST
#include <ao.h>
#endif

#include <ao_data.h>

void
ao_gps_show(void) 
{
	uint8_t	i;
	ao_mutex_get(&ao_gps_mutex);
	printf ("Date: %02d/%02d/%02d\n", ao_gps_data.year, ao_gps_data.month, ao_gps_data.day);
	printf ("Time: %02d:%02d:%02d\n", ao_gps_data.hour, ao_gps_data.minute, ao_gps_data.second);
	printf ("Lat/Lon: %ld %ld\n", (long) ao_gps_data.latitude, (long) ao_gps_data.longitude);
#if HAS_WIDE_GPS
	printf ("Alt: %ld\n", (long) AO_TELEMETRY_LOCATION_ALTITUDE(&ao_gps_data));
#else
	printf ("Alt: %d\n", AO_TELEMETRY_LOCATION_ALTITUDE(&ao_gps_data));
#endif
	printf ("Pdop/Hdop/Vdop: %u %u %u\n", ao_gps_data.pdop, ao_gps_data.hdop, ao_gps_data.vdop);
	printf ("Ground Speed/Climb Rate/Course: %u %d %u\n", ao_gps_data.ground_speed,
		ao_gps_data.climb_rate, ao_gps_data.course);
	printf ("Flags: 0x%x\n", ao_gps_data.flags);
	printf ("Sats: %d", ao_gps_tracking_data.channels);
	for (i = 0; i < ao_gps_tracking_data.channels; i++)
		printf (" %d %d",
			ao_gps_tracking_data.sats[i].svid,
			ao_gps_tracking_data.sats[i].c_n_1);
	printf ("\ndone\n");
	ao_mutex_put(&ao_gps_mutex);
}
