# Rocket

**Rocket** is an application launcher for KDE Plasma. It sorts all of your applications in a horizontal or vertical grid alphabetically, so you will always find what you are looking for at first glance. No more menu navigation, only scrolling.

Features:
 - Customizable row and column sizes
 - Theming
 - Search for keywords (such as "file manager")
 - Scroll support for touchpad, mouse and the mouse wheel
 - Keyboard navigation
 - Launch by pressing the Meta button

For more information check out the **wiki** and the **release notes**.

# Installation

Rocket has been tested on Kubuntu and on Debian, but it should also work on any platform with KDE installed.

## Release Version

For the most recent release version download the zipped file containing all the binaries and icons from the [KDE Store](https://store.kde.org/p/1507169/) and install Rocket by making the bash script ```installer.sh``` executable via ```chmod +x installer.sh``` and by running it with ```./installer.sh```.

## Git Version

Rocket uses C++ and Qt and KDE Plasma libraries to commununicate with the environment. If you would like to download Rocket and to test the most recent dev version, clone this repository and compile the code using

```qmake && make```

Beware that you need to create the folder ```~/.config/rocket``` before launching Rocket first, otherwise the program will not launch.

# Screenshot Gallery

## Default view
![Alt text](/screenshots/screenshot.jpeg?raw=true "")

## Dark Theme
![Alt text](/screenshots/screenshot_dark.jpeg?raw=true "")

## Searching
![Alt text](/screenshots/screenshot_search.jpeg?raw=true "")

## Custom Grid (Boxes Disabled, Blurred Background, Vertical Grid and more Icons)
![Alt text](/screenshots/screenshot_large_grid_noboxes.jpeg?raw=true "")

## Rocket Designer, the Customization Tool
![Alt text](/screenshots/rocket_designer.png?raw=true "")

-------------------------------------------
\* Disclaimer: I'm not a professional developer, but it works on my configuration perfectly, and I love it. However, on your custom environment, you might experience unwanted bugs -- feel free to fork and to contribute.
This software does not contain any code which messes with your base system, so if it does not work for you, it's easy to uninstall -- just follow the instructions in the release notes.

Rocket's icon was taken from the [OpenMoji](https://openmoji.org/) library.
