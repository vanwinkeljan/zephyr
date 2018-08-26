.. _lvgl-keypad-sample:

LittlevGL Keypad Sample
#######################

Overview
********

This sample application displays a number of switches and LEDs that can be
controlled via buttons. The up and down buttons, if available, are used to
navigate between the switches and the left/right buttons can used to turn-off
and on the LEDs.

In case that the sample application runs in the native POSIX port the arrow keys
are used for navigation.

Requirements
************

- `nRF52840 Preview development kit`_
- `Adafruit 2.8 inch TFT Touch Shield`_

or a simulated display environment in a native Posix application:

- :ref:`native_posix`
- `SDL2`_

Building and Running
********************

Build this sample using the following commands:

.. zephyr-app-commands::
   :zephyr-app: samples/gui/lvgl_keypad
   :board: nrf52840_pca10056
   :goals: build
   :compact:

See :ref:`nrf52840_pca10056` on how to flash the build.

or

.. zephyr-app-commands::
   :zephyr-app: samples/gui/lvgl_keypad
   :board: native_posix
   :goals: build
   :compact:

References
**********

.. target-notes::

.. _LittlevGL Web Page: https://littlevgl.com/
.. _Adafruit 2.8 inch TFT Touch Shield: https://www.adafruit.com/product/1947
.. _nRF52840 Preview development kit: http://www.nordicsemi.com/eng/Products/nRF52840-Preview-DK
.. _SDL2: https://www.libsdl.org
