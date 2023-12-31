/*
 * Copyright © 2010 Keith Packard <keithp@keithp.com>
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

package org.altusmetrum.altoslib_14;

import java.text.*;
import java.util.concurrent.*;
import java.io.*;
import java.time.*;

public class AltosGPS implements Cloneable {

	public final static int MISSING = AltosLib.MISSING;

	public int	nsat;
	public boolean	locked;
	public boolean	connected;
	public double	lat;		/* degrees (+N -S) */
	public double	lon;		/* degrees (+E -W) */
	public double	alt;		/* m */
	public int	year;
	public int	month;
	public int	day;
	public int	hour;
	public int	minute;
	public int	second;

	public double	ground_speed;	/* m/s */
	public int	course;		/* degrees */
	public double	climb_rate;	/* m/s */
	public double	pdop;		/* unitless */
	public double	hdop;		/* unitless */
	public double	vdop;		/* unitless */
	public double	h_error;	/* m */
	public double	v_error;	/* m */

	public AltosGPSSat[] cc_gps_sat;	/* tracking data */

	public void ParseGPSDate(String date) throws ParseException {
		String[] ymd = date.split("-");
		if (ymd.length != 3)
			throw new ParseException("error parsing GPS date " + date + " got " + ymd.length, 0);
		year = AltosParse.parse_int(ymd[0]);
		month = AltosParse.parse_int(ymd[1]);
		day = AltosParse.parse_int(ymd[2]);
	}

	public void ParseGPSTime(String time) throws ParseException {
		String[] hms = time.split(":");
		if (hms.length != 3)
			throw new ParseException("Error parsing GPS time " + time + " got " + hms.length, 0);
		hour = AltosParse.parse_int(hms[0]);
		minute = AltosParse.parse_int(hms[1]);
		second = AltosParse.parse_int(hms[2]);
	}

	public void ClearGPSTime() {
		year = month = day = AltosLib.MISSING;
		hour = minute = second = AltosLib.MISSING;
	}

	/* Return time since epoc in seconds */
	public long seconds() {
		if (year == AltosLib.MISSING)
			return AltosLib.MISSING;
		if (month == AltosLib.MISSING)
			return AltosLib.MISSING;
		if (day == AltosLib.MISSING)
			return AltosLib.MISSING;
		if (hour == AltosLib.MISSING)
			return AltosLib.MISSING;
		if (minute == AltosLib.MISSING)
			return AltosLib.MISSING;
		if (second == AltosLib.MISSING)
			return AltosLib.MISSING;
		OffsetDateTime	odt = OffsetDateTime.of(year, month, day, hour, minute, second, 0, ZoneOffset.UTC);
		return odt.toEpochSecond();
	}

	public  AltosLatLon lat_lon() {
		return new AltosLatLon(lat, lon);
	}

	public AltosGPS(AltosTelemetryMap map) throws ParseException {
		String	state = map.get_string(AltosTelemetryLegacy.AO_TELEM_GPS_STATE,
					       AltosTelemetryLegacy.AO_TELEM_GPS_STATE_ERROR);

		nsat = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_NUM_SAT, 0);
		if (state.equals(AltosTelemetryLegacy.AO_TELEM_GPS_STATE_LOCKED)) {
			connected = true;
			locked = true;
			lat = map.get_double(AltosTelemetryLegacy.AO_TELEM_GPS_LATITUDE, MISSING, 1.0e-7);
			lon = map.get_double(AltosTelemetryLegacy.AO_TELEM_GPS_LONGITUDE, MISSING, 1.0e-7);
			alt = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_ALTITUDE, MISSING);
			year = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_YEAR, MISSING);
			if (year != MISSING)
				year += 2000;
			month = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_MONTH, MISSING);
			day = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_DAY, MISSING);

			hour = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_HOUR, 0);
			minute = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_MINUTE, 0);
			second = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_SECOND, 0);

			ground_speed = map.get_double(AltosTelemetryLegacy.AO_TELEM_GPS_HORIZONTAL_SPEED,
						      AltosLib.MISSING, 1/100.0);
			course = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_COURSE,
					     AltosLib.MISSING);
			pdop = map.get_double(AltosTelemetryLegacy.AO_TELEM_GPS_PDOP, MISSING, 1.0);
			hdop = map.get_double(AltosTelemetryLegacy.AO_TELEM_GPS_HDOP, MISSING, 1.0);
			vdop = map.get_double(AltosTelemetryLegacy.AO_TELEM_GPS_VDOP, MISSING, 1.0);
			h_error = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_HERROR, MISSING);
			v_error = map.get_int(AltosTelemetryLegacy.AO_TELEM_GPS_VERROR, MISSING);
		} else if (state.equals(AltosTelemetryLegacy.AO_TELEM_GPS_STATE_UNLOCKED)) {
			connected = true;
			locked = false;
		} else {
			connected = false;
			locked = false;
		}
	}

	public boolean parse_string (String line, boolean says_done) {
		String[] bits = line.split("\\s+");
		if (bits.length == 0)
			return false;
		if (line.startsWith("Date:")) {
			if (bits.length < 2)
				return false;
			String[] d = bits[1].split("/");
			if (d.length < 3)
				return false;
			year = Integer.parseInt(d[0]) + 2000;
			month = Integer.parseInt(d[1]);
			day = Integer.parseInt(d[2]);
		} else if (line.startsWith("Time:")) {
			if (bits.length < 2)
				return false;
			String[] d = bits[1].split(":");
			if (d.length < 3)
				return false;
			hour = Integer.parseInt(d[0]);
			minute = Integer.parseInt(d[1]);
			second = Integer.parseInt(d[2]);
		} else if (line.startsWith("Lat/Lon:")) {
			if (bits.length < 3)
				return false;
			lat = Integer.parseInt(bits[1]) * 1.0e-7;
			lon = Integer.parseInt(bits[2]) * 1.0e-7;
		} else if (line.startsWith("Alt:")) {
			if (bits.length < 2)
				return false;
			alt = Integer.parseInt(bits[1]);
		} else if (line.startsWith("Pdop/Hdop/Vdop:")) {
			if (bits.length < 4)
				return false;
			pdop = Integer.parseInt(bits[1]) / 10.0;
			hdop = Integer.parseInt(bits[2]) / 10.0;
			vdop = Integer.parseInt(bits[3]) / 10.0;
		} else if (line.startsWith("Ground Speed/Climb Rate/Course:")) {
			if (bits.length < 6)
				return false;
			ground_speed = Integer.parseInt(bits[3]) * 1.0e-2;
			climb_rate = Integer.parseInt(bits[4]) * 1.0e-2;
			course = Integer.parseInt(bits[5]) * 2;
		} else if (line.startsWith("Flags:")) {
			if (bits.length < 2)
				return false;
			int status = Integer.decode(bits[1]);
			connected = (status & AltosLib.AO_GPS_RUNNING) != 0;
			locked = (status & AltosLib.AO_GPS_VALID) != 0;
			nsat = (status >> AltosLib.AO_GPS_NUM_SAT_SHIFT) & AltosLib.AO_GPS_NUM_SAT_MASK;
			if (!says_done)
				return false;
		} else if (line.startsWith("Sats:")) {
			if (bits.length < 2)
				return false;
			int nsvs = Integer.parseInt(bits[1]);
			cc_gps_sat = new AltosGPSSat[nsvs];
			for (int i = 0; i < nsvs; i++) {
				int	svid = Integer.parseInt(bits[2+i*2]);
				int	cc_n0 = Integer.parseInt(bits[3+i*2]);
				cc_gps_sat[i] = new AltosGPSSat(svid, cc_n0);
			}
		} else if (line.startsWith("done")) {
			return false;
		} else
			return false;
		return true;
	}

	public AltosGPS(String[] words, int i, int version) throws ParseException {
		AltosParse.word(words[i++], "GPS");
		nsat = AltosParse.parse_int(words[i++]);
		AltosParse.word(words[i++], "sat");

		connected = false;
		locked = false;
		lat = lon = 0;
		alt = 0;
		ClearGPSTime();
		if ((words[i]).equals("unlocked")) {
			connected = true;
			i++;
		} else if ((words[i]).equals("not-connected")) {
			i++;
		} else if (words.length >= 40) {
			locked = true;
			connected = true;

			if (version > 1)
				ParseGPSDate(words[i++]);
			else
				year = month = day = 0;
			ParseGPSTime(words[i++]);
			lat = AltosParse.parse_coord(words[i++]);
			lon = AltosParse.parse_coord(words[i++]);
			alt = AltosParse.parse_int(words[i++]);
			if (version > 1 || (i < words.length && !words[i].equals("SAT"))) {
				ground_speed = AltosParse.parse_double_net(AltosParse.strip_suffix(words[i++], "m/s(H)"));
				course = AltosParse.parse_int(words[i++]);
				climb_rate = AltosParse.parse_double_net(AltosParse.strip_suffix(words[i++], "m/s(V)"));
				hdop = AltosParse.parse_double_net(AltosParse.strip_suffix(words[i++], "(hdop)"));
				h_error = AltosParse.parse_int(words[i++]);
				v_error = AltosParse.parse_int(words[i++]);
			}
		} else {
			i++;
		}
		if (i < words.length) {
			AltosParse.word(words[i++], "SAT");
			int tracking_channels = 0;
			if (words[i].equals("not-connected"))
				tracking_channels = 0;
			else
				tracking_channels = AltosParse.parse_int(words[i]);
			i++;
			cc_gps_sat = new AltosGPSSat[tracking_channels];
			for (int chan = 0; chan < tracking_channels; chan++) {
				cc_gps_sat[chan] = new AltosGPSSat();
				cc_gps_sat[chan].svid = AltosParse.parse_int(words[i++]);
				/* Older versions included SiRF status bits */
				if (version < 2)
					i++;
				cc_gps_sat[chan].c_n0 = AltosParse.parse_int(words[i++]);
			}
		} else
			cc_gps_sat = new AltosGPSSat[0];
	}

	public void set_latitude(int in_lat) {
		lat = in_lat / 10.0e7;
	}

	public void set_longitude(int in_lon) {
		lon = in_lon / 10.0e7;
	}

	public void set_time(int in_hour, int in_minute, int in_second) {
		hour = in_hour;
		minute = in_minute;
		second = in_second;
	}

	public void set_date(int in_year, int in_month, int in_day) {
		year = in_year;
		month = in_month;
		day = in_day;
	}

	/*
	public void set_flags(int in_flags) {
		flags = in_flags;
	}
	*/

	public void set_altitude(int in_altitude) {
		alt = in_altitude;
	}

	public void add_sat(int svid, int c_n0) {
		if (cc_gps_sat == null) {
			cc_gps_sat = new AltosGPSSat[1];
		} else {
			AltosGPSSat[] new_gps_sat = new AltosGPSSat[cc_gps_sat.length + 1];
			for (int i = 0; i < cc_gps_sat.length; i++)
				new_gps_sat[i] = cc_gps_sat[i];
			cc_gps_sat = new_gps_sat;
		}
		AltosGPSSat	sat = new AltosGPSSat();
		sat.svid = svid;
		sat.c_n0 = c_n0;
		cc_gps_sat[cc_gps_sat.length - 1] = sat;
	}

	private void init() {
		lat = AltosLib.MISSING;
		lon = AltosLib.MISSING;
		alt = AltosLib.MISSING;
		ground_speed = AltosLib.MISSING;
		course = AltosLib.MISSING;
		climb_rate = AltosLib.MISSING;
		pdop = AltosLib.MISSING;
		hdop = AltosLib.MISSING;
		vdop = AltosLib.MISSING;
		h_error = AltosLib.MISSING;
		v_error = AltosLib.MISSING;
		ClearGPSTime();
		cc_gps_sat = null;
	}

	public AltosGPS() {
		init();
	}

	public AltosGPS clone() {
		AltosGPS	g = new AltosGPS();

		g.nsat = nsat;
		g.locked = locked;
		g.connected = connected;
		g.lat = lat;		/* degrees (+N -S) */
		g.lon = lon;		/* degrees (+E -W) */
		g.alt = alt;		/* m */
		g.year = year;
		g.month = month;
		g.day = day;
		g.hour = hour;
		g.minute = minute;
		g.second = second;

		g.ground_speed = ground_speed;	/* m/s */
		g.course = course;		/* degrees */
		g.climb_rate = climb_rate;	/* m/s */
		g.pdop = pdop;		/* unitless */
		g.hdop = hdop;		/* unitless */
		g.vdop = vdop;		/* unitless */
		g.h_error = h_error;	/* m */
		g.v_error = v_error;	/* m */

		if (cc_gps_sat != null) {
			g.cc_gps_sat = new AltosGPSSat[cc_gps_sat.length];
			for (int i = 0; i < cc_gps_sat.length; i++) {
				g.cc_gps_sat[i] = new AltosGPSSat(cc_gps_sat[i].svid,
								  cc_gps_sat[i].c_n0);
			}
		}
		return g;
	}

	public AltosGPS(AltosGPS old) {
		if (old != null) {
			nsat = old.nsat;
			locked = old.locked;
			connected = old.connected;
			lat = old.lat;		/* degrees (+N -S) */
			lon = old.lon;		/* degrees (+E -W) */
			alt = old.alt;		/* m */
			year = old.year;
			month = old.month;
			day = old.day;
			hour = old.hour;
			minute = old.minute;
			second = old.second;

			ground_speed = old.ground_speed;	/* m/s */
			course = old.course;		/* degrees */
			climb_rate = old.climb_rate;	/* m/s */
			pdop = old.pdop;		/* unitless? */
			hdop = old.hdop;		/* unitless? */
			vdop = old.vdop;		/* unitless? */
			h_error = old.h_error;		/* m */
			v_error = old.v_error;		/* m */

			if (old.cc_gps_sat != null) {
				cc_gps_sat = new AltosGPSSat[old.cc_gps_sat.length];
				for (int i = 0; i < old.cc_gps_sat.length; i++) {
					cc_gps_sat[i] = new AltosGPSSat();
					cc_gps_sat[i].svid = old.cc_gps_sat[i].svid;
					cc_gps_sat[i].c_n0 = old.cc_gps_sat[i].c_n0;
				}
			}
		} else {
			init();
		}
	}

	static public void provide_data(AltosDataListener listener, AltosLink link) throws InterruptedException {
		try {
			AltosGPS gps = new AltosGPS(link, link.config_data());
			if (gps != null)
				listener.set_gps(gps, true, true);
		} catch (TimeoutException te) {
		}
	}

	public AltosGPS (AltosLink link, AltosConfigData config_data) throws TimeoutException, InterruptedException {
		boolean says_done = config_data.compare_version("1.0") >= 0;
		init();
		link.printf("g\n");
		for (;;) {
			String line = link.get_reply_no_dialog(5000);
			if (line == null)
				throw new TimeoutException();
			if (!parse_string(line, says_done))
				break;
		}
	}
}
