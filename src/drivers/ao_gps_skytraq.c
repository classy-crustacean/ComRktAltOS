/*
 * Copyright © 2009 Keith Packard <keithp@keithp.com>
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
#include "ao.h"
#endif

#ifndef ao_gps_getchar
#define ao_gps_getchar	ao_serial1_getchar
#define ao_gps_fifo	ao_serial1_rx_fifo
#endif

#ifndef ao_gps_putchar
#define ao_gps_putchar	ao_serial1_putchar
#endif

#ifndef ao_gps_set_speed
#define ao_gps_set_speed	ao_serial1_set_speed
#endif

uint8_t ao_gps_new;
uint8_t ao_gps_mutex;
static char ao_gps_char;
static uint8_t ao_gps_cksum;
static uint8_t ao_gps_error;

AO_TICK_TYPE ao_gps_tick;
AO_TICK_TYPE ao_gps_utc_tick;
struct ao_telemetry_location	ao_gps_data;
struct ao_telemetry_satellite	ao_gps_tracking_data;

static AO_TICK_TYPE			ao_gps_next_tick;
static struct ao_telemetry_location	ao_gps_next;
static uint8_t				ao_gps_date_flags;
static struct ao_telemetry_satellite	ao_gps_tracking_next;

#define STQ_S 0xa0, 0xa1
#define STQ_E 0x0d, 0x0a
#define SKYTRAQ_MSG_2(id,a,b) \
    STQ_S, 0, 3, id, a,b, (id^a^b), STQ_E
#define SKYTRAQ_MSG_3(id,a,b,c) \
    STQ_S, 0, 4, id, a,b,c, (id^a^b^c), STQ_E
#define SKYTRAQ_MSG_8(id,a,b,c,d,e,f,g,h) \
    STQ_S, 0, 9, id, a,b,c,d,e,f,g,h, (id^a^b^c^d^e^f^g^h), STQ_E
#define SKYTRAQ_MSG_14(id,a,b,c,d,e,f,g,h,i,j,k,l,m,n) \
    STQ_S, 0,15, id, a,b,c,d,e,f,g,h,i,j,k,l,m,n, \
    (id^a^b^c^d^e^f^g^h^i^j^k^l^m^n), STQ_E

static const uint8_t ao_gps_config[] = {
	SKYTRAQ_MSG_8(0x08, 1, 0, 1, 0, 1, 0, 0, 0), /* configure nmea */
	/* gga interval */
	/* gsa interval */
	/* gsv interval */
	/* gll interval */
	/* rmc interval */
	/* vtg interval */
	/* zda interval */
	/* attributes (0 = update to sram, 1 = update flash too) */

	SKYTRAQ_MSG_2(0x3c, 0x00, 0x00), /* configure navigation mode */
	/* 0 = car, 1 = pedestrian */
	/* 0 = update to sram, 1 = update sram + flash */
};

static void
ao_gps_lexchar(void)
{
	char c;
	if (ao_gps_error)
		c = '\n';
	else
		c = ao_gps_getchar();
	ao_gps_cksum ^= c;
	ao_gps_char = c;
}

static void
ao_gps_skip_field(void)
{
	for (;;) {
		char c = ao_gps_char;
		if (c == ',' || c == '*' || c == '\n')
			break;
		ao_gps_lexchar();
	}
}

static void
ao_gps_skip_sep(void)
{
	char c = ao_gps_char;
	if (c == ',' || c == '.' || c == '*')
		ao_gps_lexchar();
}

static uint8_t ao_gps_num_width;

static int16_t
ao_gps_decimal(uint8_t max_width)
{
	int16_t	v;
	uint8_t	neg = 0;

	ao_gps_skip_sep();
	if (ao_gps_char == '-') {
		neg = 1;
		ao_gps_lexchar();
	}
	v = 0;
	ao_gps_num_width = 0;
	while (ao_gps_num_width < max_width) {
		uint8_t c = ao_gps_char;
		if (c < (uint8_t) '0' || (uint8_t) '9' < c)
			break;
		v = (int16_t) (v * 10 + (uint8_t) (c - (uint8_t) '0'));
		ao_gps_num_width++;
		ao_gps_lexchar();
	}
	if (neg)
		v = -v;
	return v;
}

static uint8_t
ao_gps_hex(void)
{
	uint8_t	v;

	ao_gps_skip_sep();
	v = 0;
	ao_gps_num_width = 0;
	while (ao_gps_num_width < 2) {
		uint8_t c = ao_gps_char;
		if ((uint8_t) '0' <= c && c <= (uint8_t) '9')
			c -= '0';
		else if ((uint8_t) 'A' <= c && c <= (uint8_t) 'F')
			c -= 'A' - 10;
		else if ((uint8_t) 'a' <= c && c <= (uint8_t) 'f')
			c -= 'a' - 10;
		else
			break;
		v = (uint8_t) ((v << 4) | c);
		ao_gps_num_width++;
		ao_gps_lexchar();
	}
	return v;
}

static int32_t
ao_gps_parse_pos(uint8_t deg_width) 
{
	int16_t		d;
	int16_t		m;
	int16_t		f;
	char c;

	d = ao_gps_decimal(deg_width);
	m = ao_gps_decimal(2);
	c = ao_gps_char;
	if (c == '.') {
		f = ao_gps_decimal(4);
		while (ao_gps_num_width < 4) {
			f *= 10;
			ao_gps_num_width++;
		}
	} else {
		f = 0;
		if (c != ',')
			ao_gps_error = 1;
	}
	return d * 10000000l + (m * 10000l + f) * 50 / 3;
}

static uint8_t
ao_gps_parse_flag(char no_c, char yes_c)
{
	uint8_t	ret = 0;
	ao_gps_skip_sep();
	if (ao_gps_char == yes_c)
		ret = 1;
	else if (ao_gps_char == no_c)
		ret = 0;
	else
		ao_gps_error = 1;
	ao_gps_lexchar();
	return ret;
}

static void
ao_nmea_finish(void)
{
	char c;
	/* Skip remaining fields */
	for (;;) {
		c = ao_gps_char;
		if (c == '*' || c == '\n' || c == '\r')
			break;
		ao_gps_lexchar();
		ao_gps_skip_field();
	}
	if (c == '*') {
		uint8_t cksum = ao_gps_cksum ^ '*';
		if (cksum != ao_gps_hex())
			ao_gps_error = 1;
	} else
		ao_gps_error = 1;
}

static void
ao_nmea_gga(void)
{
	uint8_t	i;

	/* Now read the data into the gps data record
	 *
	 * $GPGGA,025149.000,4528.1723,N,12244.2480,W,1,05,2.0,103.5,M,-19.5,M,,0000*66
	 *
	 * Essential fix data
	 *
	 *	   025149.000	time (02:51:49.000 GMT)
	 *	   4528.1723,N	Latitude 45°28.1723' N
	 *	   12244.2480,W	Longitude 122°44.2480' W
	 *	   1		Fix quality:
	 *				   0 = invalid
	 *				   1 = GPS fix (SPS)
	 *				   2 = DGPS fix
	 *				   3 = PPS fix
	 *				   4 = Real Time Kinematic
	 *				   5 = Float RTK
	 *				   6 = estimated (dead reckoning)
	 *				   7 = Manual input mode
	 *				   8 = Simulation mode
	 *	   05		Number of satellites (5)
	 *	   2.0		Horizontal dilution
	 *	   103.5,M		Altitude, 103.5M above msl
	 *	   -19.5,M		Height of geoid above WGS84 ellipsoid
	 *	   ?		time in seconds since last DGPS update
	 *	   0000		DGPS station ID
	 *	   *66		checksum
	 */

	ao_gps_next_tick = ao_time();
	ao_gps_next.flags = AO_GPS_RUNNING | ao_gps_date_flags;
	ao_gps_next.hour = (uint8_t) ao_gps_decimal(2);
	ao_gps_next.minute = (uint8_t) ao_gps_decimal(2);
	ao_gps_next.second = (uint8_t) ao_gps_decimal(2);
	ao_gps_skip_field();	/* skip seconds fraction */

	ao_gps_next.latitude = ao_gps_parse_pos(2);
	if (ao_gps_parse_flag('N', 'S'))
		ao_gps_next.latitude = -ao_gps_next.latitude;
	ao_gps_next.longitude = ao_gps_parse_pos(3);
	if (ao_gps_parse_flag('E', 'W'))
		ao_gps_next.longitude = -ao_gps_next.longitude;

	i = (uint8_t) ao_gps_decimal(0xff);
	if (i == 1)
		ao_gps_next.flags |= AO_GPS_VALID;

	i = (uint8_t) (ao_gps_decimal(0xff) << AO_GPS_NUM_SAT_SHIFT);
	if (i > AO_GPS_NUM_SAT_MASK)
		i = AO_GPS_NUM_SAT_MASK;
	ao_gps_next.flags |= i;

	ao_gps_lexchar();
	i = (uint8_t) ao_gps_decimal(0xff);
	if (i <= 25) {
		i = (uint8_t) 10 * i;
		if (ao_gps_char == '.')
			i = (i + ((uint8_t) ao_gps_decimal(1)));
	} else
		i = 255;
	ao_gps_next.hdop = i;
	ao_gps_skip_field();

	AO_TELEMETRY_LOCATION_SET_ALTITUDE(&ao_gps_next, ao_gps_decimal(0xff));

	ao_gps_skip_field();	/* skip any fractional portion */

	ao_nmea_finish();

	if (!ao_gps_error) {
		ao_mutex_get(&ao_gps_mutex);
		ao_gps_new |= AO_GPS_NEW_DATA;
		ao_gps_tick = ao_gps_utc_tick = ao_gps_next_tick;
		memcpy(&ao_gps_data, &ao_gps_next, sizeof (ao_gps_data));
		ao_mutex_put(&ao_gps_mutex);
		ao_wakeup(&ao_gps_new);
	}
}

static void
ao_nmea_gsv(void)
{
	uint8_t	c;
	uint8_t	i;
	uint8_t	done;
	/* Now read the data into the GPS tracking data record
	 *
	 * $GPGSV,3,1,12,05,54,069,45,12,44,061,44,21,07,184,46,22,78,289,47*72<CR><LF>
	 *
	 * Satellites in view data
	 *
	 *	3		Total number of GSV messages
	 *	1		Sequence number of current GSV message
	 *	12		Total sats in view (0-12)
	 *	05		SVID
	 *	54		Elevation
	 *	069		Azimuth
	 *	45		C/N0 in dB
	 *	...		other SVIDs
	 *	72		checksum
	 */
	c = (uint8_t) ao_gps_decimal(1);	/* total messages */
	i = (uint8_t) ao_gps_decimal(1);	/* message sequence */
	if (i == 1) {
		ao_gps_tracking_next.channels = 0;
	}
	done = (uint8_t) c == i;
	ao_gps_lexchar();
	ao_gps_skip_field();	/* sats in view */
	while (ao_gps_char != '*' && ao_gps_char != '\n' && ao_gps_char != '\r') {
		i = ao_gps_tracking_next.channels;
		c = (uint8_t) ao_gps_decimal(2);	/* SVID */
		if (i < AO_MAX_GPS_TRACKING)
			ao_gps_tracking_next.sats[i].svid = c;
		ao_gps_lexchar();
		ao_gps_skip_field();	/* elevation */
		ao_gps_lexchar();
		ao_gps_skip_field();	/* azimuth */
		c = (uint8_t) ao_gps_decimal(2);	/* C/N0 */
		if (i < AO_MAX_GPS_TRACKING) {
			if ((ao_gps_tracking_next.sats[i].c_n_1 = c) != 0)
				ao_gps_tracking_next.channels = i + 1;
		}
	}

	ao_nmea_finish();

	if (ao_gps_error)
		ao_gps_tracking_next.channels = 0;
	else if (done) {
		ao_mutex_get(&ao_gps_mutex);
		ao_gps_new |= AO_GPS_NEW_TRACKING;
		memcpy(&ao_gps_tracking_data, &ao_gps_tracking_next, sizeof(ao_gps_tracking_data));
		ao_mutex_put(&ao_gps_mutex);
		ao_wakeup(&ao_gps_new);
	}
}

static void
ao_nmea_rmc(void)
{
	uint8_t	a, c;
	uint8_t	i;
	/* Parse the RMC record to read out the current date */

	/* $GPRMC,111636.932,A,2447.0949,N,12100.5223,E,000.0,000.0,030407,,,A*61
	 *
	 * Recommended Minimum Specific GNSS Data
	 *
	 *	111636.932	UTC time 11:16:36.932
	 *	A		Data Valid (V = receiver warning)
	 *	2447.0949	Latitude
	 *	N		North/south indicator
	 *	12100.5223	Longitude
	 *	E		East/west indicator
	 *	000.0		Speed over ground
	 *	000.0		Course over ground
	 *	030407		UTC date (ddmmyy format)
	 *	A		Mode indicator:
	 *			N = data not valid
	 *			A = autonomous mode
	 *			D = differential mode
	 *			E = estimated (dead reckoning) mode
	 *			M = manual input mode
	 *			S = simulator mode
	 *	61		checksum
	 */
	ao_gps_skip_field();
	for (i = 0; i < 8; i++) {
		ao_gps_lexchar();
		ao_gps_skip_field();
	}
	a = (uint8_t) ao_gps_decimal(2);
	c = (uint8_t) ao_gps_decimal(2);
	i = (uint8_t) ao_gps_decimal(2);

	ao_nmea_finish();

	if (!ao_gps_error) {
		ao_gps_next.year = i;
		ao_gps_next.month = c;
		ao_gps_next.day = a;
		ao_gps_date_flags = AO_GPS_DATE_VALID;
	}
}

#define ao_skytraq_sendstruct(s) ao_skytraq_sendbytes((s), sizeof(s))

static void
ao_skytraq_sendbytes(const uint8_t *b, uint8_t l)
{
	while (l--) {
		uint8_t	c = *b++;
		if (c == 0xa0)
			ao_delay(AO_MS_TO_TICKS(500));
		ao_gps_putchar(c);
	}
}

static void
ao_gps_nmea_parse(void)
{
	uint8_t	a, b, c;

	ao_gps_cksum = 0;
	ao_gps_error = 0;

	ao_gps_lexchar();
	if (ao_gps_char != 'G')
		return;
	ao_gps_lexchar();
	if (ao_gps_char != 'P')
		return;

	ao_gps_lexchar();
	a = ao_gps_char;
	ao_gps_lexchar();
	b = ao_gps_char;
	ao_gps_lexchar();
	c = ao_gps_char;
	ao_gps_lexchar();

	if (ao_gps_char != ',')
		return;

	if (a == (uint8_t) 'G' && b == (uint8_t) 'G' && c == (uint8_t) 'A') {
		ao_nmea_gga();
	} else if (a == (uint8_t) 'G' && b == (uint8_t) 'S' && c == (uint8_t) 'V') {
		ao_nmea_gsv();
	} else if (a == (uint8_t) 'R' && b == (uint8_t) 'M' && c == (uint8_t) 'C') {
		ao_nmea_rmc();
	}
}

static uint8_t	ao_gps_updating;

void
ao_gps(void) 
{
	ao_gps_set_speed(AO_SERIAL_SPEED_9600);

	/* give skytraq time to boot in case of cold start */
	ao_delay(AO_MS_TO_TICKS(2000));

	ao_skytraq_sendstruct(ao_gps_config);

	for (;;) {
		/* Locate the begining of the next record */
		if (ao_gps_getchar() == '$') {
			ao_gps_nmea_parse();
		}
#ifndef AO_GPS_TEST
		while (ao_gps_updating) {
			ao_usb_putchar(ao_gps_getchar());
			if (ao_fifo_empty(ao_gps_fifo))
				flush();
		}
#endif
	}
}

struct ao_task ao_gps_task;

static const uint8_t ao_gps_115200[] = {
	SKYTRAQ_MSG_3(5,0,5,0)	/* Set to 115200 baud */
};

static void
ao_gps_set_speed_delay(uint8_t speed) {
	ao_delay(AO_MS_TO_TICKS(500));
	ao_gps_set_speed(speed);
	ao_delay(AO_MS_TO_TICKS(500));
}

static void
gps_update(void) 
{
	ao_gps_updating = 1;
	ao_task_minimize_latency = 1;
#if HAS_ADC
	ao_timer_set_adc_interval(0);
#endif
	ao_skytraq_sendstruct(ao_gps_115200);
	ao_gps_set_speed_delay(AO_SERIAL_SPEED_4800);
	ao_skytraq_sendstruct(ao_gps_115200);
	ao_gps_set_speed_delay(AO_SERIAL_SPEED_115200);

	/* It's a binary protocol; abandon attempts to escape */
	for (;;)
		ao_gps_putchar(ao_usb_getchar());
}

const struct ao_cmds ao_gps_cmds[] = {
	{ ao_gps_show, 	"g\0Display GPS" },
	{ gps_update,	"U\0Update GPS firmware" },
	{ 0, NULL },
};

void
ao_gps_init(void)
{
	ao_add_task(&ao_gps_task, ao_gps, "gps");
	ao_cmd_register(&ao_gps_cmds[0]);
}
