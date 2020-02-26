repo --name=local_atom --baseurl=file:///root/fedora-girlsday-respin/extra_repo/

part / --size 7120 --fstype ext4

%packages

# added for girls day
atom
#codium
fritzing
chkconfig
clang
mc
#python2
#python2-virtualenv
gnome-tweak-tool
gnome-shell-extension-topicons-plus.noarch
vim
hunspell-de
hunspell-en
#libreoffice-langpack-de

-gnome-classic-session
-abrt*
-adobe-source-han-sans-cn-fonts
-adobe-source-han-sans-tw-fonts
-adobe-source-han-serif-cn-fonts
-adobe-source-han-serif-tw-fonts
-cldr-emoji-annotation
-ibus-typing-booster
-evolution
-evolution-help
-evolution-ews
-gnome-getting-started-docs
#-libreoffice-help-en
#-libreoffice-help-de
-langpacks-core-en
-langpacks-en
-f31-backgrounds-gnome
-gnome-backgrounds
-fedora-workstation-backgrounds
-desktop-backgrounds-gnome
-gnome-maps
-gnome-contacts
-gnome-weather
-simple-scan
-sane-backends-drivers-scanners
-gnome-boxes
-libreoffice-*
-unoconv
-ModemManager
-NetworkManager-wwan
-NetworkManager-bluetooth

%end

%post --nochroot
#touch $INSTALL_ROOT/this-is-install-root
#touch $LIVE_ROOT/this-is-live-root
cp /root/fedora-girlsday-respin/redhat_girlsday_2020.png $INSTALL_ROOT/usr/share/backgrounds/
cp /root/fedora-girlsday-respin/99-platformio-udev.rules $INSTALL_ROOT/etc/udev/rules.d/
cp /root/fedora-girlsday-respin/99-platformio-udev.rules $INSTALL_ROOT/lib/udev/rules.d/
cp /root/fedora-girlsday-respin/platformio-ide.tar.gz $INSTALL_ROOT/root/
%end

%post

cat >> /etc/rc.d/init.d/livesys << EOF

# install platformio-ide for atom
# apm install platformio-ide

if [ ! -d /home/girlsday/.platformio/ ]
then
    mkdir -p /home/girlsday/
    tar -xzf /root/platformio-ide.tar.gz -C /home/girlsday/
    chown -R girlsday:girlsday /home/girlsday/
    restorecon -RvF /home/girlsday/
fi

## setup gnome default settings
mkdir -p /etc/dconf/db/local.d/

cat << EOF1 > /etc/dconf/db/local.d/00-internal-settings
# enable extensions
# [org/gnome/shell]
# enabled-extensions=['TopIcons@phocean.net']

# show date and seconds in the Systray
[org/gnome/desktop/interface]
clock-show-date=true

# show weekdays in the calendar
[org/gnome/desktop/calendar]
show-weekdate=true

# make the nautilus icons look normal again
[org/gnome/nautilus/icon-view]
default-zoom-level='standard'

## set the default background
[org/gnome/desktop/background]
picture-uri='file:///usr/share/backgrounds/redhat_girlsday_2020.png'

## set the default background
[org/gnome/desktop/screensaver]
picture-uri='file:///usr/share/backgrounds/redhat_girlsday_2020.png'

# show Icons on desktop
[org/gnome/desktop/background]
show-desktop-icons=true

#set language to german
[org/gnome/desktop/input-sources]
sources=[('xkb', 'de')]

## disable notification for Gnome Software
[org/gnome/desktop/notifications/application/org-gnome-software]
enable=false
enable-sound-alerts=false
show-banners=false
show-in-lock-screen=false

## enable automatic timezone detection
[org/gnome/desktop/datetime]
automatic-timezone=true

# enable location services for automatic timezone settings
[org/gnome/system/location]
enabled=true

## disable download-updates (disables notification during post install for new updates)
[org/gnome/software]
download-updates=false

## enable sort-directories-first
[org/gtk/settings/file-chooser]
sort-directories-first=true
EOF1

cat << EOF2 > /etc/dconf/profile/user
user-db:user
system-db:local
EOF2

dconf update

# configure Firefox
mkdir -p /usr/lib64/firefox/browser/defaults/preferences/
cat << EOF3 > /usr/lib64/firefox/browser/defaults/preferences/aa_redhat.js

// take homepage settings from redhat.properties
pref("browser.startup.homepage", "data:text/plain,browser.startup.homepage=https://www.redhat.com/de");
pref("browser.startup.homepage_reset", "data:text/plain,browser.startup.homepage=https://www.redhat.com/de");
pref("startup.homepage_override_url", "");
pref("startup.homepage_welcome_url", "https://www.redhat.com/de");

// set spellcheck to DE
pref("spellchecker.dictionary", "de-DE");

// enable Do Not Track
pref("privacy.donottrackheader.enabled", true);

// disable the sending of information to Mozilla
pref("toolkit.telemetry.prompted", 2);
pref("toolkit.telemetry.rejected", true);
pref("datareporting.healthreport.service.enabled", false);
pref("datareporting.healthreport.uploadEnabled", false);
EOF3

# move welcome app to Desktop and add Icon
#rm /home/girlsday/.config/autostart/fedora-welcome.desktop

#cat << EOF4 > /home/girlsday/Schreibtisch/fedora-welcome.desktop
#[Desktop Entry]
#Name=Welcome to Fedora
#Exec=/usr/share/anaconda/gnome/fedora-welcome
#Terminal=false
#Type=Application
#StartupNotify=true
#NoDisplay=true
#X-GNOME-Autostart-enabled=false
#Icon=/usr/share/icons/Bluecurve/256x256/apps/start-here.png
#EOF4

## add text file with 1st steps on Desktop
#mkdir -p /home/girlsday/Schreibtisch
#cat << EOF5 > /home/girlsday/Schreibtisch/1st_steps.txt
#Hi..

#Vorbereitung der Programmierumgebung:

#- Öffne Atom
#  - Drücke die "Windows" Taste, und tippe "Atom"
#  - Drücke die Return Taste
#- Klicke auf "Install a Package" auf der rechten Seite
#- Klicke auf "Open Installer"
#- Suche auf der linken Seite nach dem Paket "platformio-ide"
#- Klicke auf "Install" bei dem Paket "platformio-ide"
#- Warte bis die Installation abgeschlossen ist
#- Klicke auf "Restart" in dem grünen Fenster

#Euer Red Hat Girls Day Team
#EOF5

mkdir -p /home/girlsday/.local/share/applications/
cat << EOF6 > /home/girlsday/.local/share/applications/atom.desktop
[Desktop Entry]
Name=Atom
Comment=A hackable text editor for the 21st Century.
GenericName=Text Editor
Exec=/usr/bin/atom %F
Icon=atom
Type=Application
StartupNotify=true
Categories=GNOME;GTK;Utility;TextEditor;Development;
MimeType=text/plain;
StartupWMClass=atom
EOF6

mkdir -p /home/girlsday/.local/share/keyrings/
cat << EOF7 > /home/girlsday/.local/share/keyrings/default
Standard-Schlüsselbund
EOF7

cat << EOF8 > /home/girlsday/.local/share/keyrings/Standard-Schlüsselbund.keyring
[keyring]
display-name=Standard-Schlüsselbund
ctime=1582716630
mtime=0
lock-on-idle=false
lock-after=false
EOF8

chmod 644 /home/girlsday/.local/share/keyrings/default
chmdo 600 /home/girlsday/.local/share/keyrings/Standard-Schlüsselbund.keyring

EOF

%end
