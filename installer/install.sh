#!/usr/bin/env bash

COLOR_RESET="$(tput sgr0)"
COLOR_YELLOW="$(tput setaf 3)"

mkdir -p ~/.local/share/icons/hicolor/scalable
cp rocket.svg ~/.local/share/icons/hicolor/scalable/
cp rocketdesigner.svg ~/.local/share/icons/hicolor/scalable/

mkdir -p ~/.local/bin/
chmod +x rocket
cp rocket ~/.local/bin/rocket
chmod +x rocketdesigner
cp rocketdesigner ~/.local/bin/rocketdesigner

chmod +x rocket.desktop
cp rocket.desktop ~/.local/share/applications/
chmod +x rocketdesigner.desktop
echo "Icon=$HOME/.local/share/icons/hicolor/scalable/rocket.svg" >> ~/.local/share/applications/rocket.desktop
cp rocketdesigner.desktop ~/.local/share/applications/
echo "Icon=$HOME/.local/share/icons/hicolor/scalable/rocketdesigner.svg" >> ~/.local/share/applications/rocketdesigner.desktop

mkdir ~/.config/rocket
cp wallpaper.jpeg ~/.config/rocket

mkdir -p ~/.local/share/dbus-1/services/
cp com.friciwolf.rocket.service ~/.local/share/dbus-1/services/com.friciwolf.rocket.service
echo "Exec=$HOME/.local/bin/rocket --dbus" >> ~/.local/share/dbus-1/services/com.friciwolf.rocket.service

echo "Rocket is installed now."
echo "${COLOR_YELLOW}warning${COLOR_RESET}: be sure to add '$HOME/.local/bin' to your PATH to be able to run Rocket."
