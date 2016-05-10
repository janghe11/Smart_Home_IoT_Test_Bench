#! /bin/sh

#sudo raspi-config
#sudo dpkg-reconfigure locales
#sudo dpkg-reconfigure tzdata

#Check if it has root permission
ROOT_UID="0"

if [ "$UID" -ne "$ROOT_UID" ] ; then
	echo "Root permission is required. Please command with \"sudo -\""
	exit 1
fi

echo "Start to install Smart Home IoT Test Bench on Raspberry Pi. It takes almost 25 - 30 minutes...\n"

# Check repositories update and dist-upgrade
apt-get update
apt-get -y dist-upgrade

# Basic installation
apt-get install -y vim gcc apache2 php5 git libmysqlclient-dev zlib1g-dev autoconf autogen

# MariaDB install
echo "mariadb-server-10.0 mariadb-server/root_password password raspberry" | sudo debconf-set-selections
echo "mariadb-server-10.0 mariadb-server/root_password_again password raspberry" | sudo debconf-set-selections
echo "mariadb-server mariadb-server/root_password password raspberry" | sudo debconf-set-selections
echo "mariadb-server mariadb-server/root_password_again password raspberry" | sudo debconf-set-selections
echo "mariadb-server-5.5 mariadb-server/root_password password raspberry" | sudo debconf-set-selections
echo "mariadb-server-5.5 mariadb-server/root_password_again password raspberry" | sudo debconf-set-selections

echo "mariadb-server mysql-server/root_password password raspberry" | sudo debconf-set-selections
echo "mariadb-server mysql-server/root_password_again password raspberry" | sudo debconf-set-selections
echo "mariadb-server-5.5 mysql-server/root_password password raspberry" | sudo debconf-set-selections
echo "mariadb-server-5.5 mysql-server/root_password_again password raspberry" | sudo debconf-set-selections
echo "mariadb-server-10.0 mysql-server/root_password password raspberry" | sudo debconf-set-selections
echo "mariadb-server-10.0 mysql-server/root_password_again password raspberry" | sudo debconf-set-selections

echo "mysql-server mysql-server/root_password password raspberry" | sudo debconf-set-selections
echo "mysql-server mysql-server/root_password_again password raspberry" | sudo debconf-set-selections
apt-get install -y mariadb-server
apt-get install -y php5-mysql

# PhpMyAdmin install
echo 'phpmyadmin phpmyadmin/dbconfig-install boolean true' | sudo debconf-set-selections
echo 'phpmyadmin phpmyadmin/app-password-confirm password raspberry' | sudo debconf-set-selections
echo 'phpmyadmin phpmyadmin/mysql/admin-pass password raspberry' | sudo debconf-set-selections
echo 'phpmyadmin phpmyadmin/mysql/app-pass password raspberry' | sudo debconf-set-selections
echo 'phpmyadmin phpmyadmin/reconfigure-webserver multiselect apache2' | sudo debconf-set-selections
apt-get install -y phpmyadmin

# Create database and_rpi_avr and create table rpi_avr
mysql -uroot -praspberry -e "CREATE DATABASE and_rpi_avr"
mysql -uroot -praspberry -e "USE and_rpi_avr"
mysql -uroot -praspberry -Dand_rpi_avr -e "CREATE TABLE rpi_avr (cid char(30) NOT NULL, client_id char(30) NOT NULL, rpi_id char(20) NOT NULL, avr_id char(20) NOT NULL, avr_data char(1) NOT NULL, avr_param char(2) NULL DEFAULT NULL, rpi_time TIMESTAMP  NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY(cid)) CHARACTER SET utf8 COLLATE utf8_general_ci"

# Insert basic data to database for android

# Download required sources from github
git clone https://github.com/janghe11/CapstoneDesign_MicroProcessor.git /home/pi/CapstoneDesign_MicroProcessor
chown -R pi:pi /home/pi/CapstoneDesign_MicroProcessor

# Set apache homepage location from /var/www/html to /home/pi/public_html
mkdir /home/pi/public_html 
chown pi:pi /home/pi/public_html

\cp /home/pi/CapstoneDesign_MicroProcessor/CD_RaspberryPi2/etc/apache2/apache2.conf /etc/apache2/apache2.conf
chown root:root /etc/apache2/apache2.conf

\cp /home/pi/CapstoneDesign_MicroProcessor/CD_RaspberryPi2/etc/apache2/sites-available/000-default.conf /etc/apache2/sites-available/000-default.conf
chown root:root /etc/apache2/sites-available/000-default.conf

# Install netdata
\cp -r /home/pi/CapstoneDesign_MicroProcessor/CD_RaspberryPi2/home/pi/public_html/netdata /home/pi/public_html
chown www-data:www-data -R /home/pi/public_html/netdata
/home/pi/public_html/netdata/netdata-installer.sh --dont-wait --zlib-is-really-here

# Compile and install avr_daemon
gcc -o /home/pi/public_html/avr_daemon /home/pi/CapstoneDesign_MicroProcessor/CD_RaspberryPi2/home/pi/public_html/avr_daemon.c `mysql_config --cflags --libs`
chown pi:pi /home/pi/public_html/avr_daemon

# Install control.php
\cp /home/pi/CapstoneDesign_MicroProcessor/CD_RaspberryPi2/home/pi/public_html/control.php /home/pi/public_html
chown pi:pi /home/pi/public_html/control.php

# Add WiFi SSID and Password
echo "\n" >> /etc/wpa_supplicant/wpa_supplicant.conf
echo "network={" >> /etc/wpa_supplicant/wpa_supplicant.conf
echo "        ssid=\"Capstone_MP\"" >> /etc/wpa_supplicant/wpa_supplicant.conf
echo "        psk=capstonemp12" >> /etc/wpa_supplicant/wpa_supplicant.conf
echo "}" >> /etc/wpa_supplicant/wpa_supplicant.conf

# Add control.php and avr_daemon daemon to /etc/init.d

# Cleaning and finish
apt-get -y autoremove
apt-get autoclean

echo "Smart Home IoT Test Bench on Raspberry Pi installated successfully. Try to reboot...\n"

reboot
