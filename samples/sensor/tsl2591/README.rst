.. _tsl2591-sample:

MS5837 Sensor Sample
####################

Overview
********

Sample application that every 5 seconds retrieves the visible and infrared irradiance
from a TSL2591 sensor and print this information to the UART console

Requirements
************

- `nRF52840 Preview development kit`_
- TSL2591 sensor

Wiring
******

The nrf52840 Preview development kit should be connected as follows to the
TSL2591 sensor.

+-------------+-----------+
| | nrf52840  | | TSL2591 |
| | Pin       | | Pin     |
+=============+===========+
| P0.3        | SCL       |
+-------------+-----------+
| P0.31       | SDA       |
+-------------+-----------+

Building and Running
********************

Build this sample using the following commands:

.. zephyr-app-commands::
   :zephyr-app: samples/sensor/tsl2591
   :board: nrf52840_pca10056
   :goals: build
   :compact:

See :ref:`nrf52840_pca10056` on how to flash the build.

References
**********

.. target-notes::

.. _TSL2591 Sensor: https://ams.com/tsl25911
.. _nRF52840 Preview development kit: http://www.nordicsemi.com/eng/Products/nRF52840-Preview-DK
