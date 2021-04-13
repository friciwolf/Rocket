#!/usr/bin/env bash

mkdir -p ~/.local/share/icons/hicolor/scalable
cp rocket.svg ~/.local/share/icons/hicolor/scalable/
chmod +x rocket
cp rocket ~/.local/bin
chmod +x rocket.desktop
cp rocket.desktop ~/.local/share/applications/
mkdir ~/.config/rocket
cp wallpaper.jpeg ~/.config/rocket
