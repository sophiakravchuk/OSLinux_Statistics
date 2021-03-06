# SOrrel JUice <img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/icon.png" width="40" height="40" /> - OS Linux Statistics Application 

## Team
Sophia Kravchuk, Julia Pochynok, Sophia Haletska

## Description
**SOrrel JUice** is a simple task manager that uses file system /proc to get the information about the current state of your OS.

<p align="center">
<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/icon.png" width="150" height="150"/>
</p>

## Data
Output of various characteristics of the OS state - CPU usage, memory, number of processes, open files, etc. is taken from FS /proc.

/proc is a folder that does not exist on the disk. All subdirectories, files and information stored in them are generated by the kernel on the fly as soon as it is requested. But everything works so that there is no difference between a normal file system and proc, if you open it with a file manager.

## Visualization and Usage

Our program has two modes: day and night, which you cam change by clicking on Moon Button.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen1.png" width="470" height="340" margin-left="auto" margin-right="auto">    <img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen8.png" width="470" height="340" margin-left="auto" margin-right="auto" />



As you can see the program has four button: 

### Processes | CPU | Memory | About us

And also, the Help button opens the page with the instruction manual of our application.
If you see this text, you are on the Help page.

Detailed information about each page of our application will be provided below. 
To go to any of them, use the above buttons.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen1.png"  margin-left="auto" margin-right="auto" />

### Processes

The Processes button will redirect you to a page where you can see all the processes and information about them in the form of a table with the following columns: 
Process name, process identification number, process status, usage of the central processor by the process in the percentage, amount of virtual memory occupied by the process, amount of RAM occupied by the process, and the amount of RAM occupied by the process in the percentage.

If you click on any column names, the entire table will be sorted in descending order by the clicked parameter; if you click again, the entire table will be sorted in ascending order.

The rows in the table can also be highlighted in different shades of three colors (green, purple, red).

If the string is green, the CPU usage parameter is greater than 25%, but the memory usage does not exceed this value.
If the string is purple, then, on the contrary, the memory usage parameter is greater than 25%, but the CPU usage does not exceed this value.
If the value of both parameters exceeds 25%, the string will be red.
The hue shows the limits within which the value is located. The lightest shade indicates a value from 25% to 50%, the darkest - a value above 90%.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen2.png" width="470" height="340" margin-left="auto" margin-right="auto">    <img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen11.png" width="470" height="340" margin-left="auto" margin-right="auto" />


Also, on this page, you can right-click on the name of any process, then you will see a menu with two available options for a specific process: kill the process, process graph.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen10.png"  margin-left="auto" margin-right="auto">


### Kill process:
This option is usually used if you want to stop this process because it overloads the computer (processor or RAM) or for other reasons.
If you select this option, our program will first give you a confirmation window to ask you if you really want to stop the process's execution. Note: If the process ended after you decide to kill it, but before you click ‘OK’ to end confirmation, the program would notify you that you cannot kill the process.

### Process graph:
If you select this option, you will see a page with the dynamic visualization of information about CPU and RAM used by the process you have chosen.
Also, on this page, you have the opportunity to kill the process.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen9.png" margin-left="auto" margin-right="auto" />


### CPU

In the CPU section you will be able to see the visualization of information about the CPU usage in the form of a graph and such information about the CPU as: full processor name, CPU usage by all processes in the percentage, number of processes, how much time has passed since the last computer reboot, the number of processor cores.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen3.png" margin-left="auto" margin-right="auto">   


### Memory

In the Memory section you can see the visualization of information about the RAM usage by all processes in the form of a graph and such information about RAM as: the total amount of RAM in gigabytes, amount of memory currently used in gigabytes (in the percentage), the amount of free RAM, the amount of cached RAM on your computer.

The information in the three sections mentioned above is changing dynamically.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen4.png" margin-left="auto" margin-right="auto">   


### About us

On the About us page, you can find information about the developers of this application, a brief description of the functionality, operating systems on which you can run our application and other details.


<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen5.png" margin-left="auto" margin-right="auto">   


### Help 
On the Help page you can find all detailed infromation about application structure and usage.

<img src="https://github.com/sophiakravchuk/OSLinux_Statistics/blob/master/photos/screen6.png" margin-left="auto" margin-right="auto">   



## Run Dependencies
You can run this application in QtCreator.





