.. zephyr:board:: nrf52840dk

Overview
********

The nRF52840 Development Kit (PCA10056) hardware provides support for the
Nordic Semiconductor nRF52840 ARM Cortex-M4F CPU and the following devices:

* :abbr:`ADC (Analog to Digital Converter)`
* CLOCK
* FLASH
* :abbr:`GPIO (General Purpose Input Output)`
* :abbr:`I2C (Inter-Integrated Circuit)`
* :abbr:`MPU (Memory Protection Unit)`
* :abbr:`NVIC (Nested Vectored Interrupt Controller)`
* :abbr:`PWM (Pulse Width Modulation)`
* RADIO (Bluetooth Low Energy and 802.15.4)
* :abbr:`RTC (nRF RTC System Clock)`
* Segger RTT (RTT Console)
* :abbr:`SPI (Serial Peripheral Interface)`
* :abbr:`UART (Universal asynchronous receiver-transmitter)`
* :abbr:`USB (Universal Serial Bus)`
* :abbr:`WDT (Watchdog Timer)`

More information about the board can be found at the `nRF52840 DK website`_.
`nRF52840 Product Specification`_ contains the processor's information
and the datasheet.


Hardware
********

nRF52840 DK has two external oscillators. The frequency of the slow clock
is 32.768 kHz. The frequency of the main clock is 32 MHz.

Supported Features
==================

.. zephyr:board-supported-hw::

See `nRF52840 DK website`_ and `nRF52840 DK Hardware guide`_
for a complete list of nRF52840 Development Kit board hardware features.

Connections and IOs
===================

LED
---

* LED1 (green) = P0.13
* LED2 (green) = P0.14
* LED3 (green) = P0.15
* LED4 (green) = P0.16

Push buttons
------------

* BUTTON1 = SW1 = P0.11
* BUTTON2 = SW2 = P0.12
* BUTTON3 = SW3 = P0.24
* BUTTON4 = SW4 = P0.25
* BOOT = SW5 = boot/reset

Programming and Debugging
*************************

.. zephyr:board-supported-runners::

Applications for the ``nrf52840dk/nrf52840`` board configuration can be
built, flashed, and debugged in the usual way. See
:ref:`build_an_application` and :ref:`application_run` for more details on
building and running.

Flashing
========

Follow the instructions in the :ref:`nordic_segger` page to install
and configure all the necessary software. Further information can be
found in :ref:`nordic_segger_flashing`. Then build and flash
applications as usual (see :ref:`build_an_application` and
:ref:`application_run` for more details).

Here is an example for the :zephyr:code-sample:`hello_world` application.

First, run your favorite terminal program to listen for output.

.. code-block:: console

   $ minicom -D <tty_device> -b 115200

Replace :code:`<tty_device>` with the port where the board nRF52840 DK
can be found. For example, under Linux, :code:`/dev/ttyACM0`.

Then build and flash the application in the usual way.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: nrf52840dk/nrf52840
   :goals: build flash

Debugging
=========

Refer to the :ref:`nordic_segger` page to learn about debugging Nordic boards with a
Segger IC.


Testing the LEDs and buttons in the nRF52840 DK
***********************************************

There are 2 samples that allow you to test that the buttons (switches) and LEDs on
the board are working properly with Zephyr:

.. code-block:: console

   samples/basic/blinky
   samples/basic/button

You can build and flash the examples to make sure Zephyr is running correctly on
your board. The button and LED definitions can be found in
:zephyr_file:`boards/nordic/nrf52840dk/nrf52840dk_nrf52840.dts`.

Changing UART1 pins
*******************

The following approach can be used when an application needs to use another set
of pins for UART1:

1. Add devicetree overlay file to the main directory of your application:

   .. code-block:: devicetree

      &pinctrl {
         uart1_default_alt: uart1_default_alt {
            group1 {
               psels = <NRF_PSEL(UART_TX, 0, 14)>,
                       <NRF_PSEL(UART_RX, 0, 16)>;
            };
         };
         /* required if CONFIG_PM_DEVICE=y */
         uart1_sleep_alt: uart1_sleep_alt {
            group1 {
               psels = <NRF_PSEL(UART_TX, 0, 14)>,
                       <NRF_PSEL(UART_RX, 0, 16)>;
               low-power-enable;
            };
         };
      };

      &uart1 {
        pinctrl-0 = <&uart1_default_alt>;
        /* if sleep state is not used, use /delete-property/ pinctrl-1; and
         * skip the "sleep" entry.
         */
        pinctrl-1 = <&uart1_sleep_alt>;
        pinctrl-names = "default", "sleep";
      };

   In the overlay file above, pin P0.16 is used for RX and P0.14 is used for TX

See :ref:`set-devicetree-overlays` for further details.

Selecting the pins
==================

Pins can be configured in the board pinctrl file. To see the available mappings,
open the `nRF52840 Product Specification`_, chapter 7 'Hardware and Layout'.
In the table 7.1.1 'aQFN73 ball assignments' select the pins marked
'General purpose I/O'.  Note that pins marked as 'low frequency I/O only' can only be used
in under-10KHz applications. They are not suitable for 115200 speed of UART.

.. _nrf52840dk_nrf52811:

nRF52811 emulation on nRF52840 DK
*********************************

The ``nrf52840dk/nrf52811`` board variant is a modified version of the
:zephyr:board:`nrf52840dk` that enforces the limitations imposed by the nRF52811
IC, which is a variant of the original nRF52840. Since Nordic does not offer a
development kit for the nRF52811 you can use this board to develop for this IC
while using the nRF52840 Development Kit (PCA10056).

See `nRF52811 website`_ for the official reference on the IC itself.

References
**********

.. target-notes::

.. _nRF52840 DK website: https://www.nordicsemi.com/Software-and-Tools/Development-Kits/nRF52840-DK
.. _nRF52840 Product Specification: https://docs.nordicsemi.com/bundle/ps_nrf52840/page/keyfeatures_html5.html
.. _nRF52840 DK Hardware guide: https://docs.nordicsemi.com/bundle/ug_nrf52840_dk/page/UG/dk/intro.html
.. _J-Link Software and documentation pack: https://www.segger.com/jlink-software.html
.. _nRF52811 website: https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52811
