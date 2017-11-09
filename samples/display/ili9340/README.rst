.. _ili9340-sample:

ILI9340 Display driver
######################

Overview
********

Sample application that every 500ms updates a rectangle in one of the LCD display corners.
The color used to fill the rectangle changes for every update and cycles through red, green and blue.

Requirements
************

- `ST NUCLEO-L476RG development board`_
- `Adafruit 2.2 inch TFT Display`_

Wiring
******

The NUCLEO-L476RG should be connected as follows to the Adafruit TFT display.

+------------------+-----------------+----------------+
| | NUCLEO-L476RG  | | NUCLEO-L476RG | | Adafruit TFT |
| | Arduino Header | | Pin           | | Pin          |
+==================+=================+================+
| D3               | PB3             | SCK            |
+------------------+-----------------+----------------+
| D7               | PA8             | D/C            |
+------------------+-----------------+----------------+
| D8               | PA9             | RST            |
+------------------+-----------------+----------------+
| D11              | PA7             | MOSI           |
+------------------+-----------------+----------------+
| D12              | PA6             | MISO           |
+------------------+-----------------+----------------+
| A2               | PA4             | NSS            |
+------------------+-----------------+----------------+

Building and Running
********************

This sample can be build with following commands.

.. code-block:: console

  $ cd $ZEPHYR_BASE/samples/display/ili9340
  $ mkdir build && cd build
  $ cmake -DBOARD=nucleo_l476rg ..
  $ make

See :ref:`nucleo_l476rg_board` on how to flash the build.

References
**********

- `ILI9340 datasheet`_

.. _Adafruit 2.2 inch TFT Display: https://www.adafruit.com/product/1480
.. _ST NUCLEO-L476RG development board: http://www.st.com/en/evaluation-tools/nucleo-l476rg.html
.. _ILI9340 datasheet: https://cdn-shop.adafruit.com/datasheets/ILI9340.pdf
