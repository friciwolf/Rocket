#!/usr/bin/env bash

COLOR_RESET="$(tput sgr0)"
COLOR_YELLOW="$(tput setaf 3)"

mkdir -p ~/.local/share/icons/hicolor/scalable
cp rocket.svg ~/.local/share/icons/hicolor/scalable/
chmod +x rocket
mkdir -p ~/.local/bin/
cp rocket ~/.local/bin/rocket
chmod +x rocket.desktop
cp rocket.desktop ~/.local/share/applications/
mkdir ~/.config/rocket
cp wallpaper.jpeg ~/.config/rocket

echo "Rocket is installed now."
echo "${COLOR_YELLOW}warning${COLOR_RESET}: be sure to add '$HOME/.local/bin' to your PATH to be able to run Rocket."