/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#ifndef PIGPIOD_IF_H
#define PIGPIOD_IF_H

#include "pigpio.h"

#define PIGPIOD_IF_VERSION 1

typedef enum
{
   pigif_bad_send           = -2000,
   pigif_bad_recv           = -2001,
   pigif_bad_getaddrinfo    = -2002,
   pigif_bad_connect        = -2003,
   pigif_bad_socket         = -2004,
   pigif_bad_noib           = -2005,
   pigif_duplicate_callback = -2006,
   pigif_bad_malloc         = -2007,
   pigif_bad_callback       = -2008,
   pigif_notify_failed      = -2009,
   pigif_callback_not_found = -2010,
} piscopeError_t;


typedef void (*CBFunc_t)  (int gpio, int level, uint32_t tick);

typedef void (*CBFuncEx_t)(int gpio, int level, uint32_t tick, void * user);

typedef struct callback_s callback_t;

#define RISING_EDGE  0
#define FALLING_EDGE 1
#define EITHER_EDGE  2

double time_time(void);
/* Return the current time in seconds since the Epoch.*/

void time_sleep(double seconds);
/* Delay execution for a given number of seconds */

const char *pigpio_error(int error);
/* Return a string for a pigpio library error. */

unsigned pigpiod_if_version(void);
/* Return the pigpiod_if version. */

pthread_t *start_thread(ThreadFunc_t func, void *arg);
/* Starts a new thread of execution with func as the main routine.

   Returns a pointer to pthread_t if OK, otherwise NULL.

   The function is passed the single argument arg.

   The thread can be cancelled by passing the pointer to pthread_t to
   gpioStopThread().
*/

void stop_thread(pthread_t *pth);
/* Cancels the thread pointed at by pth.

   No value is returned.

   The thread to be stopped should have been started with gpioStartThread().
*/

int pigpio_start(char *addrStr, char *portStr);
/* Connect to the pigpio daemon.  Reserving command and
   notification streams.

   addrStr specifies the host or IP address of the Pi running
   the pigpio daemon.  It may be NULL in which case localhost
   is used unless overriden by the PIGPIO_ADDR environment
   variable.

   portStr specifies the port address used by the Pi running
   the pigpio daemon.  It may be NULL in which case "8888"
   is used unless overriden by the PIGPIO_PORT environment
   variable.
*/

void pigpio_stop(void);
/*
   Terminates the connection to the pigpio daemon and releases
   resources used by the library.
*/

int set_mode(int gpio, int mode);
/* Set the gpio mode.

   gpio: 0-53.
   mode: INPUT, OUTPUT, ALT0, ALT1, ALT2, ALT3, ALT4, ALT5.

   Returns 0 if OK, otherwise PI_BAD_GPIO, PI_BAD_MODE,
   or PI_NOT_PERMITTED.
*/

int get_mode(int gpio);
/* Get the gpio mode.

   Returns the gpio mode if OK, otherwise PI_BAD_GPIO.

   gpio: 0-53.
*/

int set_pull_up_down(int gpio, int pud);
/* Set or clear the gpio pull-up/down resistor.

   Returns 0 if OK, otherwise PI_BAD_GPIO, PI_BAD_PUD,
   or PI_NOT_PERMITTED.

   gpio: 0-53.
   pud:  PUD_UP, PUD_DOWN, PUD_OFF.
*/

int read_gpio(int gpio);
/* Read the gpio level.

   Returns the gpio level if OK, otherwise PI_BAD_GPIO.

   gpio:0-53.
*/

int write_gpio(int gpio, int level);
/*
   Write the gpio level.

   Returns 0 if OK, otherwise PI_BAD_GPIO, PI_BAD_LEVEL,
   or PI_NOT_PERMITTED.

   gpio:  0-53.
   level: 0, 1.

   Notes

   If PWM or servo pulses are active on the gpio they are switched off.
*/

int set_PWM_dutycycle(int user_gpio, int dutycycle);
/* Start (non-zero dutycycle) or stop (0) PWM pulses on the gpio.

   Returns 0 if OK, otherwise PI_BAD_USER_GPIO, PI_BAD_DUTYCYCLE,
   or PI_NOT_PERMITTED.

   user_gpio: 0-31.
   dutycycle: 0-range (range defaults to 255).

   Notes

   The set_PWM_range() function can change the default range of 255.
*/

int set_PWM_range(int user_gpio, int range_);
/* Set the range of PWM values to be used on the gpio.

   Returns 0 if OK, otherwise PI_BAD_USER_GPIO, PI_BAD_DUTYRANGE,
   or PI_NOT_PERMITTED.

   user_gpio: 0-31.
   range_:    25-40000.

   Notes

   If PWM is currently active on the gpio its dutycycle will be
   scaled to reflect the new range.

   The real range, the number of steps between fully off and fully on
   for each of the 18 available gpio frequencies is

   25(#1), 50(#2), 100(#3), 125(#4), 200(#5), 250(#6), 400(#7),
   500(#8), 625(#9), 800(#10), 1000(#11), 1250(#12), 2000(#13),
   2500(#14), 4000(#15), 5000(#16), 10000(#17), 20000(#18)

   The real value set by set_PWM_range is
   (dutycycle * real range) / range.

*/

int get_PWM_range(int user_gpio);
/* Get the range of PWM values being used on the gpio.

   Returns the dutycycle range used for the gpio if OK,
   otherwise PI_BAD_USER_GPIO.

   user_gpio: 0-31.
*/

int get_PWM_real_range(int user_gpio);
/* Get the real underlying range of PWM values being used on the gpio.

   Returns the real range used for the gpio if OK,
   otherwise PI_BAD_USER_GPIO.

   user_gpio: 0-31.
*/

int set_PWM_frequency(int user_gpio, int frequency);
/*
   Set the frequency (in Hz) of the PWM to be used on the gpio.

   Returns the numerically closest frequency if OK, otherwise
   PI_BAD_USER_GPIO or PI_NOT_PERMITTED.

   user_gpio: 0-31.
   frequency: 0- (Hz).

   The selectable frequencies depend upon the sample rate which
   may be 1, 2, 4, 5, 8, or 10 microseconds (default 5).  The
   sample rate is set when the C pigpio library is started.

   Each gpio can be independently set to one of 18 different
   PWM frequencies.

   If PWM is currently active on the gpio it will be switched
   off and then back on at the new frequency.

   1us 40000, 20000, 10000, 8000, 5000, 4000, 2500, 2000, 1600,
       1250, 1000, 800, 500, 400, 250, 200, 100, 50

   2us 20000, 10000,  5000, 4000, 2500, 2000, 1250, 1000, 800,
       625, 500, 400, 250, 200, 125, 100, 50, 25

   4us 10000, 5000, 2500, 2000, 1250, 1000, 625, 500, 400,
       313, 250, 200, 125, 100, 63, 50, 25, 13

   5us 8000, 4000, 2000, 1600, 1000, 800, 500, 400, 320,
       250, 200, 160, 100, 80, 50, 40, 20, 10

   8us 5000, 2500, 1250, 1000, 625, 500, 313, 250, 200,
       156, 125, 100, 63, 50, 31, 25, 13, 6

   10us 4000, 2000, 1000, 800, 500, 400, 250, 200, 160,
        125, 100, 80, 50, 40, 25, 20, 10, 5
*/

int get_PWM_frequency(int user_gpio);
/*
   Get the frequency of PWM being used on the gpio.

   Returns the frequency (in hertz) used for the gpio if OK,
   otherwise PI_BAD_USER_GPIO.

   user_gpio: 0-31.
*/

int set_servo_pulsewidth(int user_gpio, int pulsewidth);
/*
   Start (500-2500) or stop (0) servo pulses on the gpio.

   Returns 0 if OK, otherwise PI_BAD_USER_GPIO, PI_BAD_PULSEWIDTH or
   PI_NOT_PERMITTED.

   user_gpio:  0-31.
   pulsewidth: 0 (off), 500 (most anti-clockwise) - 2500 (most clockwise).

   The selected pulsewidth will continue to be transmitted until
   changed by a subsequent call to set_servo_pulsewidth().

   The pulsewidths supported by servos varies and should probably be
   determined by experiment. A value of 1500 should always be safe and
   represents the mid-point of rotation.

   You can DAMAGE a servo if you command it to move beyond its limits.

   OTHER UPDATE RATES:

   This function updates servos at 50Hz.  If you wish to use a different
   update frequency you will have to use the PWM functions.

   Update Rate (Hz) 50   100  200  400  500
   1E6/Hz        20000 10000 5000 2500 2000

   Firstly set the desired PWM frequency using set_PWM_frequency().

   Then set the PWM range using set_PWM_range() to 1E6/Hz.
   Doing this allows you to use units of microseconds when setting
   the servo pulse width.

   E.g. If you want to update a servo connected to gpio 25 at 400Hz

   set_PWM_frequency(25, 400);
   set_PWM_range(25, 2500);

   Thereafter use the set_PWM_dutycycle() function to move the servo,
   e.g. set_PWM_dutycycle(25, 1500) will set a 1500 us pulse. 
*/

int notify_open(void);
/*
   Get a free notification handle.

   Returns a handle greater than or equal to zero if OK,
   otherwise PI_NO_HANDLE.

   A notification is a method for being notified of gpio state
   changes via a pipe.

   Pipes are only accessible from the local machine so this function
   serves no purpose if you are using the library from a remote machine.
   The in-built (socket) notifications provided by callback()
   should be used instead.

   Notifications for handle x will be available at the pipe
   named /dev/pigpiox (where x is the handle number).
   E.g. if the function returns 15 then the notifications must be
   read from /dev/pigpio15.
*/

int notify_begin(int handle, uint32_t bits);
/*
   Start notifications on a previously opened handle.

   Returns 0 if OK, otherwise PI_BAD_HANDLE.

   handle: 0-31 (as returned by notify_open())
   bits:   a mask indicating the gpios to be notified.

   The notification sends state changes for each gpio whose
   corresponding bit in bits is set.

   Notes

   Each notification occupies 12 bytes in the fifo as follows:

   H (16 bit) seqno
   H (16 bit) flags
   I (32 bit) tick
   I (32 bit) level
*/

int notify_pause(int handle);
/*
   Pause notifications on a previously opened handle.

   Returns 0 if OK, otherwise PI_BAD_HANDLE.

   handle: 0-31 (as returned by notify_open())

   Notifications for the handle are suspended until
   notify_begin() is called again.
*/

int notify_close(int handle);
/*
   Stop notifications on a previously opened handle and
   release the handle for reuse.

   Returns 0 if OK, otherwise PI_BAD_HANDLE.

   handle: 0-31 (as returned by notify_open())
*/

int set_watchdog(int user_gpio, int timeout);
/*
   Sets a watchdog for a gpio.

   Returns 0 if OK, otherwise PI_BAD_USER_GPIO
   or PI_BAD_WDOG_TIMEOUT.

   user_gpio: 0-31.
   timeout:   0-60000.

   The watchdog is nominally in milliseconds.

   Only one watchdog may be registered per gpio.

   The watchdog may be cancelled by setting timeout to 0.

   If no level change has been detected for the gpio for timeout
   milliseconds any notification for the gpio has a report written
   to the fifo with the flags set to indicate a watchdog timeout.

   The callback() and callback_ex functions interpret the flags and will
   call registered callbacks for the gpio with level TIMEOUT.
*/

uint32_t read_bank_1(void);
/*
   Read the levels of the bank 1 gpios (gpios 0-31).

   The returned 32 bit integer has a bit set if the corresponding
   gpio is logic 1.  Gpio n has bit value (1<<n).
*/

uint32_t read_bank_2(void);
/*
   Read the levels of the bank 2 gpios (gpios 32-53).

   The returned 32 bit integer has a bit set if the corresponding
   gpio is logic 1.  Gpio n has bit value (1<<(n-32)).
*/

int clear_bank_1(uint32_t levels);
/*
   Clears gpios 0-31 if the corresponding bit in levels is set.

   Returns 0 if OK, otherwise PI_SOME_PERMITTED.

   A status of PI_SOME_PERMITTED indicates that the user is not
   allowed to write to one or more of the gpios.

   levels: a bit mask with 1 set if the corresponding gpio is
           to be cleared.
*/

int clear_bank_2(uint32_t levels);
/*
   Clears gpios 32-53 if the corresponding bit (0-21) in levels is set.

   Returns 0 if OK, otherwise PI_SOME_PERMITTED.

   A status of PI_SOME_PERMITTED indicates that the user is not
   allowed to write to one or more of the gpios.

   levels: a bit mask with 1 set if the corresponding gpio is
           to be cleared.
*/

int set_bank_1(uint32_t levels);
/*
   Sets gpios 0-31 if the corresponding bit in levels is set.

   Returns 0 if OK, otherwise PI_SOME_PERMITTED.

   A status of PI_SOME_PERMITTED indicates that the user is not
   allowed to write to one or more of the gpios.

   levels: a bit mask with 1 set if the corresponding gpio is
           to be set.
*/

int set_bank_2(uint32_t levels);
/*
   Sets gpios 32-53 if the corresponding bit (0-21) in levels is set.

   Returns 0 if OK, otherwise PI_SOME_PERMITTED.

   A status of PI_SOME_PERMITTED indicates that the user is not
   allowed to write to one or more of the gpios.

   levels: a bit mask with 1 set if the corresponding gpio is
           to be set.
*/

uint32_t get_current_tick(void);
/*

   Gets the current system tick.

   Tick is the number of microseconds since system boot.

   As tick is an unsigned 32 bit quantity it wraps around after
   2**32 microseconds, which is approximately 1 hour 12 minutes.

*/

uint32_t get_hardware_revision(void);
/*
   Get the Pi's hardware revision number.

   It is unfortunate that Pi boards have been named Revision.1 and
   Revision.2.  That use of the word revision is distinct from the
   Pi's hardware revision number.'

   The hardware revision is the last 4 characters on the Revision line
   of /proc/cpuinfo.

   The revision number can be used to determine the assignment of gpios
   to pins.

   There are at least two types of board.

   Type 1 has gpio 0 on P1-3, gpio 1 on P1-5, and gpio 21 on P1-13.
   Type 2 has gpio 2 on P1-3, gpio 3 on P1-5, gpio 27 on P1-13, and
   gpios 28-31 on P5.

   Type 1 boards have hardware revision numbers of 2 and 3.

   Type 2 boards have hardware revision numbers of 4, 5, 6, and 15.

   If the hardware revision can not be found or is not a valid
   hexadecimal number the function returns 0.
*/

int callback(int gpio, int edge, CBFunc_t f);
/*
*/

int callback_ex(int gpio, int edge, CBFuncEx_t f, void *user);
/*
*/

int callback_cancel(int id);
/*
*/

int wait_for_edge(int gpio, int edge, double timeout);
/*
*/

#endif

