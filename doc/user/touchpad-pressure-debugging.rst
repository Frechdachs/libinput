==============================================================================
Debugging touchpad pressure/size ranges
==============================================================================

:ref:`Touchpad pressure/size ranges <touchpad_pressure>` depend on
:ref:`device-quirks` entry specific to each laptop model. To check if a
pressure/size range is already defined for your device, use the
:ref:`libinput quirks <device-quirks-debugging>` tool: ::

     $ libinput quirks list /dev/input/event19

If your device does not list any quirks, it probably needs a touch
pressure/size range, a palm threshold and a thumb threshold. Start with
:ref:`touchpad_pressure_hwdb`, then :ref:`touchpad_touch_size_hwdb`. The
respective tools will exit if the required axis is not supported.


.. _touchpad_pressure_hwdb:

------------------------------------------------------------------------------
Debugging touchpad pressure ranges
------------------------------------------------------------------------------

This section describes how to determine the touchpad pressure ranges
required for a touchpad device and how to add the required
:ref:`device-quirks` locally. Note that the quirk is **not public API** and **may
change at any time**. Users are advised to :ref:`report a bug <reporting_bugs>`
with the updated pressure ranges when testing has completed.

Use the ``libinput measure touchpad-pressure`` tool provided by libinput.
This tool will search for your touchpad device and print some pressure
statistics, including whether a touch is/was considered logically down.

.. note:: This tool will only work on touchpads with pressure.

Example output of the tool is below: ::

     $ sudo libinput measure touchpad-pressure
     Ready for recording data.
     Pressure range used: 8:10
     Palm pressure range used: 65535
     Place a single finger on the touchpad to measure pressure values.
     Ctrl+C to exit
     &nbsp;
     Sequence 1190 pressure: min:  39 max:  48 avg:  43 median:  44 tags: down
     Sequence 1191 pressure: min:  49 max:  65 avg:  62 median:  64 tags: down
     Sequence 1192 pressure: min:  40 max:  78 avg:  64 median:  66 tags: down
     Sequence 1193 pressure: min:  36 max:  83 avg:  70 median:  73 tags: down
     Sequence 1194 pressure: min:  43 max:  76 avg:  72 median:  74 tags: down
     Touchpad pressure:  47 min:  47 max:  86 tags: down


The example output shows five completed touch sequences and one ongoing one.
For each, the respective minimum and maximum pressure values are printed as
well as some statistics. The ``tags`` show that sequence was considered
logically down at some point. This is an interactive tool and its output may
change frequently. Refer to the <i>libinput-measure-touchpad-pressure(1)</i> man
page for more details.

By default, this tool uses the :ref:`device-quirks` for the pressure range. To
narrow down on the best values for your device, specify the 'logically down'
and 'logically up' pressure thresholds with the  ``--touch-thresholds``
argument: ::

     $ sudo libinput measure touchpad-pressure --touch-thresholds=10:8 --palm-threshold=20


Interact with the touchpad and check if the output of this tool matches your
expectations.

.. note:: This is an interactive process. You will need to re-run the
          tool with varying thresholds until you find the right range for
          your touchpad. Attaching output logs to a bug will not help, only
          you with access to the hardware can figure out the correct
          ranges.

Once the thresholds are decided on (e.g. 10 and 8), they can be enabled with
:ref:`device-quirks` entry similar to this: ::

     $> cat /etc/libinput/local-overrides.quirks
     [Touchpad pressure override]
     MatchUdevType=touchpad
     MatchName=*SynPS/2 Synaptics TouchPad
     MatchDMIModalias=dmi:*svnLENOVO:*:pvrThinkPadX230*
     AttrPressureRange=10:8

The file name **must** be ``/etc/libinput/local-overrides.quirks``. The
The first line is the section name and can be free-form. The ``Match``
directives limit the quirk to your touchpad, make sure the device name
matches your device's name (see ``libinput record``'s output). The dmi
modalias match should be based on the information in
``/sys/class/dmi/id/modalias``.  This modalias should be shortened to the
specific system's information, usually system vendor (svn)
and product name (pn).

Once in place, run the following command to verify the quirk is valid and
works for your device: ::

     $ sudo libinput list-quirks /dev/input/event10
     AttrPressureRange=10:8

Replace the event node with the one from your device. If the
``AttrPressureRange`` quirk does not show up, re-run with ``--verbose`` and
check the output for any error messages.

If the pressure range quirk shows up correctly, restart X or the
Wayland compositor and libinput should now use the correct pressure
thresholds. The :ref:`tools` can be used to verify the correct
functionality first without the need for a restart.

Once the pressure ranges are deemed correct,
:ref:`report a bug <reporting_bugs>` to get the pressure ranges into the
repository.

.. _touchpad_touch_size_hwdb:

------------------------------------------------------------------------------
Debugging touch size ranges
------------------------------------------------------------------------------

This section describes how to determine the touchpad size ranges
required for a touchpad device and how to add the required
:ref:`device-quirks` locally. Note that the quirk is **not public API** and **may
change at any time**. Users are advised to :ref:`report a bug <reporting_bugs>`
with the updated pressure ranges when testing has completed.

Use the ``libinput measure touch-size`` tool provided by libinput.
This tool will search for your touchpad device and print some touch size
statistics, including whether a touch is/was considered logically down.

.. note:: This tool will only work on touchpads with the ``ABS_MT_MAJOR`` axis.

Example output of the tool is below: ::

     $ sudo libinput measure touch-size --touch-thresholds 10:8 --palm-threshold 14
     Using ELAN Touchscreen: /dev/input/event5
     &nbsp;
     Ready for recording data.
     Touch sizes used: 10:8
     Palm size used: 14
     Place a single finger on the device to measure touch size.
     Ctrl+C to exit
     &nbsp;
     Sequence: major: [  9.. 11] minor: [  7..  9]
     Sequence: major: [  9.. 10] minor: [  7..  7]
     Sequence: major: [  9.. 14] minor: [  6..  9]  down
     Sequence: major: [ 11.. 11] minor: [  9..  9]  down
     Sequence: major: [  4.. 33] minor: [  1..  5]  down palm

The example output shows five completed touch sequences. For each, the
respective minimum and maximum pressure values are printed as well as some
statistics. The ``down`` and ``palm`` tags show that sequence was considered
logically down or a palm at some point. This is an interactive tool and its
output may change frequently. Refer to the <i>libinput-measure-touch-size(1)</i> man
page for more details.

By default, this tool uses the :ref:`device-quirks` for the touch size range. To
narrow down on the best values for your device, specify the 'logically down'
and 'logically up' pressure thresholds with the  ``--touch-thresholds``
arguments as in the example above.

Interact with the touchpad and check if the output of this tool matches your
expectations.

.. note:: This is an interactive process. You will need to re-run the
          tool with varying thresholds until you find the right range for
          your touchpad. Attaching output logs to a bug will not help, only
          you with access to the hardware can figure out the correct
          ranges.

Once the thresholds are decided on (e.g. 10 and 8), they can be enabled with
:ref:`device-quirks` entry similar to this: ::

     $> cat /etc/libinput/local-overrides.quirks
     [Touchpad touch size override]
     MatchUdevType=touchpad
     MatchName=*SynPS/2 Synaptics TouchPad
     MatchDMIModalias=dmi:*svnLENOVO:*:pvrThinkPadX230*
     AttrTouchSizeRange=10:8

The first line is the match line and should be adjusted for the device name
(see evemu-record's output) and for the local system, based on the
information in ``/sys/class/dmi/id/modalias``. The modalias should be
shortened to the specific system's information, usually system vendor (svn)
and product name (pn).

Once in place, run the following command to verify the quirk is valid and
works for your device: ::

     $ sudo libinput list-quirks /dev/input/event10
     AttrTouchSizeRange=10:8

Replace the event node with the one from your device. If the
``AttrTouchSizeRange`` quirk does not show up, re-run with ``--verbose`` and
check the output for any error messages.

If the touch size range property shows up correctly, restart X or the
Wayland compositor and libinput should now use the correct thresholds.
The :ref:`tools` can be used to verify the correct functionality first without
the need for a restart.

Once the touch size ranges are deemed correct, :ref:`reporting_bugs` "report a
bug" to get the thresholds into the repository.

