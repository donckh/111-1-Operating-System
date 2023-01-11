Step for using kfetch device module:
1. go to the floder directory, e.g.: 310551003/

2. type 'make' for making .ko file:
make

3. type 'make load' to load module at /dev/kfetch_mod_310551003:
make load

4. type 'sudo ./kfetch' with args(e.g.: -a) to show messages collect from kernel and send to user space. Otherwise it will random assign args.
sudo ./kfetch
