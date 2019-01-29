# iAlarm
  This alarm will help to wake you up at any time of the day.
  It's also built-in timer there.
## Getting started

### Prerequisites
  * STM32F411xxx
  * 2 Nokia Displays
  * DS3231
  * Matrix Keyboard
  * Sound system
  * Micro-USB

### Installing
  ```bash
  $ git clone https://github.com/chernetskyi/iAlarm  
  ```
  1. Take the STM32F411xxx.
  2. Connect all details in the following way:
  <img src="scheme/iAlarm.png" width=600, height=600>
  2. Open the iAlarm project in the System Workbench or Keil.
  3. Build and run the project.

### Interface
  To control to the "iAlarm", you have 2 displays. One of them just show the information about the timer and the alarm, 
  the second one helps to set up the timer, the alarm or the time.<br>
  The 1st display:
  (PHOTO)<br>
  
  **---iAlarm---** - the name of the project <br>
  **16:00:47 ** - the current time <br>
  **Tuesday ** - the current day of the week <br>
  **29 Jun 2019 ** - the current date <br>
  ** TIMER  ALARM ** - info about timer and alarm(if the timer is seted, instead of the word "TIMER" it would be<br>
  a countdown. Otherwise - word "TIMER". If it is some alarm seted, so the word "ALARM" would be printed on the <br>
  black background.<br>
  
  The 2nd display:
  (PHOTO)
  **---iAlarm---** - the name of the project

## Authors
Chernetskyi Volodymyr, Hermann Yavorskyi, Serhii Rosovskyi

## License
 
