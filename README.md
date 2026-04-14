# Dynamic Round System

A fully dynamic round (circle zone) system for San Andreas Multiplayer servers with MySQL integration.

📦 Requirements

```
a_samp
Incognito Streamer Plugin
BlueG MySQL Plugin
YSI Library (y_hooks, y_iterate)
sscanf Plugin
```

Create the following table in your MySQL database:
```
CREATE TABLE `rounds` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `x` FLOAT,
  `y` FLOAT,
  `z` FLOAT,
  `world` INT,
  `interior` INT,
  `color` INT,
  `size` FLOAT
);
```
# Commands
```
/createround

Create a new round at your current position.

/createround [color] [size]

Example:

/createround red 5.0
/editround

Modify an existing round.

/editround [id] [type] [value]

Available types:

pos → Update to your current position
vw → Set virtual world
int → Set interior
color → Change color
size → Change size
delete → Remove round

Example:

/editround 1 size 10.0
/editround 1 color blue
/editround 1 delete
/nearround
```

Get the nearest round ID.

/nearround
```
