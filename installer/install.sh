#!/usr/bin/env bash

mkdir -p ~/.local/share/icons/hicolor/scalable
cp rocket.svg ~/.local/share/icons/hicolor/scalable/
chmod +x rocket
mkdir -p ~/.local/bin/
cp rocket ~/.local/bin/rocket
chmod +x rocket.desktop
cp rocket.desktop ~/.local/share/applications/
mkdir ~/.config/rocket
cp wallpaper.jpeg ~/.config/rocket
