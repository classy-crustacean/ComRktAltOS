= Release Notes for Version 1.1
include::release-head.adoc[]
:doctype: article

	Version 1.1 is a minor release. It provides a few new features
	in AltosUI and the AltOS firmware and fixes bugs.
  
	== AltOS

		AltOS Firmware New Features:

		* Add apogee-lockout value. Overrides the apogee
		  detection logic to prevent incorrect apogee charge
		  firing.

		* Force the radio frequency to 434.550MHz when the
		  debug clock pin is connected to ground at boot
		  time. This provides a way to talk to a TeleMini
		  which is configured to some unknown frequency.

		* Provide RSSI values for Monitor Idle mode. This
		  makes it easy to check radio range without needing
		  to go to flight mode.

		AltOS Fixes:

		* Fix a bug where the data reported in telemetry
		  packets was from 320ms ago.

		* Fix a bug which caused the old received telemetry
		  packets to be retransmitted over the USB link when
		  the radio was turned off and back on.
  
	== AltosUI

		AltosUI New Features:

		* Make the look-n-feel configurable, providing a choice from
		  the available options.

		* Add an 'Age' element to mark how long since a
		  telemetry packet has been received. Useful to
		  quickly gauge whether communications with the rocket
		  are still active.

		* Add 'Configure Ground Station' dialog to set the
		  radio frequency used by a particular TeleDongle
		  without having to go through the flight monitor UI.

		* Add configuration for the new apogee-lockout
		  value. A menu provides a list of reasonable values,
		  or the value can be set by hand.

		* Add Imperial units mode to present data in feet
		  instead of meters.

		AltosUI Fixes:

		* Fix a bug that caused GPS ready to happen too
		  quickly. The software was using every telemetry
		  packet to signal new GPS data, which caused GPS
		  ready to be signalled after 10 packets instead of 10
		  GPS updates.

		* Fix Google Earth data export to work with recent
		  versions. The google earth file loading code got a
		  lot pickier, requiring some minor white space
		  changes in the export code.

		* Changed how flight data are downloaded. Now there's
		  an initial dialog asking which flights to download,
		  and after that finishes, a second dialog comes up
		  asking which flights to delete.

		* Re-compute time spent in each state for the flight
		  graph; this figures out the actual boost and landing
		  times instead of using the conservative values
		  provide by the flight electronics. This improves the
		  accuracy of the boost acceleration and main descent
		  rate computations.

		* Make AltosUI run on Mac OS Lion. The default Java
		  heap space was dramatically reduced for this release
		  causing much of the UI to fail randomly. This most
		  often affected the satellite mapping download and
		  displays.

		* Change how data are displayed in the 'table' tab of
		  the flight monitoring window. This eliminates
		  entries duplicated from the header and adds both
		  current altitude and pad altitude, which are useful
		  in 'Monitor Idle' mode.
